#!/bin/bash
set -e
set -x

BUILD_TYPE="Release"
BUILD_DIR="build/${BUILD_TYPE}"
GENERATORS_DIR="${BUILD_DIR}/generators"

conan install . --build=missing --settings=build_type=${BUILD_TYPE}

cmake -S . -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_TOOLCHAIN_FILE="${GENERATORS_DIR}/conan_toolchain.cmake" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1

cmake --build "${BUILD_DIR}"

cd "${BUILD_DIR}" && ./rendering-studies
