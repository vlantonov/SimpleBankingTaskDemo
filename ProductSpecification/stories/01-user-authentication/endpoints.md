# User Authentication - API Endpoints

| Method | Path              | Description                              |
|--------|-------------------|------------------------------------------|
| POST   | /api/v1/login     | Authenticate user (auto-create on first login), returns session token |
| POST   | /api/v1/logout    | Dispose the current session              |

## Notes

- Session token is returned in the response body on successful login; clients must include it as `Authorization: Bearer <token>` on all subsequent authenticated requests.
- `POST /api/v1/logout` requires the `Authorization: Bearer <token>` header; returns 401 if no active session matches the token.
- Both endpoints behave identically for new and returning users from the caller's perspective — the auto-creation of a new user is an internal implementation detail.
