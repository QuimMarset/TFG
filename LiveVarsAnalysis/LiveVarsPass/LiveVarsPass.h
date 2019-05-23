#ifndef LIVEVARSPASS_H
#define LIVEVARSPASS_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace llvm;

    
class LiveVarsPass : public FunctionPass {

public:

    map <StringRef, set <const Value*> > liveInVars;
    map <StringRef, set <const Value*> > liveOutVars;
    map <StringRef, set <const Value*> > phiConstants;

    static char ID;

    LiveVarsPass();
    ~LiveVarsPass();

    string getInputFileName();
    StringRef getFuncName();
    
    bool runOnFunction(Function &F) override;

private:

    string inputFileName;
    StringRef funcName; // Current function running the analysis pass

    void computeUsesDefs(const BasicBlock &BB, set<const Value*> &uses, 
        set<const Value*> &defs);

    void computePhiVars(const BasicBlock& BB);

    bool iterateBasicBlock(const BasicBlock &BB, const set<const Value*> &uses, 
        const set<const Value*> &defs, map<StringRef, set<const Value*> > &livesIn, 
        map<StringRef, set<const Value*> > &livesOut);

    void printLiveVarsAnalysis(Function& F);

    void setInputFileName(StringRef name);

    void setCurrentFunc(Function& F);

};


#endif // LIVEVARSPASS_H
