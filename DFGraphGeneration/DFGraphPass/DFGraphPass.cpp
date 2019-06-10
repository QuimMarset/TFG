
#include "DFGraphPass.h"

char DFGraphPass::ID = 0;


DFGraphPass::DFGraphPass() : ModulePass(ID), DL("") {
    BBNumber = 0;
}


DFGraphPass::~DFGraphPass() {}


void DFGraphPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LiveVarsPass>();
    AU.setPreservesAll();
}


bool DFGraphPass::runOnModule(Module& M) {
    file.open("test.dot");
    DL = DataLayout(&M);
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        errs() << it->getName() << '\n';
        processFunction(*it);
        clearStructures();
    }
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        Function& F = *it;
        connectFunctionCall(F);
    }
    printGraph(M);
    file.close();
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        Function& F = *it;
        FunctionGraph& funcGraph = graphs[F.getName()];
        funcGraph.freeGraph();
    }
    return false;
}


void DFGraphPass::processFunction(Function &F) {
    BBNumber = 0;
    controlSynch = nullptr;
    liveness = &getAnalysis<LiveVarsPass>(F);
    StringRef funcName = F.getName();
    graphs[funcName] = FunctionGraph(funcName.str());
    graph = &graphs[funcName];
    bool firstBB = true;
    for (const BasicBlock& BB : F.getBasicBlockList()) {
        StringRef BBName = BB.getName();
        varsMapping.insert(make_pair(BBName, map <const Value*, Block*>()));
        varsMerges.insert(make_pair(&BB, map <const Value*, Merge*>()));
        graph->addBasicBlock(BBName, BBNumber);
        ++BBNumber;
        processBBEntryControl(&BB);
        if (firstBB) { // Function arguments
            firstBB = false;
            for (Function::const_arg_iterator arg_it = F.arg_begin(); arg_it != F.arg_end(); 
                ++arg_it) 
            {
                unsigned int argTypeSize = DL.getTypeSizeInBits(arg_it->getType());
                DFGraphComp::Argument* argBlock = new DFGraphComp::Argument(&BB, argTypeSize);
                graph->addBlockToBB(argBlock);
                varsMapping[BBName][arg_it] = argBlock;
                graph->addArgument(argBlock);
            }
        }
        else {
            processLiveIn(&BB);
        }
        processPhiConstants(&BB);
        for (BasicBlock::const_iterator inst_it = BB.begin(); inst_it != BB.end(); 
            ++inst_it) 
        {
            inst_it->print(errs()); errs() << '\n';
            if (isa <llvm::BinaryOperator>(inst_it) || isa<CmpInst>(inst_it)) 
            {
                processBinaryInst(*inst_it);
            }
            else if (isa<AllocaInst>(inst_it)) {
                processAllocaInst(*inst_it);
            }
            else if (isa<LoadInst>(inst_it)) {
                processLoadInst(*inst_it);
            }
            else if (isa<StoreInst>(inst_it)) {
                processStoreInst(*inst_it);
            }
            else if (isa<PHINode>(inst_it)) {
                processPhiInst(*inst_it);
            }
            else if (isa<CastInst>(inst_it)) {
                processCastInst(*inst_it);
            }
            else if (isa<GetElementPtrInst>(inst_it)) {
                assert(0 && "Not should appear (lowered with arithmetic op");
            }
            else if (isa<SelectInst>(inst_it)) {
                processSelectInst(*inst_it);
            }
            else if (isa<ReturnInst>(inst_it)) {
                processReturnInst(*inst_it);
            }
            else if (isa<BranchInst>(inst_it)) {
                processBranchInst(*inst_it);
            }
            else if (isa<CallInst>(inst_it)) {
                processCallInst(*inst_it);
            }
            else if (isa<ExtractElementInst>(inst_it)) {
                assert(0 && "Not supported (currently)");
            }
            else if (isa<ExtractValueInst>(inst_it)) {
                assert(0 && "Not supported (currently)");
            }
            else if (isa<InsertElementInst>(inst_it)) {
                assert(0 && "Not supported (currently)");
            }
            else if (isa<InsertValueInst>(inst_it)) {
                assert(0 && "Not supported (currently)");
            }
            else {
                assert(0 && "Not supported");
            }
        }
        processBBExitControl(&BB);
    }//TODO: Globals vars (allocate memory)
    connectMerges();
    connectControlMerges();
}



