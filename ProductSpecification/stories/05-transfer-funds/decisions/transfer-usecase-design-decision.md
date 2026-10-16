# Decision: Transfer usecase with explicit domain checks and linked ledger append

**Date**: 2026-10-16 **Scenarios**: 3.1

Transfer needs source/destination validation and symmetric ledger effects while keeping business rules out of adapters.

| Rejected | Why |
|----------|-----|
| Two-step read/write flow (B) | Separate write steps risk partial mutation and unclear rollback behavior. |
| Adapter-centric transfer logic (C) | Pushes business rules into adapter layer and weakens clean architecture boundaries. |

**Chosen**: New TransferUsecase with session port + transfer-account port; usecase performs validation and computes resulting balances, then calls one linked transfer-append operation.

## Model

- New domain request: `TransferRequest(token, destination_user, amount)` with amount validation.
- New usecase: `TransferUsecase.execute(request) -> TransferResponse(source_balance, destination_balance)`.
- New port: `ITransferSessionPort.find_username_by_token(token)`.
- New port: `ITransferAccountPort` with:
  - `find_balance_for_username(username)`
  - `destination_exists(username)`
  - `append_transfer_between(source, destination, amount)`
- New response object: source and destination post-transfer balances.

## Edge Cases

| Case | Behavior |
|------|----------|
| Missing/invalid session token | Reject with authentication error. |
| Destination user not found | Reject with destination-not-found error. |
| Source equals destination | Reject with validation error. |
| Non-positive amount | Reject with validation error. |
| Amount exceeds source balance | Reject with insufficient-funds validation error. |
| Any validation failure | No ledger mutation occurs. |
