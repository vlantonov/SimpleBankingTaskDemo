# Transaction History — Security Tests

---

## 1. Session ownership

### 1.1 User cannot fetch another user’s history by session misuse

```gherkin
Given "ola" is authenticated
And "marta" has a different account ledger
When "ola" requests transaction history for "marta"'s account
Then the request is rejected
And no ledger data is exposed
```

### 1.2 Session token mismatch is blocked

```gherkin
Given a user has a stale or mismatched token
When the token is used to request history
Then the request is rejected with an authentication error
```

---

## 2. Input validation and abuse resistance

### 2.1 History request with malformed authorization data is rejected

```gherkin
When a caller sends malformed authorization data to the history endpoint
Then the request is rejected
And no account data is returned
```

### 2.2 Repeated history requests do not reveal hidden account data

```gherkin
Given an authenticated user repeatedly requests history
When the user attempts to enumerate another account’s records through timing or variation
Then the service continues to return only the caller’s ledger entries
```

---

## 3. Data exposure boundaries

### 3.1 Empty account data does not reveal other users’ activity

```gherkin
Given "ola" has no ledger entries
And "marta" has a populated ledger
When "ola" requests transaction history
Then "ola" receives an empty list
And "marta"'s records remain hidden
```

### 3.2 History endpoint does not leak internal account identifiers

```gherkin
When a user requests transaction history
Then the response contains only transaction data
And it does not expose internal account ownership metadata beyond the authenticated user
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `different account ledger` | Another user account with a different immutable ledger |
| `session token mismatch` | Bearer token does not map to the requesting account |
| `malformed authorization data` | Missing bearer scheme, broken token format, or invalid header |
| `hidden account data` | No record of any other account is exposed in the response |
