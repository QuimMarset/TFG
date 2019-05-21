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


class DFGraphPass : public FunctionPass {

public:

    static char ID;

    DFGraphPass();
    ~DFGraphPass();

    void getAnalysisUsage(AnalysisUsage &AU) const override;
    bool runOnFunction(Function &F) override;

private:

    DFGraph graph;
    map <StringRef, map <const Value*, Block*> > varsMapping;
    map <StringRef, Block*> controlBlocks;
    map <const BasicBlock*, map <const Value*, Merge*> > varsMerges;
    map <const BasicBlock*, Merge*> controlMerges;
    DataLayout DL;
    LiveVarsPass* liveness;

    void clearStructures();

    void processBinaryInst(const Instruction &inst);
    
    void processPhiInst(const Instruction &inst);

    void processAllocaInst(const Instruction &inst);
    
    void processLoadInst(const Instruction &inst);

    void processStoreInst(const Instruction &inst);
    
    void processCastInst(const Instruction &inst);

    void processSelectInst(const Instruction &inst);

    void processReturnInst(const Instruction &inst);

    void processBranchInst(const Instruction &inst);


    void processOperator(const Value* operand, pair <Block*, const Port*> connection,
        const BasicBlock* BB);

    void processPhiConstants(const BasicBlock* BB);

    void processLiveIn(const BasicBlock* BB);

    void processBBEntryControl(const BasicBlock* BB); 
    void connectOrphanBlock(pair <Block*, const Port*> connection, const BasicBlock* BB);
    void processBBExitControl(const BasicBlock* BB);

    Fork* connectBlocks(Block* block, pair<Block*, const Port*> connection);

    void connectMerge(Merge* merge, Branch* branch, 
        const BasicBlock* BB, const BasicBlock* predBB);
    void connectMerges(const Function& F);
    void connectControlMerges();

    ConstantInterf* createConstant(const Value* operand, const BasicBlock* BB);

    void printGraph(Function& F);

};


#endif // DFGRAPHPASS_H