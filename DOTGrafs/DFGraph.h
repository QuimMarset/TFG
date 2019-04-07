#ifndef DFGRAPH_H
#define DFGRAPH_H


#include "Component.h"
#include "Channel.h"
#include <vector>
#include <fstream>
#include <string>
using namespace std;

class DFGraph {

public:

    DFGraph();
    DFGraph(const string& functionName);
    ~DFGraph();
    void addNode(Component* node);
    void addEdge(Channel &edge);
    void printGraph();

private:

    string functionName;
    ofstream DOTFile;
    vector <Component*> nodes;
    vector <Channel> edges;

};


#endif // DFGRAPH_H