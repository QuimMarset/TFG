
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/PassRegistry.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {

struct GetElemPtrPass : public FunctionPass {
    static char ID;
    GetElemPtrPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        DataLayout dl(F.getParent());
        for (Function::iterator it = F.begin(); it != F.end(); ++it) {
            BasicBlock::iterator it2 = it->begin();
            while (it2 != it->end()) {
                if (isa<GetElementPtrInst>(it2)) {
                    GetElementPtrInst* inst = cast<GetElementPtrInst>(it2);
                    IRBuilder<> builder(inst); 
                    Value* resultPtr;
                    if (inst->hasAllZeroIndices()) {
                        resultPtr = builder.CreateBitCast(inst->getOperand(0), inst->getType());
                    }
                    else {
                        Type* intType = dl.getIntPtrType(inst->getType());
                        resultPtr = builder.CreatePtrToInt(inst->getOperand(0), intType);
                        Value* index;
                        for (gep_type_iterator gti = gep_type_begin(*inst); 
                            gti != gep_type_end(*inst); ++gti) 
                        {
                            index = gti.getOperand();
                            if (ConstantInt* constant = dyn_cast<ConstantInt>(index)) {
                                if (constant->isZero()) continue;
                            }
                            if (gti.isSequential()) {
                                APInt elemSize = APInt(intType->getIntegerBitWidth(), 
                                    dl.getTypeAllocSize(gti.getIndexedType()));
                                if (elemSize != 1) {
                                    if (elemSize.isPowerOf2()) {
                                        index = builder.CreateShl(index, 
                                            ConstantInt::get(intType, elemSize.logBase2())); 
                                    } 
                                    else {
                                        index = builder.CreateMul(index, 
                                            ConstantInt::get(intType, elemSize));
                                    }
                                }
                            }
                            else if (gti.isStruct()) {
                                unsigned int idxValue = dyn_cast<ConstantInt>(index)->getZExtValue();
                                const StructLayout* sl = dl.getStructLayout(gti.getStructType());
                                index = ConstantInt::get(intType, sl->getElementOffset(idxValue));
                            }
                            resultPtr = builder.CreateAdd(resultPtr, index);
                        }
                        resultPtr = builder.CreateIntToPtr(resultPtr, inst->getType());
                    }
                    ++it2;
                    inst->replaceAllUsesWith(resultPtr);
                    inst->eraseFromParent();
                }
                else ++it2;
            }
        }
        return true;
    }
};


} 

char GetElemPtrPass::ID = 0;
static RegisterPass<GetElemPtrPass> registerGetElemPtrPass("gepPass", 
    "Lower the GetElemPtr instruction Pass", true, true);