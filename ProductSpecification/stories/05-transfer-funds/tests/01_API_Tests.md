> **Implementation Order**: Tests are numbered for sequential TDD implementation.
> Start with session and prerequisite guards, then validation, then successful transfer flow.

# Transfer Funds — API Tests

---

## 1. Transfer Prerequisite Guards

### 1.1 Transfer request without an active session is rejected

```gherkin
When a caller submits a transfer request without a valid active session
Then the request is rejected with an authentication error
```

### 1.2 Transfer to a non-existing destination user is rejected

```gherkin
Given "ola" has an active session
When "ola" transfers 25.00 to unknown user "ghost"
Then the request is rejected with destination-not-found error
And no ledger entry is created
```

### 1.3 Transfer to the same user is rejected

```gherkin
Given "ola" has an active session
When "ola" transfers 25.00 to "ola"
Then the request is rejected with validation error
And no ledger entry is created
```

---

## 2. Transfer Validation

### 2.1 Transfer amount must be positive

```gherkin
Given "ola" has an active session
And "marta" exists
When "ola" transfers 0.00 to "marta"
Then the request is rejected with validation error
And no ledger entry is created
```

### 2.2 Transfer with insufficient funds is rejected

```gherkin
Given "ola" has an active session
And "ola" balance is 10.00
And "marta" exists
When "ola" transfers 50.00 to "marta"
Then the request is rejected with insufficient-funds error
And no ledger entry is created
```

---

## 3. Successful Transfer

### 3.1 Authenticated user can transfer funds to another existing user

```gherkin
Given "ola" has an active session
And "ola" balance is 200.00
And "marta" exists with balance 50.00
When "ola" transfers 40.00 to "marta"
Then the transfer is accepted
And "ola" balance becomes 160.00
And "marta" balance becomes 90.00
And linked debit and credit ledger entries are recorded
```

### 3.2 Successful transfer appears in transaction history for both users

```gherkin
Given "ola" has an active session
And "marta" has an active session
And a successful transfer from "ola" to "marta" has been completed
When each user requests transaction history
Then "ola" history includes transfer-out event
And "marta" history includes transfer-in event
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `active session` | Valid bearer token from login endpoint |
| `destination-not-found error` | HTTP 404 from transfer endpoint |
| `validation error` | HTTP 400 for invalid request parameters |
| `insufficient-funds error` | HTTP 400 when source balance is lower than amount |
| `no ledger entry is created` | Ledger/account state unchanged after rejected transfer |
| `linked debit and credit ledger entries` | Two immutable entries sharing the same transfer identity |
