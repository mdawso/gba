# GBA

## Dependencies
- A C++ Compiler (gcc/clang)
- CMake
- Git


## Quick Start

Make sure to clone with `--recurse-submodules`.

```sh
mkdir build
cd build
cmake ..
make
./GBA <path to rom>
```

## About
This is a GBA emulator I am working on. The architecture is based on that of my previous NES emulator, a central memory bus that owns the components. It has been developed primarily on Linux, so I'm not sure about Windows compatibility.