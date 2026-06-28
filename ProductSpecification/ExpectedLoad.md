# Expected Load

- Multi-user application; each user has their own account and PIN.
- Low concurrent load: demo/development scale, expected single-digit concurrent sessions.
- No hard data volume limit, but account history is bounded to the lifetime of the demo deployment.
- Request rate: up to ~10 requests/second across all interfaces combined.
