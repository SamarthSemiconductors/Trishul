#include "trishul_uart.h"

#include "trishul_uart_stm32f401re.h"

Trishul_Status Trishul_UartInit(uint32_t peripheral_clock_hz, uint32_t baud_rate)
{
    if ((peripheral_clock_hz == 0U) || (baud_rate == 0U))
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    return Trishul_UartStm32f401reInit(peripheral_clock_hz, baud_rate);
}

Trishul_Status Trishul_UartWriteByte(uint8_t byte)
{
    return Trishul_UartStm32f401reWriteByte(byte);
}

Trishul_Status Trishul_UartWrite(const uint8_t *data, size_t length)
{
    size_t index;

    if ((data == NULL) && (length > 0U))
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    for (index = 0U; index < length; ++index)
    {
        Trishul_Status status = Trishul_UartWriteByte(data[index]);

        if (status != TRISHUL_OK)
        {
            return status;
        }
    }

    return TRISHUL_OK;
}

Trishul_Status Trishul_UartWriteString(const char *string)
{
    if (string == NULL)
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    while (*string != '\0')
    {
        Trishul_Status status = Trishul_UartWriteByte((uint8_t)*string);

        if (status != TRISHUL_OK)
        {
            return status;
        }

        ++string;
    }

    return TRISHUL_OK;
}
