# Decision: Login Input Validation

**Date**: 2026-06-28 **Scenarios**: 1.1, 1.2

Validation of user-supplied fields must happen at the domain boundary to enforce business rules regardless of the calling interface (HTTP, TCP, console).

| Rejected | Why |
|----------|-----|
| Controller-only validation | Duplicates rules across HTTP, TCP, and console adapters; domain would remain inconsistent |

**Chosen**: Domain-layer validation via `LoginRequest` value object; `ValidationException` mapped to 400 at the HTTP adapter boundary.

## Model

- `domain::LoginRequest` — value object; constructor validates `username` and `pin` are non-empty; throws `ValidationException` if either is blank
- `domain::ValidationException` — extends base `DomainException` (unchecked)
- `usecase::LoginUsecase::execute(LoginRequest)` — entry point; receives already-validated request
- `adapters::http::LoginController` — parses JSON → constructs `LoginRequest` → calls usecase
- `adapters::http::ExceptionHandler` — maps `ValidationException` → HTTP 400 `{"error": "Validation error"}`

## Edge Cases

| Case | Behavior |
|------|----------|
| Whitespace-only username | Treated as non-empty (whitespace validation is an extended test, not core) |
| Username or PIN containing special characters | Allowed — no character restriction beyond non-empty |
