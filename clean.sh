#!/usr/bin/env bash
# Remove build directory. Run from project root.
set -e
cd "$(dirname "$0")"
BUILD_DIR=build

if [ -d "$BUILD_DIR" ]; then
  echo "==> Removing $BUILD_DIR..."
  rm -rf "$BUILD_DIR"
  echo "==> Done."
else
  echo "==> No $BUILD_DIR found. Nothing to clean."
fi
