#!/bin/sh

# get the emsdk git submodule updated
git submodule update --init

emsdk/emsdk install latest
emsdk/emsdk activate latest

source emsdk/emsdk_env.sh

emcc -O3 -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
    -I . -I model \
    demo.cpp \
    model/*.cpp \
    fract_stdlib.cpp \
    custom_mandelbrot_formula.c

python3 -m http.server
