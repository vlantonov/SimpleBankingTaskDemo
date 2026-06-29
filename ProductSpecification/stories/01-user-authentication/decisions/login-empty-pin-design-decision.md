# Decision: Empty PIN Validation Reuse

**Date**: 2026-06-29 **Scenarios**: 1.2

Scenario 1.2 needed a design checkpoint, but the validation path is already established and implemented.

| Rejected | Why |
|----------|-----|
| Re-open architecture for 1.2 | No new trade-off relative to the existing login validation decision |

**Chosen**: Reuse the existing domain validation path (`LoginRequest` throws `ValidationException` for empty `pin`), keep `LoginUsecase` unchanged, and keep HTTP mapping through `ExceptionHandler` to 400.

## Model

- `domain::LoginRequest` remains the validation boundary for `username` and `pin`
- `usecase::LoginUsecase::execute(const LoginRequest&)` remains unchanged
- `adapters::http::LoginController` continues to build `LoginRequest` from parsed JSON
- `adapters::http::ExceptionHandler` continues mapping `ValidationException` to HTTP 400

## Edge Cases

| Case | Behavior |
|------|----------|
| Missing `pin` field in login JSON | Controller default `""` reaches `LoginRequest`, which throws `ValidationException` |
| Explicit empty `pin` string | `LoginRequest` throws `ValidationException` |
