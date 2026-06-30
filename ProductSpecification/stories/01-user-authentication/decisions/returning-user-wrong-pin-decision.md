# Decision: Returning user wrong PIN handling

**Date**: 2026-06-30 **Scenarios**: 3.2

Returning-user authentication must reject credential mismatches before any session side effects.

| Rejected | Why |
|----------|-----|
| Validate PIN in HTTP controller | Duplicates auth rule at adapter boundary and breaks cross-interface consistency |
| Open session first, validate later | Creates invalid session side effects for failed authentication |

**Chosen**: Validate input PIN against stored user PIN in `LoginUsecase`; on mismatch throw `InvalidCredentialsException` and stop before session creation or login-event append.

## Model

- `usecase::LoginUsecase::execute(LoginRequest)` enforces PIN match for existing users before calling `ISessionPort::open_session_for`
- `usecase::LoginUsecase` throws `domain::InvalidCredentialsException` on mismatch
- `adapters::http::ExceptionHandler` maps `InvalidCredentialsException` to HTTP 401 with `error = "Invalid credentials"`
- `IAuthEventLogPort::append(LOGIN, user)` is called only on successful authentication

## Edge Cases

| Case | Behavior |
|------|----------|
| Existing user logs in with wrong PIN | Return 401 invalid credentials; no session token; no LOGIN event |
| Existing user logs in with correct PIN | Continue normal path: open session and append LOGIN event |
