# Trishul Roadmap

## Current starter scope

The starter code intentionally solves only the smallest useful RTOS slice:

- static task creation
- one task control block per task
- round-robin task selection
- tick-based blocking with `Trishul_Delay()`
- Cortex-M4 exception-driven context switching

## Bring-up checklist

1. Hook `SysTick_Handler`, `PendSV_Handler`, and `SVC_Handler` into your startup file.
2. Make sure the port assembly file is assembled in Thumb mode.
3. Link the kernel and port sources into your board project.
4. Confirm the CPU clock passed to `Trishul_Start()` matches the real core clock.
5. Test with two visible tasks before adding any more kernel features.

## Recommended next features

1. Fix the first-task startup path if your startup code uses a different exception return flow.
2. Add an explicit idle task helper so every project has a safe fallback task.
3. Add task priorities and a ready bitmap.
4. Add critical section helpers for ISR-safe kernel data access.
5. Add semaphores and queues after scheduler stability is proven.
