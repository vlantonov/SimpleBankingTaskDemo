# Transfer Funds — Security Tests

---

## 1. Authorization Boundaries

### 1.1 User cannot transfer funds from another user account

```gherkin
Given "ola" is authenticated
And "marta" has a separate account
When "ola" attempts to transfer funds using "marta" as source account
Then the request is rejected
And no funds are moved
```

### 1.2 Invalid bearer token cannot authorize transfer

```gherkin
Given a forged or expired bearer token
When the token is used for transfer request
Then the request is rejected with authentication error
And no ledger mutation occurs
```

---

## 2. Input Hardening

### 2.1 Malformed destination identifier is rejected

```gherkin
Given an authenticated user
When the transfer destination field contains malformed input
Then the request is rejected with validation error
And no funds are moved
```

### 2.2 Extreme numeric transfer payload is rejected

```gherkin
Given an authenticated user
When transfer amount uses unsafe numeric payload
Then the request is rejected with validation error
And no ledger mutation occurs
```

---

## 3. Data Exposure Protection

### 3.1 Transfer failure does not leak another user balance

```gherkin
Given "ola" is authenticated
And "marta" exists
When "ola" submits an invalid transfer to "marta"
Then error response does not include "marta" account balance or internal identifiers
```

### 3.2 Transfer success response exposes only required balance fields

```gherkin
Given "ola" has an active session
And a valid transfer is executed
When the response is returned
Then only source and destination post-transfer balances are exposed
And no internal account metadata is leaked
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `forged or expired bearer token` | Invalid or stale Authorization token |
| `malformed input` | Invalid destination value encoding/format in request JSON |
| `unsafe numeric payload` | Numeric overflow/underflow or unsupported number format |
| `no ledger mutation occurs` | Transfer storage remains unchanged after rejected request |
| `internal identifiers` | Internal account IDs, storage keys, or session linkage details |
