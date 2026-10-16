# Transfer Funds - API Endpoints

| Method | Path               | Description                                              |
|--------|--------------------|----------------------------------------------------------|
| POST   | /api/v1/transfer   | Transfer funds from the authenticated user to another user |

## Notes

- `POST /api/v1/transfer` requires `Authorization: Bearer <token>`.
- The request body contains destination user identifier and positive transfer amount.
- Source and destination users must be different, and the source user must have sufficient balance.
- If the user is unauthenticated, destination is invalid/missing, or amount is invalid, the request fails with `401`, `404`, or `400`.
- On success, the API returns updated balances for both source and destination accounts.
