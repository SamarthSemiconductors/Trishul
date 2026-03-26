#include <stdint.h>

#include "trishul.h"

#define IDLE_STACK_WORDS 128U
#define BLINK_STACK_WORDS 128U
#define CPU_CLOCK_HZ 84000000UL
#define LD1_ON_TICKS  100U
#define LD1_OFF_TICKS 500U
#define PANIC_BLINK_DELAY_CYCLES 400000UL

#define FLASH_ACR       (*(volatile uint32_t *)0x40023C00UL)

#define GPIOA_MODER     (*(volatile uint32_t *)0x40020000UL)
#define GPIOA_OTYPER    (*(volatile uint32_t *)0x40020004UL)
#define GPIOA_OSPEEDR   (*(volatile uint32_t *)0x40020008UL)
#define GPIOA_PUPDR     (*(volatile uint32_t *)0x4002000CUL)
#define GPIOA_ODR       (*(volatile uint32_t *)0x40020014UL)

#define RCC_CR          (*(volatile uint32_t *)0x40023800UL)
#define RCC_PLLCFGR     (*(volatile uint32_t *)0x40023804UL)
#define RCC_CFGR        (*(volatile uint32_t *)0x40023808UL)
#define RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830UL)

#define FLASH_ACR_LATENCY_2WS (2UL << 0)
#define FLASH_ACR_ICEN        (1UL << 9)
#define FLASH_ACR_DCEN        (1UL << 10)
#define FLASH_ACR_PRFTEN      (1UL << 8)

#define RCC_CR_HSION          (1UL << 0)
#define RCC_CR_HSIRDY         (1UL << 1)
#define RCC_CR_PLLON          (1UL << 24)
#define RCC_CR_PLLRDY         (1UL << 25)

#define RCC_CFGR_SW_PLL       (2UL << 0)
#define RCC_CFGR_SWS_MASK     (3UL << 2)
#define RCC_CFGR_SWS_PLL      (2UL << 2)
#define RCC_CFGR_PPRE1_DIV2   (4UL << 10)

#define RCC_AHB1ENR_GPIOAEN   (1UL << 0)

#define LD1_PIN               5U
#define LD1_PIN_MASK          (1UL << LD1_PIN)

static Trishul_Tcb g_idle_tcb;
static Trishul_Tcb g_blink_tcb;
static uint32_t g_idle_stack[IDLE_STACK_WORDS];
static uint32_t g_blink_stack[BLINK_STACK_WORDS];

static void Board_ClockInit(void);
static void Board_GpioInit(void);
static void Board_Ld1Toggle(void);
static void Board_Ld1Set(uint32_t led_on);
static void Board_BusyWait(volatile uint32_t cycles);
static void Board_Panic(void);

static void IdleTask(void *argument)
{
    (void)argument;

    for (;;)
    {
        __asm volatile ("wfi");
    }
}

static void BlinkTask(void *argument)
{
    uint32_t off_ticks = (uint32_t)(uintptr_t)argument;

    for (;;)
    {
        Board_Ld1Set(1U);
        Trishul_Delay(LD1_ON_TICKS);
        Board_Ld1Set(0U);
        Trishul_Delay(off_ticks);
    }
}

int main(void)
{
    Trishul_Status status;

    Board_ClockInit();
    Board_GpioInit();

    Trishul_Init();

    status = Trishul_TaskCreate(&g_idle_tcb,
                                "idle",
                                IdleTask,
                                NULL,
                                g_idle_stack,
                                IDLE_STACK_WORDS);
    if (status != TRISHUL_OK)
    {
        Board_Panic();
    }

    status = Trishul_TaskCreate(&g_blink_tcb,
                                "blink",
                                BlinkTask,
                                (void *)(uintptr_t)LD1_OFF_TICKS,
                                g_blink_stack,
                                BLINK_STACK_WORDS);
    if (status != TRISHUL_OK)
    {
        Board_Panic();
    }

    status = Trishul_Start(CPU_CLOCK_HZ);
    if (status != TRISHUL_OK)
    {
        Board_Panic();
    }

    for (;;)
    {
    }
}

static void Board_ClockInit(void)
{
    /* Run the NUCLEO-F401RE from HSI -> PLL at 84 MHz. */
    RCC_CR |= RCC_CR_HSION;
    while ((RCC_CR & RCC_CR_HSIRDY) == 0U)
    {
    }

    FLASH_ACR = FLASH_ACR_PRFTEN |
                FLASH_ACR_ICEN |
                FLASH_ACR_DCEN |
                FLASH_ACR_LATENCY_2WS;

    /* PLLM = 16, PLLN = 336, PLLP = 4, PLLQ = 7. */
    RCC_PLLCFGR = 16UL |
                  (336UL << 6) |
                  (1UL << 16) |
                  (7UL << 24);

    RCC_CR |= RCC_CR_PLLON;
    while ((RCC_CR & RCC_CR_PLLRDY) == 0U)
    {
    }

    RCC_CFGR = RCC_CFGR_PPRE1_DIV2;
    RCC_CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL)
    {
    }
}

static void Board_GpioInit(void)
{
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA_MODER &= ~(3UL << (LD1_PIN * 2U));
    GPIOA_MODER |= (1UL << (LD1_PIN * 2U));
    GPIOA_OTYPER &= ~LD1_PIN_MASK;
    GPIOA_OSPEEDR &= ~(3UL << (LD1_PIN * 2U));
    GPIOA_OSPEEDR |= (2UL << (LD1_PIN * 2U));
    GPIOA_PUPDR &= ~(3UL << (LD1_PIN * 2U));
    GPIOA_ODR &= ~LD1_PIN_MASK;
}

static void Board_Ld1Toggle(void)
{
    GPIOA_ODR ^= LD1_PIN_MASK;
}

static void Board_Ld1Set(uint32_t led_on)
{
    if (led_on != 0U)
    {
        GPIOA_ODR |= LD1_PIN_MASK;
    }
    else
    {
        GPIOA_ODR &= ~LD1_PIN_MASK;
    }
}

static void Board_BusyWait(volatile uint32_t cycles)
{
    while (cycles > 0U)
    {
        --cycles;
    }
}

static void Board_Panic(void)
{
    for (;;)
    {
        Board_Ld1Toggle();
        Board_BusyWait(PANIC_BLINK_DELAY_CYCLES);
    }
}
