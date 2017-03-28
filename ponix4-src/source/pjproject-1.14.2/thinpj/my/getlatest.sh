#!/bin/sh

echo "getting *.cpp, *.c files..."

cp *.c* bak/
scp harrybsd@192.168.4.1:/rep/c_shared/src/*.c* ./

echo "getting *.h files..."

cp *.h bak/
scp harrybsd@192.168.4.1:/rep/c_shared/include/*.h ./
