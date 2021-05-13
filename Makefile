mizzo: thread.o io.o main.o
	g++ -std=c++11 -o mizzo thread.o io.o main.o -lpthread -lrt

thread.o: thread.h thread.cpp
	g++ -std=c++11 -c thread.cpp

io.o: io.h io.c
	gcc -c io.c
	
main.o: production.h structures.h thread.h main.cpp
	g++ -std=c++11 -c main.cpp

clean:
	rm -f *.o mizzo