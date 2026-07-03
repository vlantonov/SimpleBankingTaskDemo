# Decision: Return 401 for Missing Logout Token at HTTP Boundary

**Date**: 2026-07-04 **Scenarios**: 5.2

Missing Authorization header during logout must produce authentication semantics (401), not request-shape validation semantics (400).

| Rejected | Why |
|----------|-----|
| Make LogoutRequest/usecase accept optional token and branch on empty | Pushes HTTP header absence concern into usecase/domain layers |
| Map ValidationException to 401 in global HTTP exception handler | Broad side effect: changes meaning of unrelated validation failures |

**Chosen**: Detect missing/invalid Bearer token in LogoutController and raise InvalidCredentialsException before constructing LogoutRequest.

## Model

- `LogoutController::extract_authorization_token(...)`: keep token extraction at HTTP adapter boundary.
- `LogoutController::handle_logout(...)`: convert missing/invalid bearer token path to authentication failure (`InvalidCredentialsException`).
- `domain::LogoutRequest`: keep non-empty token validation unchanged for non-HTTP callers.
- `usecase::LogoutUsecase`: unchanged; still resolves username by token and rejects unknown token with `InvalidCredentialsException`.

## Edge Cases

| Case | Behavior |
|------|----------|
| No `Authorization` header | Return HTTP 401 with `{"error":"Authentication error","token":"","message":""}` |
| `Authorization` present but not `Bearer <token>` | Return HTTP 401 with authentication error body |
| `Authorization: Bearer ` (empty token) | Return HTTP 401 with authentication error body |
| Non-empty but unknown token | Return HTTP 401 via existing usecase invalid-credentials path |