void DFGraphPass::processBinaryInst(const Instruction &inst) 
{
    unsigned int typeSize = DL.getTypeSizeInBits(inst.getType());
    OpType opType;
    unsigned int opCode = inst.getOpcode();
    if (opCode == Instruction::Add) {
        opType = OpType::Add;
    }
    else if (opCode == Instruction::FAdd) {
        opCode = OpType::FAdd;
    }
    else if (opCode == Instruction::Sub) {
        opType = OpType::Sub;
    }
    else if (opCode == Instruction::FSub) {
        opCode = OpType::FSub;
    }
    else if (opCode == Instruction::Mul) {
        opType = OpType::Mul;
    }
    else if (opCode == Instruction::FMul) {
        opCode = OpType::FMul;
    }
    else if (opCode == Instruction::UDiv || opCode == Instruction::SDiv) {
        opType = OpType::Div;
    }
    else if (opCode == Instruction::FDiv) {
        opCode = OpType::FDiv;
    }
    else if (opCode == Instruction::URem || opCode == Instruction::SRem) {
        opType = OpType::Rem;
    }
    else if (opCode == Instruction::FRem) {
        opType = OpType::FRem;
    }
    else if (opCode == Instruction::And) {
        opType = OpType::And;
    }
    else if (opCode == Instruction::Or) {
        opType = OpType::Or;
    }
    else if (opCode == Instruction::Xor) {
        opType = OpType::Xor;
    }
    else if (opCode == Instruction::Shl) {
        opType = OpType::ShiftL;
    }
    else if (opCode == Instruction::LShr or opCode == Instruction::AShr) {
        opType = OpType::ShiftR;
    }
    else if (opCode == Instruction::ICmp) {
        const ICmpInst* cmp = cast<ICmpInst> (&inst);
        ICmpInst::Predicate pred = cmp->getPredicate();
        if (pred == ICmpInst::ICMP_EQ) {
            opType = OpType::Eq;
        }
        else if (pred == ICmpInst::ICMP_NE) {
            opType = OpType::NE;
        }
        else if (pred == ICmpInst::ICMP_SGT || pred == ICmpInst::ICMP_UGT) {
            opType = OpType::GT;
        }
        else if (pred == ICmpInst::ICMP_SGE || pred == ICmpInst::ICMP_UGE) {
            opType = OpType::GE;
        }
        else if (pred == ICmpInst::ICMP_SLT || pred == ICmpInst::ICMP_ULT) {
            opType = OpType::LT;
        }
        else if (pred == ICmpInst::ICMP_SLE || pred == ICmpInst::ICMP_ULE) {
            opType = OpType::LE;
        }
    }
    else if (opCode == Instruction::FCmp) {
        const FCmpInst* cmp = cast<FCmpInst> (&inst);
        FCmpInst::Predicate predicate = cmp->getPredicate();
        if (predicate == FCmpInst::FCMP_OEQ || predicate == FCmpInst::FCMP_UEQ) {
            opType = OpType::FEq;
        }
        else if (predicate == FCmpInst::FCMP_ONE || predicate == FCmpInst::FCMP_UNE) {
            opType = OpType::FNE;
        }
        else if (predicate == FCmpInst::FCMP_OGT || predicate == FCmpInst::FCMP_UGT) {
            opType = OpType::FGT;
        }
        else if (predicate == FCmpInst::FCMP_OGE || predicate == FCmpInst::FCMP_UGE) {
            opType = OpType::FGE;
        }
        else if (predicate == FCmpInst::FCMP_OLT || predicate == FCmpInst::FCMP_ULT) {
            opType = OpType::FLT;
        }
        else if (predicate == FCmpInst::FCMP_OLE || predicate == FCmpInst::FCMP_ULE) {
            opType = OpType::FLE;
        }
        else if (predicate == FCmpInst::FCMP_TRUE) {
            opType = OpType::True;
        }
        else if (predicate == FCmpInst::FCMP_FALSE) {
            opType = OpType::False;
        }
    } 
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    DFGraphComp::Operator* op = new DFGraphComp::Operator(opType, BB, typeSize);
    processOperator(inst.getOperand(0), make_pair(op, op->getDataInPort(0)), BB);
    processOperator(inst.getOperand(1), make_pair(op, op->getDataInPort(1)), BB);
    graph->addBlockToBB(op);
    varsMapping[BBName][&inst] = op;
}



void DFGraphPass::processPhiInst(const Instruction &inst)
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const PHINode* phi = cast<PHINode>(&inst);
    unsigned int typeSize = DL.getTypeSizeInBits(phi->getType());
    Merge* merge = new Merge(BB, typeSize);
    varsMerges[BB][&inst] = merge;
    varsMapping[BBName][&inst] = merge;
    graph->addBlockToBB(merge);
}



