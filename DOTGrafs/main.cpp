
#include "DFGraph.h"

int main () {

    vector <Component::Port> input;
    vector <Component::Port> output;
    Component::Port p1, p2, p3, p4;
    p1.name = "in1";
    p1.delay = 32;
    p2.name = "in2";
    p2.delay = 32;
    p3.name = "out1";
    p3.delay = 32;
    p4.name = "out2";
    p4.delay = 32;
    input.push_back(p1);
    input.push_back(p2);
    output.push_back(p3);
    output.push_back(p4);
    string name = "abc";
    Component comp(name, Component::BlockType::Operator, input, output);
    Buffer comp2(name, input, output, 24, false);
    DFGraph graf("prova");
    graf.addNode(&comp);
    graf.addNode(&comp2);
    graf.printGraph();
    //graf.closeGraph();
    
    return 0;

}