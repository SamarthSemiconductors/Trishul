    .syntax unified
    .cpu cortex-m4
    .thumb

    .extern main
    .extern SysTick_Handler
    .extern PendSV_Handler
    .extern SVC_Handler

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
    .thumb_func
Reset_Handler:
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
