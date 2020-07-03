# Description

wasmfract project is an experiment to take Gnofract4D capabilities to the web

## Goals
- Compile the underlaying module classes into webassembly bytecode
- Generate a fractal image in a canvas within an HTML page using the compiled webassembly module
- Add the ability to load different compiled formulas (the C sources of the formulas compiled as webassembly bytecodes)

## Dependencies and/or infrastructue

Emscripten: https://emscripten.org

## Installation

Follow these instructions https://emscripten.org/docs/getting_started/downloads.html

Dont forget to source the env file:
```bash
source emsdk_env.sh
```

# basic sample
```bash
emcc source.cpp -o index.html
python3 -m http.server
```
then open http://localhost:8000/index.html

