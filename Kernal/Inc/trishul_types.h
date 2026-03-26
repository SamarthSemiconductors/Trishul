#ifndef TRISHUL_TYPES_H
#define TRISHUL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Function pointer type used for RTOS tasks.
 *
 * Each task entry receives one user-supplied argument and is expected to run
 * forever. A task function should not return.
 */
typedef void (*Trishul_TaskEntry)(void *argument);

/*
 * Status codes returned by public Trishul APIs.
 *
 * Meanings:
 * - TRISHUL_OK: Operation completed successfully.
 * - TRISHUL_ERROR: Generic failure code reserved for broader use.
 * - TRISHUL_INVALID_ARGUMENT: One or more parameters were invalid.
 * - TRISHUL_MAX_TASKS_REACHED: No free task slots remain in the static task table.
 * - TRISHUL_SCHEDULER_EMPTY: Scheduler could not find a runnable task.
 */
typedef enum
{
    TRISHUL_OK = 0,
    TRISHUL_ERROR = -1,
    TRISHUL_INVALID_ARGUMENT = -2,
    TRISHUL_MAX_TASKS_REACHED = -3,
    TRISHUL_SCHEDULER_EMPTY = -4
} Trishul_Status;

/*
 * Scheduler-visible state of a task.
 *
 * States:
 * - TRISHUL_TASK_READY: Task can be selected to run.
 * - TRISHUL_TASK_RUNNING: Task is the current running task.
 * - TRISHUL_TASK_BLOCKED: Task is waiting for a delay to expire.
 * - TRISHUL_TASK_SUSPENDED: Reserved for future suspend/resume support.
 */
typedef enum
{
    TRISHUL_TASK_READY = 0,
    TRISHUL_TASK_RUNNING,
    TRISHUL_TASK_BLOCKED,
    TRISHUL_TASK_SUSPENDED
} Trishul_TaskState;

/*
 * Task Control Block used by the scheduler and port layer.
 *
 * Field overview:
 * - sp: Saved stack pointer used during context switching.
 * - stack_base: Start of the stack memory owned by this task.
 * - stack_words: Stack size expressed in 32-bit words.
 * - entry: Task entry function.
 * - argument: User argument passed into the task entry.
 * - name: Optional human-readable task name.
 * - delay_until_tick: Tick count at which a blocked task should wake up.
 * - run_count: Simple execution counter incremented when the scheduler runs it.
 * - state: Current scheduler state of the task.
 *
 * Important notes:
 * - The application owns the storage for this structure.
 * - The RTOS updates several fields internally after task creation.
 */
typedef struct Trishul_Tcb
{
    uint32_t *sp;
    uint32_t *stack_base;
    size_t stack_words;
    Trishul_TaskEntry entry;
    void *argument;
    const char *name;
    uint32_t delay_until_tick;
    uint32_t run_count;
    Trishul_TaskState state;
} Trishul_Tcb;

#endif
