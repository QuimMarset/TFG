
#include "DFGraph.h"

int main () {
    string name1 = "const1";
    Constant<int> const1(name1, 3);
    string name2 = "const2";
    Constant<float> const2(name2, 3.2);
    string name3 = "branch1";
    Branch branch1(name3);
    DFGraph graf("prova");
    graf.addNode(&const1);
    graf.addNode(&const2);
    graf.addNode(&branch1);
    graf.printGraph();
    
    return 0;

}