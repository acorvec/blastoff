#!/bin/bash

emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Debug --toolchain $EMROOT/cmake/Modules/Platform/Emscripten.cmake
emmake make -j 12
