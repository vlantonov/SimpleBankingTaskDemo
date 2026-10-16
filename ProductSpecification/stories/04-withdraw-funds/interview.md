# Story 4: Withdraw Funds — Interview

## Scope

IN scope:
- `withdraw` / `remove funds` for an authenticated user
- Decrease the caller’s current balance by a valid positive amount
- Require that the requested withdrawal does not exceed the available balance
- Record the financial event as an immutable ledger entry
- Available through the same interfaces as the rest of the banking demo: console, HTTP REST, and TCP socket
- Require a valid active session token before funds can be withdrawn

OUT of scope:
- Deposits, transfers, or account creation flows
- Editing or deleting prior ledger entries
- Multi-user administrative withdrawals
- Session persistence across restarts
- Formatting or presentation of account statements beyond the withdrawal result

## Key Architectural Decisions

DECISION: Withdrawals are only allowed for authenticated users with a valid active session.
No anonymous withdrawal is allowed.

DECISION: A successful withdrawal appends a financial event to the transaction ledger and updates the user’s balance deterministically from the ledger state.
The balance is derived, not a separate editable field.

DECISION: All withdrawals are immutable and append-only.
The system never rewrites historical financial records to balance an account.

DECISION: The operation behaves identically over console, HTTP, and TCP interfaces.
Transport details change, but business rules do not.

## Business Rules & Constraints

- A user must already be logged in with a valid session before withdrawing funds.
- A withdrawal amount must be a positive value and within the system’s allowed bounds.
- A successful withdrawal decreases the current account balance by the submitted amount.
- The transaction must be recorded as an append-only ledger entry, including the user, amount, and timestamp.
- The operation must reject invalid or missing authentication before applying any change.
- The system must reject withdrawals that would drive the balance below zero.
- Withdrawals must not mutate or delete prior ledger entries.

## Not Yet Implemented (Gaps)

- Withdraw domain model / value object
- Ledger append and balance update logic
- Insufficient funds validation
- Session validation for protected operations
- HTTP withdraw endpoint handler
- TCP socket withdraw handler
- Console command parser for withdraw requests

## Cross-Story Dependencies

- Story 1 (User Authentication) must be complete before any withdrawal can be authorized.
- Story 2 (Get Balance) relies on the same account and session semantics and should remain consistent after withdrawals.
- Story 3 (Deposit Funds) uses the same ledger semantics and account balance model.
- Story 5 (Transfer) and Story 6 (Transaction History) depend on the same immutable ledger used by withdrawals.

## Testing Considerations

- Fakes needed: authenticated session validation, in-memory account state, transaction ledger, and amount validation
- Key scenarios: successful withdrawal for logged-in user, rejected withdrawal for unauthenticated user, rejected withdrawal that exceeds balance, ledger append after a valid withdrawal, repeated withdrawals reduce the balance deterministically
- Important validation: account balance changes must reflect the ledger exactly and remain consistent across reads
