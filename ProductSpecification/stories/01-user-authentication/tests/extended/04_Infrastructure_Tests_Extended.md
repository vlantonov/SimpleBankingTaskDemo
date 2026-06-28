> These are additional edge case tests. Implement after core tests pass.

# User Authentication — Infrastructure Tests (Extended)

---

## 1. Partial Write Recovery

### 1.1 Truncated log entry on crash does not corrupt subsequent entries

```gherkin
Given the append-only log contains a partially written entry from a prior crash
When the service starts up and "ola" logs in
Then the login succeeds
And the new LOGIN event is appended correctly after the truncated entry
```

---

## DSL Technical Reference

| DSL Statement | Technical Implementation |
|---------------|-------------------------|
| `partially written entry` | Log file manually seeded with an incomplete line (no newline terminator) |
| `appended correctly` | New event line is well-formed and parseable |
