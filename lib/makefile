
#Compilacao das bibliotecas dinamicas

$SOURCES=$(wildcard *.cpp)

all:%.so

%.so:%.cpp
	g++ -shared -fPIC -o lib$@ $<
	
%.o:%.cpp
	g++ -fPIC -c $<
