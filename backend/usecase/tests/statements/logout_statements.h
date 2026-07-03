#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "domain/auth/logout_request.h"
#include "usecase/auth/logout_usecase.h"

namespace logout_statements_detail {

constexpr auto kValidLogoutToken = "ola.header.signature";
constexpr auto kLogoutUsername = "ola";

class ActiveSessionPortForLogoutStatements final : public usecase::ILogoutSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        if (token == kValidLogoutToken) {
            return kLogoutUsername;
        }
        return std::nullopt;
    }

    void close_session(const std::string& token) override {
        closed_tokens_.push_back(token);
    }

    const std::vector<std::string>& closed_tokens() const {
        return closed_tokens_;
    }

private:
    std::vector<std::string> closed_tokens_;
};

class CapturingLogoutEventLogPort final : public usecase::IAuthEventLogPort {
public:
    void append(const usecase::AuthEvent& event) override {
        events_.push_back(event);
    }

    const std::vector<usecase::AuthEvent>& events() const {
        return events_;
    }

private:
    std::vector<usecase::AuthEvent> events_;
};

}  // namespace logout_statements_detail

class LogoutStatements {
public:
    void assert_logout_with_valid_session_token_destroys_the_session() {
        auto setup = logout_with_active_session();
        EXPECT_THAT(setup.session_port->closed_tokens(), testing::ElementsAre(logout_statements_detail::kValidLogoutToken));
        EXPECT_THAT(
            setup.event_log_port->events(),
            testing::ElementsAre(
                testing::AllOf(
                    testing::Field(&usecase::AuthEvent::type, testing::Eq(usecase::AuthEventType::kLogout)),
                    testing::Field(&usecase::AuthEvent::username, testing::Eq(logout_statements_detail::kLogoutUsername))
                )
            )
        );
    }

private:
    struct LogoutUsecaseSetup {
        usecase::LogoutUsecase usecase;
        const logout_statements_detail::ActiveSessionPortForLogoutStatements* session_port;
        const logout_statements_detail::CapturingLogoutEventLogPort* event_log_port;
    };

    LogoutUsecaseSetup logout_with_active_session() const {
        auto setup = create_logout_usecase_with_active_session();
        setup.usecase.execute(domain::LogoutRequest{logout_statements_detail::kValidLogoutToken});
        return setup;
    }

    LogoutUsecaseSetup create_logout_usecase_with_active_session() const {
        auto session_port = std::make_unique<logout_statements_detail::ActiveSessionPortForLogoutStatements>();
        auto event_log_port = std::make_unique<logout_statements_detail::CapturingLogoutEventLogPort>();
        const auto* captured_session_port = session_port.get();
        const auto* captured_event_log_port = event_log_port.get();
        return LogoutUsecaseSetup{
            usecase::LogoutUsecase(std::move(session_port), std::move(event_log_port)),
            captured_session_port,
            captured_event_log_port
        };
    }
};