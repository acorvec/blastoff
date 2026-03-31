#!/bin/bash

cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release
ninja -j12