void DFGraphPass::processAllocaInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const AllocaInst* allocaInst = cast<AllocaInst>(&inst);
    unsigned int allocBytes = DL.getTypeAllocSize(allocaInst->getAllocatedType());
    if (allocaInst->isArrayAllocation()) {
        const ConstantInt* cstSize = cast<ConstantInt>(allocaInst->getArraySize());
        unsigned int nElems = cstSize->getZExtValue();
        allocBytes *= nElems;
    }
    unsigned int ptrSize = DL.getTypeAllocSizeInBits(allocaInst->getType());
    DFGraphComp::Constant<unsigned int>* allocBytesBlock = 
        new DFGraphComp::Constant<unsigned int>(allocBytes, BB);
    connectOrphanBlock(make_pair(allocBytesBlock, allocBytesBlock->getControlInPort()));
    DFGraphComp::Operator* allocaBlock = new DFGraphComp::Operator(OpType::Alloca, BB);
    allocaBlock->setDataInPortWidth(0, sizeof(unsigned int));
    allocaBlock->setDataOutPortWidth(ptrSize);
    allocBytesBlock->setConnectedPort(make_pair(allocaBlock, allocaBlock->getDataInPort(0)));
    graph->addBlockToBB(allocBytesBlock);
    graph->addBlockToBB(allocaBlock);
    varsMapping[BBName][&inst] = allocaBlock;
}



void DFGraphPass::processLoadInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const LoadInst* loadInst = cast<LoadInst>(&inst);
    unsigned int pointerSize = DL.getTypeSizeInBits(loadInst->getPointerOperandType());
    unsigned int valueTypeSize = DL.getTypeSizeInBits(loadInst->getType());
    DFGraphComp::Operator* loadOp = new DFGraphComp::Operator(OpType::Load, BB);
    loadOp->setDataInPortWidth(0, pointerSize);
    loadOp->setDataOutPortWidth(valueTypeSize);
    processOperator(loadInst->getPointerOperand(), make_pair(loadOp, loadOp->getDataInPort(0)),
        inst.getParent());
    varsMapping[BBName][&inst] = loadOp;
    graph->addBlockToBB(loadOp);
}



void DFGraphPass::processStoreInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    const StoreInst* storeInst = cast<StoreInst>(&inst);
    unsigned int storeValueSize = DL.getTypeSizeInBits(storeInst->getValueOperand()->getType());
    unsigned int pointerSize = DL.getTypeSizeInBits(storeInst->getPointerOperandType());
    DFGraphComp::Operator* store = new DFGraphComp::Operator(OpType::Store, BB);
    store->setDataInPortWidth(0, storeValueSize);
    store->setDataInPortWidth(1, pointerSize);
    processOperator(storeInst->getValueOperand(), make_pair(store, store->getDataInPort(0)), BB);
    processOperator(storeInst->getPointerOperand(), make_pair(store, store->getDataInPort(1)), BB);
    graph->addBlockToBB(store);
}



void DFGraphPass::processCastInst(const Instruction &inst) 
{
    const CastInst* castInst = cast<CastInst>(&inst);
    unsigned int operandTypeSize = DL.getTypeSizeInBits(castInst->getSrcTy());
    unsigned int castTypeSize = DL.getTypeSizeInBits(castInst->getDestTy());
    Value* operand = castInst->getOperand(0);
    OpType castOpType;
    unsigned int opCode = castInst->getOpcode();
    if (opCode == Instruction::Trunc) {
        castOpType = OpType::IntTrunc;
    }
    else if (opCode == Instruction::ZExt) {
        castOpType = OpType::IntZExt;
    }
    else if (opCode == Instruction::SExt) {
        castOpType = OpType::IntSExt;
    }
    else if (opCode == Instruction::FPToUI) {
        castOpType = OpType::FPointToUInt;
    }
    else if (opCode == Instruction::FPToSI) {
        castOpType = OpType::FPointToSInt;
    }
    else if (opCode == Instruction::UIToFP) {
        castOpType = OpType::UIntToFPoint;
    }
    else if (opCode == Instruction::SIToFP) {
        castOpType = OpType::SIntToFPoint;
    }
    else if (opCode == Instruction::FPTrunc) {
        castOpType = OpType::FPointTrunc;
    }
    else if (opCode == Instruction::FPExt) {
        castOpType = OpType::FPointExt;
    }
    else if (opCode == Instruction::PtrToInt) {
        castOpType = OpType::PtrToInt;
    }
    else if (opCode == Instruction::IntToPtr) {
        castOpType = OpType::IntToPtr;
    }
    else if (opCode == Instruction::BitCast) {
        castOpType = OpType::BitCast;
    }
    else if (opCode == Instruction::AddrSpaceCast) {
        castOpType = OpType::AddrSpaceCast;
    }
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    DFGraphComp::Operator* castOp = new DFGraphComp::Operator(castOpType, BB);
    castOp->setDataInPortWidth(0, operandTypeSize);
    castOp->setDataOutPortWidth(castTypeSize);
    processOperator(operand, make_pair(castOp, castOp->getDataInPort(0)), BB);
    varsMapping[BBName][&inst] = castOp;
    graph->addBlockToBB(castOp);
}



