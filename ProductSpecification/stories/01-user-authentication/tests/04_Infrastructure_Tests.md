# User Authentication — Infrastructure Tests

---

## 1. Append-Only Log Unavailable

### 1.1 Login fails gracefully when the log file cannot be written

```gherkin
Given the append-only log storage is unavailable
When "ola" attempts to log in with PIN "123"
Then the login is rejected with a server error
And no session is created
```

### 1.2 Service recovers and login succeeds after log storage is restored

```gherkin
Given the append-only log storage was unavailable and has been restored
When "ola" logs in with PIN "123"
Then the login succeeds and a session token is returned
```

---

## 2. User Data File Unavailable

### 2.1 Login fails gracefully when user data cannot be read

```gherkin
Given the user data storage is unavailable
When "ola" attempts to log in with PIN "123"
Then the login is rejected with a server error
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `log storage is unavailable` | Simulate write failure on the append-only log file (e.g., read-only filesystem or missing directory) |
| `user data storage is unavailable` | Simulate read failure on the user data file |
| `storage ... has been restored` | Restore normal file access before the subsequent request |
| `server error` | HTTP 500 |
