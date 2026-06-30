#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include "server_configurator.h"

namespace {

std::thread start_server(httplib::Server& server, int port) {
    return std::thread([&server, port]() {
        server.listen("127.0.0.1", port);
    });
}

void stop_server(httplib::Server& server, std::thread& server_thread) {
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

httplib::Result post_login_request(int port, const std::string& payload) {
    httplib::Client client("127.0.0.1", port);
    return client.Post("/api/v1/login", payload, "application/json");
}

void assert_validation_error_response(const httplib::Result& response) {
    ASSERT_NE(response, nullptr) << "Connection to test server failed";
    EXPECT_EQ(response->status, 400) << "Expected 400 Validation error, got " << response->status;
    const auto body = nlohmann::json::parse(response->body);
    EXPECT_EQ(body.value("error", ""), "Validation error");
    EXPECT_EQ(body.value("token", "missing"), "");
    EXPECT_EQ(body.value("message", "missing"), "");
}

}  // namespace

class ServerRoutingTest : public ::testing::Test {
protected:
    static constexpr int kTestPort = 18080;

    void SetUp() override {
        configure_server(server_);
        server_thread_ = start_server(server_, kTestPort);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override {
        stop_server(server_, server_thread_);
    }

    httplib::Server server_;
    std::thread server_thread_;
};

// TDD Red Phase - configure_server is a no-op stub; POST /api/v1/login returns 404 instead of 400
TEST_F(ServerRoutingTest, login_with_empty_username_returns_400_not_404) {
    const auto response = post_login_request(kTestPort, R"({"user":"","pin":"123"})");
    assert_validation_error_response(response);
}

class ServerAuthEventLogRoutingTest : public ::testing::Test {
protected:
    static constexpr int kTestPort = 18081;

    void SetUp() override {
        std::filesystem::remove("auth_events.log");
        configure_server(server_);
        server_thread_ = start_server(server_, kTestPort);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override {
        stop_server(server_, server_thread_);
        std::filesystem::remove("auth_events.log");
    }

    static std::string read_auth_event_log() {
        std::ifstream input{"auth_events.log"};
        if (!input.is_open()) {
            return {};
        }
        return std::string{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    }

    struct ParsedAuthEvent {
        std::string type;
        std::string username;
    };

    static ParsedAuthEvent parse_auth_event_line(const std::string& line) {
        std::istringstream line_stream(line);
        std::string type;
        std::string username;
        line_stream >> type >> username;
        return ParsedAuthEvent{type, username};
    }

    static void assert_auth_events_written(const std::vector<ParsedAuthEvent>& parsed_events) {
        ASSERT_EQ(parsed_events.size(), 2u);
        EXPECT_EQ(parsed_events[0].type, "USER_CREATED");
        EXPECT_EQ(parsed_events[0].username, "ola");
        EXPECT_EQ(parsed_events[1].type, "LOGIN");
        EXPECT_EQ(parsed_events[1].username, "ola");
    }

    static std::vector<ParsedAuthEvent> parse_auth_event_log(const std::string& log_contents) {
        std::vector<ParsedAuthEvent> events;
        std::istringstream stream(log_contents);
        for (std::string line; std::getline(stream, line);) {
            if (!line.empty()) {
                events.push_back(parse_auth_event_line(line));
            }
        }
        return events;
    }

    httplib::Server server_;
    std::thread server_thread_;
};

TEST_F(ServerAuthEventLogRoutingTest, first_login_writes_user_created_then_login_to_auth_event_log) {
    const auto res = post_login_request(kTestPort, R"({"user":"ola","pin":"123"})");
    ASSERT_NE(res, nullptr) << "Connection to test server failed";
    ASSERT_EQ(res->status, 200);

    const auto log_contents = read_auth_event_log();
    const auto parsed_events = parse_auth_event_log(log_contents);
    assert_auth_events_written(parsed_events);
}
