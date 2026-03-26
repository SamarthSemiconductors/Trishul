#ifndef TRISHUL_H
#define TRISHUL_H

#include "trishul_types.h"
#include "trishul_uart.h"

/*
 * Reset the RTOS state to a known power-on style condition.
 *
 * What it does:
 * - Clears the internal task table.
 * - Resets the global tick counter to 0.
 * - Marks the scheduler as not started.
 * - Clears the current-task and next-task pointers used by the port layer.
 *
 * When to call it:
 * - Once during system startup before creating any tasks.
 *
 * Important notes:
 * - This function does not configure hardware clocks, SysTick, or interrupts.
 * - This function does not allocate memory.
 */
void Trishul_Init(void);

/*
 * Create a new task using memory that the application provides.
 *
 * Parameters:
 * - tcb: Pointer to the Task Control Block that will hold the task metadata.
 * - name: Optional human-readable task name used for debugging or inspection.
 * - entry: Task entry function. This function must never return.
 * - argument: User argument passed to the entry function when the task starts.
 * - stack_buffer: Pointer to the task stack storage supplied by the application.
 * - stack_words: Size of stack_buffer in 32-bit words, not bytes.
 *
 * What it does:
 * - Validates the input pointers and minimum stack size.
 * - Fills in the TCB fields.
 * - Builds the initial stack frame so the task can start after a context restore.
 * - Adds the task to the scheduler's task table in READY state.
 *
 * Return values:
 * - TRISHUL_OK: Task was created successfully.
 * - TRISHUL_INVALID_ARGUMENT: One or more inputs were invalid.
 * - TRISHUL_MAX_TASKS_REACHED: The configured task table is already full.
 *
 * Important notes:
 * - Tasks are statically allocated. The RTOS does not allocate stack memory.
 * - The created task will not run until Trishul_Start() is called.
 */
Trishul_Status Trishul_TaskCreate(Trishul_Tcb *tcb,
                                  const char *name,
                                  Trishul_TaskEntry entry,
                                  void *argument,
                                  uint32_t *stack_buffer,
                                  size_t stack_words);

/*
 * Start the scheduler and launch the first runnable task.
 *
 * Parameters:
 * - cpu_clock_hz: Actual core clock frequency used by SysTick configuration.
 *
 * What it does:
 * - Selects the first READY task.
 * - Marks the scheduler as running.
 * - Configures the Cortex-M port layer and SysTick timer.
 * - Triggers the context restore sequence that starts the first task.
 *
 * Return values:
 * - TRISHUL_OK: Scheduler startup sequence was accepted.
 * - TRISHUL_INVALID_ARGUMENT: No valid clock was provided or no tasks exist.
 * - TRISHUL_SCHEDULER_EMPTY: No runnable task could be selected.
 *
 * Important notes:
 * - cpu_clock_hz must match the real MCU clock. If it is wrong, task delays and
 *   scheduler timing will also be wrong.
 * - On success this function normally does not return, because execution moves
 *   into the first scheduled task.
 */
Trishul_Status Trishul_Start(uint32_t cpu_clock_hz);

/*
 * Ask the scheduler to switch away from the current task.
 *
 * What it does:
 * - Requests a PendSV-based context switch if the scheduler is already running.
 *
 * Typical use:
 * - Call it when a task has finished a chunk of work and wants to give other
 *   READY tasks a chance to run.
 *
 * Important notes:
 * - If the scheduler has not started yet, this function does nothing.
 * - This is a cooperative yield request. The actual switch happens through the
 *   Cortex-M exception mechanism.
 */
void Trishul_Yield(void);

/*
 * Block the current task for a number of RTOS ticks.
 *
 * Parameters:
 * - ticks: Number of scheduler ticks to wait before the task becomes READY again.
 *
 * What it does:
 * - Records the tick at which the task should wake up.
 * - Changes the task state to BLOCKED.
 * - Requests a context switch so another task can run.
 *
 * Typical use:
 * - Periodic tasks such as blinking an LED, polling a sensor, or pacing work.
 *
 * Important notes:
 * - If ticks is 0, this function does nothing.
 * - Delay duration depends on TRISHUL_TICK_HZ and the real CPU clock passed to
 *   Trishul_Start().
 */
void Trishul_Delay(uint32_t ticks);

/*
 * Return the current RTOS tick count.
 *
 * What it does:
 * - Returns the internal scheduler tick value incremented by SysTick.
 *
 * Typical use:
 * - Time measurements, debugging, or implementing periodic logic.
 *
 * Important notes:
 * - The counter is a 32-bit value and will eventually wrap around.
 */
uint32_t Trishul_GetTickCount(void);

/*
 * Return a pointer to the TCB of the task that is currently running.
 *
 * What it does:
 * - Exposes the currently selected task as tracked by the scheduler.
 *
 * Typical use:
 * - Debugging, tracing, or simple task-aware helper code.
 *
 * Important notes:
 * - The returned pointer is owned by the RTOS/application and must not be freed.
 * - It may be NULL before the scheduler starts.
 */
Trishul_Tcb *Trishul_GetCurrentTask(void);

/*
 * Advance the RTOS tick and wake tasks whose delays have expired.
 *
 * What it does:
 * - Increments the internal tick counter.
 * - Moves expired delayed tasks from BLOCKED to READY.
 * - Requests a context switch when runnable work is available.
 *
 * Important notes:
 * - This is intended to be called from the SysTick exception handler.
 * - Application tasks should not call this directly.
 */
void Trishul_TickHandler(void);

/*
 * Scheduler-side PendSV service routine.
 *
 * What it does:
 * - Selects the next runnable task.
 * - Updates the next-task pointer used by the assembly context switch routine.
 *
 * Important notes:
 * - This is part of the Cortex-M port glue.
 * - Application tasks should not call this directly.
 */
void Trishul_PendSvHandler(void);

/*
 * Scheduler-side SVC service routine.
 *
 * What it does:
 * - Reserved entry point for supervisor-call based services.
 *
 * Important notes:
 * - In the current implementation this is a placeholder for future privileged
 *   services and does not perform application-visible work.
 */
void Trishul_SvcHandler(void);

/*
 * Pointer to the task that is currently executing.
 *
 * Important notes:
 * - Exported for the Cortex-M port assembly.
 * - Applications may inspect it for debugging, but should not modify it.
 */
extern Trishul_Tcb *g_trishul_current_tcb;

/*
 * Pointer to the task that the next context switch will restore.
 *
 * Important notes:
 * - Exported for the Cortex-M port assembly.
 * - Applications may inspect it for debugging, but should not modify it.
 */
extern Trishul_Tcb *g_trishul_next_tcb;

#endif
