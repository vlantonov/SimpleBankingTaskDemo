> **Implementation Order**: Tests are numbered for sequential TDD implementation.
> Start with unauthenticated access guard, then valid balance read, then read-only confirmation.

# Get Balance — API Tests

---

## 1. Balance — Authentication Guard

### 1.1 Balance request without a valid session is rejected

```gherkin
When a caller requests the balance without a valid active session
Then the request is rejected with an authentication error
```

### 1.2 Balance request with a fabricated token is rejected

```gherkin
When a caller presents a token that is not tied to any active session
Then the balance request is rejected with an authentication error
```

---

## 2. Balance — Valid Read

### 2.1 Authenticated user receives the current balance

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" requests the balance
Then the response contains the current balance of 250.00
```

### 2.2 Balance reflects the latest account state

```gherkin
Given "ola" has an active session
And the user account balance has been updated to 425.50
When "ola" requests the balance
Then the response contains 425.50
```

---

## 3. Balance — Read-Only Guarantee

### 3.1 Balance lookup does not create a new transaction or mutation

```gherkin
Given "ola" has an active session and a current account balance of 100.00
When "ola" requests the balance
Then the balance is returned successfully
And no new transaction record is created
And the account state remains unchanged
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `without a valid active session` | `GET /api/v1/balance` with no valid bearer token |
| `fabricated token` | A random bearer value not present in the active session map |
| `current account balance of 250.00` | Account state seeded with that balance before the request |
| `balance is returned successfully` | HTTP 200 with a JSON body containing `balance` |
| `authentication error` | HTTP 401 |
| `no new transaction record is created` | No ledger append or mutation side effect occurs during the read |
