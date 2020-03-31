#include "rt.h"

enum plaftorm_task_state
{
    TASK_NONE,
    TASK_ACTIVE,
    TASK_SLEEP,
};

struct plaftorm_task
{
    task_id id;
    plaftorm_task_state state;
    rt_task_func func;
    void *arg;
    int32_t delay;
    int32_t time;
};

const task_id RT_UNKNOWN_TASK = 0xFF;
const size_t RT_MAX_TASKS = 16;

plaftorm_task plaftorm_tasks[RT_MAX_TASKS] = {};
plaftorm_task *plaftorm_current_task;

task_id rt_create_task(rt_task_func func, void* arg)
{
    for (size_t i = 0; i < RT_MAX_TASKS; i++)
    {
        if (plaftorm_tasks[i].state == TASK_NONE)
        {
            plaftorm_tasks[i].id = i + 1;
            plaftorm_tasks[i].state = TASK_ACTIVE;
            plaftorm_tasks[i].func = func;
            plaftorm_tasks[i].arg = arg;
            plaftorm_tasks[i].delay = 0;
            plaftorm_tasks[i].time = 0;
            return plaftorm_tasks[i].id;
        }
    }

    rt_log(F("ERROR! rt_create_task(): no free task slots"));
    return RT_UNKNOWN_TASK;
}

void rt_set_delay(int32_t ms)
{
    if (plaftorm_current_task == nullptr)
    {
        rt_log(F("ERROR! rt_set_delay(): no context"));
        return;
    }

    plaftorm_current_task->state = TASK_SLEEP;
    plaftorm_current_task->delay = ms;
    plaftorm_current_task->time = millis();
}

task_id rt_current_task_id()
{
    if (plaftorm_current_task == nullptr)
    {
        return RT_UNKNOWN_TASK;
    }

    return plaftorm_current_task->id;
}

void rt_destroy_task(task_id id)
{
    for (size_t i = 0; i < RT_MAX_TASKS; i++)
    {
        if (plaftorm_tasks[i].id == id && plaftorm_tasks[i].state != TASK_NONE)
        {
            plaftorm_tasks[i].id = 0;
            plaftorm_tasks[i].state = TASK_NONE;
            plaftorm_tasks[i].func = nullptr;
            plaftorm_tasks[i].delay = 0;
            plaftorm_tasks[i].time = 0;
            return;
        }
    }

    rt_logf(F("ERROR! rt_destroy_task(): task #%d doesn't exist"), id);
}

void rt_invoke_task(plaftorm_task &task)
{
    plaftorm_current_task = &task;
    task.func(task.arg);
    plaftorm_current_task = nullptr;
}

void rt_run_task(plaftorm_task &task)
{
    switch (task.state)
    {
    case TASK_ACTIVE:
        rt_invoke_task(task);
        break;

    case TASK_SLEEP:
    {
        int32_t time = millis();
        int32_t delta = time - task.time;
        task.delay -= delta;
        task.time = time;

        if (task.delay <= 0)
        {
            task.state = TASK_ACTIVE;
            task.delay = 0;
            task.time = 0;

            rt_invoke_task(task);
        }
    }
    break;

    default:
        break;
    }
}

void rt_scheduler_loop()
{
    for (size_t i = 0; i < RT_MAX_TASKS; i++)
    {
        rt_run_task(plaftorm_tasks[i]);
    }
}
