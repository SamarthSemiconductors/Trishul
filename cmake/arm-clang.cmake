set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TRISHUL_CPU_FLAGS "--target=arm-none-eabi -mcpu=cortex-m4 -mthumb")

find_program(TRISHUL_CLANG clang)
find_program(TRISHUL_CLANGXX clang++)
find_program(TRISHUL_LLD ld.lld)
find_program(TRISHUL_LLVM_AR llvm-ar)
find_program(TRISHUL_LLVM_RANLIB llvm-ranlib)
find_program(TRISHUL_LLVM_OBJCOPY llvm-objcopy)
find_program(TRISHUL_LLVM_SIZE llvm-size)

if(NOT TRISHUL_CLANG)
    message(FATAL_ERROR "clang not found. Install clang/LLVM or use the GNU ARM toolchain file instead.")
endif()

if(NOT TRISHUL_LLD)
    message(FATAL_ERROR "ld.lld not found. Install LLVM's linker or use the GNU ARM toolchain file instead.")
endif()

set(CMAKE_C_COMPILER ${TRISHUL_CLANG})
if(TRISHUL_CLANGXX)
    set(CMAKE_CXX_COMPILER ${TRISHUL_CLANGXX})
else()
    set(CMAKE_CXX_COMPILER ${TRISHUL_CLANG})
endif()
set(CMAKE_ASM_COMPILER ${TRISHUL_CLANG})

if(TRISHUL_LLVM_AR)
    set(CMAKE_AR ${TRISHUL_LLVM_AR})
endif()

if(TRISHUL_LLVM_RANLIB)
    set(CMAKE_RANLIB ${TRISHUL_LLVM_RANLIB})
endif()

set(CMAKE_C_FLAGS_INIT "${TRISHUL_CPU_FLAGS} -ffreestanding -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS_INIT "${TRISHUL_CPU_FLAGS} -ffreestanding -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "${TRISHUL_CPU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${TRISHUL_CPU_FLAGS} -nostartfiles -Wl,--gc-sections")

if(TRISHUL_LLD)
    set(CMAKE_LINKER ${TRISHUL_LLD})
    string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " --ld-path=${TRISHUL_LLD}")
endif()

if(TRISHUL_LLVM_OBJCOPY)
    set(CMAKE_OBJCOPY ${TRISHUL_LLVM_OBJCOPY} CACHE FILEPATH "objcopy executable")
endif()

if(TRISHUL_LLVM_SIZE)
    set(CMAKE_SIZE ${TRISHUL_LLVM_SIZE} CACHE FILEPATH "size executable")
endif()
