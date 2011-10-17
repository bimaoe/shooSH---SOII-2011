g++ -fPIC -c $1.cpp
g++ -shared -fPIC -o lib$1.so $1.o
