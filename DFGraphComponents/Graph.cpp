
#include "Graph.h"


namespace DFGraphComp
{

/*
 * =================================
 *  Class BBGraph
 * =================================
*/


BBGraph::BBGraph() {}

BBGraph::BBGraph(const string &bbName){
    this->bbName = bbName;
}

BBGraph::~BBGraph() {}

void BBGraph::addBlock(Block* block) {
    blocks.push_back(block);
}

void BBGraph::freeBB() {
    for (Block* block : blocks) {
        delete block;
    }
}

void BBGraph::printBB(ostream &file) {
    assert(bbName.length() > 0);
    file << "\tsubgraph cluster_" << bbName << " { " << endl;
    for (Block* block : blocks) {
        file << "\t\t";
        block->printBlock(file);
    }
    file << "\t\tlabel = \"" << bbName << "\"" << endl;
    file << "\t}" << endl;
}


/*
 * =================================
 *  Class DFGraph
 * =================================
*/


DFGraph::DFGraph() {}

DFGraph::DFGraph(const string &functionName) {
    this->functionName = functionName;
}

DFGraph::~DFGraph() {}

void DFGraph::addBasicBlock(const BBGraph& bb) {
    basicBlocks.push_back(bb);
}

void DFGraph::addBlockToBB(Block* block) {
    BBGraph& currentBB = basicBlocks.back();
    currentBB.addBlock(block);
}

string DFGraph::getFunctionName() {
    return functionName;
}

int DFGraph::getDefaultPortWidth() {
    return defaultPortWidth;
}

void DFGraph::setDefaultPortWidth(int width) {
    defaultPortWidth = width;
}

void DFGraph::freeGraph() {
    for (BBGraph& BB : basicBlocks) {
        BB.freeBB();
    }
}

void DFGraph::printGraph(ostream &file) {
    assert(functionName.length() > 0);
    file << "digraph \"DataFlow Graph for '" + functionName + "' function\" {" << endl;
    file << "\tlabel=\"DataFlow Graph for '" + functionName + "' function\";" << endl;
    file << endl;
    if (defaultPortWidth >= 0) {
        file << "\tchannel_width = " << defaultPortWidth << endl;
    }
    for (BBGraph& bb : basicBlocks) {
        bb.printBB(file);
    }
    file << endl;
    file << "}" << endl;
}


}