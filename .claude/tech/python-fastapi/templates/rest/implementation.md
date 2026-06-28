# Controller Implementation Template -- Python/FastAPI

> Universal rules: `.claude/templates/tdd/green-rest.md`

## Tech-Specific Rules

- Use `APIRouter()` per feature; assemble into the app in `backend/application/main.py`
- Inject usecases via FastAPI `Depends(get_{usecase})`; resolver functions live next to the router and wire concrete adapter dependencies
- Request/response shapes are Pydantic `BaseModel` subclasses
- HTTP status: set `status_code=...` on the route decorator; rely on the centralized `app.exception_handler(...)` for domain errors

## Reference (read before generating)

- Router example: `backend/adapters/rest/src/router/{feature}/{feature}_router.py`
- Request model example: `backend/adapters/rest/src/dto/{feature}/{feature}_request.py`
- Response model example: `backend/adapters/rest/src/dto/{feature}/{feature}_response.py`
- Dependency wiring: `backend/adapters/rest/src/router/{feature}/dependencies.py`

## Key Paths

- Routers: `backend/adapters/rest/src/router/`
- Pydantic DTOs: `backend/adapters/rest/src/dto/`
- Exception handlers: `backend/adapters/rest/src/error/`
- Tests: `backend/adapters/rest/tests/router/`
