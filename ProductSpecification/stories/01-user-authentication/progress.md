# Story 1: User Authentication — Progress

## Spec
- [x] interview
- [x] story
- [x] api-spec
- [x] test-spec

## Backend Scenarios (01_API_Tests.md)

### 1.1 Login with empty username is rejected
- [x] red-acceptance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery (http)
- [x] red-adapter http
- [x] green-adapter http
- [x] red-adapter http (coverage: malformed JSON body → parse_error uncaught in LoginController)
- [x] green-adapter http (coverage: malformed JSON body → parse_error uncaught in LoginController)
- [x] red-adapter http (server routing: wire POST /api/v1/login into banking_server main.cpp)
- [x] green-adapter http (server routing: wire POST /api/v1/login into banking_server main.cpp)
- [x] green-acceptance

### 1.2 Login with empty PIN is rejected
- [S] red-acceptance (empty PIN already rejected: LoginRequest throws ValidationException mapped to HTTP 400)
- [x] design
- [S] red-usecase (capability exists: LoginRequest rejects empty pin in domain; no usecase production change)
- [S] green-usecase (no implementation needed in usecase layer for empty-pin validation)
- [x] adapters-discovery
- [S] red-adapter http (no new http adapter behavior: controller already constructs LoginRequest with pin and delegates to mapped exception handler)
- [S] green-adapter http (no adapter implementation changes required for empty-pin validation path)
- [S] green-acceptance (no disabled acceptance test exists for scenario 1.2; enable-only step not applicable)

### 2.1 First login creates the user and opens a session
- [x] red-acceptance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery
- [x] red-adapter http (wire LoginUsecase IUserPort/ISessionPort from adapter layer; default usecase in-layer ports are insufficient)
- [x] green-adapter http (return login success JSON body with message/token from LoginResponse)
- [S] red-adapter http (coverage: server login route hits first-login in-memory path already implemented; no failing red test to add)
- [S] green-adapter http (coverage: server login route hits first-login in-memory path already implemented; no green change needed)
- [x] green-acceptance

### 2.2 First login appends USER_CREATED and LOGIN events to the log
- [x] red-acceptance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery
- [x] red-adapter http (server login composition does not wire IAuthEventLogPort to persistent logging adapter)
- [x] green-adapter http (append USER_CREATED and LOGIN events to auth_events.log via adapter)
- [x] green-acceptance

### 3.1 Returning user with correct PIN receives a welcome message and new token
- [S] red-acceptance (returning-user login already implemented: LoginUsecase find_or_create handles existing users, FileAuthEventLogPort logs LOGIN events on every login, LoginController returns welcome message and token)
- [S] design (existing implementation validated)
- [S] red-usecase (capability exists: LoginUsecase already handles both first-login and returning-user paths)
- [S] green-usecase (no implementation needed in usecase layer for returning-user login)
- [S] adapters-discovery (no new adapters needed)
- [S] green-acceptance (test proves functionality works; feature already complete)

### 3.2 Returning user with wrong PIN is rejected and no session is created
- [x] red-acceptance
- [x] design (see ADR: decisions/returning-user-wrong-pin-decision.md)
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery
- [x] red-adapter http (map InvalidCredentialsException to HTTP 401 in ExceptionHandler)
- [x] green-adapter http (return HTTP 401 body with error="Invalid credentials")
- [S] red-adapter http (coverage: preserve non-JWT token values in LoginController response mapping; capability already implemented in LoginController token mapping)
- [x] green-adapter http (coverage: preserve non-JWT token values in LoginController response mapping)
- [x] green-acceptance

### 4.1 Second login while a session is already active is rejected
- [x] red-acceptance
- [x] design (see ADR: decisions/active-session-conflict-decision.md)
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery (http)
- [x] red-adapter http (server TokenSessionPort does not track active sessions, so usecase conflict guard never triggers end-to-end)
- [x] green-adapter http (track active sessions in server-composed TokenSessionPort so second login is rejected before new side effects)
- [x] red-adapter http (map SessionConflictException to HTTP 409 with error body for login endpoint)
- [x] green-adapter http (return HTTP 409 with error="Session conflict" and empty token/message)
- [x] green-acceptance

### 5.1 Logout with a valid session token destroys the session
- [x] red-acceptance
- [x] design
- [x] red-usecase
- [x] green-usecase
- [x] adapters-discovery (http)
- [x] red-adapter http (no inbound logout endpoint wiring: POST /api/v1/logout is not registered and cannot invoke LogoutUsecase)
- [x] green-adapter http (wire logout endpoint/controller and server composition to return expected 200 logout response and persist LOGOUT event)
- [x] green-acceptance

### 5.2 Logout without a session token is rejected
- [x] red-acceptance
- [x] design (see ADR: decisions/logout-without-token-auth-error-decision.md)
- [S] red-usecase (capability exists: usecase already rejects unknown/empty tokens; missing bearer token handling belongs to HTTP boundary per ADR, so usecase files require zero changes)
- [S] green-usecase (no usecase implementation needed; scenario 5.2 is resolved in HTTP adapter by boundary auth handling)
- [x] adapters-discovery (http)
- [x] red-adapter http (missing/invalid Authorization currently flows into LogoutRequest validation and returns HTTP 400; scenario requires HTTP 401 authentication error)
- [x] green-adapter http (handle missing/invalid bearer token at LogoutController boundary and return HTTP 401 Authentication error before request validation)
- [x] red-adapter http (coverage: malformed Authorization bearer variants return HTTP 401)
- [x] green-adapter http (coverage: malformed Authorization bearer variants return HTTP 401)
- [x] green-acceptance

