# Brief Product Description

1. It's a simple ATM application that manages bank accounts for multiple users.
2. Users issue text commands (login, logout, deposit, withdraw, transfer, get balance, history) via a console interface or over a remote interface (HTTP REST API and TCP socket).
3. The main goal is to let users securely manage their account funds and view transaction history from any supported interface.
4. The underlying logic is:
   - A user authenticates with a PIN via `login`; a session is created and held until `logout`.
   - Authenticated users can query their balance and perform deposits, withdrawals, and transfers to other users.
   - Every financial operation is recorded as an immutable append-only entry in persistent storage (filesystem); no record is ever modified or deleted.
   - The same command set is available over a console interface, an HTTP REST API, and a TCP socket interface.
