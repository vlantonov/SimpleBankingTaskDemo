# Story 2: Get Balance — Interview

## Scope

IN scope:
- `balance` / `get balance` for an authenticated user
- Return the current account balance for the logged-in user
- Available through the same interfaces as the rest of the banking demo: console, HTTP REST, and TCP socket
- Read-only access that requires a valid active session token

OUT of scope:
- Deposits, withdrawals, transfers, or account creation flows
- Transaction edits or deletion
- Balance history/statement formatting beyond the current amount
- Multi-user or admin balance lookup
- Session persistence across server restarts

## Key Architectural Decisions

DECISION: The balance command is available only to an authenticated user with a valid session.
No anonymous balance lookup is allowed.

DECISION: The balance returned is the current account total for the authenticated user.
It is derived from the user’s transaction ledger and not manually stored as a mutable field.

DECISION: Balance access is read-only; the system never mutates or deletes past financial records to answer a balance request.

DECISION: The operation behaves identically over console, HTTP, and TCP interfaces.
The transport changes, but the business rule does not.

## Business Rules & Constraints

- A user must already be logged in with a valid session before requesting balance.
- If no valid session exists, the request must be rejected with an authentication error.
- The response must reflect the user’s current balance at the time of the request.
- Balance reads must not create side effects, write new records, or mutate the ledger.
- All financial events remain append-only; balance is derived from the ledger rather than a separate editable balance field.

## Not Yet Implemented (Gaps)

- Account balance domain model / value object
- Ledger or balance calculation logic
- Session validation for protected endpoints
- HTTP balance endpoint handler
- TCP socket balance handler
- Console command parser for balance requests

## Cross-Story Dependencies

- Story 1 (User Authentication) must be complete before any balance request can be authorized.
- Story 3 (Deposit), Story 4 (Withdraw), and Story 5 (Transfer) will all change the same underlying balance state.
- Story 6 (Transaction History) depends on the same transaction ledger and should be consistent with the same account semantics.

## Testing Considerations

- Fakes needed: authenticated session validation, in-memory user/account state, transaction ledger or account balance calculator
- Key scenarios: successful balance read for logged-in user, rejected balance read for unauthenticated user, balance after a deposit/withdrawal, balance remains consistent across repeated reads
- Important validation: the balance response must reflect the current aggregate and not a stale cached value
