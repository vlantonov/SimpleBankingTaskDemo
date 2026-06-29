# ADR: First login orchestration for scenario 2.1

## Status
Accepted

## Context
Scenario 2.1 requires that the first login for a missing user returns success with a welcome message and a session token.

The project follows clean architecture boundaries:
- domain holds business rules and validation
- usecase orchestrates the user-visible operation
- adapters implement storage/session interfaces

Two approaches were considered:
1. Ports+Domain orchestration in usecase
2. Single composite storage call that performs find-or-create and session creation

## Decision
Use **Ports+Domain orchestration in the usecase**.

The 2.1 usecase will:
1. validate login request values through domain objects
2. query user by username
3. create user when absent
4. open a session and obtain token through a session port
5. return success response with `Welcome, {user}` and token

Business decisions stay in domain/usecase, while adapters only execute persistence/session mechanics.

## Consequences
Positive:
- Preserves clean architecture boundaries
- Keeps scenario logic explicit and testable in usecase tests
- Fits upcoming scenario 2.2 event logging extension without moving logic out of usecase

Trade-offs:
- Introduces additional port/DTO wiring compared with a single composite storage call

## Follow-up
- Implement in `red-usecase` and `green-usecase` for scenario 2.1
- Revisit only if later scenarios require atomically coupled persistence behavior not expressible via current ports
