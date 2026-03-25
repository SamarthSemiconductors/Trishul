# Trishul

Trishul is a small educational RTOS for ARM Cortex-M microcontrollers.
This repository now includes a minimal starter kit you can build on:

- A tiny kernel API with static task creation
- A round-robin scheduler with tick-based delays
- An ARM Cortex-M4 port skeleton using `SysTick`, `SVC`, and `PendSV`
- An STM32F401 example that shows how to wire the pieces together

## Repository layout

- `Kernal/Inc`: public kernel headers
- `Kernal/Src`: kernel implementation
- `Port/ARM_Cortex_M4`: Cortex-M4-specific port layer
- `Examples/STM32F401_Blinky`: first board bring-up example
- `cmake`: cross-compilation toolchain files
- `Docs`: implementation notes and next steps

## First milestone

The starter code is built around one goal:

`Run two statically allocated tasks on a Cortex-M4 using SysTick + PendSV`

That milestone gives you the foundation for everything else:

- task control blocks
- stack initialization
- task selection
- preemptive context switching
- tick-based delays

## What is intentionally missing

The starter keeps the scope small on purpose. It does not yet include:

- mutexes, semaphores, or queues
- dynamic heap allocation
- interrupt-safe critical section nesting
- priority-based scheduling
- board-specific startup code or linker scripts

## Suggested next steps

1. Wire the example into your STM32 startup code and vector table.
2. Confirm `SysTick_Handler`, `SVC_Handler`, and `PendSV_Handler` reach the port.
3. Toggle a GPIO from two tasks to validate context switching.
4. Add a ready list and priorities after the scheduler is stable.
5. Add synchronization primitives only after task switching is proven.

## Build

Trishul now includes a CMake-based embedded build setup.

### GNU Arm Embedded Toolchain

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake

cmake --build build
```

Or use the helper script, which also writes timestamped log files:

```bash
chmod +x build.sh
./build.sh
```

### Clang/LLVM

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-clang.cmake

cmake --build build
```

This path expects `clang`, `ld.lld`, and the LLVM binary utilities to be installed.

To build with Clang through the helper script:

```bash
TOOLCHAIN_FILE=cmake/arm-clang.cmake ./build.sh
```

### Build outputs

When the STM32 example target is enabled, the build produces:

- `trishul_stm32f401_blinky`
- `trishul_stm32f401_blinky.bin`
- `trishul_stm32f401_blinky.hex`
- `trishul_stm32f401_blinky.map`

The helper script also writes:

- `logs/configure_YYYYMMDD_HHMMSS.log`
- `logs/build_YYYYMMDD_HHMMSS.log`

### Memory configuration

The example linker script is generated from CMake cache values. If your STM32F401
variant has different memory sizes, override them at configure time:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake \
  -DTRISHUL_STM32_FLASH_LENGTH=512K \
  -DTRISHUL_STM32_RAM_LENGTH=96K
```

### Notes

- The startup file is intentionally minimal and only covers the core exception vectors used by the RTOS starter.
- `SystemInit()` is currently a stub. Replace it with your board clock and memory init later if needed.
- The example still contains TODOs for GPIO and board-specific setup in `main.c`.
