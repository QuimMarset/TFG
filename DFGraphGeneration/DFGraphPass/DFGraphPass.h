#ifndef DFGRAPHPASS_H
#define DFGRAPHPASS_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "../../DFGraphComponents/Graph.h"
#include "../../LiveVarsAnalysis/LiveVarsPass/LiveVarsPass.h"

using namespace std;
using namespace llvm;
using namespace DFGraphComp;


class DFGraphPass : public ModulePass {

public:

    static char ID;

    DFGraphPass();
    ~DFGraphPass();

    void getAnalysisUsage(AnalysisUsage &AU) const override;
    bool runOnModule(Module &M) override;

private:

    DataLayout DL;
    ofstream file;
    map <StringRef, FunctionGraph> graphs;
    int BBNumber;

    LiveVarsPass* liveness;
    FunctionGraph* graph;
    map <StringRef, map <const Value*, Block*> > varsMapping;
    map <StringRef, Block*> controlBlocks;
    map <const BasicBlock*, map <const Value*, Merge*> > varsMerges;
    map <const BasicBlock*, Merge*> controlMerges;
    DFGraphComp::Operator* controlSynch;

    void processFunction(Function& F);

    void clearStructures();

    void processUnaryInst(const Instruction &inst);

    void processBinaryInst(const Instruction &inst);
    
    void processPhiInst(const Instruction &inst);

    void processAllocaInst(const Instruction &inst);
    
    void processLoadInst(const Instruction &inst);

    void processStoreInst(const Instruction &inst);
    
    void processCastInst(const Instruction &inst);

    void processSelectInst(const Instruction &inst);

    void processReturnInst(const Instruction &inst);

    void processBranchInst(const Instruction &inst);

    void processCallInst(const Instruction& inst);

    void processOperator(const Value* operand, Block* connecBlock,
        int connecPort, const BasicBlock* BB);

    void processLiveIn(const BasicBlock* BB);
    void processPhiConstants(const BasicBlock* BB);

    void processBBEntryControl(const BasicBlock* BB); 
    void connectOrphanBlock(Block* connecBlock, int connecPort);
    void processBBExitControl(const BasicBlock* BB);

    void connectBlocks(Block* block, Block* connecBlock,
        int connecPort, const Value* value = nullptr);


    void connectMerge(Merge* merge, Block* block,
        const BasicBlock* predBB, const Value* value = nullptr);
    void connectMerges();
    void connectControlMerges();

    void connectFunctionCall(Function& F);

    ConstantInterf* createConstant(const Value* operand, const BasicBlock* BB);

    void printGraph(Module& M);

};


#endif // DFGRAPHPASS_H