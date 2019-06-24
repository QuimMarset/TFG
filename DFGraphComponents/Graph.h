#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Block.h"
#include "llvm/ADT/StringRef.h"

using namespace std;
using namespace llvm;

namespace DFGraphComp 
{


class BBGraph
{

public:

    BBGraph();
    BBGraph(const string &BBName, int id);
    ~BBGraph();

    void addBlock(Block *block);
    void addControlBlock(Block* block);

    unsigned int getId();
    void setId(unsigned int id);

    string getBBName();

    void freeBB();

    void printBBNodes(ostream &file);
    void printBBEdges(ostream &file);

private:

    string BBName;
    vector <Block*> blocks;
    vector <Block*> controlBlocks;
    unsigned int id;

};


class FunctionGraph 
{

public:

    FunctionGraph();
    FunctionGraph(const string& functionName);
    ~FunctionGraph();

    void addBasicBlock(StringRef BBName, int id);

    bool existsBB(StringRef BBName);

    void setCurrentBB(StringRef BBName);

    void addBlockToBB(Block* block);
    void addBlockToBB(StringRef BBName, Block* block);

    void addControlBlockToBB(Block* block);
    void addControlBlockToBB(StringRef BBName, Block* block);

    unsigned int getBBId();
    unsigned int getBBId(StringRef BBName);
    void setBBId(unsigned int id);
    void setBBId(StringRef BBName, unsigned int id);

    void addArgument(Argument* block);
    Argument* getArgument(unsigned int index);
    unsigned int getNumArguments();

    Block* getFunctionResult();
    void setFunctionResult(Block* block);

    Entry* getFunctionControlIn();
    void setFunctionControlIn(Entry* block);
    
    Block* getFunctionControlOut();
    void setFunctionControlOut(Block* block);

    unsigned int getTimesCalled();
    void increaseTimesCalled();

    void addWrapperCallParam(Merge* block);
    Merge* getWrapperCallParam(unsigned int index);

    Merge* getWrapperControlIn();
    void setWrapperControlIn(Merge* block);

    void addWrapperControlFork(Fork* block);
    Fork* getWrapperControlFork(unsigned int index);
    
    Demux* getWrapperControlOut();
    void setWrapperControlOut(Demux* block);

    Demux* getWrapperResult();
    void setWrapperResult(Demux* block);

    FunctionCall* getFunctionCallBlock(unsigned int index);
    void addFunctionCallBlock(FunctionCall* block);

    string getFunctionName();
    void setFunctionName(const string& funcitonName);

    int getDefaultPortWidth();
    void setDefaultPortWidth(unsigned int width);

    void freeGraph();

    void printNodes(ostream &file);
    void printEdges(ostream &file);

private:

    int defaultPortWidth;
    string functionName;
    map <StringRef, BBGraph> basicBlocks;
    BBGraph* currentBB;

    vector <DFGraphComp::Argument*> arguments;
    Block* result;
    Entry* controlIn;
    Block* controlOut;

    struct CallWrapper 
    {
        unsigned int timesCalled;
        vector <FunctionCall*> callBlocks;
        vector <Merge*> paramsCall;
        Merge* controlIn;
        vector <Fork*> controlInForks;
        Demux* controlOut;
        Demux* result;
    };
    CallWrapper wrapper;
    
};



}


#endif // GRAPH_H