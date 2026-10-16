# Transfer Funds — Infrastructure Tests

---

## 1. Database Connection Failure

### 1.1 Transfer request fails safely when persistence is unavailable

```gherkin
Given the transfer service is running
And persistence becomes unavailable
When an authenticated user submits a transfer request
Then the request fails with infrastructure error
And no partial transfer is recorded
```

---

## 2. Recovery After Persistence Failure

### 2.1 Transfer succeeds again after persistence recovers

```gherkin
Given persistence was unavailable and then recovered
And "ola" has an active session
And "marta" exists
When "ola" submits a valid transfer request
Then the transfer is processed successfully
And source and destination balances are updated correctly
```

---

## 3. Session Validation Service Failure

### 3.1 Transfer request fails safely when session validation dependency is down

```gherkin
Given session validation dependency is unavailable
When a transfer request is submitted
Then the request is rejected with dependency error
And no transfer ledger mutation occurs
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `persistence becomes unavailable` | Database/file storage adapter failure during transfer operation |
| `no partial transfer is recorded` | Neither debit nor credit ledger entry is persisted |
| `session validation dependency is unavailable` | Session lookup path returns dependency failure |
| `dependency error` | 5xx response mapped from infrastructure failure |
