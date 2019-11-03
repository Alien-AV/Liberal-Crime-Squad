mkdir build-cross-linux-to-mingw
cd build-cross-linux-to-mingw
cmake .. -DCMAKE_TOOLCHAIN_FILE=Toolchain-cross-mingw32-linux.cmake
make -j4
