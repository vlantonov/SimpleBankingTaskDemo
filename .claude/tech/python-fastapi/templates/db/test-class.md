# Database Storage Test Template -- Python/FastAPI

## Test Class Rules

- Use a SQLAlchemy session pointed at a transient test database (SQLite in-memory `sqlite+pysqlite:///:memory:` is fastest; a per-test Postgres schema works when SQL-dialect parity matters)
- Wrap each test in a transaction that rolls back in teardown — pytest fixture yields the session, then `session.rollback()` and `session.close()`
- Inject the storage class under test by constructing it directly with the fixture session
- Use class-level docstring with Gherkin-style description

## DB-Specific Failure Patterns

| Current Implementation | Expected Test Failure |
|----------------------|----------------------|
| `return None` | `assert result is not None` or `assert result == expected` |
| `return []` | `assert result != []` or `assert result == expected` |
| Missing `session.flush()` after `session.add(...)` | Lookup returns stale state; assertion against fresh read fails |

## Reference (read before generating)

- Test example: `backend/adapters/db/tests/access/{feature}/test_{feature}_storage.py`
- Test setup: `backend/adapters/db/tests/conftest.py` (engine + session fixtures, schema bootstrap)
- Storage example: `backend/adapters/db/src/access/{feature}/{feature}_storage.py`
- Mapped class example: `backend/adapters/db/src/model/{feature}/{feature}_row.py`

## Schema Bootstrap

- For in-memory SQLite, create schema once per test session: `Base.metadata.create_all(engine)` in a session-scoped fixture.
- For Postgres, run Alembic migrations against a disposable test database in the same fixture.

## Naming Convention

- Test file: `test_{entity}_storage_{method}.py`
- Test method: `test_should_{expected_behavior}`

## Key Paths

- Tests: `backend/adapters/db/tests/access/`
- Production: `backend/adapters/db/src/access/`
- Mapped classes: `backend/adapters/db/src/model/`
- Migrations: `backend/adapters/db/migrations/`
