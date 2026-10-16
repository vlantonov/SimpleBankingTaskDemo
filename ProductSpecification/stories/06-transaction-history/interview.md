# Story 6: Transaction History — Interview

## Scope

IN scope:
- Retrieve the authenticated user’s transaction history from the append-only ledger
- Present a list of all money movements tied to the current account
- Show enough detail to explain what happened, when it happened, and the resulting amount
- Work consistently across the same interfaces as the rest of the banking demo: console, HTTP REST, and TCP socket
- Require a valid active session before exposing history

OUT of scope:
- Editing or deleting historical entries
- Administrative views for all users at once
- Transaction export, CSV reporting, or statement formatting beyond the basic ledger list
- Server-side retention policies or archiving beyond the demo runtime
- Cross-account or business reporting dashboards

## Key Architectural Decisions

DECISION: Transaction history is derived from the immutable ledger, not from a second editable balance table.
The current balance remains a computed result of prior ledger entries, while history is the source of truth.

DECISION: History is only accessible to the authenticated user who owns the account.
No user can query another account’s ledger.

DECISION: The ledger is append-only.
Every successful deposit, withdrawal, and transfer is recorded as a new immutable event.

DECISION: The transport layer is irrelevant to the business rule.
Console, HTTP, and TCP call the same use case and must produce the same transaction list semantics.

## Business Rules & Constraints

- A user must already be logged in with a valid session before requesting transaction history.
- The system must return only the caller’s own transaction history.
- Each ledger entry must include enough metadata to identify the event: transaction type, amount, timestamp, and account/user context.
- The default history order should be newest-first so the most recent activity appears first.
- Empty history is valid and should return an empty list without error.
- A request for history must not mutate account state or alter previous ledger entries.
- The operation must reject unauthenticated or invalid session requests before returning account data.

## Not Yet Implemented (Gaps)

- Transaction ledger domain model and event type definitions
- Query logic to fetch a user’s ledger entries
- Session validation for protected history operations
- HTTP transaction-history endpoint
- TCP transaction-history request/response handling
- Console command support for viewing history

## Cross-Story Dependencies

- Story 1 (User Authentication) must be complete before any history request can be authorized.
- Story 2 (Get Balance) depends on the same account semantics and should remain consistent with the ledger.
- Story 3 (Deposit Funds) and Story 4 (Withdraw Funds) both append ledger entries that must appear in history.
- Story 5 (Transfer Funds) will also write ledger events that deserve the same history treatment.

## Testing Considerations

- Fakes needed: authenticated session lookup, ledger query repository, and empty/non-empty history fixtures
- Key scenarios: authenticated user gets their full transaction history, unauthenticated user gets rejected, no-history user gets an empty list, entries are returned in reverse chronological order, deposit and withdrawal records appear in the same ledger stream
- Important validation: only the current user’s events are returned and the order is deterministic across repeated reads
