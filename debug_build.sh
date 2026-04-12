#!/bin/bash
set -e
set -x

BUILD_TYPE="Debug"
BUILD_DIR="build/${BUILD_TYPE}"

cmake -S . -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1

cmake --build "${BUILD_DIR}"

cd "${BUILD_DIR}" && ./rendering-studies
