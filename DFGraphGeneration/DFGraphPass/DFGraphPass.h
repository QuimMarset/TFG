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
    // Used to get the width of the different operands of a instruction
    DataLayout DL;
    // Print the final graph
    ofstream file;
    map <StringRef, FunctionGraph> graphs;
    // Number that will become the id of each BB
    unsigned int BBNumber;

    LiveVarsPass* liveness; // Result of the previous Pass
    FunctionGraph* graph;
    // For each BB keep track of which block carries each temporal in the LLVM IR
    map <StringRef, map <const Value*, Block*> > varsMapping;
    // The same but with the control block, that do not have a Value
    map <StringRef, Block*> controlBlocks;
    /* Used to keep a reference of the merge blocks that we will connect
        at the end of each function as it may need connecting one of its
        input to a block that is not yet processed, like in a loop
    */
    map <const BasicBlock*, map <const Value*, Merge*> > varsMerges;
    map <const BasicBlock*, Merge*> controlMerges;
    /* Reference of the block that will be used to synchronize the control of each called
        function in each BB */
    DFGraphComp::Operator* controlSynch;

    void processFunction(Function& F);

    void clearStructures();

    void processBinaryInst(const Instruction &inst);

    void processCmpInst(const Instruction &inst);
    
    void processPhiInst(const Instruction &inst);
    
    // Not available in LLVM version <= 7, commented in the source code
    void processFNegInst(const Instruction &inst);

    void processAllocaInst(const Instruction &inst);
    
    void processLoadInst(const Instruction &inst);

    void processStoreInst(const Instruction &inst);
    
    void processCastInst(const Instruction &inst);

    void processSelectInst(const Instruction &inst);

    void processReturnInst(const Instruction &inst);

    // Used to create branches for the live variables at the end of a BB
    void processBranchInst(const Instruction &inst);

    void processCallInst(const Instruction& inst);

    void processOperator(const Value* operand, Block* connecBlock,
        int connecPort, const BasicBlock* BB);

    // Add merges to represent live variables at th beginning of a BB
    void processLiveIn(const BasicBlock* BB);

    /* We have to create the constants that can appear in some phi, but they will
        appear as Values in the BB of the phi, not in the BB that they should be placed.
        Therefore, we keep a reference when we compute the live variable analysis,
        and then when we process the BB, we create all the constants. We have to create them 
        at the moment the BB is processed, otherwise we will lose reference of the control block
        that should connect with that constant and trigger it */
    void processPhiConstants(const BasicBlock* BB);

    // Create control modules to trigger constants
    void processBBEntryControl(const BasicBlock* BB); 
    void processBBExitControl(const BasicBlock* BB);
    
    void connectOrphanCst(ConstantInterf* connecBlock);

    void connectBlocks(Block* block, Block* connecBlock,
        int connecPort, const Value* value = nullptr);

    void connectMerge(Merge* merge, Block* block,
        const BasicBlock* predBB, const Value* value = nullptr);
    void connectMerges();
    void connectControlMerges();

    /* Method to change all the connections made to dummy blocks to the real blocks
        of the called function */
    void connectFunctionCall(Function& F);

    ConstantInterf* createConstant(const Value* operand, const BasicBlock* BB);

    void printGraph(Module& M);

};


#endif // DFGRAPHPASS_H