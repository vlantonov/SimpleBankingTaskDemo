# Story 1: User Authentication — Interview

## Scope

IN scope:
- `login [user] [pin]` — creates a new user session; prints "Welcome, {user}" on success
- `logout` — disposes the current session
- Auto-create user on first login (no separate account-creation command)
- Available via console, HTTP REST, and TCP socket interfaces

OUT of scope:
- PIN change
- Admin-managed account creation
- Session expiry / timeout
- Showing a list of active sessions

## Key Architectural Decisions

DECISION: First login with a new username auto-creates that user with the given PIN.  
No separate registration step. If the username already exists, the PIN must match.

DECISION: Wrong PIN → reject with an error message. The user stays logged out.

DECISION: Single session per user — a second `login` for the same user while a session is active is rejected with an error.

DECISION: Sessions are in-memory only. A server restart invalidates all active sessions. No session persistence across restarts.

DECISION: HTTP interface returns a session token in the response body on successful login.  
Subsequent authenticated requests must carry this token (header or query param — to be decided in api-spec).

DECISION: All auth events are written to the append-only persistent log:
- USER_CREATED (on first login)
- LOGIN (on each successful login)
- LOGOUT (on each logout)

## Business Rules & Constraints

- A user that has never logged in before is created automatically on first `login`.
- PIN is matched exactly (case-sensitive string comparison is fine for this demo).
- At most one active session per user at any time — attempting a second `login` while already logged in is rejected.
- `logout` without an active session → error message (not a no-op).
- All storage writes are append-only: no modification of existing records.

## NOT Yet Implemented (Gaps)

- User entity / domain model
- Session entity / domain model
- Append-only file storage adapter
- HTTP controller for login/logout
- TCP socket handler for login/logout
- Console command parser for login/logout

## Cross-Story Dependencies

- Stories 2–6 (Get Balance, Deposit, Withdraw, Transfer, History) all require a valid authenticated session — they depend on Story 1 being complete first.

## Testing Considerations

- Fakes needed: in-memory user store, in-memory session store, fake append-only log
- Key test scenarios: successful first login (auto-create), successful re-login, wrong PIN rejection, second login rejection, successful logout, logout without session
