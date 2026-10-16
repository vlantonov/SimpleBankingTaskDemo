# Transfer Funds

## Brief Description
Authenticated users can transfer funds from their account to another existing user. A successful transfer records immutable ledger effects for both sides and keeps balances derived from ledger data.

## Flow
1. User sends transfer request while authenticated.
2. System validates active session token for caller.
3. System validates amount is positive and allowed.
4. System validates destination user exists and is not the same as source user.
5. System validates source user has sufficient funds.
6. System appends transfer debit for source and transfer credit for destination as linked immutable events.
7. System returns success response with updated balance state.
8. If any validation fails, request is rejected and no ledger mutation occurs.

## Acceptance Criteria
- Authenticated user can transfer a valid amount to another existing user.
- Unauthenticated user is rejected with an authentication error.
- Transfer to the same user is rejected.
- Transfer to a non-existing destination user is rejected.
- Transfer amount must be positive; invalid amounts are rejected.
- Transfer that exceeds source balance is rejected.
- Successful transfer updates source and destination balances by exactly the same amount in opposite directions.
- Transfer history remains immutable and compatible with transaction-history queries.

## Validation Rules
| Field | Rule |
|-------|------|
| session | Must be valid and active for caller |
| amount | Must be positive and within allowed numeric bounds |
| destinationUser | Must exist and be different from source user |
| sourceBalance | Must be sufficient before transfer is recorded |
| ledger | Must append linked debit and credit events atomically |

## Core Requirements
- Story 1 session/auth rules are enforced before any transfer logic.
- Balance values remain derived from immutable ledger entries.
- Transfer business rules live in domain/usecase; adapters stay thin.
- Failed validations do not mutate ledger state.
- Behavior remains consistent across console, HTTP, and TCP interfaces.
