#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <fstream>
#include "Block.h"
using namespace std;

class Cluster {

public:

    Cluster();
    Cluster(const string &basicBlockName);
    ~Cluster();

    void addBlock(Block* block);

    void printBasicBlock(ofstream &file);

private:

    string basicBlockName;
    vector <Block*> blocks;

};

#endif // CLUSTER_H