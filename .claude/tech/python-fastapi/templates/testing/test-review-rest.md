# Test Review Patterns: REST Adapter Layer (Python/FastAPI)

Python/`unittest.mock` code examples for FastAPI router test anti-patterns. For universal rules: `.claude/templates/testing/test-review-patterns.md`

## Python-Specific Rules (REST)

1. **Use descriptive assertion messages** — add a second argument to `assert` for clear failure messages
2. **Use exact expected values in mock setups** — `mock.assert_called_once_with(exact_request)` instead of `mock.assert_called()`. Pydantic request models implement `__eq__` for free, so strict matching is straightforward; strict mocks catch router bugs that loose matchers miss
3. **No `unittest.mock.ANY` when exact value is known** — if the expected argument is known, use the exact value in `assert_called_once_with()`
4. **Override dependencies, do not monkeypatch handlers** — replace usecases via `app.dependency_overrides[get_usecase] = lambda: mock_usecase`. Never patch the router module attribute directly.
5. **Use `AsyncMock` for async usecases** — if the production usecase exposes `async def execute(...)`, the mock must be `AsyncMock()` so `await usecase.execute(...)` resolves correctly.

## Anti-Pattern Examples

### BAD: unittest.mock.ANY when exact value is known
```python
mock_usecase.execute.assert_called_once_with(USER_ID, unittest.mock.ANY)
# Should use exact expected request — Pydantic models compare by value
mock_usecase.execute.assert_called_once_with(USER_ID, expected_request)
```

### BAD: mock.assert_called() without verifying arguments
```python
mock_usecase.execute.assert_called_once()
# Should verify exact arguments
mock_usecase.execute.assert_called_once_with(USER_ID, SomeRequest(period="monthly", active=True))
```

### BAD: Checking call_args manually instead of assert_called_once_with
```python
mock_usecase.execute(USER_ID, unittest.mock.ANY)
args, kwargs = mock_usecase.execute.call_args
assert args[1].field == "expected"
# Should just use exact match — strict mock catches wrong arguments immediately
mock_usecase.execute.assert_called_once_with(USER_ID, expected_request)
```

### BAD: Patching the router module instead of overriding the dependency
```python
with patch("backend.adapters.rest.src.router.tasks.create_task_router.create_task", mock_usecase):
    response = client.post(...)
# Should override the FastAPI dependency
app.dependency_overrides[get_create_task] = lambda: mock_usecase
response = client.post(...)
```
