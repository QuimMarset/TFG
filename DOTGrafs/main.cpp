
#include "DFGraph.h"
#include <iostream>

int main () {

    Constant<int> const1("fact", 1);
    Constant<int> const2("i", 2);
    Constant<int> const3("n", 5);
    Cluster clust1("Entry");
    clust1.addBlock(&const1);
    clust1.addBlock(&const2);
    clust1.addBlock(&const3);

    Merge merge1("mergeFact", 2);
    Merge merge2("mergeI", 2);
    Merge merge3("mergeN", 2);
    Fork fork1("forkN", 2);
    Fork fork2("forkI", 2);
    Operator op1("opIcmp", 2);
    Fork fork3("forkCmp", 3);
    Branch branch1("branchN");
    Branch branch2("branchI");
    Branch branch3("branchFact");
    Cluster clust2("While_cond");
    clust2.addBlock(&merge1);
    clust2.addBlock(&merge2);
    clust2.addBlock(&merge3);
    clust2.addBlock(&fork1);
    clust2.addBlock(&fork2);
    clust2.addBlock(&op1);
    clust2.addBlock(&fork3);
    clust2.addBlock(&branch1);
    clust2.addBlock(&branch2);
    clust2.addBlock(&branch3);

    Constant<int> const4("constIncr", 1);
    Fork fork4("forkI2", 2);
    Operator op2("opAdd", 2);
    Operator op3("opMul", 2, 3, 2);
    Cluster clust3("While_body");
    clust3.addBlock(&fork4);
    clust3.addBlock(&op2);
    clust3.addBlock(&op3);
    clust3.addBlock(&const4);

    Operator op4("opRet", 1, false);
    Cluster clust4("While_end");
    clust4.addBlock(&op4);

    Channel channel1("fact", "mergeFact", "out", "in1");
    Channel channel2("i", "mergeI", "out", "in1");
    Channel channel3("n", "mergeN", "out", "in1");
    Channel channel4("mergeN", "forkN", "out", "in");
    Channel channel5("mergeI", "forkI", "out", "in");
    Channel channel6("forkN", "opIcmp", "out1", "in1");
    Channel channel7("forkN", "branchN", "out2", "in");
    Channel channel8("forkI", "opIcmp", "ou1", "in2");
    Channel channel9("forkI", "branchI", "out2", "in");
    Channel channel10("mergeFact", "branchFact", "out", "in");
    Channel channel11("opIcmp", "forkCmp", "out", "in");
    Channel channel12("forkCmp", "branchN", "out1", "inCondition");
    Channel channel13("forkCmp", "branchI", "out2", "inCondition");
    Channel channel14("forkCmp", "branchFact", "out3", "inCondition");
    Channel channel15("branchN", "mergeN", "outTrue", "in2");
    Channel channel16("branchI", "forkI2", "outTrue", "in");
    Channel channel17("branchFact", "opMul", "outTrue", "in1");
    Channel channel18("branchFact", "opRet", "outFalse", "in");
    Channel channel19("constIncr", "opAdd", "out", "in1");
    Channel channel20("forkI2", "opAdd", "out1", "in2");
    Channel channel21("forkI2", "opMul", "out2", "in2");
    Channel channel22("opAdd", "mergeI", "out", "in2");
    Channel channel23("opMul", "mergeFact", "out", "in2");


    DFGraph graf("Fact5");
    graf.addCluster(&clust1);
    graf.addCluster(&clust2);
    graf.addCluster(&clust3);
    graf.addCluster(&clust4);

    graf.addEdge(&channel1);
    graf.addEdge(&channel2);
    graf.addEdge(&channel3);
    graf.addEdge(&channel4);
    graf.addEdge(&channel5);
    graf.addEdge(&channel6);
    graf.addEdge(&channel7);
    graf.addEdge(&channel8);
    graf.addEdge(&channel9);
    graf.addEdge(&channel10);
    graf.addEdge(&channel11);
    graf.addEdge(&channel12);
    graf.addEdge(&channel13);
    graf.addEdge(&channel14);
    graf.addEdge(&channel15);
    graf.addEdge(&channel16);
    graf.addEdge(&channel17);
    graf.addEdge(&channel18);
    graf.addEdge(&channel19);
    graf.addEdge(&channel20);
    graf.addEdge(&channel21);
    graf.addEdge(&channel22);
    graf.addEdge(&channel23);

    graf.printGraph();
    
    return 0;

}