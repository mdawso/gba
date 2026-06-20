# GBA

## Dependencies
- A C++ Compiler (gcc/clang)
- CMake
- Git
- Raylib build dependencies ([See here](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux))

## Quick Start

Make sure to clone with `--recurse-submodules`.
Alternatively run `git submodule init` then `git submodule update` after cloning.

```sh
mkdir build
cd build
cmake ..
make
./GBA <path to rom>
```

## About
This is a GBA emulator I am working on. The architecture is based on that of my previous NES emulator, a central memory bus that owns the components, with a CPU acting as an interpreter, stepping through memory, decoding and executing instructions. It has been developed primarily on Linux.

### This is WIP. The CPU is mostly complete and can run ARM + ARMv4T machine code but there are no graphics, sound or controls.