#!/usr/bin/env bash
# Build Scan Tailor and generate .deb package. Run from project root.
set -e
cd "$(dirname "$0")"
BUILD_DIR=build
JOBS=$(nproc 2>/dev/null || echo 2)

echo "==> Configuring (Release, prefix /usr for .deb)..."
mkdir -p "$BUILD_DIR"
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr

echo "==> Building (-j$JOBS)..."
cmake --build "$BUILD_DIR" -j"$JOBS"

echo "==> Creating .deb package..."
(cd "$BUILD_DIR" && cpack -G DEB -C Release)

DEB=$(ls -t "$BUILD_DIR"/scantailor_*.deb 2>/dev/null | head -1)
if [ -n "$DEB" ]; then
  echo "==> Done. Package: $DEB"
  echo "    Install: sudo dpkg -i $DEB"
else
  echo "==> Build finished (check $BUILD_DIR for .deb)"
fi
