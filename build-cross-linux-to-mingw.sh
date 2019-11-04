mkdir build-cross-linux-to-mingw
cd build-cross-linux-to-mingw
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchain-cross-linux-to-mingw.cmake
make -j4
