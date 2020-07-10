# Description

wasmfract project is an experiment to take Gnofract4D capabilities to the web

## Goals
- Compile the underlaying module classes into webassembly bytecode
- Generate a fractal image in a canvas within an HTML page using the compiled webassembly module
- Add the ability to load different compiled formulas (the C sources of the formulas compiled as webassembly bytecodes)

## Dependencies and/or infrastructue

Emscripten: https://emscripten.org

# run the demo

```bash
./build_demo.sh
```
