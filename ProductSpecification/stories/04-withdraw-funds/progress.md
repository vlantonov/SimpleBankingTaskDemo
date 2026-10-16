# Story 4: Withdraw Funds — Progress

## Spec
- [x] interview
- [x] story
- [x] api-spec
- [x] test-spec

## Backend
- [x] red-acceptance: withdraw funds for an authenticated user decreases the balance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery (http)
  - Check 1 (ports): http — the server has no withdraw route or withdraw-specific session/account adapter implementations, so the missing wiring requires `red-adapter http` and `green-adapter http` work.
  - Check 2 (exceptions): http — [S] `ExceptionHandler` already maps `InvalidCredentialsException` to HTTP 401 and `ValidationException` to HTTP 400, so no new exception-mapping adapter work is required.
  - Check 3 (response shape): http — the HTTP boundary must return the updated balance in the JSON response, but the server never registers a withdraw endpoint, so the missing response body and route require adapter steps.
- [~] red-adapter http
- [ ] green-adapter http
- [ ] green-acceptance
