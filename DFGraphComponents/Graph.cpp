
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

void BBGraph::printBB(ostream &file) {
    assert(bbName.length() > 0);
    file << "\tsubgraph cluster_" << bbName << " { " << endl;
    for (Block* block : blocks) {
        file << "\t\t";
        block->printBlock(file);
        block->closeBlock(file);
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

void DFGraph::addBranch(Branch* branch) {
    branches.push_back(branch);
}


void DFGraph::printGraph(ostream &file) {
    assert(functionName.length() > 0);
    file << "digraph \"DataFlow Graph for '" + functionName + "' function\" {" << endl;
    file << "\tlabel=\"DataFlow Graph for '" + functionName + "' function\";" << endl;
    file << endl;
    for (BBGraph& bb : basicBlocks) {
        bb.printBB(file);
    }
    for (Branch* br : branches) {
        br->printBlock(file);
    }
    file << endl;
    file << "}" << endl;
}


}