void DFGraphPass::processSelectInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const SelectInst* selectInst = cast<SelectInst>(&inst);
    unsigned int typeSize = DL.getTypeSizeInBits(selectInst->getType());
    Select* selectBlock = new Select(BB, typeSize);
    processOperator(selectInst->getTrueValue(), make_pair(selectBlock, 
        selectBlock->getDataInTruePort()), BB);
    processOperator(selectInst->getFalseValue(), make_pair(selectBlock, 
        selectBlock->getDataInFalsePort()), BB);
    processOperator(selectInst->getCondition(), make_pair(selectBlock, 
        selectBlock->getConditionInPort()), BB);
    graph->addBlockToBB(selectBlock);
    varsMapping[BBName][&inst] = selectBlock;
}



void DFGraphPass::processReturnInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    if (inst.getNumOperands() > 0) {
        Value* operand = inst.getOperand(0);
        unsigned int typeSize = DL.getTypeSizeInBits(operand->getType());
        Return* retBlock = new Return(BB, typeSize);
        processOperator(operand, make_pair(retBlock, retBlock->getInPort()), BB);
        graph->addBlockToBB(retBlock);
        varsMapping[BBName][&inst] = retBlock;
        Block* functionReturn = graph->getFunctionResult();
        if (functionReturn != nullptr) {
            Merge* mergeRet;
            if (functionReturn->getBlockType() == BlockType::Exit_Block) {
                mergeRet = new Merge(nullptr, typeSize);
                functionReturn->setConnectedPort(make_pair(mergeRet, 
                    mergeRet->addDataInPort()));
                graph->addBlockToBB(functionReturn->getParentBB()->getName(), mergeRet);
                graph->setFunctionResult(mergeRet);
            }
            else {
                mergeRet = (Merge*)functionReturn;
            }
            retBlock->setConnectedPort(make_pair(mergeRet, mergeRet->addDataInPort()));
        }
        else {
            graph->setFunctionResult(retBlock);
        }
    }
}



