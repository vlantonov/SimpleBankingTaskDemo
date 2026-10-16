# Get Balance - Notes & Considerations

## Warnings

### Functional Warnings
- Balance requests are only valid for a logged-in user; missing or invalid sessions must fail before any balance is exposed.
- Balance must reflect the latest account state, not a stale cached copy that can drift from ledger state.

### Technical Warnings
- If the balance is derived from a ledger, the implementation must avoid storing a second mutable balance field that can become inconsistent.
- Account lookup must remain tied to the authenticated session and never allow cross-user access.

---

## Suggestions & Future Enhancements

### Functional Suggestions
- Add clear formatting for currency display in the UI layer.
- Consider explicit zero-account and negative-balance handling once account semantics are formalized.

### Technical Suggestions
- Use a single source of truth for account totals to reduce drift risk.
- Keep account reads isolated from write flows to simplify auditability.

---

## Technical Notes

### Load Considerations
- The expected demo load is low and single-digit concurrent sessions, so the balance read path can stay simple and synchronous.
- The operation should remain fast because it is a small, read-only lookup over the current account state.

### Security Considerations
- Authorization must check the session before reading any account data.
- Balance endpoints must never accept a session that belongs to another user or a fabricated token.

### Infrastructure Notes
- The read path is in-process and should fit the same in-memory session model used by Story 1.
- No external dependency is needed unless later stories introduce shared persistence or distributed session storage.

### Integration Notes
- See `interview.md` for the story constraints and the authenticated-session model.
- The HTTP and TCP integrations should share the same validation and business logic to keep behavior consistent.

---

## Additional Context

- Story 2 depends on the session, account, and ledger model established by Story 1.
- The balance operation is read-only and must remain consistent with the deposit, withdraw, and transfer stories that follow.
