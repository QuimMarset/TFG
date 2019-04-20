
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm-7/llvm/Analysis/LoopInfo.h"
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace llvm;

namespace {

    
    struct DOTGraph : public FunctionPass {

        static char ID;
        DOTGraph() : FunctionPass(ID) {}

        void getAnalysisUsage(AnalysisUsage &AU) {
            AU.addRequired<LoopInfoWrapperPass>();
            AU.setPreservesAll();
        }        

        bool runOnFunction(Function &F) override {
            LoopInfo &liveness = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
            // errs() << livenes.fileName << '\n';

            return false;
        }

    };
}

char DOTGraph::ID = 0;
static RegisterPass<DOTGraph> X("dotGraph", "Create Data Flow Graph from LLVM IR function Pass",
                            false /* Only looks at CFG */,
                            false /* Analysis Pass */);