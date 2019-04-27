
#include "DFGraphPass.h"

char DFGraphPass::ID = 0;

DFGraphPass::DFGraphPass() : FunctionPass(ID) {}

DFGraphPass::~DFGraphPass() {}

void DFGraphPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LiveVarsPass>();
    AU.setPreservesAll();
}

bool DFGraphPass::runOnFunction(Function &F) {
    LiveVarsPass &liveness = getAnalysis<LiveVarsPass>();
    errs() << liveness.getInputFileName() << '\n';
    return false;
}


static RegisterPass<DFGraphPass> registerDFGraphPass("dfGraphPass", 
    "Create Data Flow Graph from LLVM IR function Pass",
    false /* Only looks at CFG */,
    false /* Analysis Pass */);