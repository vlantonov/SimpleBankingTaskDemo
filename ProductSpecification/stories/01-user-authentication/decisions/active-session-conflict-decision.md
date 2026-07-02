# Decision: Active session conflict on second login

**Date**: 2026-07-02 **Scenarios**: 4.1

Second login must enforce the single-active-session rule before any new session side effects.

| Rejected | Why |
|----------|-----|
| Detect conflict inside `ISessionPort::open_session_for` | Hides business policy in adapter internals and weakens usecase clarity |
| Allow second login and replace prior token | Violates scenario 4.1 expectation to reject with conflict |

**Chosen**: In `LoginUsecase`, check for an existing active session first; if present throw `SessionConflictException`, map it to HTTP 409, and stop before token creation or LOGIN append.

## Model

- Add `ISessionPort::has_active_session_for(username)` to expose active-session state
- `usecase::LoginUsecase::execute(LoginRequest)` checks active session before `find_or_create_user` and `open_session_for`
- Add `domain::SessionConflictException` for conflict failure path
- `adapters::http::ExceptionHandler` maps `SessionConflictException` to HTTP 409 with `error = "Session conflict"`
- `IAuthEventLogPort::append(LOGIN, user)` runs only when no active session conflict

## Edge Cases

| Case | Behavior |
|------|----------|
| Existing user with active session logs in again | Return 409 session conflict; no new token; no LOGIN event |
| Existing user without active session logs in | Continue normal validation and session creation path |
| First login with no active session | Continue create-user path and append USER_CREATED then LOGIN |
