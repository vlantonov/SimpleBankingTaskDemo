# Story 3: Deposit Funds — Interview

## Scope

IN scope:
- `deposit` / `add funds` for an authenticated user
- Increase the caller’s current balance by a valid positive amount
- Record the financial event as an immutable ledger entry
- Available through the same interfaces as the rest of the banking demo: console, HTTP REST, and TCP socket
- Require a valid active session token before funds can be added

OUT of scope:
- Withdrawals, transfers, or account creation flows
- Editing or deleting prior ledger entries
- Multi-user administrative deposits
- Session persistence across restarts
- Formatting or presentation of account statements beyond the deposit result

## Key Architectural Decisions

DECISION: Deposits are only allowed for authenticated users with a valid active session.
No anonymous deposit is allowed.

DECISION: A successful deposit appends a financial event to the transaction ledger and updates the user’s balance deterministically from the ledger state.
The balance is derived, not a separate editable field.

DECISION: All deposits are immutable and append-only.
The system never rewrites historical financial records to balance an account.

DECISION: The operation behaves identically over console, HTTP, and TCP interfaces.
Transport details change, but business rules do not.

## Business Rules & Constraints

- A user must already be logged in with a valid session before depositing funds.
- A deposit amount must be a positive value and within the system’s allowed bounds.
- A successful deposit increases the current account balance by the submitted amount.
- The transaction must be recorded as an append-only ledger entry, including the user, amount, and timestamp.
- The operation must reject invalid or missing authentication before applying any change.
- Deposits must not mutate or delete prior ledger entries.

## Not Yet Implemented (Gaps)

- Deposit domain model / value object
- Ledger append and balance update logic
- Session validation for protected operations
- HTTP deposit endpoint handler
- TCP socket deposit handler
- Console command parser for deposit requests

## Cross-Story Dependencies

- Story 1 (User Authentication) must be complete before any deposit can be authorized.
- Story 2 (Get Balance) relies on the same account and session semantics and should remain consistent after deposits.
- Story 4 (Withdraw) and Story 5 (Transfer) will operate on the same account state and ledger semantics.
- Story 6 (Transaction History) depends on the same immutable ledger used by deposits.

## Testing Considerations

- Fakes needed: authenticated session validation, in-memory account state, transaction ledger, and amount validation
- Key scenarios: successful deposit for logged-in user, rejected deposit for unauthenticated user, rejected negative or zero amount, ledger append after a valid deposit, repeated deposits accumulate correctly
- Important validation: account balance changes must reflect the ledger exactly and remain consistent across reads
