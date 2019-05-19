
// #include <iostream>
// #include "Graph.h"
// using namespace std;
// using namespace DFGraphComp;

// int main () {

//     DFGraph graph("test");
//     graph.addBasicBlock(BBGraph("BB1"));

//     Constant<int>* const1 = new Constant<int>(1);
//     Constant<int>* const2 = new Constant<int>(2);
//     Constant<int>* const3 = new Constant<int>(5);
//     pair<Block*, const Port*> cp = const1->getConnectedPort();
//     graph.addBlockToBB(const1);
//     graph.addBlockToBB(const2);
//     graph.addBlockToBB(const3);

//     graph.addBasicBlock(BBGraph("BB2"));
//     Merge* merge1 = new Merge(2);
//     Merge* merge2 = new Merge(2);
//     Merge* merge3 = new Merge(2);
//     Fork* fork1 = new Fork(2);
//     Fork* fork2 = new Fork(2);
//     fork2->setConnectedPort(make_pair(nullptr, nullptr));
//     fork2->setDataOutPortDelay(0, 34);
//     BinaryOperator* op1 = new BinaryOperator(BinaryOpType::LE);
//     Fork* fork3 = new Fork(3);
//     Branch* branch1 = new Branch(32);
//     Branch* branch2 = new Branch(32);
//     Branch* branch3 = new Branch(32);
//     graph.addBlockToBB(merge1);
//     graph.addBlockToBB(merge2);
//     graph.addBlockToBB(merge3);
//     graph.addBlockToBB(fork1);
//     graph.addBlockToBB(fork2);
//     graph.addBlockToBB(op1);
//     graph.addBlockToBB(fork3);
//     graph.addBlockToBB(branch1);
//     graph.addBlockToBB(branch2);
//     graph.addBlockToBB(branch3);

//     graph.addBasicBlock(BBGraph("BB3"));
//     Constant<int>* const4 = new Constant<int>(1);
//     Fork* fork4 = new Fork(2);
//     BinaryOperator* op2 = new BinaryOperator(BinaryOpType::Add);
//     BinaryOperator* op3 = new BinaryOperator(BinaryOpType::Mul);
//     graph.addBlockToBB(const4);
//     graph.addBlockToBB(fork4);
//     graph.addBlockToBB(op2);
//     graph.addBlockToBB(op3);

//     graph.addBasicBlock(BBGraph("BB4"));
//     Return* ret = new Return(32);
//     graph.addBlockToBB(ret);

//     ofstream file;
//     file.open("Test.dot");
//     graph.printGraph(file);
//     graph.freeGraph();
//     file.close();

//     // Channel channel1("fact", "mergeFact", "out", "in1");
//     // Channel channel2("i", "mergeI", "out", "in1");
//     // Channel channel3("n", "mergeN", "out", "in1");
//     // Channel channel4("mergeN", "forkN", "out", "in");
//     // Channel channel5("mergeI", "forkI", "out", "in");
//     // Channel channel6("forkN", "opIcmp", "out1", "in1");
//     // Channel channel7("forkN", "branchN", "out2", "in");
//     // Channel channel8("forkI", "opIcmp", "ou1", "in2");
//     // Channel channel9("forkI", "branchI", "out2", "in");
//     // Channel channel10("mergeFact", "branchFact", "out", "in");
//     // Channel channel11("opIcmp", "forkCmp", "out", "in");
//     // Channel channel12("forkCmp", "branchN", "out1", "inCondition");
//     // Channel channel13("forkCmp", "branchI", "out2", "inCondition");
//     // Channel channel14("forkCmp", "branchFact", "out3", "inCondition");
//     // Channel channel15("branchN", "mergeN", "outTrue", "in2");
//     // Channel channel16("branchI", "forkI2", "outTrue", "in");
//     // Channel channel17("branchFact", "opMul", "outTrue", "in1");
//     // Channel channel18("branchFact", "opRet", "outFalse", "in");
//     // Channel channel19("constIncr", "opAdd", "out", "in1");
//     // Channel channel20("forkI2", "opAdd", "out1", "in2");
//     // Channel channel21("forkI2", "opMul", "out2", "in2");
//     // Channel channel22("opAdd", "mergeI", "out", "in2");
//     // Channel channel23("opMul", "mergeFact", "out", "in2");


//     // DFGraph graf("Fact5");
//     // graf.addCluster(&clust1);
//     // graf.addCluster(&clust2);
//     // graf.addCluster(&clust3);
//     // graf.addCluster(&clust4);

//     // graf.addEdge(&channel1);
//     // graf.addEdge(&channel2);
//     // graf.addEdge(&channel3);
//     // graf.addEdge(&channel4);
//     // graf.addEdge(&channel5);
//     // graf.addEdge(&channel6);
//     // graf.addEdge(&channel7);
//     // graf.addEdge(&channel8);
//     // graf.addEdge(&channel9);
//     // graf.addEdge(&channel10);
//     // graf.addEdge(&channel11);
//     // graf.addEdge(&channel12);
//     // graf.addEdge(&channel13);
//     // graf.addEdge(&channel14);
//     // graf.addEdge(&channel15);
//     // graf.addEdge(&channel16);
//     // graf.addEdge(&channel17);
//     // graf.addEdge(&channel18);
//     // graf.addEdge(&channel19);
//     // graf.addEdge(&channel20);
//     // graf.addEdge(&channel21);
//     // graf.addEdge(&channel22);
//     // graf.addEdge(&channel23);

//     // graf.printGraph();
    
//     return 0;

// }