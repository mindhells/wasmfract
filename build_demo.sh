#!/bin/sh

source /Users/alberto/projects/emsdk/emsdk_env.sh

em++ -O3 -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
    -I . -I model \
    demo.cpp \
    model/*.cpp \
    fract_stdlib.cpp \
    custom_mandelbrot_formula.c