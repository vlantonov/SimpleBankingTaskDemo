#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include "usecase/auth/login_usecase.h"
#include "domain/domain_exception.h"
#include "scope/login_scope.h"

namespace {

class FirstLoginUserPortForAuthStatements final : public usecase::IUserPort {
public:
    std::optional<usecase::AuthUser> find_by_username(const std::string&) const override {
        return std::nullopt;
    }

    usecase::AuthUser create(const std::string& username, const std::string& pin) override {
        return usecase::AuthUser{username, pin};
    }
};

class TokenSessionPortForAuthStatements final : public usecase::ISessionPort {
public:
    std::string open_session_for(const std::string& username) override {
        return username + ".header.signature";
    }
};

class ExistingUserPortForAuthStatements final : public usecase::IUserPort {
public:
    std::optional<usecase::AuthUser> find_by_username(const std::string& username) const override {
        return usecase::AuthUser{username, "123"};
    }

    usecase::AuthUser create(const std::string&, const std::string&) override {
        throw std::logic_error("create should not be called for existing user");
    }
};

class RecordingSessionPortForAuthStatements final : public usecase::ISessionPort {
public:
    std::string open_session_for(const std::string& username) override {
        opened_usernames_.push_back(username);
        return username + ".header.signature";
    }

    const std::vector<std::string>& opened_usernames() const {
        return opened_usernames_;
    }

private:
    std::vector<std::string> opened_usernames_;
};

class CapturingAuthEventLogPortForAuthStatements final : public usecase::IAuthEventLogPort {
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

}  // namespace

class AuthStatements {
public:
    explicit AuthStatements(usecase::LoginUsecase& login_usecase)
        : login_usecase_(login_usecase) {}

    void assert_login_rejected_with_validation_error() {
        EXPECT_THAT(
            [&]() { login_usecase_.execute(login_scope_with_empty_username().to_login_request()); },
            testing::ThrowsMessage<domain::ValidationException>(testing::Eq("Validation error"))
        );
    }

    void assert_first_login_creates_user_and_opens_session() {
        EXPECT_THAT(
            login_usecase_.execute(login_scope_for_first_login().to_login_request()),
            testing::AllOf(
                testing::Field(&usecase::LoginResponse::message, testing::Eq("Welcome, ola")),
                testing::Field(&usecase::LoginResponse::token, testing::Eq("ola.header.signature"))
            )
        );
    }

    void assert_first_login_appends_user_created_and_login_events_to_log() {
        const auto captured_events = execute_first_login_and_capture_events();
        assert_user_created_and_login_events(captured_events);
    }

    void assert_returning_user_with_wrong_pin_is_rejected_without_side_effects() {
        auto setup = create_usecase_with_existing_user_and_recording_ports();

        EXPECT_THAT(
            [&]() { setup.usecase.execute(login_scope_for_existing_user_with_wrong_pin().to_login_request()); },
            testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Invalid credentials"))
        );
        assert_no_side_effects(*setup.session_port, *setup.event_log_port);
    }

private:
    struct ExistingUserUsecaseSetup {
        usecase::LoginUsecase usecase;
        const RecordingSessionPortForAuthStatements* session_port;
        const CapturingAuthEventLogPortForAuthStatements* event_log_port;
    };

    ExistingUserUsecaseSetup create_usecase_with_existing_user_and_recording_ports() const {
        auto session_port = std::make_unique<RecordingSessionPortForAuthStatements>();
        auto event_log_port = std::make_unique<CapturingAuthEventLogPortForAuthStatements>();

        const auto* captured_session_port = session_port.get();
        const auto* captured_event_log_port = event_log_port.get();

        return ExistingUserUsecaseSetup{
            usecase::LoginUsecase(
                std::make_unique<ExistingUserPortForAuthStatements>(),
                std::move(session_port),
                std::move(event_log_port)
            ),
            captured_session_port,
            captured_event_log_port
        };
    }

    void assert_no_side_effects(
        const RecordingSessionPortForAuthStatements& session_port,
        const CapturingAuthEventLogPortForAuthStatements& event_log_port
    ) const {
        EXPECT_THAT(session_port.opened_usernames(), testing::IsEmpty());
        EXPECT_THAT(event_log_port.events(), testing::IsEmpty());
    }

    void assert_user_created_and_login_events(const std::vector<usecase::AuthEvent>& captured_events) const {
        EXPECT_THAT(
            captured_events,
            testing::ElementsAre(
                testing::AllOf(
                    testing::Field(&usecase::AuthEvent::type, testing::Eq(usecase::AuthEventType::kUserCreated)),
                    testing::Field(&usecase::AuthEvent::username, testing::Eq("ola"))
                ),
                testing::AllOf(
                    testing::Field(&usecase::AuthEvent::type, testing::Eq(usecase::AuthEventType::kLogin)),
                    testing::Field(&usecase::AuthEvent::username, testing::Eq("ola"))
                )
            )
        );
    }

    std::vector<usecase::AuthEvent> execute_first_login_and_capture_events() const {
        auto event_log_port = std::make_unique<CapturingAuthEventLogPortForAuthStatements>();
        const auto* captured_port = event_log_port.get();
        usecase::LoginUsecase usecase_with_capturing_log(
            std::make_unique<FirstLoginUserPortForAuthStatements>(),
            std::make_unique<TokenSessionPortForAuthStatements>(),
            std::move(event_log_port)
        );

        usecase_with_capturing_log.execute(login_scope_for_first_login().to_login_request());
        return captured_port->events();
    }

    LoginScope login_scope_with_empty_username() const {
        return LoginScope::Builder().with_username("").with_pin("123").build();
    }

    LoginScope login_scope_for_first_login() const {
        return LoginScope::Builder().with_username("ola").with_pin("123").build();
    }

    LoginScope login_scope_for_existing_user_with_wrong_pin() const {
        return LoginScope::Builder().with_username("ola").with_pin("999").build();
    }

    usecase::LoginUsecase& login_usecase_;
};
