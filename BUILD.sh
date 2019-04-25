if [ ! -d "./build" ]; then
  mkdir build
fi
cd build
cmake .. -G "Visual Studio 15 2017 Win64"