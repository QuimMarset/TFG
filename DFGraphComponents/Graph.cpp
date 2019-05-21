
#include "Graph.h"


namespace DFGraphComp
{

/*
 * =================================
 *  Class BBGraph
 * =================================
*/


BBGraph::BBGraph() {
    BBName = "";
}

BBGraph::BBGraph(const string &BBName){
    this->BBName = BBName;
}

BBGraph::~BBGraph() {}

void BBGraph::addBlock(Block* block) {
    blocks.push_back(block);
}

void BBGraph::addControlBlock(Block* block) {
    controlBlocks.push_back(block);
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

void BBGraph::printBB(ostream &file) {
    assert(BBName.length() > 0);
    file << "\tsubgraph cluster_" << BBName << " { " << endl;
    for (Block* block : blocks) {
        file << "\t\t";
        block->printBlock(file);
    }
    file << "\t\tlabel = \"" << BBName << "\"" << endl;
    file << "\t}" << endl;
}

void BBGraph::printChannels(ostream& file) {
    file << "// " << BBName << endl;
    for (Block* block : blocks) {
        cout << block->getBlockName() << endl;
        block->printChannels(file);
    }
}

void BBGraph::printControlBlocks(ostream& file) {
    assert(BBName.length() > 0);
    file << "\tsubgraph cluster_Control_" << BBName << " { " << endl;
    for (Block* block : controlBlocks) {
        file << "\t\t";
        block->printBlock(file);
    }
    file << "\t\tlabel = \"Control_" << BBName << "\"" << endl;
    file << "\t}" << endl;
}

void BBGraph::printControlChannels(ostream& file) {
    file << "// Control_" << BBName << endl;
    for (Block* block: controlBlocks) {
        block->printChannels(file);
    }
}

/*
 * =================================
 *  Class DFGraph
 * =================================
*/


DFGraph::DFGraph() {
    functionName = "";
    defaultPortWidth = -1;
}

DFGraph::DFGraph(const string &functionName) {
    this->functionName = functionName;
    defaultPortWidth = -1;
}

DFGraph::~DFGraph() {}

void DFGraph::addBasicBlock() {
    basicBlocks.push_back("BB" + to_string(basicBlocks.size()));
}

void DFGraph::addBlockToBB(Block* block) {
    BBGraph& currentBB = basicBlocks.back();
    currentBB.addBlock(block);
}

void DFGraph::addControlBlockToBB(Block* block) {
    BBGraph& currentBB = basicBlocks.back();
    currentBB.addControlBlock(block);
}

string DFGraph::getFunctionName() {
    return functionName;
}

void DFGraph::setFunctionName(const string& funcitonName) {
    this->functionName = funcitonName;
}

int DFGraph::getDefaultPortWidth() {
    return defaultPortWidth;
}

void DFGraph::setDefaultPortWidth(int width) {
    defaultPortWidth = width;
}

void DFGraph::freeGraph() {
    for (unsigned int i = 0; i < basicBlocks.size(); ++i) {
        basicBlocks[i].freeBB();
    }
    basicBlocks.clear();
}

void DFGraph::printGraph(ostream &file) {
    assert(functionName.length() > 0);
    file << "digraph \"DataFlow Graph for '" + functionName + "' function\" {" << endl;
    file << "\tlabel=\"DataFlow Graph for '" + functionName + "' function\";" << endl;
    file << endl;
    if (defaultPortWidth >= 0) {
        file << "\tchannel_width = " << defaultPortWidth << endl;
    }
    for (unsigned int i = 0; i < basicBlocks.size(); ++i) {
        file << endl;
        basicBlocks[i].printBB(file);
    }
    for (unsigned int i = 0; i < basicBlocks.size(); ++i) {
        file << endl;
        basicBlocks[i].printControlBlocks(file);
    }
    for (unsigned int i = 0; i < basicBlocks.size(); ++i) {
        file << endl;
        basicBlocks[i].printChannels(file);
    }
    for (unsigned int i = 0; i < basicBlocks.size(); ++i) {
        file << endl;
        basicBlocks[i].printControlChannels(file);
    }
    file << endl;
    file << "}" << endl;
}


}