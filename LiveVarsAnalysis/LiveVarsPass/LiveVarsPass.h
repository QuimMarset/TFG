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

    using FuncName = StringRef;
    using BBName = StringRef;
    using UseSet = set <StringRef>;
    using DefSet = set <StringRef>;
    using LiveInSet = set <StringRef>;
    using LiveOutSet = set <StringRef>;
    using FuncLiveInVars = map <BBName, LiveInSet>;
    using FuncLiveOutVars = map <BBName, LiveOutSet>;

    vector <FuncLiveInVars> liveInVars;
    vector <FuncLiveOutVars> liveOutVars;
    map <FuncName, int> indexation;

    static char ID;

    LiveVarsPass();
    ~LiveVarsPass();

    string getInputFileName();
    int getIndexCurrentFunction();
    FuncName getCurrentFuncName();
    
    int getIndexFunc(FuncName name);
    
    bool runOnFunction(Function &F) override;

private:

    string inputFileName;
    FuncName funcName; // Current function running the analysis pass
    int index; // Index with the position of the function's maps

    void computeUsesDefs(const BasicBlock &BB, UseSet &uses, DefSet &defs);
    bool iterateBasicBlock(const BasicBlock &BB, const UseSet &uses, const DefSet &defs, 
        FuncLiveInVars &livesIn, FuncLiveOutVars &livesOut);
    void printLiveVarsAnalysis();
    void setInputFileName(StringRef name);
    void setCurrentFunc(const Function &F);

};



#endif // LIVEVARSPASS_H
