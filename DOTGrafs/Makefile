CFLAGS = -g -Wall -std=c++11
CC = g++
Test: Attribute.o SupportTypes.o Block.o Cluster.o Channel.o DFGraph.o main.o 
	$(CC) $(CFLAGS) -o Test Attribute.o SupportTypes.o Block.o Cluster.o Channel.o DFGraph.o main.o
main.o: main.cpp DFGraph.h
	$(CC) $(CFLAGS) -c main.cpp
DFGraph.o: DFGraph.h DFGraph.cpp Cluster.h Channel.h
	$(CC) $(CFLAGS) -c DFGraph.cpp
Cluster.o: Cluster.h Cluster.cpp Block.h
	$(CC) $(CFLAGS) -c Cluster.cpp
Channel.o: Channel.h Channel.cpp
	$(CC) $(CFLAGS) -c Channel.cpp
Block.o: Block.h Block.cpp AttributeUniValue.h AttributeMultiValue.h SupportTypes.h
	$(CC) $(CFLAGS) -c Block.cpp
SupportTypes.o : SupportTypes.h SupportTypes.cpp
	$(CC) $(CFLAGS) -c SupportTypes.cpp
Attribute.o: Attribute.h Attribute.cpp
	$(CC) $(CFLAGS) -c Attribute.cpp
clean:
	rm -f *.o Test