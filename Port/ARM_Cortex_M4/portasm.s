    .syntax unified
    .thumb

    .global Trishul_PortPendSvAsm
    .global Trishul_PortSVCStartAsm
    .extern g_trishul_current_tcb
    .extern g_trishul_next_tcb
    .extern Trishul_PendSvHandler

/* Save R4-R11 on the process stack, ask the kernel for the next task, then restore it. */
Trishul_PortPendSvAsm:
    mrs r0, psp
    ldr r1, =g_trishul_current_tcb
    ldr r2, [r1]
    cbz r2, pend_restore
    stmdb r0!, {r4-r11}
    str r0, [r2]

pend_restore:
    push {r3, lr}
    bl Trishul_PendSvHandler
    pop {r3, lr}

    ldr r1, =g_trishul_next_tcb
    ldr r2, [r1]
    ldr r0, [r2]
    ldmia r0!, {r4-r11}
    msr psp, r0

    ldr r1, =g_trishul_current_tcb
    str r2, [r1]

    bx lr

/* Load PSP from the first task and exit exception return using the process stack. */
Trishul_PortSVCStartAsm:
    ldr r0, =g_trishul_current_tcb
    ldr r0, [r0]
    ldr r0, [r0]
    ldmia r0!, {r4-r11}
    msr psp, r0
    movs r0, #2
    msr control, r0
    isb
    ldr lr, =0xFFFFFFFD
    cpsie i
    bx lr
