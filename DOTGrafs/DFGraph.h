#ifndef DFGRAPH_H
#define DFGRAPH_H

#include <vector>
#include <fstream>
#include <string>
#include <assert.h>
#include "Cluster.h"
#include "Channel.h"
using namespace std;

class DFGraph {

public:

    DFGraph();
    DFGraph(const string& functionName);
    ~DFGraph();

    string getFunctionName();
    void setFunctionName(const string &functionName);

    void addCluster(Cluster* cluster);
    void addEdge(Channel* edge);

    void printGraph();

private:

    string functionName;
    ofstream DOTFile;
    vector <Cluster*> clusters;
    vector <Channel*> edges;

};


#endif // DFGRAPH_H