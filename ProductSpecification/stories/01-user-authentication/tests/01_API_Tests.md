> **Implementation Order**: Tests are numbered for sequential TDD implementation.
> Start with login validation, then first-login (auto-create), then returning user, then conflict detection, then logout.

# User Authentication — API Tests

---

## 1. Login — Input Validation

### 1.1 Login with empty username is rejected

```gherkin
When the user attempts to log in with an empty username and a PIN
Then the login is rejected with a validation error
```

### 1.2 Login with empty PIN is rejected

```gherkin
When the user attempts to log in with a username and an empty PIN
Then the login is rejected with a validation error
```

---

## 2. Login — First Login (Auto-Create)

### 2.1 First login creates the user and opens a session

```gherkin
Given no account exists for "ola"
When "ola" logs in with PIN "123"
Then the server responds with a welcome message for "ola"
And a session token is returned
```

### 2.2 First login appends USER_CREATED and LOGIN events to the log

```gherkin
Given no account exists for "ola"
When "ola" logs in with PIN "123"
Then a USER_CREATED event for "ola" is recorded in the append-only log
And a LOGIN event for "ola" is recorded in the append-only log
```

---

## 3. Login — Returning User

### 3.1 Returning user with correct PIN receives a welcome message and new token

```gherkin
Given "ola" has an account with PIN "123"
And "ola" has no active session
When "ola" logs in with PIN "123"
Then the server responds with a welcome message for "ola"
And a session token is returned
And a LOGIN event for "ola" is recorded in the append-only log
```

### 3.2 Returning user with wrong PIN is rejected and no session is created

```gherkin
Given "ola" has an account with PIN "123"
When "ola" attempts to log in with PIN "999"
Then the login is rejected with an invalid credentials error
And no session token is returned
```

---

## 4. Login — Conflict (Already Active Session)

### 4.1 Second login while a session is already active is rejected

```gherkin
Given "ola" is logged in with an active session
When "ola" attempts to log in again
Then the login is rejected with a session conflict error
```

---

## 5. Logout

### 5.1 Logout with a valid session token destroys the session

```gherkin
Given "ola" is logged in with an active session
When "ola" logs out using the session token
Then the logout succeeds
And the session token is no longer valid
And a LOGOUT event for "ola" is recorded in the append-only log
```

### 5.2 Logout without a session token is rejected

```gherkin
When an unauthenticated caller attempts to log out
Then the logout is rejected with an authentication error
```

### 5.3 Logout with an invalid session token is rejected

```gherkin
When a caller attempts to log out using an unrecognised token
Then the logout is rejected with an authentication error
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `logs in with PIN "X"` | `POST /api/v1/login` with `{"user": "...", "pin": "X"}` |
| `attempts to log in with ...` | `POST /api/v1/login` expecting an error response |
| `a session token is returned` | Response body contains non-empty `token` field |
| `the session token is no longer valid` | Subsequent `POST /api/v1/logout` with that token returns 401 |
| `logs out using the session token` | `POST /api/v1/logout` with `Authorization: Bearer <token>` |
| `unauthenticated caller attempts to log out` | `POST /api/v1/logout` with no `Authorization` header |
| `unrecognised token` | A token value not matching any in-memory session |
| `welcome message for "ola"` | Response `message` field equals `"Welcome, ola"` |
| `USER_CREATED event ... is recorded` | Append-only log file contains a USER_CREATED entry for that user |
| `LOGIN event ... is recorded` | Append-only log file contains a LOGIN entry for that user |
| `LOGOUT event ... is recorded` | Append-only log file contains a LOGOUT entry for that user |
| `validation error` | HTTP 400 |
| `invalid credentials error` | HTTP 401 |
| `session conflict error` | HTTP 409 |
| `authentication error` | HTTP 401 |
