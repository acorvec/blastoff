#!/bin/bash

cp -r ../resource/ ./
#emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Debug --toolchain $EMROOT/cmake/Modules/Platform/Emscripten.cmake
emcmake cmake .. -G Ninja -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release --toolchain $EMROOT/cmake/Modules/Platform/Emscripten.cmake
#emmake make -j 12
ninja -j12
