

DIR_FONT=./font/
DIR_LIBS=./lib/
DIR_BIN=./bin/

COMPILER=g++ -g

LIBS=-lProcess -lParser -lJob -lBuiltin -lRedirector 

LFLAGS=-L$(DIR_LIBS) $(LIBS)

all: clean dynamics $(DIR_BIN)shooSH

$(DIR_BIN)shooSH: main.o
	$(COMPILER) -o $@ $< $(LFLAGS)
	
main.o: main.cpp
	$(COMPILER) -c $< -o $@

	
dynamics: Redirector.o Parser.o Job.o Process.o Builtin.o 
	g++ -shared -fPIC -o $(DIR_LIBS)libRedirector.so Redirector.o
	g++ -shared -fPIC -o $(DIR_LIBS)libParser.so Parser.o
	g++ -shared -fPIC -o $(DIR_LIBS)libJob.so Job.o
	g++ -shared -fPIC -o $(DIR_LIBS)libProcess.so Process.o
	g++ -shared -fPIC -o $(DIR_LIBS)libBuiltin.so Builtin.o
	
Redirector.o:$(DIR_LIBS)Redirector.cpp
	g++ -fPIC -c $< 
Parser.o:$(DIR_LIBS)Parser.cpp
	g++ -fPIC -c $<
Job.o:$(DIR_LIBS)Job.cpp
	g++ -fPIC -c $< 
Process.o:$(DIR_LIBS)Process.cpp
	g++ -fPIC -c $< 
Builtin.o:$(DIR_LIBS)Builtin.cpp
	g++ -fPIC -c $< 

#dynamics: 
#	g++ -shared -fPIC -o %@ $<
#	
#$(DIR_LIBS)%.so: $(DIR_LIBS)%.o
#	g++ -shared -fPIC -o %@ $<
#
#$(DIR_LIBS)%.o%: $(DIR_LIBS)%.cpp
#	g++ -fPIC -c $<
#

clean:
	rm -rf *.o shooSH *.so

removeObj:
	rm -rf *.o
