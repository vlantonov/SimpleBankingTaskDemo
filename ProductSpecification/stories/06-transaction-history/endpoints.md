# Transaction History - API Endpoints

| Method | Path              | Description                                              |
|--------|-------------------|----------------------------------------------------------|
| GET    | /api/v1/history   | Retrieve the authenticated user’s immutable transaction history |

## Notes

- `GET /api/v1/history` requires `Authorization: Bearer <token>`.
- The endpoint returns only the authenticated user’s ledger entries.
- Entries are returned newest-first to make recent activity easy to review.
- If the user is unauthenticated, the request fails with `401` and an error body.
- If the user has no history, the response is an empty array.
