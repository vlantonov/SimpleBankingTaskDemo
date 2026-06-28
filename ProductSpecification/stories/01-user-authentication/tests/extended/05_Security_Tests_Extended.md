> These are additional edge case tests. Implement after core tests pass.

# User Authentication — Security Tests (Extended)

---

## 1. Token Isolation

### 1.1 One user's session token cannot be used for another user's session

```gherkin
Given "ola" and "bob" are both logged in with separate session tokens
When "bob" attempts to use "ola"'s token to log out
Then the request is rejected with an authentication error
And "ola"'s session remains active
```

---

## 2. Token After Account Inactivity

### 2.1 Session token from a previous server run is rejected after restart

```gherkin
Given "ola" obtained a session token before the server was restarted
When "ola" attempts to use that token after the server restarts
Then the request is rejected with an authentication error
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `authentication error` | HTTP 401 |
| `server was restarted` | Server process stopped and restarted; in-memory session map is cleared |
