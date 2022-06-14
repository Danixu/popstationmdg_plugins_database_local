#!/bin/bash

## Common folders
mkdir -p build/windows
mkdir -p build/linux
mkdir -p bin/windows
mkdir -p bin/linux

#################
# Linux version #
#################
: '
# Compile the library
echo "Compiling the Linux version"
# Compile the library
echo -e "\tCompiling the Library"
# First we need sqlite3
gcc -Ithirdparty/sqlite/ -fPIC -c thirdparty/sqlite/sqlite3.c -o build/linux/sqlite3.o -pthread -ldl
# Then compile the library
g++ \
  -g \
  -Iinclude \
  -Ithirdparty/popstationmdg/src/plugins/ \
  -Ithirdparty/sqlite/ \
  \
  -std=c++17 -fPIC -DBUILD_LIB -shared -static-libgcc -static-libstdc++ -pthread \
  \
  thirdparty/popstationmdg/src/plugins/export.cpp \
  src/local_sqlite_database.cpp \
  \
  build/linux/sqlite3.o \
  \
  -o bin/linux/local_sqlite_database.so

echo -e "\tCompiling the Test Program"
# Test Program
g++ \
  -g \
  -Iinclude \
  -Ithirdparty/popstationmdg/src/plugins/ \
  -Ithirdparty/sqlite/ \
  \
  thirdparty/popstationmdg/src/plugins/export.cpp \
  thirdparty/popstationmdg/src/logger/logger.cpp \
  thirdparty/popstationmdg/src/plugins/plugin_handler_common.cpp \
  thirdparty/popstationmdg/src/plugins/plugin_handler_database.cpp \
  src/test.cpp \
  \
  -static-libgcc -static-libstdc++ -std=c++17 -ldl -pthread \
  \
  -o bin/linux/test
cp data/test.db bin/linux/test.db
'



###################
# Windows version #
###################

echo "Compiling the Windows version"
# Compile the library
echo -e "\tCompiling the Library"
# First we need sqlite3
x86_64-w64-mingw32-gcc -Ithirdparty/sqlite/ -fPIC -c thirdparty/sqlite/sqlite3.c -o build/windows/sqlite3.o -pthread
# Then compile the library
x86_64-w64-mingw32-g++ \
  -g \
  -Iinclude \
  -Ithirdparty/popstationmdg/src/plugins/ \
  -Ithirdparty/sqlite/ \
  \
  -std=c++17 -fPIC -DBUILD_LIB -shared -static-libgcc -static-libstdc++ \
  \
  thirdparty/popstationmdg/src/plugins/export.cpp \
  src/local_sqlite_database.cpp \
  \
  build/windows/sqlite3.o \
  \
  -o bin/windows/local_sqlite_database.dll

echo -e "\tCompiling the Test Program"
# Test Program
x86_64-w64-mingw32-g++ \
  -g \
  -Iinclude \
  -Ithirdparty/popstationmdg/src/plugins/ \
  -Ithirdparty/sqlite/ \
  \
  thirdparty/popstationmdg/src/plugins/export.cpp \
  thirdparty/popstationmdg/src/logger/logger.cpp \
  thirdparty/popstationmdg/src/plugins/plugin_handler_common.cpp \
  thirdparty/popstationmdg/src/plugins/plugin_handler_database.cpp \
  src/test.cpp \
  \
  -static-libgcc -static-libstdc++ -std=c++17 \
  \
  -o bin/windows/test.exe
cp data/test.db bin/windows/test.db