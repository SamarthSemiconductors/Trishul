#include "trishul_uart_stm32f401re.h"

#define GPIOA_MODER     (*(volatile uint32_t *)0x40020000UL)
#define GPIOA_AFRL      (*(volatile uint32_t *)0x40020020UL)

#define RCC_APB1ENR     (*(volatile uint32_t *)0x40023840UL)
#define RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830UL)

#define USART2_SR       (*(volatile uint32_t *)0x40004400UL)
#define USART2_DR       (*(volatile uint32_t *)0x40004404UL)
#define USART2_BRR      (*(volatile uint32_t *)0x40004408UL)
#define USART2_CR1      (*(volatile uint32_t *)0x4000440CUL)

#define RCC_AHB1ENR_GPIOAEN   (1UL << 0)
#define RCC_APB1ENR_USART2EN  (1UL << 17)

#define USART_CR1_UE          (1UL << 13)
#define USART_CR1_TE          (1UL << 3)
#define USART_CR1_RE          (1UL << 2)

#define USART_SR_TXE          (1UL << 7)

#define USART2_TX_PIN         2U
#define USART2_RX_PIN         3U
#define GPIO_AF7_USART        7U

static uint32_t Trishul_UartStm32f401reComputeBrr(uint32_t peripheral_clock_hz,
                                                  uint32_t baud_rate)
{
    return (peripheral_clock_hz + (baud_rate / 2U)) / baud_rate;
}

Trishul_Status Trishul_UartStm32f401reInit(uint32_t peripheral_clock_hz, uint32_t baud_rate)
{
    uint32_t brr;

    if ((peripheral_clock_hz == 0U) || (baud_rate == 0U))
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    brr = Trishul_UartStm32f401reComputeBrr(peripheral_clock_hz, baud_rate);
    if (brr == 0U)
    {
        return TRISHUL_INVALID_ARGUMENT;
    }

    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    GPIOA_MODER &= ~((3UL << (USART2_TX_PIN * 2U)) |
                     (3UL << (USART2_RX_PIN * 2U)));
    GPIOA_MODER |= (2UL << (USART2_TX_PIN * 2U)) |
                   (2UL << (USART2_RX_PIN * 2U));

    GPIOA_AFRL &= ~((0xFUL << (USART2_TX_PIN * 4U)) |
                    (0xFUL << (USART2_RX_PIN * 4U)));
    GPIOA_AFRL |= (GPIO_AF7_USART << (USART2_TX_PIN * 4U)) |
                  (GPIO_AF7_USART << (USART2_RX_PIN * 4U));

    USART2_CR1 = 0UL;
    USART2_BRR = brr;
    USART2_CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2_CR1 |= USART_CR1_UE;

    return TRISHUL_OK;
}

Trishul_Status Trishul_UartStm32f401reWriteByte(uint8_t byte)
{
    while ((USART2_SR & USART_SR_TXE) == 0U)
    {
    }

    USART2_DR = byte;
    return TRISHUL_OK;
}
