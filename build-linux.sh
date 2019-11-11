mkdir build-linux
cd build-linux
if [ ! -v COMPILER ]; then
	COMPILER=g++
fi	
	
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER
make -j4