void DFGraphPass::processCallInst(const Instruction& inst) {
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const Value* value;
    Block* blockVar;
    const CallInst& callInst = cast<CallInst>(inst);
    Function* func = callInst.getCalledFunction();
    StringRef funcName = func->getName();
    if (graphs.find(funcName) == graphs.end()) {
        graphs[funcName] = FunctionGraph(funcName.str());
    }
    FunctionGraph& funcGraph = graphs[funcName];
    int timesCalled = funcGraph.getTimesCalled();
    FunctionCall* callBlock = new FunctionCall();
    funcGraph.addFunctionCallBlock(callBlock);
    Block* control = controlBlocks[BBName];
    if (timesCalled == 0) {
        funcGraph.setWrapperControlIn(control);
        for (unsigned int i = 0; i < callInst.getNumArgOperands(); ++i) {
            value = callInst.getArgOperand(i);
            if (isa<llvm::Constant>(value)) {
                blockVar = createConstant(value, BB);
                graph->addBlockToBB(blockVar);
            }
            else {
                blockVar = varsMapping[BBName][value];
                blockVar->setConnectedPort(make_pair(callBlock, nullptr));
            }
            funcGraph.addWrapperCallParam(blockVar);
        }
        funcGraph.setFirstCallInst(&callInst);
    }
    else {
        if (timesCalled == 1) {
            Block* controlIn = funcGraph.getWrapperControlIn();
            Merge* mergeControl = new Merge(nullptr, 0);
            funcGraph.setWrapperControlIn(mergeControl);
            Fork* fork = new Fork(nullptr, 0);
            connectBlocks(controlIn, make_pair(fork, fork->getDataInPort()));
            fork->setConnectedPort(make_pair(mergeControl, mergeControl->addDataInPort()));
            funcGraph.addWrapperControlFork(fork);
            Block* paramBlock;
            Merge* paramMerge;
            const CallInst* prevCall = funcGraph.getFirstCallInst();
            unsigned int typeSize;
            for (unsigned int i = 0; i < prevCall->getNumArgOperands(); ++i) {
                value = prevCall->getArgOperand(i);
                typeSize = DL.getTypeSizeInBits(value->getType());
                paramBlock = funcGraph.getWrapperCallParam(i);
                paramMerge = new Merge(nullptr, typeSize);
                connectBlocks(paramBlock, make_pair(paramMerge, paramMerge->addDataInPort()), value);
                funcGraph.setWrapperCallParam(i, paramMerge);
            }
            if (!func->getType()->isVoidTy()) {
                typeSize = DL.getTypeSizeInBits(func->getType());
                Demux* result = new Demux(nullptr, typeSize);
                fork->setConnectedPort(make_pair(result, result->addControlInPort()));
                funcGraph.setWrapperResult(result);
            }
            Demux* controlOut = new Demux(nullptr, 0);
            fork->setConnectedPort(make_pair(controlOut, controlOut->addControlInPort()));
            funcGraph.setWrapperControlOut(controlOut);
        }
        Merge* controlIn = (Merge*)funcGraph.getWrapperControlIn();
        Fork* controlFork = new Fork(nullptr, 0);
        Demux* result = (Demux*)funcGraph.getWrapperResult();
        Demux* controlOut = (Demux*)funcGraph.getWrapperControlOut();
        connectBlocks(control, make_pair(controlFork, controlFork->getDataInPort()));
        controlFork->setConnectedPort(make_pair(controlIn, controlIn->addDataInPort()));
        if (result != nullptr) {
            controlFork->setConnectedPort(make_pair(result, result->addControlInPort()));
        }
        controlFork->setConnectedPort(make_pair(controlOut, controlOut->addControlInPort()));
        funcGraph.addWrapperControlFork(controlFork);
        Block* param;
        for (unsigned int i = 0; i < callInst.getNumArgOperands(); ++i) {
            Merge* mergeParam = (Merge*)funcGraph.getWrapperCallParam(i);
            value = callInst.getArgOperand(i);
            if (isa<llvm::Constant>(value)) {
                param = createConstant(value, BB);
                graph->addBlockToBB(param);
            }
            else {
                param = varsMapping[BBName][value];
            }
            connectBlocks(param, make_pair(mergeParam, mergeParam->addDataInPort()), value);
        }
    }
    if (!func->getType()->isVoidTy()) {
        varsMapping[BBName][&inst] = callBlock;
    }
    if (controlSynch == nullptr) {
        controlSynch = new DFGraphComp::Operator(OpType::Synchronization, BB, 0);
    }
    callBlock->setConnectedPortResult(make_pair(controlSynch, controlSynch->addInputPort(0)));
    funcGraph.increaseTimesCalled();  
}



void DFGraphPass::connectFunctionCall(Function& F) {
    FunctionGraph& funcGraph = graphs[F.getName()];
    FunctionCall* callBlock;
    if (funcGraph.getTimesCalled() == 1) {
        const CallInst* call = funcGraph.getFirstCallInst();
        Block* controlIn = funcGraph.getWrapperControlIn();
        Entry* controlInF = funcGraph.getFunctionControlIn();
        const Value* value;
        connectBlocks(controlIn, make_pair(controlInF, controlInF->getControlInPort()));
        for (unsigned int i = 0; i < call->getNumArgOperands(); ++i) {
            DFGraphComp::Argument* arg = funcGraph.getArgument(i);
            Block* param = funcGraph.getWrapperCallParam(i);
            value = call->getArgOperand(i);
            connectBlocks(param, make_pair(arg, arg->getControlInPort()), value);
        }
        Block* ret = funcGraph.getFunctionResult();
        if (ret != nullptr) {
            callBlock = funcGraph.getFunctionCallBlock(0);
            ret->setConnectedPort(callBlock->getConnecDataPort());
        }
        Block* controlOut = funcGraph.getFunctionControlOut();
        controlOut->setConnectedPort(callBlock->getConnecControlPort());
    }
    else if (funcGraph.getTimesCalled() > 1) {
        Merge* controlIn = (Merge*)funcGraph.getWrapperControlIn();
        Entry* controlInF = funcGraph.getFunctionControlIn();
        controlIn->setConnectedPort(make_pair(controlInF, controlInF->getControlInPort()));
        for (unsigned int i = 0; i < funcGraph.getNumArguments(); ++i) {
            Merge* param = (Merge*)funcGraph.getWrapperCallParam(i);
            DFGraphComp::Argument* arg = funcGraph.getArgument(i);
            param->setConnectedPort(make_pair(arg, arg->getControlInPort()));
        }
        Block* ret = funcGraph.getFunctionResult();
        if (ret != nullptr) {
            Demux* res = funcGraph.getWrapperResult();
            ret->setConnectedPort(make_pair(res, res->getDataInPort()));
            for (unsigned int i = 0; i < funcGraph.getTimesCalled(); ++i) {
                callBlock = funcGraph.getFunctionCallBlock(i);
                res->addDataOutPort();
                res->setConnectedPort(callBlock->getConnecDataPort());
            }
        }
        Block* controlOutF = funcGraph.getWrapperControlOut();
        Demux* controlOut = funcGraph.getWrapperControlOut();
        controlOutF->setConnectedPort(make_pair(controlOut, controlOut->getDataInPort()));
        for (unsigned int i = 0; i < funcGraph.getTimesCalled(); ++i) {
            callBlock = funcGraph.getFunctionCallBlock(i);
            controlOut->addDataOutPort();
            controlOut->setConnectedPort(callBlock->getConnecControlPort());
        }
    }
}



