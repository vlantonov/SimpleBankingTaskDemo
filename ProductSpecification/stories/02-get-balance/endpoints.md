# Get Balance - API Endpoints

| Method | Path             | Description                                  |
|--------|------------------|----------------------------------------------|
| GET    | /api/v1/balance  | Return the authenticated user's current balance |

## Notes

- `GET /api/v1/balance` requires `Authorization: Bearer <token>`.
- The token must belong to an active session for the caller.
- If the user is unauthenticated or the token is invalid, the request fails with `401` and an error body.
- The response contains only the current balance value; the operation is read-only and does not mutate account state.
