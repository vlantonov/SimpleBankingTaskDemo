---
name: test-acceptance
description: Run acceptance tests (backend API or load suite). Use when user wants to run E2E acceptance tests or mentions /test-acceptance command.
---

# Run Acceptance Tests

## Pre-Checks

Read ports from `infrastructure/.env` and verify required services are up **before** running tests.

### Backend (always required)

Check the health endpoint (see the selected profile's `.claude/tech/{backend}/infrastructure.md` for the exact path):

```bash
source infrastructure/.env && curl -s -o /dev/null -w "%{http_code}" http://localhost:$BACKEND_PORT{health_endpoint} 2>/dev/null || echo "UNAVAILABLE"
```

- `200` → OK.
- `UNAVAILABLE` or non-200 → start backend first: `Skill tool: skill="run-backend"`. Wait for startup.

### Load (required when argument is `load`)

Load tests run only against a backend booted from the pre-baked Standard Load Baseline image (`docker-compose.load.yml`) — never a dev backend. **Two pre-checks are required — both must pass before launching the suite.** The health endpoint alone is not enough: a dev backend started by `run-backend` also returns `200` from the health endpoint while having an empty DB, and the load suite will then fail every test with confusing 401s at the login step. Catch this upfront with a baseline-login probe.

**Step 1 — health check** (same path as Backend above):

```bash
source infrastructure/.env && curl -s -o /dev/null -w "%{http_code}" http://localhost:$BACKEND_PORT{health_endpoint} 2>/dev/null || echo "UNAVAILABLE"
```

- `200` → proceed to Step 2.
- `UNAVAILABLE` or non-200 → **ERROR and STOP. Do NOT start the backend via `run-backend`.** A backend started by `run-backend` has an empty DB; load tests would silently produce meaningless results. The load backend must be booted from the baked baseline image — point the user to `infrastructure/load-baseline/README.md` ("Wiring load tests to the baked image").

**Step 2 — baked-baseline DB probe.** Confirm the running Postgres container for this repo instance is the baked baseline image. (A raw-curl login probe is unreliable: the application's auth filter may block unauthenticated POSTs before any DB lookup. Probe the DB container directly.)

```bash
source infrastructure/.env && docker ps --filter "name=postgres-container-$REPO_INDEX" --format '{{.Image}}' 2>/dev/null
```

- Output starts with the project's load-baseline image name (e.g., `load-baseline-postgres:`) → baked baseline DB confirmed, proceed to run the suite.
- Output empty (no container running for this repo index) → **ERROR and STOP.** No Postgres container is running for this repo index. Point the user to `infrastructure/load-baseline/README.md` ("Wiring load tests to the baked image").
- Output is a non-baseline image (e.g., `postgres:16`) → **ERROR and STOP.** The Postgres container for this repo index is the dev image, not the baked baseline. Load tests would fail every login with 401 against an empty DB. Tell the user the container needs to be swapped to the baked baseline via the compose flow in `infrastructure/load-baseline/README.md`. Do NOT swap the container yourself.

**Never report "Backend healthy, launching load suite" after Step 1 alone.** A green Step 1 with a red Step 2 is exactly the dev-backend-with-empty-DB trap.

## Action

Run via script (loads ports from `.env`).

```bash
infrastructure/scripts/test-acceptance.sh
```

With arguments:
- `{ClassName}` → runs only the named test class
- `load` → runs the load suite (requires the baked-baseline backend — see Load pre-check)
- `load {ClassName}` → runs only the named load test class
- (no args) → runs all acceptance tests

**Always pass the test class name** when running a specific test — never run the full suite just to check one test.

## Execution Strategy

Follow `tdd-rules.md` "Stop on first failure" protocol:

1. **Launch in background:** `run_in_background: true` — note the output file path from the result. Store `SEEN=0` to track lines already shown.
2. **Poll with separate Bash calls:** Make repeated **individual** Bash calls (NOT a loop inside one call — that hides output until the loop finishes). Each call checks for new lines and the terminal signal. The exact pattern depends on the selected profile's build tool — see `.claude/tech/{backend}/infrastructure.md` for tool-specific signals (e.g., `BUILD SUCCESSFUL`/`BUILD FAILED` for Gradle, `OK`/`FAIL` for `go test`, `passed`/`failed` for `pytest`, `ctest` exit codes for CMake). Use grep against the appropriate keywords.
3. **If success signal observed** → read output file, report pass counts. Done.
4. **If failure observed** → stop suite, then **immediately disclose to the user**:
   > "Stopped after the first failure. N tests passed, 1 failed, M tests did not run — there may be additional failures in the remaining tests."
   Calculate M from total test count minus passed minus failed. **Never present a stopped-early run as if it were a complete run.** Then: read stack trace from the output file, investigate root cause. Do NOT collect further failures.
   - **Compilation error** → read error lines, report immediately. No need to check infrastructure.
   - **Infrastructure error** (connection reset, container down) → re-check backend health.
   - **Application error** (assertion failure, wrong status) → investigate and fix.
   - **NEVER dismiss a failure as "not related", "pre-existing", or "from another story".** Every failure is your problem right now. Either fix the root cause or create a task with `/task` — but never report results and move on while the build is red.

## Post-Run Cleanup (load only)

When the load-testing session is finished — NOT between iterative re-runs — stop the load infrastructure. It is resource-heavy and, unlike the dev backend, not meant to stay up. See `.claude/rules/infrastructure.md` "Load-Test Infrastructure".

- Iterative `green-acceptance` re-runs reuse the same warm backend + baseline container — leave them running.
- Once load testing is done, stop the load backend (the specific PID you started) and the baseline container (by name, matching your repo index).

## Output

Report the test results from output. Always include pass/fail counts and how many tests did not run (if stopped early). If any test failed and you could not fix it, create a task to track the fix — do not leave failures untracked.
