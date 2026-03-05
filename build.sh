#!/bin/bash

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug --toolchain $EMROOT/cmake/Modules/Platform/Emscripten.cmake
make -j 12