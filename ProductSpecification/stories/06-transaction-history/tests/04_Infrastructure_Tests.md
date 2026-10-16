# Transaction History — Infrastructure Tests

---

## 1. Ledger availability

### 1.1 History query fails cleanly when the account ledger is unavailable

```gherkin
Given the account ledger service is unavailable
When a user requests transaction history
Then the request fails with a server error
And no partial history is exposed
```

---

## 2. Recovery after failure

### 2.1 History service recovers after a transient ledger failure

```gherkin
Given the ledger service has just recovered from a temporary outage
When a user requests transaction history
Then the system resumes normal history retrieval
And the user receives the current ledger state
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `account ledger service is unavailable` | The persistence layer or ledger repository is not reachable |
| `server error` | HTTP 500 or equivalent system-level failure response |
| `temporary outage` | A transient ledger error that resolves automatically |
