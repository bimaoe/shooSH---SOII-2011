
#compilacao do programa principal
LIBS_DIR=./
LIBS=-lRedirector -lJob -lParser -lProcess

LFLAGS=-L$(LIBS_DIR) $(LIBS)

#SOURCES=$(wildcard *.cpp)
LDYNAM=$(./lib/)

COMPILER=g++

all: clean dynamics shooSH 

shooSH: main.o
	export LD_LIBRARY_PATH=~/Documents/shooSH---SOII-2011/
	g++ -o shooSH main.o $(LFLAGS)

main.o:main.cpp
	$(COMPILER) -c $< -o $@

	
	
dynamics: Redirector.o Parser.o Job.o Process.o
	g++ -shared -fPIC -o libRedirector.so Redirector.o
	g++ -shared -fPIC -o libParser.so Parser.o
	g++ -shared -fPIC -o libJob.so Job.o
	g++ -shared -fPIC -o libProcess.so Process.o
	
Redirector.o:Redirector.cpp
	g++ -fPIC -c $<
Parser.o:Parser.cpp
	g++ -fPIC -c $<
Job.o:Job.cpp
	g++ -fPIC -c $<
Process.o:Process.cpp
	g++ -fPIC -c $<

clean:
	@-rm -f *.o *~ shooSH *.so
	
remake:
	$(MAKE) clean
	$(MAKE)
