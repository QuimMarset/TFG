#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <fstream>
#include <assert.h>
#include "Block.h"
using namespace std;

class Cluster {

public:

    Cluster();
    Cluster(const string &basicBlockName);
    ~Cluster();

    string getBasicBlockName();
    void setBasicBlockName(const string &basicBlockName);

    void addBlock(Block* block);

    void printBasicBlock(ostream &file); 
    // friend ostream &operator << (ostream &file, const Cluster &cluster);

private:

    string basicBlockName;
    vector <Block*> blocks;

};

#endif // CLUSTER_H