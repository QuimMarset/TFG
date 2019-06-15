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

    int getId();

    string getBBName();

    void freeBB();

    void printBBNodes(ostream &file);
    void printBBEdges(ostream &file);

private:

    string BBName;
    vector <Block*> blocks;
    vector <Block*> controlBlocks;
    int id;

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

    int getBBId();
    int getBBId(StringRef BBName);

    void addArgument(Argument* block);
    Argument* getArgument(unsigned int index);
    unsigned int getNumArguments();

    Block* getFunctionResult();
    void setFunctionResult(Block* block);

    Entry* getFunctionControlIn();
    void setFunctionControlIn(Entry* block);
    
    Exit* getFunctionControlOut();
    void setFunctionControlOut(Exit* block);

    unsigned int getTimesCalled();
    void increaseTimesCalled();

    void addWrapperCallParam(Block* block);
    Block* getWrapperCallParam(unsigned int index);
    void setWrapperCallParam(unsigned int index, Block* block);

    Block* getWrapperControlIn();
    void setWrapperControlIn(Block* block);

    void addWrapperControlFork(Fork* block);
    Fork* getWrapperControlFork(unsigned int index);
    
    Demux* getWrapperControlOut();
    void setWrapperControlOut(Demux* block);

    Demux* getWrapperResult();
    void setWrapperResult(Demux* block);

    FunctionCall* getFunctionCallBlock(unsigned int index);
    void addFunctionCallBlock(FunctionCall* block);

    const CallInst* getFirstCallInst();
    void setFirstCallInst(const CallInst* inst);

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
    Exit* controlOut;

    struct CallWrapper 
    {
        unsigned int timesCalled;
        vector <FunctionCall*> callBlocks;
        vector <Block*> paramsCall;
        Block* controlIn;
        vector <Fork*> controlInForks;
        Demux* controlOut;
        Demux* result;
        const CallInst* firstCallInst;
    };

    CallWrapper wrapper;
    
    
    // vector <BBGraph> basicBlocks;

};



}


#endif // GRAPH_H