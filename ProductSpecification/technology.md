# Technology Profile

tech-profile:
  backend: go-stdlib

<!--
Available backend profiles for microservices:
  - cpp-cmake      (C++ / CMake)
  - python-django  (Python / Django)
  - python-fastapi (Python / FastAPI)
  - go-stdlib      (Go / standard library)

Set the `backend` key above to the profile that matches this service.
Each profile lives in `.claude/tech/{profile}/` with its own coding, tdd,
infrastructure bindings and code templates.
-->

## Backend

See the selected profile's bindings for the language/framework
conventions, idioms, and commands:

- `.claude/tech/{backend}/coding.md` — coding idioms
- `.claude/tech/{backend}/tdd.md` — TDD patterns
- `.claude/tech/{backend}/infrastructure.md` — build/run/test commands

## Conventions

Operational values (test disable marker, not-implemented marker, run/test
commands, coverage report path, health endpoint) are declared in the
selected profile's binding files. Skills and agents resolve commands from
those files rather than from this document.
