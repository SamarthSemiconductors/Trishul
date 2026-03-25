#include "trishul.h"

#include "trishul_config.h"
#include "trishul_internal.h"
#include "port.h"

static Trishul_Tcb *s_task_table[TRISHUL_MAX_TASKS];
static size_t s_task_count;
static uint32_t s_tick_count;
static bool s_scheduler_started;

Trishul_Tcb *g_trishul_current_tcb;
Trishul_Tcb *g_trishul_next_tcb;

static void Trishul_WakeExpiredTasks(void)
{
    size_t i;

    for (i = 0; i < s_task_count; ++i)
    {
        Trishul_Tcb *task = s_task_table[i];

        if ((task->state == TRISHUL_TASK_BLOCKED) &&
            ((int32_t)(s_tick_count - task->delay_until_tick) >= 0))
        {
            task->state = TRISHUL_TASK_READY;
        }
    }
}

void Trishul_Init(void)
{
    size_t i;

    for (i = 0; i < TRISHUL_MAX_TASKS; ++i)
    {
        s_task_table[i] = NULL;
    }

    s_task_count = 0U;
    s_tick_count = 0U;
    s_scheduler_started = false;
    g_trishul_current_tcb = NULL;
    g_trishul_next_tcb = NULL;
}

Trishul_Status Trishul_TaskCreate(Trishul_Tcb *tcb,
                                  const char *name,
                                  Trishul_TaskEntry entry,
                                  void *argument,
                                  uint32_t *stack_buffer,
                                  size_t stack_words)
{
    if ((tcb == NULL) || (entry == NULL) || (stack_buffer == NULL) || (stack_words < 32U))
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    if (s_task_count >= TRISHUL_MAX_TASKS)
    {
        return TRISHUL_MAX_TASKS_REACHED;
    }

    tcb->stack_base = stack_buffer;
    tcb->stack_words = stack_words;
    tcb->entry = entry;
    tcb->argument = argument;
    tcb->name = name;
    tcb->delay_until_tick = 0U;
    tcb->run_count = 0U;
    tcb->state = TRISHUL_TASK_READY;
    tcb->sp = Trishul_PortInitStack(stack_buffer, stack_words, entry, argument);

    s_task_table[s_task_count] = tcb;
    ++s_task_count;

    return TRISHUL_OK;
}

Trishul_Status Trishul_Start(uint32_t cpu_clock_hz)
{
    if ((s_task_count == 0U) || (cpu_clock_hz == 0U))
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    g_trishul_current_tcb = Trishul_SchedulerSelectNext();

    if (g_trishul_current_tcb == NULL)
    {
        return TRISHUL_SCHEDULER_EMPTY;
    }

    Trishul_SchedulerMarkRunning(g_trishul_current_tcb);
    g_trishul_next_tcb = g_trishul_current_tcb;
    s_scheduler_started = true;

    Trishul_PortSetup(cpu_clock_hz, TRISHUL_TICK_HZ);
    Trishul_PortStartFirstTask();
    return TRISHUL_OK;
}

void Trishul_Yield(void)
{
    if (!s_scheduler_started)
    {
        return;
    }

    Trishul_PortRequestContextSwitch();
}

void Trishul_Delay(uint32_t ticks)
{
    if ((!s_scheduler_started) || (g_trishul_current_tcb == NULL) || (ticks == 0U))
    {
        return;
    }

    g_trishul_current_tcb->delay_until_tick = s_tick_count + ticks;
    g_trishul_current_tcb->state = TRISHUL_TASK_BLOCKED;
    Trishul_PortRequestContextSwitch();
}

uint32_t Trishul_GetTickCount(void)
{
    return s_tick_count;
}

Trishul_Tcb *Trishul_GetCurrentTask(void)
{
    return g_trishul_current_tcb;
}

Trishul_Tcb *Trishul_SchedulerSelectNext(void)
{
    size_t current_index = s_task_count - 1U;
    size_t offset;

    if (s_task_count == 0U)
    {
        return NULL;
    }

    if (g_trishul_current_tcb != NULL)
    {
        size_t i;

        for (i = 0; i < s_task_count; ++i)
        {
            if (s_task_table[i] == g_trishul_current_tcb)
            {
                current_index = i;
                break;
            }
        }
    }

    for (offset = 1U; offset <= s_task_count; ++offset)
    {
        size_t candidate_index = (current_index + offset) % s_task_count;
        Trishul_Tcb *candidate = s_task_table[candidate_index];

        if ((candidate != NULL) && (candidate->state == TRISHUL_TASK_READY))
        {
            return candidate;
        }
    }

    if ((g_trishul_current_tcb != NULL) && (g_trishul_current_tcb->state == TRISHUL_TASK_RUNNING))
    {
        return g_trishul_current_tcb;
    }

    return NULL;
}

void Trishul_SchedulerMarkRunning(Trishul_Tcb *task)
{
    size_t i;

    for (i = 0; i < s_task_count; ++i)
    {
        if (s_task_table[i] != NULL)
        {
            if ((s_task_table[i] != task) && (s_task_table[i]->state == TRISHUL_TASK_RUNNING))
            {
                s_task_table[i]->state = TRISHUL_TASK_READY;
            }
        }
    }

    if (task != NULL)
    {
        task->state = TRISHUL_TASK_RUNNING;
        ++task->run_count;
    }
}

bool Trishul_SchedulerHasReadyTask(void)
{
    size_t i;

    for (i = 0; i < s_task_count; ++i)
    {
        if ((s_task_table[i] != NULL) &&
            (s_task_table[i]->state == TRISHUL_TASK_READY))
        {
            return true;
        }
    }

    return false;
}

void Trishul_TickHandler(void)
{
    ++s_tick_count;
    Trishul_WakeExpiredTasks();

    if (Trishul_SchedulerHasReadyTask())
    {
        Trishul_PortRequestContextSwitch();
    }
}

void Trishul_PendSvHandler(void)
{
    Trishul_Tcb *next = Trishul_SchedulerSelectNext();

    if (next == NULL)
    {
        g_trishul_next_tcb = g_trishul_current_tcb;
        return;
    }

    g_trishul_next_tcb = next;
    Trishul_SchedulerMarkRunning(next);
}

void Trishul_SvcHandler(void)
{
    /* Reserved for future privileged services. */
}

void Trishul_TaskExitError(void)
{
    for (;;)
    {
        /* A task should not return. Replace with board panic logic later. */
    }
}
