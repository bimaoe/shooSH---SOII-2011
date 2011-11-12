
#Compilacao das bibliotecas dinamicas

all: Redirector.o Parser.o Job.o Process.o
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
