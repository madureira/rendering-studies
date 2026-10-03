#!/bin/bash
set -e

#BUILD_TYPE="RelWithDebInfo"
BUILD_TYPE="Debug"
BUILD_DIR="build/${BUILD_TYPE}"

cmake -S . -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DASSIMP_WARNINGS_AS_ERRORS=OFF

cmake --build "${BUILD_DIR}" -j

cd "${BUILD_DIR}" && ./rendering-studies
