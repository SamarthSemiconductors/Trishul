#include "port.h"

#include "trishul.h"
#include "trishul_internal.h"

#define SYSTICK_CTRL (*(volatile uint32_t *)0xE000E010UL)
#define SYSTICK_LOAD (*(volatile uint32_t *)0xE000E014UL)
#define SYSTICK_VAL  (*(volatile uint32_t *)0xE000E018UL)

#define SCB_ICSR     (*(volatile uint32_t *)0xE000ED04UL)
#define SCB_SHPR2    (*(volatile uint32_t *)0xE000ED1CUL)
#define SCB_SHPR3    (*(volatile uint32_t *)0xE000ED20UL)

#define SCB_ICSR_PENDSVSET (1UL << 28)
#define SYSTICK_CTRL_ENABLE (1UL << 0)
#define SYSTICK_CTRL_TICKINT (1UL << 1)
#define SYSTICK_CTRL_CLKSOURCE (1UL << 2)

#define PORT_MIN_INTERRUPT_PRIORITY 0xFFU
#define PORT_PENDSV_PRIORITY_SHIFT 16U
#define PORT_SYSTICK_PRIORITY_SHIFT 24U

extern void Trishul_PortPendSvAsm(void);
extern void Trishul_PortSVCStartAsm(void);

uint32_t *Trishul_PortInitStack(uint32_t *stack_buffer,
                                size_t stack_words,
                                Trishul_TaskEntry entry,
                                void *argument)
{
    uint32_t *sp = stack_buffer + stack_words;

    *(--sp) = 0x01000000UL;                  /* xPSR */
    *(--sp) = (uint32_t)entry;               /* PC */
    *(--sp) = (uint32_t)Trishul_TaskExitError; /* LR */
    *(--sp) = 0x12121212UL;                  /* R12 */
    *(--sp) = 0x03030303UL;                  /* R3 */
    *(--sp) = 0x02020202UL;                  /* R2 */
    *(--sp) = 0x01010101UL;                  /* R1 */
    *(--sp) = (uint32_t)argument;            /* R0 */

    *(--sp) = 0x11111111UL;                  /* R11 */
    *(--sp) = 0x10101010UL;                  /* R10 */
    *(--sp) = 0x09090909UL;                  /* R9 */
    *(--sp) = 0x08080808UL;                  /* R8 */
    *(--sp) = 0x07070707UL;                  /* R7 */
    *(--sp) = 0x06060606UL;                  /* R6 */
    *(--sp) = 0x05050505UL;                  /* R5 */
    *(--sp) = 0x04040404UL;                  /* R4 */

    return sp;
}

void Trishul_PortSetup(uint32_t cpu_clock_hz, uint32_t tick_hz)
{
    uint32_t reload = (cpu_clock_hz / tick_hz) - 1UL;

    SCB_SHPR3 = (PORT_MIN_INTERRUPT_PRIORITY << PORT_PENDSV_PRIORITY_SHIFT) |
                (PORT_MIN_INTERRUPT_PRIORITY << PORT_SYSTICK_PRIORITY_SHIFT);
    SCB_SHPR2 |= (0x00U << 24U);

    SYSTICK_LOAD = reload;
    SYSTICK_VAL = 0UL;
    SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE |
                   SYSTICK_CTRL_TICKINT |
                   SYSTICK_CTRL_ENABLE;
}

void Trishul_PortRequestContextSwitch(void)
{
    SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

void Trishul_PortStartFirstTask(void)
{
    __asm volatile ("svc 0");

    for (;;)
    {
    }
}

void SysTick_Handler(void)
{
    Trishul_TickHandler();
}

void PendSV_Handler(void)
{
    Trishul_PortPendSvAsm();
}

void SVC_Handler(void)
{
    Trishul_PortSVCStartAsm();
}
