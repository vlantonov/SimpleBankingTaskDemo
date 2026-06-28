# Python/FastAPI Infrastructure Idioms

Tech binding for `infrastructure.md`. Load alongside the universal rules.

## Health Check

- FastAPI health endpoint: `source infrastructure/.env && curl http://localhost:$BACKEND_PORT/health`

## Process Safety

- Never kill by executable name: `taskkill //IM python.exe` or `pkill uvicorn` — use port-based stop scripts instead.
- Never run `pkill python` or `killall python` — it kills ALL Python processes system-wide, breaking parallel sessions.

## Config Fallback Syntax

Each file type has its own fallback pattern:
- Python settings (pydantic-settings): declare fields with defaults on a `BaseSettings` subclass; values load from env vars, e.g., `port: int = 8000`.
- Plain env access: `os.environ.get('VAR', 'fallback')`.
- Docker Compose, shell scripts: `${VAR:-fallback}` (colon-dash).

## Run Command

- `uvicorn backend.application.main:app --host 0.0.0.0 --port $BACKEND_PORT --reload` (dev). Use `--factory` mode when the app is created by a function (`backend.application.main:create_app`).
- Single-worker mode for development; gunicorn + uvicorn workers (`gunicorn -k uvicorn.workers.UvicornWorker`) for production-like profiling.

## Acceptance Tests

- `test-acceptance.sh` exports env vars then runs pytest against the acceptance suite: `pytest acceptance/`.
- Acceptance tests hit the running ASGI server over HTTP (via `httpx.Client(base_url=...)`), not the in-process `TestClient` — that path is reserved for REST adapter tests.
