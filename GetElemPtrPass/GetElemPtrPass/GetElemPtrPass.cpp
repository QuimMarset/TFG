
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
        DataLayout DL(F.getParent());
        for (Function::iterator it = F.begin(); it != F.end(); ++it) {
            BasicBlock::iterator it2 = it->begin();
            while (it2 != it->end()) {
                if (isa<GetElementPtrInst>(it2)) {
                    GetElementPtrInst* inst = cast<GetElementPtrInst>(it2);
                    IRBuilder<> builder(inst); 
                    Value* resultPtr;
                    unsigned int numCsts = 0;
                    if (inst->hasAllZeroIndices()) { // Simplu change types
                        resultPtr = builder.CreateBitCast(inst->getOperand(0), inst->getType());
                    }
                    else {
                        Type* intType = DL.getIntPtrType(inst->getType()); // Integer type with same size as the pointer
                        unsigned int typeSize = DL.getTypeSizeInBits(intType);
                        resultPtr = builder.CreatePtrToInt(inst->getOperand(0), intType); // Pointer to integer
                        APInt intOffset = APInt(typeSize, 0); // store the constant offset of all the constant indices
                        if (inst->accumulateConstantOffset(DL, intOffset)) {
                            resultPtr = builder.CreateAdd(resultPtr, 
                                ConstantInt::get(intType, intOffset));
                        }
                        else {
                            Value* cstOffset = ConstantInt::get(intType, APInt(typeSize, 0));
                            bool isConstant = false;
                            Value* index;
                            for (gep_type_iterator gti = gep_type_begin(*inst); 
                                gti != gep_type_end(*inst); ++gti) 
                            {
                                index = gti.getOperand();
                                if (ConstantInt* constant = dyn_cast<ConstantInt>(index)) {
                                    if (constant->isZero()) continue;
                                    else isConstant = true;
                                }
                                if (gti.isSequential()) { // array type
                                    APInt elemSize = APInt(intType->getIntegerBitWidth(), 
                                        DL.getTypeAllocSize(gti.getIndexedType())); // size of each element in the array
                                    if (elemSize != 1) { // multiply index by size
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
                                    const StructLayout* sl = DL.getStructLayout(gti.getStructType());
                                    index = ConstantInt::get(intType, sl->getElementOffset(idxValue));
                                }
                                if (isConstant) { // an instruction is not created, but the Value performs the addition
                                    cstOffset = builder.CreateAdd(cstOffset, index);
                                    isConstant = false;
                                    numCsts++;
                                }
                                else {
                                    resultPtr = builder.CreateAdd(resultPtr, index); // instruction is created
                                }
                            }
                            if (numCsts > 0) {
                                resultPtr = builder.CreateAdd(resultPtr, cstOffset); // create the addition instruction of the constant offset
                            }
                        }
                        resultPtr = builder.CreateIntToPtr(resultPtr, inst->getType()); // return to pointer of the indexed type
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