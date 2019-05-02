
#include "Cluster.h"


namespace DFGraphComp
{


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

string Cluster::getBasicBlockName() {
    return basicBlockName;
}

void Cluster::setBasicBlockName(const string &basicBlockName) {
    this->basicBlockName = basicBlockName;
}

void Cluster::addBlock(const Block& block) {
    blocks.push_back(block);
}

void Cluster::addChannel(const Channel& channel) {
    channels.push_back(channel);
}

void Cluster::printBasicBlock(ostream &file) {
    assert(basicBlockName.length() > 0);
    file << "\tsubgraph cluster_" << basicBlockName << " { " << endl;
    for (Block& block : blocks) {
        file << "\t\t";
        block.printBlock(file);
        block.closeBlock(file);
    }
    file << "\t\tlabel = \"" << basicBlockName << "\"" << endl;
    file << "\t}" << endl;
}

// ostream &operator << (ostream &out, const Cluster &cluster) {
//     cluster.printBasicBlock(out);
//     return out;
// }


} // Close namespace