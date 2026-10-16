# Deposit Funds

## Brief Description
Users with an active session can deposit funds into their account. A successful deposit appends a ledger event and increases the user’s current balance.

## Flow
1. User sends `deposit [amount]` while authenticated.
2. System validates that the session token is active and belongs to the caller.
3. System validates the deposit amount is positive and within allowable limits.
4. System appends a deposit ledger entry for the authenticated user.
5. System re-computes the current balance from the account ledger.
6. Response confirms the successful deposit and current balance.
7. If the user is not authenticated, or the amount is invalid, the request is rejected.

## Acceptance Criteria
- Authenticated user can deposit a valid positive amount successfully.
- Unauthenticated user is rejected with an authentication error.
- Zero or negative deposit amounts are rejected.
- A valid deposit appends a ledger record and increases the account total.
- Deposit does not modify previous ledger entries or mutate historical transaction records.
- Behaviour is consistent across console, HTTP, and TCP interfaces.

## Validation Rules
| Field | Rule |
|-------|------|
| session | Must be valid and active for the current user |
| amount | Must be a positive numeric amount within allowed bounds |
| ledger | New deposit is appended as an immutable event |

## Core Requirements
- Story 1 login/session flow must be complete before deposit is available.
- Deposits are append-only and recorded in the transaction ledger.
- Balance updates are derived from the ledger, not stored as a separate mutable field.
- Authorization is enforced before any funds are added.
- All accounting changes are side-effecting but history remains immutable.