void DFGraphPass::processBranchInst(const Instruction &inst)
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const BranchInst* branchInst = cast<BranchInst>(&inst);
    if (branchInst->isConditional()) {
        Value* condition = branchInst->getCondition();
        const Value* value;
        unsigned int typeSize;
        Branch* branch;
        const set <const Value*>& BBLiveOut = liveness->liveOutVars[BBName];
        for (set <const Value*>::const_iterator it = BBLiveOut.begin();
            it != BBLiveOut.end(); ++it)
        {
            value = *it;
            typeSize = DL.getTypeSizeInBits(value->getType());
            branch = new Branch(BB, typeSize);
            processOperator(value, make_pair(branch, branch->getDataInPort()), BB);
            processOperator(condition, make_pair(branch, branch->getConditionInPort()), BB);
            graph->addBlockToBB(branch);
            varsMapping[BBName][value] = branch;
        }
    }
}



void DFGraphPass::processOperator(const Value* operand, 
    pair<Block*, const Port*> connection, const BasicBlock* BB) 
{
    StringRef BBName = BB->getName();
    if (isa<llvm::Constant>(operand)) {
        ConstantInterf* constant = createConstant(operand, BB);
        constant->setConnectedPort(connection);
        graph->addBlockToBB(constant);
    }
    else if (isa<Instruction>(operand) || isa<llvm::Argument>(operand)) {
        Block* block = varsMapping[BBName][operand];
        connectBlocks(block, connection, operand);
    }
}



void DFGraphPass::processLiveIn(const BasicBlock* BB) {
    StringRef BBName = BB->getName();
    set <const Value*> liveIn = liveness->liveInVars[BBName];
    const Value* value;
    unsigned int typeSize;
    if (pred_size(BB) > 1) {
        for (set <const Value*>::const_iterator it = liveIn.begin();
            it != liveIn.end(); ++it)
        {
            value = *it;
            typeSize = DL.getTypeSizeInBits(value->getType());
            Merge* merge = new Merge(BB, typeSize);
            varsMapping[BBName][value] = merge;
            graph->addBlockToBB(merge);
            varsMerges[BB][value] = merge;
        }
    }
    else {
        const BasicBlock* predBB = *pred_begin(BB);
        for (set <const Value*>::const_iterator it = liveIn.begin();
            it != liveIn.end(); ++it) 
        {
            value = *it;
            varsMapping[BBName][value] = varsMapping[predBB->getName()][value];
        }
    }
}


void DFGraphPass::processPhiConstants(const BasicBlock* BB) {
    StringRef BBName = BB->getName();
    const Value* value;
    for (set <const Value*>::const_iterator it = liveness->phiConstants[BBName].begin();
        it != liveness->phiConstants[BBName].end(); ++it)
    {
        value = *it;
        ConstantInterf* cst = createConstant(value, BB);
        varsMapping[BBName][value] = cst;
        graph->addBlockToBB(cst);
    }
}


void DFGraphPass::processBBEntryControl(const BasicBlock* BB) 
{
    StringRef BBName = BB->getName();
    Block* controlEntry;
    if (pred_empty(BB)) {
        Entry* entry = new Entry(BB);
        graph->addControlBlockToBB(entry);
        graph->setFunctionControlIn(entry);
        controlEntry = entry;
    }
    else if (pred_size(BB) > 1) {
        Merge* merge = new Merge(BB, 0);
        graph->addControlBlockToBB(merge);
        controlMerges[BB] = merge;
        controlEntry = merge;
    }
    else {
        controlEntry = controlBlocks[(*pred_begin(BB))->getName()];
    }
    controlBlocks[BBName] = controlEntry;
}



