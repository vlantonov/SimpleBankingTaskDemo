# Get Balance

## Brief Description
Users with an active session can request their current account balance. The response is the account total for the authenticated user and is read-only.

## Flow
1. User sends `balance` or `get balance` while authenticated.
2. System validates that the session token is active and belongs to the caller.
3. System reads the current account balance for that user.
4. Response returns the balance value without altering account history.
5. If the user is not authenticated, the request is rejected.

## Acceptance Criteria
- Authenticated user can retrieve current balance successfully.
- Unauthenticated user is rejected with an authentication error.
- Returned balance matches the user's current account total.
- Balance lookup does not create new ledger entries or mutate account state.
- Behaviour is consistent across console, HTTP, and TCP interfaces.

## Validation Rules
| Field | Rule |
|-------|------|
| session | Must be valid and active for the current user |
| balance | Must be numeric and reflect current account state |
| auth state | No anonymous balance lookup allowed |

## Core Requirements
- Story 1 login/session flow must be complete before balance is available.
- Balance is derived from the underlying account ledger, not stored as a separate mutable field.
- Authorization is enforced before any balance data is returned.
- Response is read-only and side-effect free.
