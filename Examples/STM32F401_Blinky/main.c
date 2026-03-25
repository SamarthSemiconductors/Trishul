#include <stdint.h>

#include "trishul.h"

#define IDLE_STACK_WORDS 128U
#define BLINK_STACK_WORDS 128U
#define CPU_CLOCK_HZ 16000000UL

static Trishul_Tcb g_idle_tcb;
static Trishul_Tcb g_blink_tcb;
static uint32_t g_idle_stack[IDLE_STACK_WORDS];
static uint32_t g_blink_stack[BLINK_STACK_WORDS];

static void Board_ClockInit(void);
static void Board_GpioInit(void);
static void Board_LedToggle(void);
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
    uint32_t period_ticks = (uint32_t)(uintptr_t)argument;

    for (;;)
    {
        Board_LedToggle();
        Trishul_Delay(period_ticks);
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
                                (void *)(uintptr_t)500U,
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
    /* TODO: Replace with STM32 clock tree setup. */
}

static void Board_GpioInit(void)
{
    /* TODO: Configure the LED GPIO pin here. */
}

static void Board_LedToggle(void)
{
    /* TODO: Toggle the board LED here. */
}

static void Board_Panic(void)
{
    for (;;)
    {
    }
}