void DFGraphPass::processBBExitControl(const BasicBlock* BB) {
    StringRef BBName = BB->getName();
    Block* controlExit;
    Block* control = controlBlocks[BBName];
    if (controlSynch != nullptr) {
        connectBlocks(control, make_pair(controlSynch, controlSynch->addInputPort(0)));
        graph->addControlBlockToBB(controlSynch);
        control = controlSynch;
    }
    if (succ_empty(BB)) {
        Exit* exitBlock = new Exit(BB);
        connectBlocks(control, make_pair(exitBlock, exitBlock->getInPort()));
        graph->addControlBlockToBB(exitBlock);
        controlExit = exitBlock;
        graph->setFunctionControlOut(exitBlock);
    }
    else if (succ_size(BB) > 1) {
        const BranchInst* branchInst = cast<BranchInst>(BB->getTerminator());
        Branch* branch = new Branch(BB, 0);
        controlExit = branch;
        processOperator(branchInst->getCondition(), make_pair(branch, 
            branch->getConditionInPort()), BB);
        connectBlocks(control, make_pair(branch, branch->getDataInPort()));
        graph->addControlBlockToBB(branch);
    
    }
    else controlExit = controlSynch;
    if (controlExit != nullptr) controlBlocks[BBName] = controlExit;
}



void DFGraphPass::connectOrphanBlock(pair <Block*, const Port*> connection) 
{
    const BasicBlock* parentBB = connection.first->getParentBB();
    StringRef parentBBName = parentBB->getName();
    Block* control = controlBlocks[parentBBName];
    connectBlocks(control, connection);
}



void DFGraphPass::connectBlocks(Block* block, pair<Block*, const Port*> connection,
    const Value* value) 
{
    const BasicBlock* currBB = connection.first->getParentBB();
    Branch* branch = nullptr;
    if (block->getBlockType() == BlockType::Branch_Block) {
        const BasicBlock* oldBB = block->getParentBB();
        branch = (Branch*)block;
        const BranchInst* branchInst = cast<BranchInst>(oldBB->getTerminator());
        const BasicBlock* BBFalse = branchInst->getSuccessor(1);
        if (!branch->isCurrentPortSet()) {
            if (varsMapping.find(BBFalse->getName()) == varsMapping.end()) {
                branch->setCurrentPort(Branch::True);
            }
            else branch->setCurrentPort(Branch::False);
        }
    }
    if (block->connectionAvailable()) {
        block->setConnectedPort(connection);
    }
    else {
        pair <Block*, const Port*> prevConnection = block->getConnectedPort();
        const BasicBlock* prevBB = prevConnection.first->getParentBB();
        StringRef prevBBName = prevBB->getName();
        Fork* fork;
        if (prevBB == nullptr) fork = new Fork(currBB);
        else fork = new Fork(prevBB);
        fork->setDataPortWidth(connection.second->getWidth());
        fork->setConnectedPort(prevConnection);
        fork->setConnectedPort(connection);
        block->setConnectedPort(make_pair(fork, fork->getDataInPort()));
        if (value != nullptr) {
            if (prevBB != nullptr) {
                if (prevBB == currBB) graph->addBlockToBB(fork);
                else graph->addBlockToBB(prevBBName, fork);
            }
            else graph->addBlockToBB(fork);
            varsMapping[prevBBName][value] = fork;
        }
        else {
            if (prevBB != nullptr) {
                if (prevBB == currBB) graph->addControlBlockToBB(fork);
                else graph->addControlBlockToBB(prevBBName, fork);
            }
            else graph->addControlBlockToBB(fork);
            controlBlocks[prevBBName] = fork;
        }
    }
    if (branch != nullptr) branch->setCurrentPort(Branch::None);
}


void DFGraphPass::connectMerge(Merge* merge, Block* block,
    const BasicBlock* predBB, const Value* value)
{
    if (block->getBlockType() == BlockType::Branch_Block) {
        Branch* branch = (Branch*)block;
        const BasicBlock* BBBranch = branch->getParentBB();
        const BranchInst* brInst = cast<BranchInst>(BBBranch->getTerminator());
        const BasicBlock* BBTrue = brInst->getSuccessor(0);
        const BasicBlock* BBFalse = brInst->getSuccessor(1);
        int idBBMerge = graph->getBBId(merge->getParentBB()->getName());
        int idBBTrue = graph->getBBId(BBTrue->getName());
        int idBBFalse = graph->getBBId(BBFalse->getName());
        int idPredBB = graph->getBBId(predBB->getName());
        if ((idPredBB >= idBBTrue and idPredBB < idBBFalse) or idBBMerge == idBBTrue) {
            branch->setCurrentPort(Branch::True);
        }
        else if ((idPredBB >= idBBFalse and idPredBB < idBBMerge) or idBBMerge == idBBFalse) {
            branch->setCurrentPort(Branch::False);
        }
    }
    connectBlocks(block, make_pair(merge, merge->addDataInPort()), value);
}


