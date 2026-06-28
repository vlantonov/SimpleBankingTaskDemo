> These are additional edge case tests. Implement after core tests pass.

# User Authentication — API Tests (Extended)

---

## 1. Username Case Sensitivity

### 1.1 Usernames are treated as case-sensitive

```gherkin
Given "Ola" has an account with PIN "123"
When "ola" attempts to log in with PIN "123"
Then the login is rejected with an invalid credentials error
```

---

## 2. Whitespace Handling

### 2.1 Username with leading/trailing whitespace is rejected

```gherkin
When a login request is submitted with a username containing leading or trailing whitespace
Then the request is rejected with a validation error
```

### 2.2 PIN with leading/trailing whitespace is rejected

```gherkin
When a login request is submitted with a PIN containing leading or trailing whitespace
Then the request is rejected with a validation error
```

---

## 3. Concurrent First Login

### 3.1 Simultaneous first-login requests for the same new user result in exactly one account

```gherkin
Given no account exists for "ola"
When two login requests for "ola" with PIN "123" arrive simultaneously
Then exactly one USER_CREATED event is recorded for "ola"
And at most one session is active for "ola"
```

---

## 4. Append-Only Log Ordering

### 4.1 Log events appear in chronological order

```gherkin
Given "ola" logs in, then logs out, then logs in again
When the append-only log is inspected
Then the events appear in the order: USER_CREATED, LOGIN, LOGOUT, LOGIN
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `invalid credentials error` | HTTP 401 |
| `validation error` | HTTP 400 |
| `exactly one USER_CREATED event` | Append-only log contains exactly one USER_CREATED line for that username |
| `arrive simultaneously` | Two concurrent HTTP requests submitted within the same millisecond window |
