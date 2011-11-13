

DIR_FONT=./font/
DIR_LIBS=./lib/
DIR_BIN=./bin/

LIBS=-lProcess -lParser -lJob -lBuiltin

LFLAGS=-L$(DIR_LIBS) $(LIBS)

all: dynamics shooSH

shooSH: main.o
	g++ -o $@ $< $(LFLAGS)
	
main.o: main.cpp
	g++ -c $< -o $@

dynamics: $(SOURCES:$(DIR_LIBS).cpp=($DIR_LIBS).so)
	
$(DIR_LIBS)%.so: $(DIR_LIBS)%.o
	g++ -shared -fPIC -o %@ $<

$(DIR_LIBS): $(DIR_LIBS)%.cpp
	g++ -fPIC -c $<

clean:
	rm -rf *.o shooSH *.so
	
