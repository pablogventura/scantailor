# Building Scan Tailor

## Requirements

- **CMake** 3.16 or later
- **Qt5** 5.15 or later (Widgets, Xml, Network)
- **C++14**-capable compiler (GCC 5+, Clang 3.4+, MSVC 2015+)
- **Boost** 1.65+ (unit_test_framework, prg_exec_monitor)
- **libjpeg**, **zlib**, **libpng**, **libtiff**
- On Linux: **X11/XRender** (libxrender-dev)

## Linux

```bash
# Debian/Ubuntu
sudo apt-get install cmake qtbase5-dev qttools5-dev \
  libboost-test-dev libjpeg-dev zlib1g-dev libpng-dev libtiff-dev libxrender-dev

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Windows

Use Qt 5.15+ from the installer or build from source. Set `Qt5_DIR` to the CMake config path (e.g. `C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5`). Ensure libjpeg, zlib, libpng, and libtiff are available (vcpkg or manual build).

## macOS

Install Qt5 via Homebrew (`brew install qt@5`) or the official installer. Then:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(brew --prefix qt@5)
cmake --build .
```

## Options

- `-DENABLE_OPENGL=ON` — Enable OpenGL-accelerated view (experimental).
- `-DDEBUG_CLI=ON` — Enable CLI debug output.

## Tests

After building, run:

```bash
cd build && ctest --output-on-failure
```

## Output formats

The application supports output in **TIFF** (default), **PNG**, and **JPEG**. Use the "Output Format" option in the Output stage or the CLI flag `--output-format=tiff|png|jpeg`. Assembling pages into a single PDF is typically done with external tools (e.g. img2pdf, ImageMagick) after exporting images from Scan Tailor.
