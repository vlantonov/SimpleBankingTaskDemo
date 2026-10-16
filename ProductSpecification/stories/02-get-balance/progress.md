# Story 2: Get Balance — Progress

## Spec
- [x] interview
- [x] story
- [x] api-spec
- [x] test-spec

## Backend
- [x] red-acceptance: get balance for an authenticated user returns the current balance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery (http)
  - Check 1 (ports): http — `GetBalanceUsecase` depends on `IGetBalanceSessionPort` and `IAccountPort`, but the server has no balance controller or account adapter wired to the active session/account state yet, so add `red-adapter http` / `green-adapter http`.
  - Check 2 (exceptions): http — `InvalidCredentialsException` is thrown by the balance usecase, but no inbound balance HTTP handler currently maps it to the required 401 response, so add `red-adapter http` / `green-adapter http`.
  - Check 3 (response shape): http — `/api/v1/balance` does not yet return the expected JSON body with the authenticated user's balance, so add `red-adapter http` / `green-adapter http`.
- [x] red-adapter http
- [x] green-adapter http
- [x] green-acceptance
