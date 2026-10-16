# Story 5: Transfer Funds — Interview

## Scope

IN scope:
- Transfer funds from the authenticated user account to another existing user account
- Validate active session before processing a transfer
- Validate transfer amount and target user identity before any ledger mutation
- Record transfer as immutable ledger events compatible with balance and history features
- Keep behavior consistent across console, HTTP REST, and TCP interfaces

OUT of scope:
- Editing or deleting transfer history entries
- Cross-bank or external payment rails
- Bulk/batch transfers
- Scheduled transfers or recurring payments
- Session model changes (reuse Story 1 auth/session semantics)

## Key Architectural Decisions

DECISION: Transfer is an authenticated, ledger-backed operation.
The usecase must validate session first, then apply transfer rules, then append immutable events.

DECISION: Source and destination account balances remain derived from ledger entries.
No mutable "balance" field is introduced.

DECISION: Transfer should be represented as paired, traceable ledger effects.
The debit on source and credit on destination must be linked by the same transfer identity.

DECISION: Transport adapters remain thin.
Business rules (amount validation, self-transfer checks, user existence, insufficient funds) stay in domain/usecase.

## Business Rules & Constraints

- Caller must have a valid active session.
- Amount must be positive and within the same numeric constraints used by deposit/withdraw flows.
- Source and destination users must be different.
- Destination user must exist.
- Source user must have sufficient funds.
- Successful transfer must atomically produce a debit for source and a credit for destination in persistent storage semantics.
- Failed validation must not mutate ledger state.
- Transfer events must remain compatible with Story 6 transaction history ordering and rendering rules.

## Not Yet Implemented (Gaps)

- Transfer request/response domain contracts
- Transfer usecase orchestration and domain validation rules
- Storage port support for dual-account transfer mutation/query needs
- HTTP transfer endpoint and mappings
- TCP transfer command handling
- Console transfer command handling
- Acceptance scenarios and adapter-level tests specific to transfer errors and success paths

## Cross-Story Dependencies

- Depends on Story 1 for session/token validation semantics.
- Depends on Stories 2/3/4 for consistent balance derivation and ledger behavior.
- Must preserve compatibility with Story 6 history semantics for transfer-related entries.

## Testing Considerations

- Success scenario: authenticated user transfers valid amount to another existing user.
- Auth failure scenario: missing/invalid token is rejected with no ledger mutation.
- Domain failure scenarios: invalid amount, self-transfer, missing destination user, insufficient funds.
- Consistency scenario: source balance decreases and destination balance increases by exactly the same amount.
- History scenario: resulting entries are visible in transaction history in expected order.
