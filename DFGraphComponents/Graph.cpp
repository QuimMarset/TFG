
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

unsigned int BBGraph::getId() {
    return id;
}

void BBGraph::setId(unsigned int id) {
    this->id = id;
}

string BBGraph::getBBName() {
    return BBName;
}

void BBGraph::freeBB() {
    for (unsigned int i = 0; i < blocks.size(); ++i) {
        delete blocks[i];
    }
    for (unsigned int i = 0; i < controlBlocks.size(); ++i) {
        delete controlBlocks[i];
    }
}

void BBGraph::printBBNodes(ostream &file) {
    assert(BBName.length() > 0 && "Needed name");
    file << "\t\tsubgraph cluster_" << BBName << " {" << endl;
    for (unsigned int i = 0; i < blocks.size(); ++i) {
        file << "\t\t\t";
        blocks[i]->printBlock(file);
    }
    if (controlBlocks.size() > 0) {
        file << "\t\t\tsubgraph cluster_Control_" << BBName << " {" << endl;
        for (unsigned int i = 0; i < controlBlocks.size(); ++i) {
            file << "\t\t\t\t";
            controlBlocks[i]->printBlock(file);
        }
        file << "\t\t\t\tlabel = \"Control_" << BBName << "\"" << endl;
        file << "\t\t\t\tcolor = red" << endl;
        file << "\t\t\t}" << endl;
    }
    file << "\t\t\tlabel = \"" << BBName << "\"" << endl;
    file << "\t\t}" << endl;
}

void BBGraph::printBBEdges(ostream& file) {
    for (unsigned int i = 0; i < blocks.size(); ++i) {
        if (i == 0 and blocks[i]->getConnectedPort().first != nullptr)
            file << "\t// " << BBName << endl;
        blocks[i]->printChannels(file);
    }
    if (controlBlocks.size() > 0) {
        for (unsigned int i = 0; i < controlBlocks.size(); ++i) {
            if (i == 0 and controlBlocks[i]->getConnectedPort().first != nullptr)
                file << "\t// Control_" << BBName << endl;
            controlBlocks[i]->printChannels(file);
        }
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
    /* Stored a name like BB0 or BB1 because some names LLVM creates are
        incompatible with what DOT permits in the creation of a subgraph */
    basicBlocks[BBName] = BBGraph("BB" + to_string(basicBlocks.size()), id);
    currentBB = &basicBlocks[BBName];
}

bool FunctionGraph::existsBB(StringRef BBName) {
    return (basicBlocks.find(BBName) != basicBlocks.end());
}

void FunctionGraph::setCurrentBB(StringRef BBName) {
    assert(basicBlocks.find(BBName) != basicBlocks.end() && "BB not found");
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

unsigned int FunctionGraph::getBBId() {
    return currentBB->getId();
}

unsigned int FunctionGraph::getBBId(StringRef BBName) {
    return basicBlocks[BBName].getId();
}

void FunctionGraph::setBBId(unsigned int id) {
    currentBB->setId(id);
}

void FunctionGraph::setBBId(StringRef BBName, unsigned int id) {
    basicBlocks[BBName].setId(id);
}

void FunctionGraph::addArgument(Argument* block) {
    arguments.push_back(block);
}

Argument* FunctionGraph::getArgument(unsigned int index) {
    assert(index < arguments.size() && "Wrong argument");
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

Block* FunctionGraph::getFunctionControlOut() {
    return controlOut;
}

void FunctionGraph::setFunctionControlOut(Block* block) {
    controlOut = block;
}

unsigned int FunctionGraph::getTimesCalled() {
    return wrapper.timesCalled;
}

void FunctionGraph::increaseTimesCalled() {
    wrapper.timesCalled += 1;
}

void FunctionGraph::addWrapperCallArg(Merge* block) {
    wrapper.argsCall.push_back(block);
}

Merge* FunctionGraph::getWrapperCallArg(unsigned int index) {
    assert(index < wrapper.argsCall.size() && "Wrong wrapper parameter");
    return wrapper.argsCall[index];
}

Merge* FunctionGraph::getWrapperControlIn() {
    return wrapper.controlIn;
}

void FunctionGraph::setWrapperControlIn(Merge* block) {
    wrapper.controlIn = block;
}

void FunctionGraph::addWrapperControlFork(Fork* block) {
    wrapper.controlInForks.push_back(block);
}

Fork* FunctionGraph::getWrapperControlFork(unsigned int index) {
    assert(index < wrapper.controlInForks.size() && "Wrong wrapper fork");
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
    assert(index < wrapper.callBlocks.size() && "Wrong wrapper dummy block");
    return wrapper.callBlocks[index];
}

void FunctionGraph::addFunctionCallBlock(FunctionCall* block) {
    wrapper.callBlocks.push_back(block);
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

void FunctionGraph::printNodes(ostream &file, Function& F) {
    assert(functionName.length() > 0 && "Need function name");
    file << "\tsubgraph cluster_" + functionName + " {" << endl;
    file << "\t\tlabel = \"DataFlow Graph for '" + functionName + "' function\";" << endl;
    if (defaultPortWidth >= 0) {
        file << "\t\tchannel_width = " << defaultPortWidth << endl;
    }
    for (const BasicBlock& BB : F.getBasicBlockList()) {
        basicBlocks[BB.getName()].printBBNodes(file);
    }
    if (controlOut != nullptr and controlOut->getBlockType() == BlockType::Merge_Block) {
        file << "\t\t// Outter blocks" << endl;
        file << "\t\t";
        controlOut->printBlock(file);
        file << "\t\t";
        result->printBlock(file);
    }
    if (wrapper.timesCalled > 1) {
        file << "\t\t// Call wrapper blocks" << endl;
        file << "\t\t";
        wrapper.controlIn->printBlock(file);
        for (unsigned int i = 0; i < wrapper.argsCall.size(); ++i) {
            file << "\t\t";
            wrapper.argsCall[i]->printBlock(file);
        }
        for (unsigned int i = 0; i < wrapper.controlInForks.size(); ++i) {
            file << "\t\t";
            wrapper.controlInForks[i]->printBlock(file);
        }
        file << "\t\t";
        wrapper.result->printBlock(file);
        file << "\t\t";
        wrapper.controlOut->printBlock(file);
    }
    file << "\t}" << endl;
}

void FunctionGraph::printEdges(ostream& file, Function& F) {
    file << "\t// " << functionName << " Channels" << endl; 
    for (const BasicBlock& BB : F.getBasicBlockList()) {
        basicBlocks[BB.getName()].printBBEdges(file);
    }
    if (controlOut != nullptr and controlOut->getBlockType() == BlockType::Merge_Block) {
        file << "\t// Outter blocks channels" << endl;
        controlOut->printChannels(file);
        result->printChannels(file);
    }
    if (wrapper.timesCalled > 1) {
        file << "\t// Call wrapper channels" << endl;
        wrapper.controlIn->printChannels(file);
        for (unsigned int i = 0; i < wrapper.argsCall.size(); ++i) {
            wrapper.argsCall[i]->printChannels(file);
        }
        for (unsigned int i = 0; i < wrapper.controlInForks.size(); ++i) {
            wrapper.controlInForks[i]->printChannels(file);
        }
        wrapper.result->printChannels(file);
        wrapper.controlOut->printChannels(file);
    }
}


}