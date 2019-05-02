#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <fstream>
#include <assert.h>
#include "Block.h"
#include "Channel.h"
using namespace std;

namespace DFGraphComp
{


class Cluster {

public:

    Cluster();
    Cluster(const string &basicBlockName);
    ~Cluster();

    string getBasicBlockName();
    void setBasicBlockName(const string &basicBlockName);

    void addBlock(const Block& block);
    void addChannel(const Channel& channel);

    void printBasicBlock(ostream &file); 
    // friend ostream &operator << (ostream &file, const Cluster &cluster);

private:

    string basicBlockName;
    vector <Block> blocks;
    vector <Channel> channels;

};


} // Close namespace

#endif // CLUSTER_H