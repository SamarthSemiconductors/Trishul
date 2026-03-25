#ifndef TRISHUL_H
#define TRISHUL_H

#include "trishul_types.h"

void Trishul_Init(void);
Trishul_Status Trishul_TaskCreate(Trishul_Tcb *tcb,
                                  const char *name,
                                  Trishul_TaskEntry entry,
                                  void *argument,
                                  uint32_t *stack_buffer,
                                  size_t stack_words);
Trishul_Status Trishul_Start(uint32_t cpu_clock_hz);
void Trishul_Yield(void);
void Trishul_Delay(uint32_t ticks);
uint32_t Trishul_GetTickCount(void);
Trishul_Tcb *Trishul_GetCurrentTask(void);

/* Called from the Cortex-M exception handlers. */
void Trishul_TickHandler(void);
void Trishul_PendSvHandler(void);
void Trishul_SvcHandler(void);

/* Exported so the port assembly can save and restore task stack pointers. */
extern Trishul_Tcb *g_trishul_current_tcb;
extern Trishul_Tcb *g_trishul_next_tcb;

#endif
