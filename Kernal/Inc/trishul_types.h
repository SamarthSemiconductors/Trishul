#ifndef TRISHUL_TYPES_H
#define TRISHUL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*Trishul_TaskEntry)(void *argument);

typedef enum
{
    TRISHUL_OK = 0,
    TRISHUL_ERROR = -1,
    TRISHUL_INVALID_ARGUMENT = -2,
    TRISHUL_MAX_TASKS_REACHED = -3,
    TRISHUL_SCHEDULER_EMPTY = -4
} Trishul_Status;

typedef enum
{
    TRISHUL_TASK_READY = 0,
    TRISHUL_TASK_RUNNING,
    TRISHUL_TASK_BLOCKED,
    TRISHUL_TASK_SUSPENDED
} Trishul_TaskState;

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
