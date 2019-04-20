
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm-7/llvm/Analysis/LoopInfo.h"
#include "../../LivenessPass/Liveness/Liveness.h"
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

        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<Liveness>();
            AU.setPreservesAll();
        }        

        bool runOnFunction(Function &F) override {
            Liveness &liveness = getAnalysis<Liveness>();
            errs() << liveness.fileName << '\n';
            
            return false;
        }

    };
}

char DOTGraph::ID = 0;
static RegisterPass<DOTGraph> Y("dotGraph", "Create Data Flow Graph from LLVM IR function Pass",
                            false /* Only looks at CFG */,
                            false /* Analysis Pass */);