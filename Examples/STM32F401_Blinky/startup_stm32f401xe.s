    .syntax unified
    .cpu cortex-m4
    .thumb

    .extern main
    .extern SysTick_Handler
    .extern PendSV_Handler
    .extern SVC_Handler
    .extern __data_load_start__
    .extern __data_start__
    .extern __data_end__
    .extern __bss_start__
    .extern __bss_end__

    .section .isr_vector, "a", %progbits
    .global __vector_table
    .global __stack_top

__vector_table:
    .word __stack_top
    .word Reset_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word Default_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

    .section .text.Reset_Handler, "ax", %progbits
    .global Reset_Handler
    .thumb_func
Reset_Handler:
    ldr r0, =__data_load_start__
    ldr r1, =__data_start__
    ldr r2, =__data_end__

copy_data:
    cmp r1, r2
    bcs zero_bss
    ldr r3, [r0], #4
    str r3, [r1], #4
    b copy_data

zero_bss:
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
    movs r3, #0

clear_bss:
    cmp r1, r2
    bcs call_system_init
    str r3, [r1], #4
    b clear_bss

call_system_init:
    bl SystemInit
    bl main

reset_loop:
    b reset_loop

    .section .text.SystemInit, "ax", %progbits
    .global SystemInit
    .thumb_func
SystemInit:
    bx lr

    .section .text.Default_Handler, "ax", %progbits
    .global Default_Handler
    .thumb_func
Default_Handler:
default_loop:
    b default_loop
