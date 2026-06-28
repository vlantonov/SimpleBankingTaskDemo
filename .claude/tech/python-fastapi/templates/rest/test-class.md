# Controller Test Template -- Python/FastAPI

> Universal rules: `.claude/templates/tdd/red-rest.md`

## Tech-Specific Rules

- Use `fastapi.testclient.TestClient(app)` for synchronous handlers; `httpx.AsyncClient(app=app, base_url="http://test")` with `pytest-asyncio` for async handlers
- Override usecases with `app.dependency_overrides[get_{usecase}] = lambda: mock_usecase` in the fixture; clear with `app.dependency_overrides.clear()` in teardown
- Use mock usecases (`unittest.mock.Mock` / `MagicMock`) — adapter tests verify wiring and serialization, not domain logic
- Create exactly ONE test method, add `@pytest.mark.skip`
- Use class-level docstring with Gherkin-style description
- Setup: `mock_usecase.execute.return_value = ...`
- Execute: `response = client.get("/api/...")` or `response = client.post("/api/...", json=payload)`
- Verify: `assert response.json() == expected` and `assert response.status_code == 201`

## Expected Response JSON

Create in `backend/adapters/rest/tests/fixtures/{feature}/` or inline as dict literals for small responses.

## Reference (read before generating)

- Test example: `backend/adapters/rest/tests/router/{feature}/test_{feature}_router.py`
- Test setup: `backend/adapters/rest/tests/conftest.py` (app factory fixture + override helpers)
- Request model example: `backend/adapters/rest/src/dto/{feature}/{feature}_request.py`
- JSON fixture example: `backend/adapters/rest/tests/fixtures/` (look for existing JSON files)

## Key Paths

- Tests: `backend/adapters/rest/tests/router/`
- Production: `backend/adapters/rest/src/router/`
- DTOs: `backend/adapters/rest/src/dto/`
- Fixtures: `backend/adapters/rest/tests/fixtures/`
