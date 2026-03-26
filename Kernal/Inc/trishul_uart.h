#ifndef TRISHUL_UART_H
#define TRISHUL_UART_H

#include <stddef.h>
#include <stdint.h>

#include "trishul_types.h"

/*
 * Initialize the public UART interface for the selected board backend.
 *
 * Parameters:
 * - peripheral_clock_hz: Clock frequency driving the UART peripheral.
 * - baud_rate: Desired UART baud rate such as 9600 or 115200.
 *
 * What it does:
 * - Validates the inputs.
 * - Delegates the hardware setup to the target-specific UART driver.
 *
 * Return values:
 * - TRISHUL_OK: UART was configured successfully.
 * - TRISHUL_INVALID_ARGUMENT: A clock or baud value was invalid.
 *
 * Important notes:
 * - The clock must match the UART peripheral bus clock, not necessarily the CPU
 *   core clock.
 * - In the current STM32F401RE backend this configures USART2 on PA2/PA3.
 */
Trishul_Status Trishul_UartInit(uint32_t peripheral_clock_hz, uint32_t baud_rate);

/*
 * Transmit a single byte over UART.
 *
 * Parameters:
 * - byte: The raw 8-bit value to send.
 *
 * What it does:
 * - Waits until the UART transmit register is ready.
 * - Writes one byte to the hardware data register.
 *
 * Return values:
 * - TRISHUL_OK: The byte was accepted for transmission.
 *
 * Important notes:
 * - This is a blocking call in the current implementation.
 * - It is suitable for simple debug prints, but not ideal for high-throughput
 *   communication without buffering or interrupt/DMA support.
 */
Trishul_Status Trishul_UartWriteByte(uint8_t byte);

/*
 * Transmit a buffer of bytes over UART.
 *
 * Parameters:
 * - data: Pointer to the bytes to send.
 * - length: Number of bytes to send from the buffer.
 *
 * What it does:
 * - Iterates through the buffer and sends each byte with Trishul_UartWriteByte().
 *
 * Return values:
 * - TRISHUL_OK: Entire buffer was transmitted.
 * - TRISHUL_INVALID_ARGUMENT: data was NULL while length was non-zero.
 *
 * Important notes:
 * - This is a blocking API.
 * - Passing length = 0 is valid and results in no transmission.
 */
Trishul_Status Trishul_UartWrite(const uint8_t *data, size_t length);

/*
 * Transmit a null-terminated C string over UART.
 *
 * Parameters:
 * - string: Pointer to a null-terminated string.
 *
 * What it does:
 * - Sends characters one by one until the terminating '\\0' is reached.
 *
 * Return values:
 * - TRISHUL_OK: Entire string was transmitted.
 * - TRISHUL_INVALID_ARGUMENT: string was NULL.
 *
 * Important notes:
 * - This function does not automatically append '\\r' or '\\n'.
 * - Use an explicit string such as "Hello\\r\\n" when terminal line endings are
 *   needed.
 */
Trishul_Status Trishul_UartWriteString(const char *string);

#endif
