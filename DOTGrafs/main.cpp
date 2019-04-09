
#include "DFGraph.h"
#include <iostream>

int main () {
    string name1 = "const1";
    Constant<int> const1(name1, 3);
    string name2 = "const2";
    Constant<float> const2(name2, 3.2);
    string name3 = "branch1";
    Branch branch1(name3);
    Cluster clus1("bb1");
    clus1.addBlock(&const1);
    //clus1.addBlock(const2);
    clus1.addBlock(&branch1);
    Cluster clus2("bb2");
    clus2.addBlock(&const2);
    DFGraph graf("prova");
    Channel ch("const1", "branch1", "out", "in");
    graf.addEdge(&ch);
    graf.addCluster(&clus1);
    graf.addCluster(&clus2);
    branch1.setInPortDelay(0, 16);
    graf.printGraph();
    
    return 0;

}