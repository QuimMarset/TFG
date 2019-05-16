CFLAGS = -g -Wall -std=c++11
CC = g++
Test: SupportTypes.o Block.o Graph.o main.o 
	$(CC) $(CFLAGS) -o Test SupportTypes.o Block.o Graph.o main.o
main.o: main.cpp Graph.h
	$(CC) $(CFLAGS) -c main.cpp
Graph.o: Graph.h Graph.cpp Block.h Block.cpp
	$(CC) $(CFLAGS) -c Graph.cpp
Block.o: Block.h Block.cpp
	$(CC) $(CFLAGS) -c Block.cpp
SupportTypes.o : SupportTypes.h SupportTypes.cpp
	$(CC) $(CFLAGS) -c SupportTypes.cpp

clean:
	rm -f *.o Test