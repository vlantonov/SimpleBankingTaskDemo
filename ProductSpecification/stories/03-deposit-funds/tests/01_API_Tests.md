> **Implementation Order**: Tests are numbered for sequential TDD implementation.
> Start with authentication guard, then valid deposit, then invalid amount validation.

# Deposit Funds — API Tests

---

## 1. Deposit — Authentication Guard

### 1.1 Deposit request without a valid session is rejected

```gherkin
When a caller attempts to deposit funds without a valid active session
Then the request is rejected with an authentication error
```

### 1.2 Deposit request with a fabricated token is rejected

```gherkin
When a caller presents a token that is not tied to any active session
Then the deposit request is rejected with an authentication error
```

---

## 2. Deposit — Valid Deposit

### 2.1 Authenticated user can deposit a valid positive amount

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" deposits 75.50
Then the response contains the updated balance of 325.50
```

### 2.2 Deposit appends a new ledger event and keeps historical records immutable

```gherkin
Given "ola" has an active session and a current account balance of 250.00
And "ola" already has a previous ledger entry for a prior deposit
When "ola" deposits 12.00
Then the new balance is 262.00
And the prior ledger entry remains unchanged
And exactly one new deposit ledger record is created
```

---

## 3. Deposit — Invalid Amount Validation

### 3.1 Zero value deposit is rejected

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" deposits 0.00
Then the request is rejected with a validation error
```

### 3.2 Negative deposit is rejected

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" deposits -10.00
Then the request is rejected with a validation error
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `without a valid active session` | `POST /api/v1/deposit` with no valid bearer token |
| `fabricated token` | A random bearer value not present in the active session map |
| `current account balance of 250.00` | Account state seeded with that balance before the request |
| `deposits 75.50` | HTTP request body containing `{"amount": 75.50}` |
| `updated balance of 325.50` | HTTP success response with the new account balance |
| `validation error` | HTTP 400 |
| `authentication error` | HTTP 401 |
| `exactly one new deposit ledger record is created` | New ledger entry is appended; prior transaction history remains unchanged |
