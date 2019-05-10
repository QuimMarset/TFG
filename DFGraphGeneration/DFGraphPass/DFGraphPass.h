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

    void processBinaryInst(const Instruction &inst, 
        map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl);
    
    void processPhiInst(const Instruction &inst,
        map <StringRef, map <StringRef, Block*> >& vars, DFGraph& graph, 
        const DataLayout &dl);

    void processAllocaInst(const Instruction &inst,
        map <StringRef, Block*>& vars, DFGraph& graph, const DataLayout &dl);
    
    void processLoadInst(const Instruction &inst,
        map <StringRef, Block*>& vars, DFGraph& graph, const DataLayout &dl);

    void processStoreInst(const Instruction &inst,
        map <StringRef, Block*>& vars, DFGraph& graph, const DataLayout &dl);
    


    void processOperator(const Value* operand, pair <Block*, const Port*> connection,
        map <StringRef, Block*>& bbVars, DFGraph& graph);

    void processLiveVars(const LiveVarsPass& liveness, const BasicBlock& bb);
    void createBlockControl();
    void printGraph();

};


#endif // DFGRAPHPASS_H