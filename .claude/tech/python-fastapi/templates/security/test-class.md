# Security Adapter Test Template -- Python/FastAPI

## Test Class Rules

- Pure unit tests — NO FastAPI app instance or TestClient
- Mock dependencies with `unittest.mock.Mock()` / `MagicMock()` (use `AsyncMock` for async ports)
- Construct the class under test manually in `setup_method`
- Use `FakeClock` for deterministic time; avoid `unittest.mock.patch("time.time")` because dependencies may inject the clock directly
- Use class-level docstring with Gherkin-style description

## Security-Specific Failure Patterns

| Current Implementation | Expected Test Failure |
|----------------------|----------------------|
| Wrong mock interaction | Mock verification failure |
| Dependency that should raise `HTTPException(401)` returns `None` | `pytest.raises(HTTPException)` fails |

## Reference (read before generating)

- Existing test: `backend/adapters/security/tests/dependencies/test_require_session.py`
- JWT service: `backend/adapters/security/src/jwt_service.py`
- Config: `backend/adapters/security/src/jwt_config.py`
- Auth dependency: `backend/adapters/security/src/dependencies/require_session.py`

## Test Pattern

1. **Setup** (`setup_method`): create mocks, fixed clock, construct class/dependency under test
2. **Mock**: `mock_dependency.method.return_value = value` (or `mock_dependency.method.side_effect = ...`)
3. **Execute**: call the dependency/method under test
4. **Assert**: use `assert actual == expected` with descriptive messages
5. **Verify** (optional): `mock_dependency.method.assert_called_once_with(args)` for side-effect verification

## Naming Convention

- Test file: `test_{component_name}.py`
- Test method: `test_should_{expected_behavior}`

## Key Paths

- Tests: `backend/adapters/security/tests/`
- Production: `backend/adapters/security/src/`
