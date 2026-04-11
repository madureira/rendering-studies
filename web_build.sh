#!/bin/bash

# Exit on error, print each command
set -e
set -x

# ============================================================
# WEB BUILD SCRIPT (Emscripten -> WebAssembly + WebGL 2.0)
# ============================================================
# Requires Emscripten SDK to be activated first:
#   source ~/emsdk/emsdk_env.sh
# ============================================================

BUILD_DIR="build/Web"

# Create the build directory if needed
mkdir -p "$BUILD_DIR"

# Configure with Emscripten toolchain
emcmake cmake . \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=11

# Build (use all available CPU cores)
cmake --build "$BUILD_DIR" -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Web build complete! Output files in $BUILD_DIR/:"
ls -lh "$BUILD_DIR"/index.* 2>/dev/null || true
echo ""
echo "To serve locally:"
echo "  python3 -m http.server 8080 --directory $BUILD_DIR"
echo "  Then open: http://localhost:8080"
