#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Block.h"

using namespace std;

namespace DFGraphComp 
{


class BBGraph
{

public:

    BBGraph();
    BBGraph(const string &BBName);
    ~BBGraph();

    void addBlock(Block *block);
    void addControlBlock(Block* block);

    string getBBName();

    void freeBB();

    void printBB(ostream &file);
    void printChannels(ostream& file);
    void printControlBlocks(ostream& file);
    void printControlChannels(ostream& file);

private:

    string BBName;
    vector <Block*> blocks;
    vector <Block*> controlBlocks;

};


class DFGraph 
{

public:

    DFGraph();
    DFGraph(const string& functionName);
    ~DFGraph();

    void addBasicBlock();

    void addBlockToBB(Block* block);
    void addControlBlockToBB(Block* block);

    string getFunctionName();
    void setFunctionName(const string& funcitonName);

    int getDefaultPortWidth();
    void setDefaultPortWidth(int width = -1);

    void freeGraph();

    void printGraph(ostream &file);

private:
    int defaultPortWidth;
    string functionName;
    vector <BBGraph> basicBlocks;

};



}


#endif // GRAPH_H