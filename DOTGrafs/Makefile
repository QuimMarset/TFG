CFLAGS = -g -Wall -std=c++11
CC = g++
Test: Attribute.o Component.o Channel.o DFGraph.o main.o   
	$(CC) $(CFLAGS) -o Test Attribute.o Component.o Channel.o DFGraph.o main.o
main.o: main.cpp DFGraph.h
	$(CC) $(CFLAGS) -c main.cpp
DFGraph.o: DFGraph.h DFGraph.cpp Component.h Channel.h
	$(CC) $(CFLAGS) -c DFGraph.cpp
Attribute.o: Attribute.h Attribute.cpp
	$(CC) $(CFLAGS) -c Attribute.cpp
Component.o: Component.h Component.cpp AttributeUniValue.h AttributeMultiValue.h
	$(CC) $(CFLAGS) -c Component.cpp
Channel.o: Channel.h Channel.cpp
	$(CC) $(CFLAGS) -c Channel.cpp
clean:
	rm -f *.o Test