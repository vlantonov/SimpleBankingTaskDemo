# Security Adapter Implementation Template -- Python/FastAPI

## Rules

- Pure unit tests in test code — no FastAPI app instance, no TestClient
- Constructor injection via `__init__` parameters; the FastAPI dependency layer (`Depends(get_jwt_service)`) wires concrete instances at the router boundary
- JWT operations via `JwtService` (using `PyJWT` or `python-jose`), configuration via `JwtConfig` dataclass
- Use `Clock` protocol for time operations (inject `FakeClock` in tests)
- Session/auth enforcement: implement as a FastAPI dependency (`async def require_session(...)` raising `HTTPException(401)`) rather than a middleware where possible; reserve middleware for cross-cutting concerns (CORS, request IDs, exception translation)

## Reference (read before generating)

- JWT service: `backend/adapters/security/src/jwt_service.py`
- Config: `backend/adapters/security/src/jwt_config.py`
- Auth dependency: `backend/adapters/security/src/dependencies/require_session.py`
- User-id resolver dependency: `backend/adapters/security/src/dependencies/current_user_id.py`
- Exception translation middleware (if used): `backend/adapters/security/src/middleware/`

## Key Paths

- Production: `backend/adapters/security/src/`
- Tests: `backend/adapters/security/tests/`
