> **Implementation Order**: Tests are numbered for sequential TDD implementation.
> Start with session guard, then empty history, then ordering and ownership checks.

# Transaction History — API Tests

---

## 1. History — Authentication Guard

### 1.1 History request without a valid session is rejected

```gherkin
When a caller requests transaction history without a valid active session
Then the request is rejected with an authentication error
```

### 1.2 History request with a fabricated token is rejected

```gherkin
When a caller presents a token that is not tied to any active session
Then the history request is rejected with an authentication error
```

---

## 2. History — Empty State

### 2.1 Authenticated user with no ledger entries receives an empty list

```gherkin
Given "ola" has an active session and no transaction history
When "ola" requests transaction history
Then the response contains an empty transaction list
```

---

## 3. History — Valid Read

### 3.1 Authenticated user can fetch their own transaction history

```gherkin
Given "ola" has an active session
And "ola" has deposited 50.00 and then withdrawn 15.00
When "ola" requests transaction history
Then the response contains both ledger events for "ola"
And the most recent event appears first
```

### 3.2 History entries are returned in reverse chronological order

```gherkin
Given "ola" has an active session
And "ola" has a ledger with three dated events
When "ola" requests transaction history
Then the events are returned from newest to oldest
```

---

## 4. History — Ownership and Protection

### 4.1 User cannot retrieve another user’s transaction history

```gherkin
Given "ola" has an active session
And "marta" has a different ledger with recorded transactions
When "ola" requests transaction history
Then only "ola"'s events are returned
And "marta"'s events are not included
```

### 4.2 History read does not mutate prior ledger entries

```gherkin
Given "ola" has an active session
And "ola" has an existing ledger with multiple entries
When "ola" requests transaction history
Then the response does not change any stored ledger entry
And the ledger remains immutable
```

---

## 5. History — Real Ledger Integration

### 5.1 Deposit and withdrawal events both appear in the same history stream

```gherkin
Given "ola" has an active session
And "ola" has a deposit event followed by a withdrawal event
When "ola" requests transaction history
Then the response includes both event types
And the sequence reflects the actual ledger order
```

### 5.2 History response contains enough metadata to explain each event

```gherkin
Given "ola" has an active session
And "ola" has a recorded transaction
When "ola" requests transaction history
Then each item includes the event type
And each item includes the amount
And each item includes the timestamp
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `without a valid active session` | `GET /api/v1/history` with no valid bearer token |
| `fabricated token` | A random bearer value not present in the active session map |
| `no transaction history` | Account ledger seeded with zero entries |
| `deposited 50.00 and then withdrawn 15.00` | Sequential ledger events created through the account flow |
| `empty transaction list` | HTTP 200 with `{"transactions": []}` |
| `authentication error` | HTTP 401 |
| `only "ola"'s events are returned` | Query result filtered to the authenticated account owner |
| `most recent event appears first` | Reverse chronological ordering in the response |
| `immutable` | Prior ledger entries remain unchanged after a read |
