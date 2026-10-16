> These are additional edge case tests. Implement after core tests pass.

# Get Balance — Extended Security Tests

---

## 1. Token Boundary Cases

### 1.1 Expired or revoked token is rejected

```gherkin
Given "ola" has a token that is no longer valid
When the balance endpoint is called with that token
Then the request is rejected with an authentication error
```

### 1.2 Empty bearer token is rejected

```gherkin
When a caller sends an empty bearer token to the balance endpoint
Then the request is rejected with an authentication error
```