### 5.3 Logout with an invalid session token is rejected
- [S] red-acceptance (capability exists: LogoutController rejects invalid bearer tokens via InvalidCredentialsException before request validation, and LogoutUsecase already rejects unknown tokens)
- [S] design (existing implementation already satisfies the scenario; no new design decision required)
- [S] red-usecase (capability exists: LogoutUsecase rejects unknown token with InvalidCredentialsException)
- [S] green-usecase (no usecase change needed; invalid-token rejection already implemented)
- [S] adapters-discovery (no new adapter needed; usecase/HTTP boundary already handle invalid token path)
- [S] green-acceptance (acceptance behavior already covered by the current logout implementation; no new disabled acceptance test is required)

## Security Scenarios (05_Security_Tests.md)

### S1.1 Repeated failed login attempts are rate-limited
- [x] red-acceptance (failing acceptance check added and disabled after red validation: expected 429 Too many requests, actual 401 Invalid credentials)
- [x] design (design: track consecutive failed login attempts per username, throttle after the configured threshold, and surface HTTP 429 with error="Too many requests" via the login controller/exception handler)
- [x] red-usecase (refactor review: implementation intentionally missing; keep the scope test-only, preserve the strict assertion pattern, and do not introduce speculative production refactor before green implementation)
- [x] green-usecase
- [x] adapters-discovery
  - Check 1 (ports): http adapter — existing server composition already wires LoginUsecase to InMemoryUserPort and TokenSessionPort; no new port adapter is required
  - Check 2 (exceptions): LoginController/ExceptionHandler — RateLimitException is not yet mapped to HTTP 429, so the login boundary still needs explicit throttling error handling
  - Check 3 (response shape): LoginController — the success response shape is already correct for valid logins, but the throttled path still needs an error body with error="Too many requests" and the correct 429 status
- [x] red-adapter http (real HTTP validation: actual 401 Invalid credentials, expected 429 Too many requests; the controller/exception handler still treats the threshold breach as invalid credentials)
- [x] green-adapter http (map RateLimitException to HTTP 429 with error="Too many requests" and preserve the login success response shape for valid requests)
- [x] green-acceptance

### S2.1 Session token is not reusable after logout
- [S] red-acceptance (capability exists: scenario 5.1 logout_with_a_valid_session_token_destroys_the_session already validates that a logged-out token is rejected on reuse)
- [S] design (existing implementation already satisfies the requirement; no new design decision needed)
- [S] red-usecase (capability exists: logout usecase already rejects reused/unknown tokens during session close)
- [S] green-usecase (no additional usecase implementation required)
- [S] adapters-discovery (no new adapter behavior needed; the logout flow already handles token invalidation at the HTTP boundary)
- [S] green-acceptance (existing acceptance test covers the same behavior through the valid logout/reuse flow)

### S2.2 Session token is sufficiently random
- [x] red-acceptance (failing acceptance check added and disabled after red validation: current tokens are username-derived, with values like "ola-session-token" and "bob-session-token" containing the user names)
- [x] design (secure random token: generate a non-user-derived session identifier, keep the existing internal/external token translation, and validate it with distinct-user login checks)
- [x] red-usecase (session token generation is intentionally random and must not include the username; the usecase assertions now assert the token contract instead of a literal username-derived value)
- [x] green-usecase (generate a random, non-sequential token in the session port and keep the session map keyed by the random value)
- [x] adapters-discovery (no extra adapter ports required; the change is confined to session token generation and the existing login/logout mapping)
- [x] green-acceptance

### S2.3 An arbitrary token cannot access a protected endpoint
- [S] red-acceptance (capability exists: logout rejects unknown or malformed bearer tokens before protected actions run)
- [S] design (existing invalid-token rejection at the logout boundary already satisfies the requirement; no new design needed)
- [S] red-usecase (LogoutUsecase throws Authentication error when a token is not found in the session map)
- [S] green-usecase (existing guard already rejects arbitrary session tokens; no additional implementation required)
- [S] adapters-discovery (no new adapter port required; the logout flow already validates the bearer token)
- [S] green-acceptance (existing logout path rejects fabricated tokens with HTTP 401 before any action executes)

### S3.1 Excessively long username is rejected
- [x] red-acceptance (failing acceptance reproduces the issue: the long-name login currently returns HTTP 200 instead of HTTP 400 Validation error)
- [~] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### S3.2 Excessively long PIN is rejected
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### S4.1 PIN is not echoed in any response body or log
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

## Infrastructure Scenarios (04_Infrastructure_Tests.md)

### I1.1 Login fails gracefully when the log file cannot be written
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### I1.2 Service recovers and login succeeds after log storage is restored
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### I2.1 Login fails gracefully when user data cannot be read
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance
