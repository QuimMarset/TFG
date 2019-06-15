
#include "Graph.h"


namespace DFGraphComp
{

/*
 * =================================
 *  Class BBGraph
 * =================================
*/


BBGraph::BBGraph() {}

BBGraph::BBGraph(const string &BBName, int id){
    this->BBName = BBName;
    this->id = id;
}

BBGraph::~BBGraph() {}

void BBGraph::addBlock(Block* block) {
    blocks.push_back(block);
}

void BBGraph::addControlBlock(Block* block) {
    controlBlocks.push_back(block);
}

int BBGraph::getId() {
    return id;
}

string BBGraph::getBBName() {
    return BBName;
}

void BBGraph::freeBB() {
    for (Block* block : blocks) {
        delete block;
    }
    for (Block* block : controlBlocks) {
        delete block;
    }
}

void BBGraph::printBBNodes(ostream &file) {
    assert(BBName.length() > 0);
    file << "\tsubgraph cluster_" << BBName << " { " << endl;
    for (Block* block : blocks) {
        file << "\t\t";
        block->printBlock(file);
    }
    file << endl;
    file << "\t\tsubgraph cluster_Control_" << BBName << "{" << endl;
    for (Block* block : controlBlocks) {
        file << "\t\t\t";
        block->printBlock(file);
    }
    file << "\t\t\tlabel = \"Control_" << BBName << "\"" << endl;
    file << "\t\t\tcolor = red" << endl;
    file << "\t\t}" << endl;

    file << "\t\tlabel = \"" << BBName << "\"" << endl;
    file << "\t}" << endl;
}

void BBGraph::printBBEdges(ostream& file) {
    file << "// " << BBName << endl;
    for (Block* block : blocks) {
        block->printChannels(file);
    }
    file << "// Control_" << BBName << "\"" << endl;
    for (Block* block : controlBlocks) {
        block->printChannels(file);
    }
}


/*
 * =================================
 *  Class FunctionGraph
 * =================================
*/


FunctionGraph::FunctionGraph() {}

FunctionGraph::FunctionGraph(const string &functionName) {
    this->functionName = functionName;
    defaultPortWidth = -1;
    result = nullptr;
    controlOut = nullptr;
    controlIn = nullptr;
    wrapper.timesCalled = 0;
    wrapper.controlIn = nullptr;
    wrapper.controlOut = nullptr;
    wrapper.result = nullptr;
}

FunctionGraph::~FunctionGraph() {}

void FunctionGraph::addBasicBlock(StringRef BBName, int id) {
    basicBlocks[BBName] = BBGraph("BB" + to_string(basicBlocks.size()), id);
    currentBB = &basicBlocks[BBName];
}

bool FunctionGraph::existsBB(StringRef BBName) {
    return (basicBlocks.find(BBName) != basicBlocks.end());
}

void FunctionGraph::setCurrentBB(StringRef BBName) {
    assert(basicBlocks.find(BBName) != basicBlocks.end());
    currentBB = &basicBlocks[BBName];
}

void FunctionGraph::addBlockToBB(Block* block) {
    currentBB->addBlock(block);
}

void FunctionGraph::addBlockToBB(StringRef BBName, Block* block) {
    basicBlocks[BBName].addBlock(block);
}

void FunctionGraph::addControlBlockToBB(Block* block) {
    currentBB->addControlBlock(block);
}

void FunctionGraph::addControlBlockToBB(StringRef BBName, Block* block) {
    basicBlocks[BBName].addControlBlock(block);
}

int FunctionGraph::getBBId() {
    return currentBB->getId();
}

int FunctionGraph::getBBId(StringRef BBName) {
    return basicBlocks[BBName].getId();
}

void FunctionGraph::addArgument(Argument* block) {
    arguments.push_back(block);
}

Argument* FunctionGraph::getArgument(unsigned int index) {
    assert(index < arguments.size());
    return arguments[index];
}

unsigned int FunctionGraph::getNumArguments() {
    return arguments.size();
}

Block* FunctionGraph::getFunctionResult() {
    return result;
}

void FunctionGraph::setFunctionResult(Block* block) {
    result = block;
}

Entry* FunctionGraph::getFunctionControlIn() {
    return controlIn;
}

void FunctionGraph::setFunctionControlIn(Entry* block) {
    controlIn = block;
}

Exit* FunctionGraph::getFunctionControlOut() {
    return controlOut;
}

void FunctionGraph::setFunctionControlOut(Exit* block) {
    controlOut = block;
}

unsigned int FunctionGraph::getTimesCalled() {
    return wrapper.timesCalled;
}

void FunctionGraph::increaseTimesCalled() {
    wrapper.timesCalled += 1;
}

void FunctionGraph::addWrapperCallParam(Block* block) {
    wrapper.paramsCall.push_back(block);
}

Block* FunctionGraph::getWrapperCallParam(unsigned int index) {
    assert(index < wrapper.paramsCall.size());
    return wrapper.paramsCall[index];
}

void FunctionGraph::setWrapperCallParam(unsigned int index, Block* block) {
    assert(index < wrapper.paramsCall.size());
    wrapper.paramsCall[index] = block;
}

Block* FunctionGraph::getWrapperControlIn() {
    return wrapper.controlIn;
}

void FunctionGraph::setWrapperControlIn(Block* block) {
    wrapper.controlIn = block;
}

void FunctionGraph::addWrapperControlFork(Fork* block) {
    wrapper.controlInForks.push_back(block);
}

Fork* FunctionGraph::getWrapperControlFork(unsigned int index) {
    assert(index < wrapper.controlInForks.size());
    return wrapper.controlInForks[index];
}

Demux* FunctionGraph::getWrapperControlOut() {
    return wrapper.controlOut;
}

void FunctionGraph::setWrapperControlOut(Demux* block) {
    wrapper.controlOut = block;
}

Demux* FunctionGraph::getWrapperResult() {
    return wrapper.result;
}

void FunctionGraph::setWrapperResult(Demux* block) {
    wrapper.result = block;
}

FunctionCall* FunctionGraph::getFunctionCallBlock(unsigned int index) {
    assert(index < wrapper.callBlocks.size());
    return wrapper.callBlocks[index];
}

void FunctionGraph::addFunctionCallBlock(FunctionCall* block) {
    wrapper.callBlocks.push_back(block);
}

const CallInst* FunctionGraph::getFirstCallInst() {
    return wrapper.firstCallInst;
}

void FunctionGraph::setFirstCallInst(const CallInst* inst) {
    wrapper.firstCallInst = inst;
}

string FunctionGraph::getFunctionName() {
    return functionName;
}

void FunctionGraph::setFunctionName(const string& funcitonName) {
    this->functionName = funcitonName;
}

int FunctionGraph::getDefaultPortWidth() {
    return defaultPortWidth;
}

void FunctionGraph::setDefaultPortWidth(unsigned int width) {
    defaultPortWidth = width;
}

void FunctionGraph::freeGraph() {
    for (map <StringRef, BBGraph>::iterator it = basicBlocks.begin();
        it != basicBlocks.end(); ++it) 
    {
        it->second.freeBB();
    }
    basicBlocks.clear();
}

void FunctionGraph::printNodes(ostream &file) {
    assert(functionName.length() > 0);
    file << "subgraph cluster_" + functionName + "{" << endl;
    file << "\tlabel=\"DataFlow Graph for '" + functionName + "' function\";" << endl;
    if (defaultPortWidth >= 0) {
        file << endl;
        file << "\tchannel_width = " << defaultPortWidth << endl;
    }
    for (map <StringRef, BBGraph>::iterator it = basicBlocks.begin();
        it != basicBlocks.end(); ++it) 
    {
        file << endl;
        it->second.printBBNodes(file);
    }
    file << endl;
    if (wrapper.timesCalled > 1) {
        file << "// Call wrapper blocks" << endl;
        file << '\t';
        wrapper.controlIn->printBlock(file);
        for (Block* param : wrapper.paramsCall) {
            Merge* merge = (Merge*)param;
            file << '\t';
            merge->printBlock(file);
        }
        for (Fork* fork : wrapper.controlInForks) {
            file << '\t';
            fork->printBlock(file);
        }
        file << '\t';
        wrapper.result->printBlock(file);
        file << '\t';
        wrapper.controlOut->printBlock(file);
    }
    file << "}" << endl;
}

void FunctionGraph::printEdges(ostream& file) {
    file << endl;
    file << "// " << functionName << " Channels" << endl; 
    file << endl;
    for (map <StringRef, BBGraph>::iterator it = basicBlocks.begin();
        it != basicBlocks.end(); ++it) 
    {
        file << endl;
        it->second.printBBEdges(file);
    }
    file << endl;
    if (wrapper.timesCalled > 1) {
        file << "// Call wrapper channels" << endl;
        wrapper.controlIn->printChannels(file);
        for (Block* param : wrapper.paramsCall) {
            Merge* merge = (Merge*)param;
            merge->printChannels(file);
        }
        for (Fork* fork : wrapper.controlInForks) {
            fork->printChannels(file);
        }
        wrapper.result->printChannels(file);
        wrapper.controlOut->printChannels(file);
    }
}


}