void DFGraphPass::connectMerges() {
    const BasicBlock* BB;
    const BasicBlock* predBB;
    const Value* value;
    const Value* predValue;
    Merge* merge;
    Block* predBlock;
    for (map <const BasicBlock*, map <const Value*, Merge*> >::const_iterator it =
        varsMerges.begin(); it != varsMerges.end(); ++it)
    {
        BB = it->first;
        for (map <const Value*, Merge*>::const_iterator it2 = varsMerges[BB].begin();
            it2 != varsMerges[BB].end(); ++it2) 
        {
            value = it2->first;
            merge = it2->second;
            if (isa<PHINode>(value) and cast<PHINode>(value)->getParent() == BB) {
                const PHINode* phi = cast<PHINode>(value);
                for (unsigned int i = 0; i < phi->getNumIncomingValues(); ++i) {
                    predBB = phi->getIncomingBlock(i);
                    predValue = phi->getIncomingValue(i);
                    // if (isa<llvm::Constant>(predValue)) {
                    //     ConstantInterf* cst = createConstant(predValue, predBB);
                    //     cst->setConnectedPort(make_pair(merge, merge->addDataInPort()));
                    //     graph->addBlockToBB(predBB->getName(), cst);
                    // }
                    // else if (isa<Instruction>(predValue) or isa<llvm::Argument>(predValue)) {
                    //     predBlock = varsMapping[predBB->getName()][predValue];
                    //     connectMerge(merge, predBlock, predBB, predValue);
                    // }
                    predBlock = varsMapping[predBB->getName()][predValue];
                    connectMerge(merge, predBlock, predBB, predValue);
                }
            }
            else {
                for (const_pred_iterator it3 = pred_begin(BB); it3 != pred_end(BB); ++it3) {
                    predBB = *it3;
                    predBlock = varsMapping[predBB->getName()][value];
                    connectMerge(merge, predBlock, predBB, value);
                }
            }
        }
    }
}



void DFGraphPass::connectControlMerges() {
    const BasicBlock* BB;
    const BasicBlock* predBB;
    Merge* merge;
    Block* predBlock;
    for (map <const BasicBlock*, Merge*>::const_iterator it = controlMerges.begin(); 
        it != controlMerges.end(); ++it) 
    {
        BB = it->first;
        merge = it->second;
        for (const_pred_iterator it2 = pred_begin(BB); it2 != pred_end(BB); ++it2) {
            predBB = (*it2);
            predBlock = controlBlocks[predBB->getName()];
            connectMerge(merge, predBlock, predBB);
        }
    }
}



ConstantInterf* DFGraphPass::createConstant(const Value* operand, const BasicBlock* BB) {
    ConstantInterf* constant;
    Type* type = operand->getType();
    if (type->isIntegerTy()) { //TODO: More types (vector and struct types)
        const ConstantInt* cst = cast<ConstantInt>(operand);
        if (cst->getBitWidth() <= 32) {
            constant = new DFGraphComp::Constant<int>((int)cst->getSExtValue(), BB);
        }
        else {
            constant = new DFGraphComp::Constant<long>(cst->getSExtValue(), BB);
        }
    }
    else if (type->isPointerTy()) {
        constant = new DFGraphComp::Constant<void*>(nullptr, BB);
    }
    else if (type->isFloatTy()) {
        const ConstantFP* cst = cast<ConstantFP>(operand);
        constant = new DFGraphComp::Constant<float>(cst->getValueAPF().convertToFloat(), BB);
    }
    else if (type->isDoubleTy()) {
        const ConstantFP* cst = cast<ConstantFP>(operand);
        constant = new DFGraphComp::Constant<double>(cst->getValueAPF().convertToFloat(), BB);
    }
    connectOrphanBlock(make_pair(constant, constant->getControlInPort()));
    return constant;
}


void DFGraphPass::printGraph(Module& M) {
    file << "digraph \"DataFlow Graph for '" << M.getModuleIdentifier() << "' file\" {" << endl;
    file << "\tlabel=\"DataFlow Graph for '" << M.getModuleIdentifier() << "' file\";" << endl;
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        Function& F = *it;
        file << endl;
        graphs[F.getName()].printNodes(file);
    }
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        Function& F = *it;
        file << endl;
        graphs[F.getName()].printEdges(file);
    }
    file << endl;
    file << '}' << endl;
}


void DFGraphPass::clearStructures() {
    varsMapping.clear();
    controlBlocks.clear();
    varsMerges.clear();
    controlMerges.clear();
}



static RegisterPass<DFGraphPass> registerDFGraphPass("dfGraphPass", 
    "Create Data Flow Graph from LLVM IR function Pass",
    false /* Only looks at CFG */,
    false /* Analysis Pass */);