> These are additional edge case tests. Implement after core tests pass.

# Get Balance — Extended API Tests

---

## 1. Edge Cases

### 1.1 Zero-balance account is returned correctly

```gherkin
Given "ola" has an active session and a zero balance
When "ola" requests the balance
Then the response contains 0.00
```

### 1.2 Balance read after logout is rejected

```gherkin
Given "ola" had an active session and then logged out
When "ola" requests the balance again
Then the request is rejected with an authentication error
```
