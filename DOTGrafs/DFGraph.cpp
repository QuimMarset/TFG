
#include "DFGraph.h"


DFGraph::DFGraph() {
    functionName = "";
}

DFGraph::DFGraph(const string& functionName) {
    this->functionName = functionName;
    string fileName = functionName + ".dot";
    DOTFile.open(fileName);   
}

DFGraph::~DFGraph() {
    functionName = "";
    //nodes.clear();
    clusters.clear();
    edges.clear();
    DOTFile.close();
}

string DFGraph::getFunctionName() {
    return functionName;
}

void DFGraph::setFunctionName(const string &functionName) {
    this->functionName = functionName;
}

// void DFGraph::addNode(Block* node) {
//     nodes.push_back(node);
// }

void DFGraph::addCluster(Cluster* cluster) {
    clusters.push_back(cluster);
}

void DFGraph::addEdge(Channel* edge) {
    edges.push_back(edge);
}

void DFGraph::printGraph() {

    DOTFile << "digraph \"DataFlow Graph for '" + functionName + "' function\" {" << endl;
    DOTFile << "\tlabel=\"DataFlow Graph for '" + functionName + "' function\";" << endl;
    DOTFile << endl;
    for (Cluster* cluster : clusters) {
        cluster->printBasicBlock(DOTFile);
    }
    DOTFile << endl;
    for (Channel* edge : edges) {
        DOTFile << "\t";
        edge->printChannel(DOTFile);
    }
    DOTFile << endl;
    DOTFile << "}" << endl;
}

