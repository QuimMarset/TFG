
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugInfo.h"
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace llvm;

namespace {

    typedef StringRef BlockName;
    typedef set <Value*> UseSet;
    typedef set <Value*> DefSet;
    typedef set <Value*> LiveIn;
    typedef set <Value*> LiveOut;

    static string fileName;
    
    struct Liveness : public FunctionPass {

        static char ID;
        Liveness() : FunctionPass(ID) {}

        void computeUsesDefs(BasicBlock &B, DefSet &defs, UseSet &uses) {
        
            for (BasicBlock::iterator it = B.begin(); it != B.end(); ++it) {
                Value* lval = llvm::cast<Value>(it);            
                if (!isa<PHINode>(*it)) {
                    for (User::op_iterator op = it->op_begin(); op != it->op_end(); ++op) {
                        Value *rval = *op;
                        if((isa<Instruction>(rval) || isa<Argument>(rval)) 
                                and defs.find(rval) == defs.end()) {
                            uses.insert(rval);  
                        }
                    }
                }  
                if (uses.find(lval) == uses.end()) {
                    defs.insert(lval);
                } 
            }
        }

        bool iterateBlock(BasicBlock &B, map <BlockName, LiveIn> &livesIn, map <BlockName, 
                            LiveOut> &livesOut, const DefSet &defs, const UseSet &uses) {
            
            BlockName name = B.getName();
            LiveIn newLivesIn, aux;
            std::set_difference(livesOut[name].begin(), livesOut[name].end(),
                                defs.begin(), defs.end(), 
                                inserter(aux, aux.begin()));
            std::set_union(aux.begin(), aux.end(),
                            uses.begin(), uses.end(), 
                            inserter(newLivesIn, newLivesIn.begin()));

            bool changes = false;
            for (LiveIn::iterator it = newLivesIn.begin(); it != newLivesIn.end(); it++) {
                pair<LiveIn::iterator, bool> res = livesIn[name].insert(*it);
                changes = changes | res.second;
            }

            if (&(*B.begin()) == B.getFirstNonPHI()) {
                for (pred_iterator it = pred_begin(&B); it != pred_end(&B); ++it) {
                    BlockName name_i = (*it)->getName();
                    livesOut[name_i].insert(newLivesIn.begin(), newLivesIn.end());
                }
            }
            else {
                for (BasicBlock::iterator it = B.begin(); &(*it) != B.getFirstNonPHI(); ++it) {
                    PHINode* phi = cast<PHINode> (it);
                    for (unsigned int i = 0; i < phi->getNumIncomingValues(); ++i) {
                        Value* val_i = phi->getIncomingValue(i);
                        BlockName name_i = phi->getIncomingBlock(i)->getName();
                        if (isa<Instruction>(val_i) || isa<Argument>(val_i)) {
                            livesOut[name_i].insert(val_i);
                        }
                        livesOut[name_i].insert(newLivesIn.begin(), newLivesIn.end());
                    }
                }
            }
            return changes;
        }        

        bool runOnFunction(Function &F) override {
            
            map <BlockName, UseSet> uses;
            map <BlockName, DefSet> defs;
            map <BlockName, LiveIn> livesIn;
            map <BlockName, LiveOut> livesOut;

            for (Function::iterator bb = F.begin(); bb != F.end(); ++bb) {
                BlockName name = bb->getName();
                uses.insert(make_pair(name, UseSet()));
                defs.insert(make_pair(name, DefSet()));
                computeUsesDefs(*bb, defs[name], uses[name]);
                livesIn.insert(make_pair(name, LiveIn()));
                livesOut.insert(make_pair(name, LiveOut()));
            }

            bool changes;
            Function::BasicBlockListType &blocks = F.getBasicBlockList();
            do
            {
                changes = false;
                for (Function::BasicBlockListType::reverse_iterator bb = blocks.rbegin(); 
                        bb != blocks.rend(); ++bb) {
                    BlockName name = bb->getName();
                    bool change = iterateBlock(*bb, livesIn, livesOut, defs[name], uses[name]);
                    changes = changes | change; 
                }
            } while (changes);

            if (fileName.empty()) {
                fileName = F.getParent()->getModuleIdentifier();
                fileName = fileName.substr(0, fileName.length()-3);
            }
            string resultFile = fileName + "_" + F.getName().str() + "_LVA.txt";
            ofstream file;
            file.open(resultFile);

            for (Function::iterator bb = F.begin(); bb != F.end(); ++bb) {
                StringRef name = bb ->getName();
                file << "Block " << name.str() << '\n';
                file << "Lives In" << '\n';
                for (LiveIn::iterator it = livesIn[name].begin(); it != livesIn[name].end(); ++it) {
                    file << (*it)->getName().str() << '\n';
                }
                file << "Lives Out" << '\n';
                for (LiveOut::iterator it = livesOut[name].begin(); it != livesOut[name].end(); ++it) {
                    file << (*it)->getName().str() << '\n';
                }
            }

            file.close();

            return false;
        }

    };
}

char Liveness::ID = 0;
static RegisterPass<Liveness> X("liveAnalysis", "Live Variable Analysis Pass",
                            false /* Only looks at CFG */,
                            false /* Analysis Pass */);