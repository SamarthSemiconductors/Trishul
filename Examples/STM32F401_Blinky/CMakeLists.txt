set(TRISHUL_STM32_FLASH_ORIGIN "0x08000000" CACHE STRING "STM32F401 flash origin")
set(TRISHUL_STM32_FLASH_LENGTH "256K" CACHE STRING "STM32F401 flash size")
set(TRISHUL_STM32_RAM_ORIGIN "0x20000000" CACHE STRING "STM32F401 RAM origin")
set(TRISHUL_STM32_RAM_LENGTH "64K" CACHE STRING "STM32F401 RAM size")

set(TRISHUL_LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/stm32f401_blinky.ld)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f401_blinky.ld.in
    ${TRISHUL_LINKER_SCRIPT}
    @ONLY
)

add_executable(trishul_stm32f401_blinky
    main.c
    startup_stm32f401xe.s
)

target_link_libraries(trishul_stm32f401_blinky
    PRIVATE
        trishul_kernel
)

target_compile_options(trishul_stm32f401_blinky
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
        $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
        $<$<COMPILE_LANGUAGE:C>:-Wall>
        $<$<COMPILE_LANGUAGE:C>:-Wextra>
        $<$<COMPILE_LANGUAGE:C>:-Wpedantic>
)

target_link_options(trishul_stm32f401_blinky
    PRIVATE
        -T${TRISHUL_LINKER_SCRIPT}
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/trishul_stm32f401_blinky.map
)

if(CMAKE_OBJCOPY)
    add_custom_command(TARGET trishul_stm32f401_blinky POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
                $<TARGET_FILE:trishul_stm32f401_blinky>
                ${CMAKE_CURRENT_BINARY_DIR}/trishul_stm32f401_blinky.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex
                $<TARGET_FILE:trishul_stm32f401_blinky>
                ${CMAKE_CURRENT_BINARY_DIR}/trishul_stm32f401_blinky.hex
        COMMENT "Generating binary and Intel HEX images"
    )
endif()

if(CMAKE_SIZE)
    add_custom_command(TARGET trishul_stm32f401_blinky POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:trishul_stm32f401_blinky>
        COMMENT "Firmware size"
    )
endif()
