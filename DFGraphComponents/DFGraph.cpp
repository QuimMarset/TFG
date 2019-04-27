
#include "DFGraph.h"


DFGraph::DFGraph() {
    functionName = "";
}

DFGraph::DFGraph(const string& functionName) {
    this->functionName = functionName;
    DOTFile.open(functionName + ".dot");   
}

DFGraph::~DFGraph() {
    DOTFile.close();
}

string DFGraph::getFunctionName() {
    return functionName;
}

void DFGraph::setFunctionName(const string &functionName) {
    this->functionName = functionName;
}

void DFGraph::addCluster(Cluster* cluster) {
    clusters.push_back(cluster);
}

void DFGraph::addEdge(Channel* edge) {
    edges.push_back(edge);
}

void DFGraph::printGraph() {
    assert(functionName.length() > 0);
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

