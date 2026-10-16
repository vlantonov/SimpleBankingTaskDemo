# Get Balance — Security Tests

---

## 1. Session Protection

### 1.1 Unauthenticated user cannot read the balance

```gherkin
When an unauthenticated caller requests the balance
Then the request is rejected with an authentication error
```

### 1.2 Arbitrary token cannot access a protected balance endpoint

```gherkin
When a caller submits a fabricated session token to the balance endpoint
Then the request is rejected with an authentication error
```

### 1.3 A token for another user cannot read that user's balance

```gherkin
Given "ola" and "bob" each have active sessions
When "ola" uses "bob"'s session token to request the balance
Then the request is rejected with an authentication error
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `unauthenticated caller` | `GET /api/v1/balance` without a valid bearer token |
| `fabricated session token` | A random token value not present in the session registry |
| `another user's session token` | bearer token belonging to a different active session |
| `authentication error` | HTTP 401 |
