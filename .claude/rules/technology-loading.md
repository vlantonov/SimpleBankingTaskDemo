# Technology Loading

## Profile Resolution

The active technology profile is declared in `ProductSpecification/technology.md` via the `tech-profile:` block. Microservices have a single concern — `backend` — pointing to a tech profile directory.

```yaml
tech-profile:
  backend: {backend-profile}
```

The `backend` value resolves to `.claude/tech/{backend-profile}/`.

## Concern-to-Layer Mapping

All layers and skills resolve to the single `backend` concern:

| Layer / Skill context | Concern key |
|-----------------------|-------------|
| Domain, usecase, application, HTTP/gRPC adapters, DB adapters, message/email adapters, scheduling | `backend` |
| Acceptance tests (black-box API) | `backend` |

## Directory Structure

```
ProductSpecification/technology.md          # Declares the backend tech-profile
.claude/tech/
  {backend-profile}/                        # One directory per available profile
    coding.md                               # Language/framework coding conventions
    tdd.md                                  # Tech-specific TDD patterns
    infrastructure.md                       # Build/run/deploy config
    templates/                              # Tech-specific code scaffolds
      {layer}/                              # One subdirectory per adapter/layer
```

The exact files and template subdirectories vary per profile — inspect `.claude/tech/{backend-profile}/` to see what it provides.

## How Agents and Skills Load Tech Bindings

1. **Read profile**: Read `ProductSpecification/technology.md`, extract the `backend` value from the `tech-profile:` block.
2. **Load tech rules**: Read `.claude/tech/{backend}/{binding}.md` for the relevant binding (`coding`, `tdd`, `infrastructure`).
3. **Resolve templates**: Template paths are `.claude/tech/{backend}/templates/{template-dir}/`.

## Conventions Table

Each profile's `coding.md` / `infrastructure.md` includes a **Conventions** section with key operational values (test disable marker, not-implemented marker, run/test commands, coverage report path). Skills that dispatch commands or check markers should read from the profile's binding files rather than hardcoding values.

## Adding a New Tech Profile

To support a new language or framework:
1. Create `.claude/tech/{profile-name}/` with `coding.md`, `tdd.md`, `infrastructure.md`, and `templates/`.
2. Set `backend: {profile-name}` in `ProductSpecification/technology.md`.
3. No changes needed to universal rules, agents, or skills — they resolve via the single `backend` concern.

## Available Profiles

| Profile | Language / Framework |
|---------|----------------------|
| `cpp-cmake`      | C++ / CMake |
| `python-django`  | Python / Django |
| `python-fastapi` | Python / FastAPI |
| `go-stdlib`      | Go / standard library |
