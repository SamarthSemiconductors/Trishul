set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TRISHUL_CPU_FLAGS "-mcpu=cortex-m4 -mthumb")

find_program(TRISHUL_GCC arm-none-eabi-gcc)
find_program(TRISHUL_GXX arm-none-eabi-g++)
find_program(TRISHUL_OBJCOPY arm-none-eabi-objcopy)
find_program(TRISHUL_SIZE arm-none-eabi-size)

if(NOT TRISHUL_GCC)
    message(FATAL_ERROR "arm-none-eabi-gcc not found. Install the GNU Arm Embedded Toolchain or point CMake at your compiler.")
endif()

if(NOT TRISHUL_GXX)
    set(TRISHUL_GXX ${TRISHUL_GCC})
endif()

set(CMAKE_C_COMPILER ${TRISHUL_GCC})
set(CMAKE_CXX_COMPILER ${TRISHUL_GXX})
set(CMAKE_ASM_COMPILER ${TRISHUL_GCC})

set(CMAKE_C_FLAGS_INIT "${TRISHUL_CPU_FLAGS} -ffreestanding -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS_INIT "${TRISHUL_CPU_FLAGS} -ffreestanding -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "${TRISHUL_CPU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${TRISHUL_CPU_FLAGS} -nostartfiles -Wl,--gc-sections")

set(CMAKE_OBJCOPY ${TRISHUL_OBJCOPY} CACHE FILEPATH "objcopy executable")
set(CMAKE_SIZE ${TRISHUL_SIZE} CACHE FILEPATH "size executable")
