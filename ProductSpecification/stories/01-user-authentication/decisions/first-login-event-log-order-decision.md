# Decision: First-login event logging in usecase order

**Date**: 2026-06-30 **Scenarios**: 2.2

Scenario 2.2 requires explicit append-only log entries for first login with deterministic order.

| Rejected | Why |
|----------|-----|
| Side effects in user/session ports | Hides business sequence and makes order assertions indirect |
| Logging in HTTP controller | Breaks clean boundaries and misses non-HTTP entry points |

**Chosen**: Emit events from the usecase via an event-log port, appending `USER_CREATED` before `LOGIN` on first login.

## Model

- Add usecase port for append-only auth event writes
- Usecase emits `USER_CREATED` when account is created during login
- Usecase emits `LOGIN` after session creation succeeds
- Keep event sequencing in usecase orchestration, not adapters

## Edge Cases

| Case | Behavior |
|------|----------|
| Existing user login | Only `LOGIN` appended; no `USER_CREATED` |
| User creation succeeds but session open fails | Keep append-only semantics; follow red/green behavior for failure handling |
| Non-HTTP adapters invoke login | Same event sequence through shared usecase |
