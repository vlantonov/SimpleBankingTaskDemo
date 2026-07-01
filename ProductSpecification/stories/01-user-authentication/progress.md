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
- [~] green-adapter http (coverage: preserve non-JWT token values in LoginController response mapping)
- [ ] green-acceptance

### 4.1 Second login while a session is already active is rejected
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### 5.1 Logout with a valid session token destroys the session
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### 5.2 Logout without a session token is rejected
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### 5.3 Logout with an invalid session token is rejected
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

## Security Scenarios (05_Security_Tests.md)

### S1.1 Repeated failed login attempts are rate-limited
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### S2.1 Session token is not reusable after logout
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### S2.2 Session token is sufficiently random
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### S2.3 An arbitrary token cannot access a protected endpoint
- [ ] red-acceptance
- [ ] design
- [ ] red-usecase
- [ ] green-usecase
- [ ] adapters-discovery
- [ ] green-acceptance

### S3.1 Excessively long username is rejected
- [ ] red-acceptance
- [ ] design
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
