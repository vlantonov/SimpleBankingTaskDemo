# Decision: Withdraw Validation Before Mutation

**Date**: 2026-10-16 **Scenarios**: 2.1, 3.1, 3.2, 4.1

Withdrawal rules must be enforced in the domain layer before any ledger entry is appended. The same validation logic must be used regardless of calling interface (HTTP, TCP, or console).

| Rejected | Why |
|----------|-----|
| Controller-only amount checks | The rule is business-critical and must remain consistent across adapters |
| Allowing negative or zero withdrawals | This violates the domain contract and invalidates accounting consistency |
| Allowing a withdrawal over the available balance | It creates an invalid ledger state and leaks accounting logic into transport adapters |

**Chosen**: Domain-layer validation via `WithdrawRequest` and a `WithdrawUsecase` that ensures the amount is positive and the balance is sufficient before writing the ledger event.

## Model

- `domain::WithdrawRequest` — value object; constructor validates `amount > 0` and throws `ValidationException` for non-positive values
- `domain::Account` / ledger state — computes the current balance from immutable ledger entries
- `usecase::WithdrawUsecase::execute(WithdrawRequest)` — verifies session validity, checks available balance, appends a withdrawal ledger event, returns the updated balance
- `adapters::http::WithdrawController` — parses JSON → constructs `WithdrawRequest` → calls the usecase
- `adapters::http::ExceptionHandler` — maps `ValidationException` and insufficient funds failures to 400 responses, `InvalidCredentialsException` to 401

## Edge Cases

| Case | Behavior |
|------|----------|
| Zero amount | Rejected with validation error |
| Negative amount | Rejected with validation error |
| Amount greater than available balance | Rejected without mutating the account state |
| Unauthenticated caller | Rejected before any withdrawal logic executes |
