# Story 4: Withdraw Funds — Progress

## Spec
- [x] interview
- [x] story
- [~] api-spec
- [ ] test-spec

## Backend
- [ ] red-acceptance: withdraw funds for an authenticated user decreases the balance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery (http)
  - Check 1 (ports): http — `WithdrawUsecase` will need a session validation port and an account/ledger port, so confirm the route and adapter wiring before implementing the HTTP handler.
  - Check 2 (exceptions): http — invalid session and insufficient funds errors must map to the required 401/400 responses, so add the adapter mapping when the usecase is in place.
  - Check 3 (response shape): http — the withdraw endpoint must return the updated balance in the JSON response body, so verify the contract before the adapter implementation.
- [ ] red-adapter http
- [ ] green-adapter http
- [ ] green-acceptance
