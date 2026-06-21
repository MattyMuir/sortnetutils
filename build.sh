#!/bin/bash

module load cmake/3.30.5
module load llvm/20.1.8

cmake -G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE="Release" \
	-D CMAKE_CXX_COMPILER=clang++ \
	-D CMAKE_PREFIX_PATH="~/sdklib" \
	-D CMAKE_INSTALL_PREFIX="~/sdklib/sortnetutils" \
	-B "./build"

cmake --build ./build --parallel
cmake --install ./build