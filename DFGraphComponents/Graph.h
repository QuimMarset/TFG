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
    BBGraph(const string &bbName);
    ~BBGraph();

    void addBlock(Block *block);

    void freeBB();

    void printBB(ostream &file);

private:

    string bbName;
    vector <Block*> blocks;


};


class DFGraph 
{

public:

    DFGraph();
    DFGraph(const string &functionName);
    ~DFGraph();

    void addBasicBlock(const BBGraph& bb);
    void addBlockToBB(Block* block);
    void addBranch(Branch* branch);

    string getFunctionName();

    int getDefaultPortWidth();
    void setDefaultPortWidth(int width);

    void freeGraph();

    void printGraph(ostream &file);

private:
    int defaultPortWidth;
    string functionName;
    vector <BBGraph> basicBlocks;
};



}


#endif // GRAPH_H