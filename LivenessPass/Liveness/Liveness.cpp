
#include "Liveness.h"


char LiveVarsAnalysis::ID = 0;

// Public Functions

LiveVarsAnalysis::LiveVarsAnalysis() : FunctionPass(ID) {}

LiveVarsAnalysis::~LiveVarsAnalysis() {}

string LiveVarsAnalysis::getInputFileName() {
    return inputFileName;
}

LiveVarsAnalysis::FuncName LiveVarsAnalysis::getCurrentFuncName() {
    return funcName;
}

int LiveVarsAnalysis::getIndexCurrentFunction() {
    return index;
}

int LiveVarsAnalysis::getIndexFunc(FuncName name) {
    if (indexation.find(name) != indexation.end()) {
        return indexation[name];
    }
    return -1; // Function not found
}

bool LiveVarsAnalysis::runOnFunction(Function &F) {
    if (inputFileName.empty()) {
        setInputFileName(F.getParent()->getModuleIdentifier());
    }
    setCurrentFunc(F);

    map <BBName, UseSet> uses;
    map <BBName, DefSet> defs;
    for (Function::const_iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        BBName name = bb_it->getName();
        uses.insert(make_pair(name, UseSet()));
        defs.insert(make_pair(name, DefSet()));
        computeUsesDefs(*bb_it, uses[name], defs[name]);
    }

    bool changes;
    Function::BasicBlockListType &blocks = F.getBasicBlockList();
    do {
        changes = false;
        for (Function::BasicBlockListType::const_reverse_iterator bb_it = blocks.rbegin(); 
                bb_it != blocks.rend(); ++bb_it) {
            BBName name = bb_it->getName();
            bool change = iterateBasicBlock(*bb_it, uses[name], defs[name],  
                liveInVars[index], liveOutVars[index]);
            changes = changes | change; 
        }
    } 
    while (changes);

    printLiveVarsAnalysis();

    return false;
}

// Private Functions

void LiveVarsAnalysis::computeUsesDefs(const BasicBlock &BB, UseSet &uses, 
        DefSet &defs) {
    for (BasicBlock::const_iterator inst_it = BB.begin(); inst_it != BB.end(); ++inst_it) {    
        if (!isa<PHINode>(*inst_it)) {
            for (User::const_op_iterator op_it = inst_it->op_begin(); op_it != inst_it->op_end(); ++op_it) {
                if((isa<Instruction>(op_it->get()) || isa<Argument>(op_it->get())) 
                        and defs.find(op_it->get()->getName()) == defs.end()) {
                    uses.insert(op_it->get()->getName());  
                }
            }
        }  
        if (!inst_it->getType()->isVoidTy() and uses.find(inst_it->getName()) == uses.end()) {
            defs.insert(inst_it->getName());
        }
    }    
}

bool LiveVarsAnalysis::iterateBasicBlock(const BasicBlock &BB, const UseSet &uses, 
        const DefSet &defs, FuncLiveInVars &livesIn, FuncLiveOutVars &livesOut) {
    BBName name = BB.getName();
    LiveInSet newLivesIn, aux;
    set_difference(livesOut[name].begin(), livesOut[name].end(),
                        defs.begin(), defs.end(), 
                        inserter(aux, aux.begin()));
    set_union(aux.begin(), aux.end(),
                    uses.begin(), uses.end(), 
                    inserter(newLivesIn, newLivesIn.begin()));

    bool changes = false;
    for (LiveInSet::const_iterator it = newLivesIn.begin(); it != newLivesIn.end(); it++) {
        pair<LiveInSet::iterator, bool> res = livesIn[name].insert(*it);
        changes = changes | res.second;
    }

    if (&(*BB.begin()) == BB.getFirstNonPHI()) {
        for (const_pred_iterator it = pred_begin(&BB); it != pred_end(&BB); ++it) {
            BBName name_i = (*it)->getName();
            livesOut[name_i].insert(newLivesIn.begin(), newLivesIn.end());
        }
    }
    else {
        for (BasicBlock::const_iterator it = BB.begin(); &(*it) != BB.getFirstNonPHI(); ++it) {
            const PHINode* phi = cast<PHINode> (it);
            for (unsigned int i = 0; i < phi->getNumIncomingValues(); ++i) {
                Value* val_i = phi->getIncomingValue(i);
                BBName name_i = phi->getIncomingBlock(i)->getName();
                if (isa<Instruction>(val_i) || isa<Argument>(val_i)) {
                    livesOut[name_i].insert(val_i->getName());
                }
                livesOut[name_i].insert(newLivesIn.begin(), newLivesIn.end());
            }
        }
    }
    return changes;
}

void LiveVarsAnalysis::printLiveVarsAnalysis() {
    string resultFile = inputFileName + "_" + funcName.str() + "_LVA.txt";
    ofstream file;
    file.open(resultFile);
    for (FuncLiveInVars::const_iterator it = liveInVars[index].begin(); 
            it != liveInVars[index].end(); ++it) {
        assert(liveOutVars[index].find(it->first) != liveOutVars[index].end());
        file << "Block " << it->first.str() << '\n';
        file << "Live In\n";
        for (LiveInSet::const_iterator it2 = liveInVars[index][it->first].begin(); 
                it2 != liveInVars[index][it->first].end(); ++it2) {
            file << it2->str() << '\n';
        }
        file << "Live Out\n";
        for (LiveOutSet::const_iterator it2 = liveOutVars[index][it->first].begin(); 
                it2 != liveOutVars[index][it->first].end(); ++it2) {
            file << it2->str() << '\n';
        }
    }
    file.close();
}

void LiveVarsAnalysis::setInputFileName(StringRef name) {
    // Assuming .ll extension
    string aux = name.str();
    assert(aux.substr(aux.length()-3, 3) == ".ll");
    inputFileName = aux.substr(0, aux.length()-3);
}

void LiveVarsAnalysis::setCurrentFunc(const Function &F) {
    funcName = F.getName();
    index = liveInVars.size();
    indexation.insert(make_pair(funcName, index));
    liveInVars.push_back(FuncLiveInVars());
    liveOutVars.push_back(FuncLiveOutVars());
    for (Function::const_iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        liveInVars[index].insert(make_pair(bb_it->getName(), LiveInSet()));
        liveOutVars[index].insert(make_pair(bb_it->getName(), LiveOutSet()));
    }
}


static RegisterPass<LiveVarsAnalysis> registerLiveVarsAnalysis("liveAnalysis", 
    "Live Variable Analysis Pass",
    false /* Only looks at CFG */,
    false /* Analysis Pass */);
