#!/usr/bin/env bash
# Build Scan Tailor (Release). Run from project root.
set -e
cd "$(dirname "$0")"
BUILD_DIR=build
JOBS=$(nproc 2>/dev/null || echo 2)

echo "==> Configuring (Release)..."
mkdir -p "$BUILD_DIR"
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release "$@"

echo "==> Building (-j$JOBS)..."
cmake --build "$BUILD_DIR" -j"$JOBS"

echo "==> Done. Run: $BUILD_DIR/scantailor  or  $BUILD_DIR/scantailor-cli"
