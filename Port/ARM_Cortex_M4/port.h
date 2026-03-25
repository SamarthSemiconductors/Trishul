#ifndef TRISHUL_PORT_H
#define TRISHUL_PORT_H

#include <stddef.h>
#include <stdint.h>

#include "trishul_types.h"

uint32_t *Trishul_PortInitStack(uint32_t *stack_buffer,
                                size_t stack_words,
                                Trishul_TaskEntry entry,
                                void *argument);
void Trishul_PortSetup(uint32_t cpu_clock_hz, uint32_t tick_hz);
void Trishul_PortStartFirstTask(void);
void Trishul_PortRequestContextSwitch(void);

#endif
