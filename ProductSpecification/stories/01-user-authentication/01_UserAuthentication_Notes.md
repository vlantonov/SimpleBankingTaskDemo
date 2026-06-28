# User Authentication - Notes & Considerations

## Warnings

### Functional Warnings
- **Logout without session**: Must return a clear error, not silently succeed — the console/HTTP/TCP caller needs to know.
- **Concurrent login race**: Two simultaneous `login` requests for the same new user could both attempt to create the account. The storage layer must serialize writes; the append-only log should treat the first write as authoritative and subsequent logins as duplicate-user errors or successful re-logins after checking the persisted PIN.
- **Session state on restart**: Sessions are in-memory only. After a restart, users will receive auth errors until they log in again. Clients must handle this gracefully.

### Technical Warnings
- **PIN stored in plaintext**: For a demo this is acceptable; in production the PIN would be hashed (bcrypt/argon2). Do not treat the current design as production-ready.
- **Session token entropy**: The token must be cryptographically random (not sequential or user-derived) to prevent guessing attacks.
- **Append-only log integrity**: The log file must be opened in append mode only. Any code path that truncates or overwrites the file violates the core requirement.

---

## Suggestions & Future Enhancements

### Functional Suggestions
- PIN change command (`change-pin [old-pin] [new-pin]`)
- Session expiry / timeout (auto-logout after N minutes of inactivity)
- Multi-session support with per-device tokens

### Technical Suggestions
- Hash PINs with bcrypt before persisting the USER_CREATED record
- Use a UUID or secure random hex string as the session token
- Add a SESSION_EXPIRED event type to the log for future audit purposes

---

## Technical Notes

### Load Considerations
- Low concurrent load (single-digit sessions). In-memory session map is sufficient.
- No pagination or bulk operations — login/logout are single-row operations.

### Security Considerations
- **OWASP A07 (Identification and Authentication Failures)**: Enforce single session per user; reject re-login without prior logout.
- **OWASP A02 (Cryptographic Failures)**: PIN is stored as-is for demo; flag for hashing in any production hardening pass.
- **Brute-force PIN guessing**: No rate limiting in scope; document as a known gap.

### Infrastructure Notes
- Session map lives in the application process. A load-balanced deployment with multiple instances would need a shared session store (Redis, etc.) — out of scope for this demo but worth noting.
- The append-only log file path must be configurable via environment variable or config file; do not hardcode.

### Integration Notes
- HTTP clients must include the session token in all subsequent requests (header TBD — likely `Authorization: Bearer <token>` or `X-Session-Token: <token>`; exact scheme resolved in api-spec).
- TCP clients must carry the token as part of each command message (exact framing resolved in api-spec).
- Console interface can hold the token in memory for the lifetime of the process.

---

## Additional Context
- See `interview.md` for full architectural decisions and scope boundaries.
- No external API integrations for this story — all logic is internal.
