# Withdraw Funds

## Brief Description
Users with an active session can withdraw funds from their account. A successful withdrawal appends a ledger event and reduces the user’s current balance while ensuring the account never drops below zero.

## Flow
1. User sends `withdraw [amount]` while authenticated.
2. System validates that the session token is active and belongs to the caller.
3. System validates the withdrawal amount is positive and within allowable limits.
4. System checks that the requested amount is less than or equal to the available account balance.
5. System appends a withdrawal ledger entry for the authenticated user.
6. System re-computes the current balance from the account ledger.
7. Response confirms the successful withdrawal and current balance.
8. If the user is not authenticated, the amount is invalid, or the balance is insufficient, the request is rejected.

## Acceptance Criteria
- Authenticated user can withdraw a valid positive amount successfully.
- Unauthenticated user is rejected with an authentication error.
- Zero or negative withdrawal amounts are rejected.
- Withdrawal that exceeds the available balance is rejected.
- A valid withdrawal appends a ledger record and decreases the account total.
- Withdrawal does not modify previous ledger entries or mutate historical transaction records.
- Behaviour is consistent across console, HTTP, and TCP interfaces.

## Validation Rules
| Field | Rule |
|-------|------|
| session | Must be valid and active for the current user |
| amount | Must be a positive numeric amount within allowed bounds |
| balance | Must be sufficient for the withdrawal amount |
| ledger | New withdrawal is appended as an immutable event |

## Core Requirements
- Story 1 login/session flow must be complete before withdrawal is available.
- Withdrawals are append-only and recorded in the transaction ledger.
- Balance updates are derived from the ledger, not stored as a separate mutable field.
- Authorization is enforced before any funds are removed.
- Insufficient funds must be rejected before any balance mutation occurs.
- All accounting changes are side-effecting but history remains immutable.
