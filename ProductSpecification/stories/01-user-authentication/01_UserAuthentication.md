# User Authentication

## Brief Description
Users authenticate with a username and PIN. First-time login auto-creates the account. A session token is returned for subsequent authenticated requests.

## Flow
1. User sends `login [user] [pin]`
2. If user does not exist → create user with the given PIN, write USER_CREATED to the append-only log
3. If user exists and PIN matches → create session, write LOGIN to the log, return "Welcome, {user}" + session token
4. If user exists and PIN does not match → reject with error message, no session created
5. If user already has an active session → reject with error message (single session per user)
6. User sends `logout`
7. If active session exists → destroy session, write LOGOUT to the log
8. If no active session → return error message

## Acceptance Criteria
- `login ola 123` (new user) → "Welcome, ola" + session token; USER_CREATED and LOGIN written to log
- `login ola 123` (returning user, correct PIN) → "Welcome, ola" + session token; LOGIN written to log
- `login ola 999` (wrong PIN for existing user) → error message, no session
- `login ola 123` while ola already has a session → rejected with error
- `logout` with active session → session destroyed, LOGOUT written to log
- `logout` with no active session → error message
- All storage writes are append-only (no modifications to existing records)
- Behaviour is identical across console, HTTP, and TCP interfaces

## Validation Rules
| Field  | Rule                                       |
|--------|--------------------------------------------|
| user   | Non-empty string, no whitespace            |
| pin    | Non-empty string, matched exactly (case-sensitive) |

## Core Requirements
- Sessions are in-memory only; server restart invalidates all sessions
- Append-only log records: USER_CREATED, LOGIN, LOGOUT events
- HTTP interface returns a session token in the response body on success
- All other stories require a valid session — login must be implemented first
