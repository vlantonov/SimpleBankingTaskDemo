# Story 3: Deposit Funds — Progress

## Spec
- [x] interview
- [x] story
- [x] api-spec
- [x] test-spec

## Backend
- [x] red-acceptance: deposit funds for an authenticated user increases the balance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery (http)
  - Check 1 (ports): http — `DepositUsecase` will depend on a session validation port and an account/ledger port, so confirm the route and adapter wiring before implementing the HTTP handler.
  - Check 2 (exceptions): http — invalid session and invalid amount exceptions must be mapped to the required 401/400 responses, so add the adapter mapping when the usecase is in place.
  - Check 3 (response shape): http — the deposit endpoint must return the updated balance in the JSON response body, so verify the contract before the adapter implementation.
- [x] red-adapter http
- [x] green-adapter http
- [x] green-acceptance
