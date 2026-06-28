# Python/FastAPI Coding Idioms

Tech binding for `coding-rules.md`. Shared section structure: `.claude/templates/coding/coding-sections.md`.

## Deployment

- In-memory state to avoid: module-level `dict`/`set`, mutable default arguments, singleton caches in closures, class-level mutable attributes, `lru_cache` on stateful callables.
- FastAPI dependencies returned from `Depends()` are re-resolved per request — never hold request-scoped state on module-level singletons.

## Clean Architecture

- `domain`: pure Python — no FastAPI, Pydantic, SQLAlchemy, or httpx imports; no framework decorators.
- `usecase`: ports are `Protocol` classes. May use `contextlib.AbstractContextManager` for transaction scopes; no FastAPI/SQLAlchemy imports.
- `adapters`: FastAPI routers, Pydantic request/response models, SQLAlchemy repositories, APScheduler/Celery tasks, httpx external API clients, fastapi-mail / aiosmtplib senders.

## Domain-Driven Design

- Validation: throws `ValidationException` (custom domain exception).
- Enum: `enum.Enum` or `StrEnum`. `parse(value: str)` classmethod, `.value` for lowercase.
- Optional: `Optional[T]` / `T | None`. Collections: `tuple`/`frozenset` (immutable), `list` (mutable). Adapters convert `None`.
- Forbidden dispatch: `isinstance`. Adapters dispatch from SQLAlchemy rows or Pydantic models.
- Typed list: `list[ArchivedTask]` not `list[BaseProtocol]` + `isinstance`.
- Parameter object: `dataclass` or `NamedTuple`.
- Immutable: `@dataclass(frozen=True)`. Transitions via `dataclasses.replace()`.

## Code Generation

- `@dataclass` for domain DTOs/VOs. `@dataclass(frozen=True)` for immutable.
- Adapter request/response: Pydantic `BaseModel` (FastAPI auto-validates and serializes).
- DI: FastAPI `Depends()` at the router boundary; usecases use plain `__init__` constructor injection wired in a composition root (`backend/application/`).
- Builders: class methods (`create`, `of`, `from_`).

## Naming

- Persistence: `{Name}Row` (SQLAlchemy mapped class). Converters: `to_dto()` / `to_row()` / `to_domain()`.
- Adapter Pydantic models: `{Action}Request` / `{Action}Response` (snake_case JSON via `model_config = ConfigDict(alias_generator=to_snake, populate_by_name=True)` if mixing styles).
- Modules: `snake_case`.

## Immutability

- `frozen=True` dataclasses for domain types, `tuple` over `list`, `frozenset` for sets.
- Pydantic models at the boundary: `model_config = ConfigDict(frozen=True)` for request/response DTOs.

## Accessor Chains

- `a.b.c.format()` → convenience property `a.c_value`.

## Optional/None Handling

- `match/case` or ternary — never `if x is not None: return x.value`.
- `or` for defaults, conditional expressions for mapping.

## Null Boundary

- Router handlers: Pydantic validates required fields; convert `Optional` fields to domain values before building DTOs.
- Request dataclasses (usecase layer): `field(default=None)` typed as `Optional[T]`.

## Request DTO Conversions

- Examples: `str` date → `datetime`, `str` → enum parse.
- Methods: `to_timestamp()`, `parsed_action_type()` on the request model.

## Branching

- `match/case` over `if/return` chains (Python 3.10+).

## Controllers

- FastAPI `APIRouter` per feature. Handler signature: `async def handler(req: CreateTaskRequest, usecase: CreateTaskUsecase = Depends(get_create_task))`.
- Convert via `req.to_usecase_request()`, call usecase, return Pydantic response model.
- Status codes: `status_code=status.HTTP_200_OK` (default for body), `status.HTTP_201_CREATED` on creation, `status.HTTP_204_NO_CONTENT` for no-body responses (return `Response(status_code=204)` or raise nothing with `response_class=Response`).
- Errors via centralized exception handlers (`app.exception_handler(DomainException)` registered in the application factory).

## Storage Adapters

- SQLAlchemy `Mapped` classes (or `Table` + dataclass mapping) ≠ domain. `@classmethod from_domain(cls, domain)` + `to_domain()`. Never expose ORM rows outside the storage class.
- No `itertools.groupby()`, `defaultdict` grouping, or row dicts — use SQLAlchemy `relationship(lazy="selectin"|"joined")` and `select(...).options(selectinload(...))` so the ORM does the assembly.
- Trivial: `return [row.to_domain() for row in session.scalars(select(TaskRow)).all()]`.
- Query objects: `dataclass` carrying filter values; storage builds the `select()` from them.
- Sessions: get a `Session`/`AsyncSession` via FastAPI dependency (e.g., `get_db`) at the router boundary; pass it down to repositories via `Depends`. Never open sessions inside the usecase.

## Refactor Agent — Python Terms

| Generic term (in agent) | Python equivalent |
|--------------------------|-------------------|
| Qualified enum references in logic | `from module import EnumValue` |
| Type-checking/type dispatch in domain or usecase | `isinstance`, `type()` checks |
| Base-type list re-partitioned with type checks | `list[BaseProtocol]` + `isinstance` |
| Immutable data class | `@dataclass(frozen=True)` (domain) / Pydantic `BaseModel` with `frozen=True` (boundary) |
| Collection pipeline terminal operation | `list()` / comprehension terminal |
| Manual per-field assertion for immutable data types | Applies to frozen dataclasses and Pydantic models |

## Scan Checklist — Storage Grep Patterns

| # | Grep pattern / indicator |
|---|--------------------------|
| A33 | `itertools.groupby`, `defaultdict(list)`, `dict[..., list[...]]`, Row/Projection classes assembled by hand |
| A34 | Count SQLAlchemy `Session`/`AsyncSession` / mapper references per storage class |
| A42 | Static/class methods returning SQLAlchemy `select()` expressions or composite `where(...)` filters built from a query object |
| A43 | `session.execute(text("..."))` with manual column extraction, `Row._mapping` index access, untyped tuple unpacking from query results |
| A44 | Inline `select(...).join(...).where(...).options(...)` chains >5 lines inside a storage method instead of an extracted AdapterQuery |

## HTTP Clients

- Production: `httpx.AsyncClient` (preferred for async FastAPI) or `httpx.Client` (sync). Tests: `unittest.mock` at adapter boundary, or `respx` for transport-level stubbing when the client surface is wide.

## Error Handling

- Domain: `DomainException(Exception)`. Bubble to a centralized exception handler registered with `app.add_exception_handler(DomainException, handle_domain_exception)`.
