# Deposit Funds - API Endpoints

| Method | Path               | Description                                            |
|--------|--------------------|--------------------------------------------------------|
| POST   | /api/v1/deposit    | Create a deposit for the authenticated user             |

## Notes

- `POST /api/v1/deposit` requires `Authorization: Bearer <token>`.
- The request body contains a single numeric `amount` value to add to the authenticated user’s balance.
- If the user is unauthenticated or the amount is invalid, the request fails with `401` or `400` and an error body.
- On success, the API returns the updated balance for the authenticated user.
