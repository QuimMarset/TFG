CFLAGS = -g -Wall -std=c++11
CC = g++
Test: Attribute.o SupportTypes.o Block.o Channel.o DFGraph.o main.o 
	$(CC) $(CFLAGS) -o Test Attribute.o SupportTypes.o Block.o Channel.o DFGraph.o main.o
main.o: main.cpp DFGraph.h
	$(CC) $(CFLAGS) -c main.cpp
DFGraph.o: DFGraph.h DFGraph.cpp Block.h Channel.h
	$(CC) $(CFLAGS) -c DFGraph.cpp
SupportTypes.o : SupportTypes.h SupportTypes.cpp
	$(CC) $(CFLAGS) -c SupportTypes.cpp
Attribute.o: Attribute.h Attribute.cpp
	$(CC) $(CFLAGS) -c Attribute.cpp
Block.o: Block.h Block.cpp AttributeUniValue.h AttributeMultiValue.h SupportTypes.h
	$(CC) $(CFLAGS) -c Block.cpp
Channel.o: Channel.h Channel.cpp
	$(CC) $(CFLAGS) -c Channel.cpp
clean:
	rm -f *.o Test