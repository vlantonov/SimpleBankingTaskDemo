# Database Storage Implementation Template -- Python/FastAPI

## Rules

- Replace stub implementation with actual logic using SQLAlchemy (sync `Session` or async `AsyncSession`)
- Map between SQLAlchemy mapped class and domain via `from_domain(...)` / `to_domain()`; never return ORM rows from the storage class
- Use the session passed in via constructor (resolved through FastAPI `Depends(get_db)` at the router boundary). Do not open new sessions inside the storage class
- Transactions: rely on the request-scoped session's `begin()`/`commit()`/`rollback()`; for explicit boundaries inside a usecase, use `with session.begin_nested():`

## Reference (read before generating)

- Storage example: `backend/adapters/db/src/access/{feature}/{feature}_storage.py`
- Mapped class example: `backend/adapters/db/src/model/{feature}/{feature}_row.py`
- Migration directory: `backend/adapters/db/migrations/versions/` (Alembic)

## SQLAlchemy Query Methods (sync API shown — async mirrors with `await session.execute(...)`)

| Operation | Code |
|-----------|------|
| Find by field | `session.scalar(select(TaskRow).where(TaskRow.email == email))` |
| Find many | `list(session.scalars(select(TaskRow).where(TaskRow.board_id == board_id)))` |
| Create | `row = TaskRow.from_domain(domain); session.add(row); session.flush()` |
| Check exists | `session.scalar(select(exists().where(TaskRow.email == email)))` |
| Eager load (single) | `session.execute(select(BoardRow).options(joinedload(BoardRow.columns)).where(...))` |
| Eager load (collection) | `session.execute(select(BoardRow).options(selectinload(BoardRow.columns).selectinload(ColumnRow.tasks)).where(BoardRow.id == board_id))` |

## Mapped-Class Conversion

- `row.to_domain()` — convert SQLAlchemy row to domain object
- `RowClass.from_domain(domain)` — classmethod to build a row from a domain object

## Migrations (Alembic)

- Generate: `alembic revision --autogenerate -m "{message}"`
- Apply: `alembic upgrade head`
- Migration files live under `backend/adapters/db/migrations/versions/`

## Key Paths

- Storage: `backend/adapters/db/src/access/`
- Mapped classes: `backend/adapters/db/src/model/`
- Migrations: `backend/adapters/db/migrations/`
- Alembic env: `backend/adapters/db/migrations/env.py`
