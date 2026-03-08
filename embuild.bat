:: fuck emscripten. all my homies hate emscripten
:: (seriously, what the fuck is wrong with this god-awful package?)

emcmake cmake -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Debug --toolchain $EMROOT/cmake/Modules/Platform/Emscripten.cmake
emmake make -j 12
