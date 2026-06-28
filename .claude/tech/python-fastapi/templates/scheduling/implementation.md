# Scheduling Adapter Implementation Template -- Python/FastAPI

## Rules

- Jobs are APScheduler `AsyncIOScheduler` jobs registered alongside the FastAPI app (start in a `@app.on_event("startup")` / lifespan handler). Celery + `celery-beat` is an acceptable alternative if a broker is already in use.
- Jobs delegate immediately to a usecase — no business logic in the job
- Schedule expressions and lock durations come from `pydantic-settings` (env-backed)
- Use a Postgres advisory lock (or `redis-py` `Lock`) for distributed locking in multi-instance deployments

## Reference (read before generating)

- Existing job: `backend/adapters/scheduling/src/cleanup_job.py`
- Config: `backend/adapters/scheduling/src/scheduling_config.py`
- Usecase port: `backend/usecase/src/{feature}/{feature}_usecase.py`

## Pattern (APScheduler)

1. Create job class with constructor injection of the usecase
2. Register the schedule in the app lifespan handler: `scheduler.add_job(job.execute, CronTrigger.from_crontab(config.cron))`
3. Wrap execution in a distributed lock for multi-instance safety
4. Delegate to usecase: `await self.usecase.execute()`

## Pattern (Celery, if applicable)

1. Define `@shared_task` function (or class-based task)
2. Resolve dependencies from the application factory inside the task body
3. Add a `celery-beat` schedule entry (config file or RedBeat)
4. Add a distributed lock (Redis-based) for multi-instance safety
5. Delegate to usecase: `usecase.execute()`

## Configuration

Add to `pydantic-settings`:
```python
class SchedulingSettings(BaseSettings):
    cleanup_cron: str = "0 3 * * *"      # daily at 3 AM
    cleanup_lock_duration: int = 300     # 5 minutes
```

## Key Paths

- Jobs: `backend/adapters/scheduling/src/`
- Config: `backend/adapters/scheduling/src/scheduling_config.py`
- Usecase ports: `backend/usecase/src/`
