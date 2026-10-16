# Withdraw Funds - API Endpoints

| Method | Path               | Description                                              |
|--------|--------------------|----------------------------------------------------------|
| POST   | /api/v1/withdraw   | Withdraw funds from the authenticated user’s account     |

## Notes

- `POST /api/v1/withdraw` requires `Authorization: Bearer <token>`.
- The request body contains a single numeric `amount` value to subtract from the authenticated user’s balance.
- The amount must be positive and not exceed the available balance for the caller.
- If the user is unauthenticated, the token is invalid, or the amount is not allowed, the request fails with `401` or `400` and an error body.
- On success, the API returns the updated balance for the authenticated user.
