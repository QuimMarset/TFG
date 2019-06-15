
#include "LiveVarsPass.h"


char LiveVarsPass::ID = 0;

// Public Functions

LiveVarsPass::LiveVarsPass() : FunctionPass(ID) {}


LiveVarsPass::~LiveVarsPass() {}


string LiveVarsPass::getInputFileName() {
    return inputFileName;
}


StringRef LiveVarsPass::getFuncName() {
    return funcName;
}


bool LiveVarsPass::runOnFunction(Function &F) {
    if (inputFileName.empty()) {
        setInputFileName(F.getParent()->getModuleIdentifier());
    }
    else { // Reset maps for other functions in the LLVM files
        liveInVars.clear(); 
        liveOutVars.clear();
        phiConstants.clear();
    }
    setCurrentFunc(F);
    map <StringRef, set<const Value*> > uses;
    map <StringRef, set<const Value*> > defs;
    StringRef BBName;
    for (Function::const_iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        BBName = bb_it->getName();
        uses.insert(make_pair(BBName, set<const Value*>()));
        defs.insert(make_pair(BBName, set<const Value*>()));
        computeUsesDefs(*bb_it, uses[BBName], defs[BBName]);
        if (&(*(bb_it->begin())) != bb_it->getFirstNonPHI()) {
            computePhiVars(*bb_it);
        }
    }
    bool changes;
    Function::BasicBlockListType &blocks = F.getBasicBlockList();
    do {
        changes = false;
        for (Function::BasicBlockListType::const_reverse_iterator bb_it = blocks.rbegin(); 
                bb_it != blocks.rend(); ++bb_it) {
            BBName = bb_it->getName();
            bool change = iterateBasicBlock(*bb_it, uses[BBName], defs[BBName],  
                liveInVars, liveOutVars);
            changes = changes | change; 
        }
    } 
    while (changes);
    printLiveVarsAnalysis(F);
    return false;
}


// Private Functions

void LiveVarsPass::computeUsesDefs(const BasicBlock& BB, set<const Value*>& uses, 
        set<const Value*>& defs) 
{
    for (BasicBlock::const_iterator inst_it = BB.begin(); inst_it != BB.end(); ++inst_it) {    
        if (!isa<PHINode>(*inst_it)) {
            for (User::const_op_iterator op_it = inst_it->op_begin(); op_it != inst_it->op_end(); 
                ++op_it) 
            {
                if ((isa<Instruction>(op_it->get()) || isa<Argument>(op_it->get())) 
                    and defs.find(op_it->get()) == defs.end()) 
                {
                    uses.insert(op_it->get());  
                }
            }
        }  
        if (!inst_it->getType()->isVoidTy() and uses.find(&(*inst_it)) == uses.end()) {
            defs.insert(&(*inst_it));
        }
    }    
}


void LiveVarsPass::computePhiVars(const BasicBlock& BB) {
    StringRef predBBName;
    for (BasicBlock::const_iterator it = BB.begin(); &(*it) != BB.getFirstNonPHI(); ++it) {
        const PHINode* phi = cast<PHINode> (it);
        const Value* value;
        for (unsigned int i = 0; i < phi->getNumIncomingValues(); ++i) {
            value = phi->getIncomingValue(i);
            predBBName = phi->getIncomingBlock(i)->getName();
            if (isa<Instruction>(value) || isa<Argument>(value)) {
                liveOutVars[predBBName].insert(value);
            }
            else if (isa<Constant>(value)) {
                phiConstants[predBBName].insert(make_pair(phi, i));
            }
        }
    }
}


bool LiveVarsPass::iterateBasicBlock(const BasicBlock &BB, const set<const Value*> &uses, 
    const set<const Value*> &defs, map<StringRef, set<const Value*> > &livesIn, 
    map<StringRef, set<const Value*> > &livesOut) 
{
    StringRef BBName = BB.getName();
    set<const Value*> newLivesIn, aux;
    set_difference(livesOut[BBName].begin(), livesOut[BBName].end(),
        defs.begin(), defs.end(), 
        inserter(aux, aux.begin()));
    set_union(aux.begin(), aux.end(),
        uses.begin(), uses.end(), 
        inserter(newLivesIn, newLivesIn.begin()));
    bool changes = false;
    for (set<const Value*>::const_iterator it = newLivesIn.begin(); 
        it != newLivesIn.end(); it++) 
    {
        changes = changes | livesIn[BBName].insert(*it).second;
    }
    StringRef predBBName;
    for (const_pred_iterator it = pred_begin(&BB); it != pred_end(&BB); ++it) {
        predBBName = (*it)->getName();
        livesOut[predBBName].insert(newLivesIn.begin(), newLivesIn.end());
    }
    return changes;
}


void LiveVarsPass::printLiveVarsAnalysis(Function& F) {
    ofstream file;
    file.open(inputFileName + "_" + F.getName().str() + "_LVA.txt");
    StringRef BBName;
    for (Function::const_iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        BBName = bb_it->getName();
        file << "Block " << BBName.str() << '\n';
        file << "Live In\n";
        for (set<const Value*>::const_iterator var_it = liveInVars[BBName].begin(); 
            var_it != liveInVars[BBName].end(); ++var_it) 
        {
            file << (*var_it)->getName().str() << '\n';
        }
        file << "Live Out\n";
        for (set<const Value*>::const_iterator var_it = liveOutVars[BBName].begin(); 
            var_it != liveOutVars[BBName].end(); ++var_it) 
        {
            file << (*var_it)->getName().str() << '\n';
        }
    }
    file.close();
}


void LiveVarsPass::setInputFileName(StringRef name) {
    // Assuming .ll extension
    string aux = name.str();
    assert(aux.substr(aux.length()-3, 3) == ".ll");
    inputFileName = aux.substr(0, aux.length()-3);
}


void LiveVarsPass::setCurrentFunc(Function& F) {
    funcName = F.getName();
    for (Function::const_iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        liveInVars.insert(make_pair(bb_it->getName(), set <const Value*>()));
        liveOutVars.insert(make_pair(bb_it->getName(), set <const Value*>()));
        phiConstants.insert(make_pair(bb_it->getName(), 
            set <pair <const PHINode*, unsigned int> >()));
    }
}


static RegisterPass<LiveVarsPass> registerLiveVarsPass("liveVarsPass", 
    "Live Variable Analysis Pass",
    false /* Only looks at CFG */,
    false /* Analysis Pass */);
