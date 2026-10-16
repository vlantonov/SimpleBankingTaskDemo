# Transfer Funds — Integration Tests

---

## 1. End-to-End Account Consistency

### 1.1 Transfer updates both accounts consistently in one operation

```gherkin
Given "ola" has an active session with balance 200.00
And "marta" has balance 50.00
When "ola" transfers 40.00 to "marta"
Then source and destination balances are updated consistently
And both users see the updated balances on subsequent reads
```

---

## 2. Cross-Feature Integration

### 2.1 Transfer result is visible in balance and history flows

```gherkin
Given a successful transfer from "ola" to "marta"
When "ola" checks current balance and transaction history
And "marta" checks current balance and transaction history
Then both balance responses reflect the transfer
And both history responses include matching transfer events
```

---

## 3. Failure Isolation

### 3.1 Failed transfer does not affect unrelated account operations

```gherkin
Given "ola" submits an invalid transfer request
When "ola" and "marta" later request balances and histories
Then both accounts remain consistent with pre-transfer state
And no phantom transfer events appear
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `updated balances on subsequent reads` | GET balance endpoint reflects post-transfer state |
| `matching transfer events` | History shows corresponding transfer-out/transfer-in entries |
| `phantom transfer events` | No transfer entries persisted for failed operations |
