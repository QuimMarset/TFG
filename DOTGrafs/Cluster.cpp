
#include "Cluster.h"


Cluster::Cluster() {
    basicBlockName = "";
}

Cluster::Cluster(const string &basicBlockName) {
    this->basicBlockName = basicBlockName;
}

Cluster::~Cluster() {
    basicBlockName = "";
    blocks.clear();
}

void Cluster::addBlock(Block* block) {
    blocks.push_back(block);
}

void Cluster::printBasicBlock(ostream &file) const {
    file << "subgraph cluster_" << basicBlockName << " { " << endl;
    for (Block* block : blocks) {
        file << "\t";
        block->printBlock(file);
        block->closeBlock(file);
    }
    file << "\tlabel = \"" << basicBlockName << "\"" << endl;
    file << "}" << endl;
}

ostream &operator << (ostream &out, const Cluster &cluster) {
    cluster.printBasicBlock(out);
    return out;
}