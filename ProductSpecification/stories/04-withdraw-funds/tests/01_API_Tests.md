> **Implementation Order**: Tests are numbered for sequential TDD implementation.
> Start with authentication guard, then valid withdrawal, then insufficient funds and invalid amount validation.

# Withdraw Funds — API Tests

---

## 1. Withdraw — Authentication Guard

### 1.1 Withdraw request without a valid session is rejected

```gherkin
When a caller attempts to withdraw funds without a valid active session
Then the request is rejected with an authentication error
```

### 1.2 Withdraw request with a fabricated token is rejected

```gherkin
When a caller presents a token that is not tied to any active session
Then the withdrawal request is rejected with an authentication error
```

---

## 2. Withdraw — Valid Withdrawal

### 2.1 Authenticated user can withdraw a valid positive amount

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" withdraws 75.50
Then the response contains the updated balance of 174.50
```

### 2.2 Withdrawal appends a new ledger event and keeps historical records immutable

```gherkin
Given "ola" has an active session and a current account balance of 250.00
And "ola" already has a previous ledger entry for a prior withdrawal
When "ola" withdraws 12.00
Then the new balance is 238.00
And the prior ledger entry remains unchanged
And exactly one new withdrawal ledger record is created
```

---

## 3. Withdraw — Invalid Amount Validation

### 3.1 Zero value withdrawal is rejected

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" withdraws 0.00
Then the request is rejected with a validation error
```

### 3.2 Negative withdrawal is rejected

```gherkin
Given "ola" has an active session and a current account balance of 250.00
When "ola" withdraws -10.00
Then the request is rejected with a validation error
```

---

## 4. Withdraw — Insufficient Funds Validation

### 4.1 Withdrawal above the available balance is rejected

```gherkin
Given "ola" has an active session and a current account balance of 100.00
When "ola" withdraws 150.00
Then the request is rejected with a validation error
And the balance remains 100.00
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `without a valid active session` | `POST /api/v1/withdraw` with no valid bearer token |
| `fabricated token` | A random bearer value not present in the active session map |
| `current account balance of 250.00` | Account state seeded with that balance before the request |
| `withdraws 75.50` | HTTP request body containing `{"amount": 75.50}` |
| `updated balance of 174.50` | HTTP success response with the new account balance |
| `validation error` | HTTP 400 |
| `authentication error` | HTTP 401 |
| `exactly one new withdrawal ledger record is created` | New ledger entry is appended; prior transaction history remains unchanged |
