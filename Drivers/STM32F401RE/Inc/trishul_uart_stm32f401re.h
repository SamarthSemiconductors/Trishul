#ifndef TRISHUL_UART_STM32F401RE_H
#define TRISHUL_UART_STM32F401RE_H

#include <stdint.h>

#include "trishul_types.h"

Trishul_Status Trishul_UartStm32f401reInit(uint32_t peripheral_clock_hz, uint32_t baud_rate);
Trishul_Status Trishul_UartStm32f401reWriteByte(uint8_t byte);

#endif
