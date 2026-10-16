# Story 5: Transfer Funds — Progress

## Spec
- [x] interview
- [x] story
- [x] api-spec
- [x] test-spec

## Backend Scenarios (01_API_Tests.md)

### Scenario 1: Authenticated user can transfer funds to another existing user
- [x] red-acceptance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] red-usecase (coverage: transfer rejects missing active session)
- [x] green-usecase (coverage: transfer rejects missing active session)
- [x] red-usecase (coverage: transfer rejects unknown destination account)
- [x] green-usecase (coverage: transfer rejects unknown destination account)
- [x] adapters-discovery (http)
	- Check 1 (ports): http — `TransferUsecase` depends on `ITransferSessionPort` and `ITransferAccountPort`, but the server wiring has no transfer-specific session/account adapters yet, so `red-adapter http` and `green-adapter http` are required.
	- Check 2 (exceptions): http — [S] `ExceptionHandler` already maps `InvalidCredentialsException` to 401 and `ValidationException` to 400, which covers the current transfer usecase rejection paths.
	- Check 3 (response shape): http — transfer acceptance expects a transfer endpoint returning updated source/destination balances, but no `/api/v1/transfer` route/controller response exists yet, so adapter steps are required.
- [x] red-adapter http
- [x] green-adapter http
- [x] green-acceptance
