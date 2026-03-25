#!/usr/bin/env bash

set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
LOG_DIR="${LOG_DIR:-${ROOT_DIR}/logs}"
TOOLCHAIN_FILE="${TOOLCHAIN_FILE:-cmake/arm-none-eabi-gcc.cmake}"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
CONFIGURE_LOG="${LOG_DIR}/configure_${TIMESTAMP}.log"
BUILD_LOG="${LOG_DIR}/build_${TIMESTAMP}.log"

mkdir -p "${BUILD_DIR}" "${LOG_DIR}"

echo "Using build directory: ${BUILD_DIR}"
echo "Using log directory: ${LOG_DIR}"
echo "Configure log: ${CONFIGURE_LOG}"
echo "Build log: ${BUILD_LOG}"

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" "$@" 2>&1 | tee "${CONFIGURE_LOG}"
configure_status=${PIPESTATUS[0]}
if [ "${configure_status}" -ne 0 ]; then
    echo "Configure failed. See ${CONFIGURE_LOG}"
    exit "${configure_status}"
fi

cmake --build "${BUILD_DIR}" 2>&1 | tee "${BUILD_LOG}"
build_status=${PIPESTATUS[0]}
if [ "${build_status}" -ne 0 ]; then
    echo "Build failed. See ${BUILD_LOG}"
    exit "${build_status}"
fi

echo "Build completed successfully."
echo "Logs written to ${LOG_DIR}"
