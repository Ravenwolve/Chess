rm -r ./build
mkdir build
cd build
cmake ..
cmake --build . --config $1