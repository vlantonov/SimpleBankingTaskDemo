# Test Review Patterns: Database Adapter Layer (Python/FastAPI)

Python/pytest code examples for persistence adapter test anti-patterns. For universal rules: `.claude/templates/testing/test-review-patterns.md`

## Python-Specific Rules (Database)

1. **Use descriptive assertion messages** — add a second argument to `assert` for clear failure messages
2. **Prefer dataclass / Pydantic equality** — replace 2+ sequential per-field `assert` calls with `assert actual == expected` (frozen dataclasses and Pydantic models do deep structural comparison by default)
3. **Use timedelta for timestamp comparisons** — assert `abs(actual - expected) < timedelta(minutes=1)`. Never truncate to minutes — truncation causes flaky failures at minute boundaries
4. **Always `session.flush()` before assertions that read back** — without a flush, identity-map reads succeed against in-memory state but the test misses real INSERT/UPDATE bugs

## Anti-Pattern Examples

Persistence adapter tests share many patterns with usecase and acceptance tests. Common issues:

- **Loose existence checks** on returned entities — use `== expected` not `is not None`
- **Missing field assertions** on `to_domain()` results — assert ALL domain fields after round-trip
- **Timestamp precision mismatches** — use timedelta within 60 seconds, never truncate
- **Tests that pass against the SQLAlchemy identity map but would fail against a fresh session** — flush + expire before assertions, or use a fresh session for the read path

See `test-review-usecase.md` for Statements purity patterns and `test-review-acceptance.md` for assertion strictness patterns — both apply to database adapter tests.
