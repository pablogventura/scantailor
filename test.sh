#!/usr/bin/env bash
# Build and run tests. Run from project root.
set -e
cd "$(dirname "$0")"
BUILD_DIR=build

if [ ! -d "$BUILD_DIR" ]; then
  echo "==> No build dir. Run ./build.sh first."
  exit 1
fi

echo "==> Running tests..."
cd "$BUILD_DIR"
ctest --output-on-failure "$@"
