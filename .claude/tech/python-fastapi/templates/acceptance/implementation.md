# Acceptance Test Implementation (Green Phase) -- Python/FastAPI

> Universal workflow: `.claude/templates/tdd/green-acceptance.md`

## Tech-Specific Details

- **Disable marker**: `@pytest.mark.skip` decorator — remove to enable, re-add on failure
- **Test target**: `{test_file_name}` (file name for pytest `-k` filter)
- **Error terminology**: "exception handler" (FastAPI `app.exception_handler(...)`)
