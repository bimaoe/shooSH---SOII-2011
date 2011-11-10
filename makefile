
#compilacao do programa principal
LIBS_DIR=./lib
LIBS=-lRedirector -lJob -lParser -lProcess

LFLAGS=-L$(LIBS_DIR) $(LIBS)

SOURCES=$(wildcard *.cpp)
LDYNAM=$(./lib/make)

all: clean shooSH dynamics

shooSH: $(SOURCES:.cpp=.o)
	g++ -o $@ $^ $(LFLAGS)

%.o:%.cpp
	g++ -c $< -o $@
	
dynamics:$(LDYNAM)

%.d:%.cpp
	g++ $< -MM -MT '$*.o $*.d ' -MD	

clean:
	@-rm -f *.o *~ shooSH 
	
remake:
	$(MAKE) clean
	$(MAKE)
