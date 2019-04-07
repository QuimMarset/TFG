
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
    nodes.clear();
    edges.clear();
    DOTFile.close();
}

void DFGraph::addNode(Component* node) {
    nodes.push_back(node);
}
void DFGraph::addEdge(Channel &edge) {
    edges.push_back(edge);
}


void DFGraph::printGraph() {

    DOTFile << "digraph \"DataFlow Graph for '" + functionName + "' function\" {" << endl;
    DOTFile << "\tlabel=\"DataFlow Graph for '" + functionName + "' function\";" << endl;
    DOTFile << endl;
    for (Component* node : nodes) {
        DOTFile << "\t";
        node->printBlock(DOTFile);
        node->closeBlock(DOTFile);
    }
    for (Channel &edge : edges) {
        
    }
    DOTFile << endl;
    DOTFile << "}" << endl;
}

