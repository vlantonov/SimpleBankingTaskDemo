# User Authentication — Security Tests

---

## 1. Brute-Force PIN Protection

### 1.1 Repeated failed login attempts are rate-limited

```gherkin
Given "ola" has an account with PIN "123"
When an attacker submits multiple consecutive failed login attempts for "ola"
Then further login attempts for "ola" are throttled or rejected
```

---

## 2. Session Token Security

### 2.1 Session token is not reusable after logout

```gherkin
Given "ola" is logged in with an active session token
When "ola" logs out
And the same token is used to attempt another logout
Then the second logout is rejected with an authentication error
```

### 2.2 Session token is sufficiently random

```gherkin
When multiple users log in independently
Then each session token is unique and not derivable from the username or login time
```

### 2.3 An arbitrary token cannot access a protected endpoint

```gherkin
When a caller presents a fabricated session token to a protected endpoint
Then the request is rejected with an authentication error
```

---

## 3. Input Validation Against Oversized Payloads

### 3.1 Excessively long username is rejected

```gherkin
When a login request is submitted with a username exceeding the maximum allowed length
Then the request is rejected with a validation error
And the server does not crash or hang
```

### 3.2 Excessively long PIN is rejected

```gherkin
When a login request is submitted with a PIN exceeding the maximum allowed length
Then the request is rejected with a validation error
And the server does not crash or hang
```

---

## 4. Credential Confidentiality

### 4.1 PIN is not echoed in any response body or log

```gherkin
Given "ola" logs in with PIN "123"
When the server responds
Then the PIN "123" does not appear in the response body
And the PIN "123" does not appear in the application log output
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `multiple consecutive failed login attempts` | Send N `POST /api/v1/login` requests with wrong PIN in rapid succession (exact N from rate-limit config) |
| `throttled or rejected` | HTTP 429 Too Many Requests or HTTP 401 with no further processing |
| `fabricated session token` | A random string not matching any in-memory session |
| `protected endpoint` | `POST /api/v1/logout` |
| `username exceeding the maximum allowed length` | String longer than the declared max (e.g., >256 chars) |
| `PIN exceeding the maximum allowed length` | String longer than the declared max (e.g., >64 chars) |
| `validation error` | HTTP 400 |
| `authentication error` | HTTP 401 |
| `not derivable from username or login time` | Tokens from N independent logins have no shared prefix or sequential pattern |
