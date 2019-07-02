#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Block.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"

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
    /* This id is used when we have to connect a branch with some non-direct successor
        that we need to know if we have to use the true or the false port. In fact
        we use it when we have to connect the merges at the end of processing all the BB
        inside the function. This id is simply the ordinal in which this block has been processed */
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

    void addWrapperCallArg(Merge* block);
    Merge* getWrapperCallArg(unsigned int index);

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

    void printNodes(ostream &file, Function& F);
    void printEdges(ostream &file, Function& F);

private:

    int defaultPortWidth;
    string functionName;
    map <StringRef, BBGraph> basicBlocks;
    BBGraph* currentBB;

    /* In addition to storing the BB, we also store the possible blocks that will
        form the wrapper, as well as keeping references of the blocks that will be
        connected to the wrapper, or with the blocks in the caller function if this 
        function is called only once. */
    vector <DFGraphComp::Argument*> arguments;
    Block* result;
    Entry* controlIn;
    Block* controlOut;

    struct CallWrapper 
    {
        /*Know the times called to add the wrapper if needed, modifying the connections 
            of the first call */
        unsigned int timesCalled;
        /* We keep the dummy block in the caller block representing each function call
            to later modify the needed connections */
        vector <FunctionCall*> callBlocks;
        vector <Merge*> argsCall;
        Merge* controlIn;
        vector <Fork*> controlInForks;
        Demux* controlOut;
        Demux* result;
    };
    CallWrapper wrapper;
    
};



}


#endif // GRAPH_H