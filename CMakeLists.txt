cmake_minimum_required(VERSION 3.20)

project(Trishul VERSION 0.1.0 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(TRISHUL_BUILD_STM32F401_EXAMPLE "Build the STM32F401 blinky firmware target" ON)

add_library(trishul_kernel STATIC
    Kernal/Src/trishul.c
    Kernal/Src/trishul_uart.c
    Drivers/STM32F401RE/Src/trishul_uart_stm32f401re.c
    Port/ARM_Cortex_M4/port.c
    Port/ARM_Cortex_M4/portasm.s
)

target_include_directories(trishul_kernel
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Kernal/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F401RE/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Port/ARM_Cortex_M4
)

target_compile_options(trishul_kernel
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
        $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
        $<$<COMPILE_LANGUAGE:C>:-Wall>
        $<$<COMPILE_LANGUAGE:C>:-Wextra>
        $<$<COMPILE_LANGUAGE:C>:-Wpedantic>
)

if(TRISHUL_BUILD_STM32F401_EXAMPLE)
    add_subdirectory(Examples/STM32F401_Blinky)
endif()
