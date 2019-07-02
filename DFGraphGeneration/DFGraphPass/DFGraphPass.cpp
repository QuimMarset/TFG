
#include "DFGraphPass.h"

char DFGraphPass::ID = 0;

DFGraphPass::DFGraphPass() : ModulePass(ID), DL("") {}

DFGraphPass::~DFGraphPass() {}

void DFGraphPass::getAnalysisUsage(AnalysisUsage &AU) const {
    /* Pass that will be needed to execute before this one */
    AU.addRequired<LiveVarsPass>();
    AU.setPreservesAll();
}


bool DFGraphPass::runOnModule(Module& M) {
    string fileName = M.getModuleIdentifier();
    file.open(fileName.substr(0, fileName.size()-3) + ".dot");
    DL = DataLayout(&M);
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        processFunction(*it);
        clearStructures();
    }
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        connectFunctionCall(*it);
    }
    printGraph(M);
    file.close();
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        FunctionGraph& funcGraph = graphs[it->getName()];
        funcGraph.freeGraph();
    }
    return false;
}


void DFGraphPass::processFunction(Function &F) {
    BBNumber = 0;
    if (F.isDeclaration()) {
        assert(0 && "Function without body cannot be handled");
    }
    liveness = &getAnalysis<LiveVarsPass>(F);
    StringRef funcName = F.getName();
    if (graphs.find(funcName) == graphs.end()) {
        graphs[funcName] = FunctionGraph(funcName.str());
    }
    graph = &graphs[funcName];
    bool firstBB = true;
    for (const BasicBlock& BB : F.getBasicBlockList()) {
        controlSynch = nullptr;
        StringRef BBName = BB.getName();
        if (!graph->existsBB(BBName)) {
            varsMapping.insert(make_pair(BBName, map <const Value*, Block*>()));
            varsMerges.insert(make_pair(&BB, map <const Value*, Merge*>()));
            graph->addBasicBlock(BBName, BBNumber);
        }
        else {
            graph->setCurrentBB(BBName);
            graph->setBBId(BBNumber);
        }
        ++BBNumber;
        processBBEntryControl(&BB);
        if (firstBB) {
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
            if (isa <llvm::BinaryOperator>(inst_it)) {
                processBinaryInst(*inst_it);
            }
            else if (isa<llvm::CmpInst>(inst_it)) {
                processCmpInst(*inst_it);
            }
            // else if (inst_it->getOpcode == Instruction::FNeg) {
            //     processFNegInst(*inst_it);
            // }
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
            else if (isa<GetElementPtrInst>(inst_it) or 
                isa<SwitchInst>(*inst_it)) 
            {
                assert(0 && "Not should appear (lowered with a pass");
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
            else {
                assert(0 && "Instruction not currently supported");
            }
        }
        processBBExitControl(&BB);
    }
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
    const BasicBlock* BB = inst.getParent();
    DFGraphComp::Operator* op = new DFGraphComp::Operator(opType, BB, typeSize);
    processOperator(inst.getOperand(0), op, 0, BB);
    processOperator(inst.getOperand(1), op, 1, BB);
    graph->addBlockToBB(op);
    varsMapping[BB->getName()][&inst] = op;
}



void DFGraphPass::processCmpInst(const Instruction &inst) {
    OpType opType;
    unsigned int opCode = inst.getOpcode();
    if (opCode == Instruction::ICmp) {
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
    else {
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
    DFGraphComp::Operator* op = new DFGraphComp::Operator(opType, BB, 
        DL.getTypeSizeInBits(inst.getOperand(0)->getType()));
    op->setDataOutPortWidth(DL.getTypeSizeInBits(inst.getType()));
    processOperator(inst.getOperand(0), op, 0, BB);
    processOperator(inst.getOperand(1), op, 1, BB);
    graph->addBlockToBB(op);
    varsMapping[BB->getName()][&inst] = op;
}



void DFGraphPass::processFNegInst(const Instruction &inst) {
   const BasicBlock* BB = inst.getParent(); 
   OpType opType = OpType::FNeg;
   unsigned int typeSize = DL.getTypeSizeInBits(inst.getType());
   DFGraphComp::Operator* op = new DFGraphComp::Operator(opType, BB, typeSize);
   processOperator(inst.getOperand(0), op, 0, BB);
   graph->addBlockToBB(op);
   varsMapping[BB->getName()][&inst] = op;
}



void DFGraphPass::processPhiInst(const Instruction &inst) {
    const BasicBlock* BB = inst.getParent();
    const PHINode* phi = cast<PHINode>(&inst);
    if (varsMerges[BB].find(phi) == varsMerges[BB].end()) {
        unsigned int typeSize = DL.getTypeSizeInBits(phi->getType());
        Merge* merge = new Merge(BB, typeSize);
        varsMerges[BB][&inst] = merge;
        varsMapping[BB->getName()][&inst] = merge;
        graph->addBlockToBB(merge);
    }
}



void DFGraphPass::processAllocaInst(const Instruction &inst) {
    const BasicBlock* BB = inst.getParent();
    const AllocaInst* allocaInst = cast<AllocaInst>(&inst);
    unsigned int allocBytes = DL.getTypeAllocSize(allocaInst->getAllocatedType());
    if (allocaInst->isArrayAllocation()) {
        const ConstantInt* cstSize = cast<ConstantInt>(allocaInst->getArraySize());
        unsigned int nElems = cstSize->getZExtValue();
        allocBytes *= nElems;
    }
    DFGraphComp::Constant<unsigned int>* allocBytesCst = 
        new DFGraphComp::Constant<unsigned int>(allocBytes, BB);
    connectOrphanCst(allocBytesCst);
    DFGraphComp::Operator* allocaBlock = new DFGraphComp::Operator(OpType::Alloca, BB);
    allocaBlock->setDataInPortWidth(0, sizeof(unsigned int));
    unsigned int ptrSize = DL.getTypeAllocSizeInBits(allocaInst->getType());
    allocaBlock->setDataOutPortWidth(ptrSize);
    allocBytesCst->setConnectedPort(allocaBlock, 0);
    graph->addBlockToBB(allocBytesCst);
    graph->addBlockToBB(allocaBlock);
    varsMapping[BB->getName()][&inst] = allocaBlock;
}



void DFGraphPass::processLoadInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    const LoadInst* loadInst = cast<LoadInst>(&inst);
    unsigned int pointerSize = DL.getTypeSizeInBits(loadInst->getPointerOperandType());
    unsigned int valueSize = DL.getTypeSizeInBits(loadInst->getType());
    DFGraphComp::Operator* loadOp = new DFGraphComp::Operator(OpType::Load, BB);
    loadOp->setDataInPortWidth(0, pointerSize);
    loadOp->setDataOutPortWidth(valueSize);
    processOperator(loadInst->getPointerOperand(), loadOp, 0, BB);
    varsMapping[BB->getName()][&inst] = loadOp;
    graph->addBlockToBB(loadOp);
}



void DFGraphPass::processStoreInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    const StoreInst* storeInst = cast<StoreInst>(&inst);
    unsigned int storedValueSize = DL.getTypeSizeInBits(storeInst->getValueOperand()->getType());
    unsigned int pointerSize = DL.getTypeSizeInBits(storeInst->getPointerOperandType());
    DFGraphComp::Operator* store = new DFGraphComp::Operator(OpType::Store, BB);
    store->setDataInPortWidth(0, storedValueSize);
    store->setDataInPortWidth(1, pointerSize);
    processOperator(storeInst->getValueOperand(), store, 0, BB);
    processOperator(storeInst->getPointerOperand(), store, 1, BB);
    graph->addBlockToBB(store);
}



void DFGraphPass::processCastInst(const Instruction &inst) 
{
    const CastInst* castInst = cast<CastInst>(&inst);
    unsigned int operandSize = DL.getTypeSizeInBits(castInst->getSrcTy());
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
    DFGraphComp::Operator* castOp = new DFGraphComp::Operator(castOpType, BB);
    castOp->setDataInPortWidth(0, operandSize);
    castOp->setDataOutPortWidth(castTypeSize);
    processOperator(operand, castOp, 0, BB);
    varsMapping[BB->getName()][&inst] = castOp;
    graph->addBlockToBB(castOp);
}



void DFGraphPass::processSelectInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    const SelectInst* selectInst = cast<SelectInst>(&inst);
    unsigned int typeSize = DL.getTypeSizeInBits(selectInst->getType());
    Select* selectBlock = new Select(BB, typeSize);
    processOperator(selectInst->getTrueValue(), selectBlock, 0, BB);
    processOperator(selectInst->getFalseValue(), selectBlock, 1, BB);
    processOperator(selectInst->getCondition(), selectBlock, 2, BB);
    graph->addBlockToBB(selectBlock);
    varsMapping[BB->getName()][&inst] = selectBlock;
}



void DFGraphPass::processReturnInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    if (inst.getNumOperands() > 0) {
        Value* operand = inst.getOperand(0);
        unsigned int typeSize = DL.getTypeSizeInBits(operand->getType());
        Return* retBlock = new Return(BB, typeSize);
        processOperator(operand, retBlock, 0, BB);
        graph->addBlockToBB(retBlock);
        varsMapping[BB->getName()][&inst] = retBlock;
        Block* functionReturn = graph->getFunctionResult();
        if (functionReturn != nullptr) {
            Merge* mergeRet;
            if (functionReturn->getBlockType() == BlockType::Exit_Block) {
                mergeRet = new Merge(nullptr, typeSize);
                functionReturn->setConnectedPort(mergeRet, mergeRet->addDataInPort());
                graph->setFunctionResult(mergeRet);
            }
            else {
                mergeRet = (Merge*)functionReturn;
            }
            retBlock->setConnectedPort(mergeRet, mergeRet->addDataInPort());
        }
        else {
            graph->setFunctionResult(retBlock);
        }
    }
}


void changeConnection(pair <Block*, int> origin, pair <Block*, int> newEnd) {
    Block* originBlock = origin.first;
    if (originBlock->getBlockType() != BlockType::Fork_Block) {
        if (originBlock->getBlockType() == BlockType::Branch_Block) {
            Branch* originBranch = (Branch*)originBlock;
            if (origin.second == 0) originBranch->setCurrentPort(false);
            else originBranch->setCurrentPort(true);
        }
        else if (originBlock->getBlockType() == BlockType::Demux_Block) {
            Demux* originDemux = (Demux*)originBlock;
            originDemux->setCurrentConnectedPort(origin.second);
        }
        pair <Block*, int> connection = originBlock->getConnectedPort();
        if (connection.first->getBlockType() == BlockType::Fork_Block) {
            Fork* originFork = (Fork*)connection.first;
            originFork->setOutPort(0, newEnd);
        }
        else {
            originBlock->setConnectedPort(newEnd);
        }
    }
    else {
        Fork* originFork = (Fork*)originBlock;
        originFork->setOutPort(origin.second, newEnd);
    }
    
}



void DFGraphPass::processCallInst(const Instruction& inst) {
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const Value* value;
    Block* blockVar;
    const CallInst& callInst = cast<CallInst>(inst);
    StringRef funcName = callInst.getCalledFunction()->getName();
    if (graphs.find(funcName) == graphs.end()) {
        graphs[funcName] = FunctionGraph(funcName.str());
    }
    FunctionGraph& funcGraph = graphs[funcName];
    int timesCalled = funcGraph.getTimesCalled();
    FunctionCall* callBlock = new FunctionCall(BB);
    funcGraph.addFunctionCallBlock(callBlock);
    if (timesCalled == 0) {
        blockVar = controlBlocks[BBName];
        connectBlocks(blockVar, callBlock, 0);
        blockVar = controlBlocks[BBName];
        callBlock->setInputContPort(blockVar, blockVar->getOutputPortIndex());
        for (unsigned int i = 0; i < funcGraph.getNumArguments(); ++i) {
            value = callInst.getArgOperand(i);
            if (isa<llvm::Constant>(value)) {
                blockVar = createConstant(value, BB);
                graph->addBlockToBB(blockVar);
            }
            else {
                blockVar = varsMapping[BBName][value];
            }
            connectBlocks(blockVar, callBlock, i+1, value);
            if (!isa<llvm::Constant>(value)) blockVar = varsMapping[BBName][value];
            callBlock->addInputArgPort(blockVar, blockVar->getOutputPortIndex());
        }
        if (!callInst.getType()->isVoidTy()) {
            varsMapping[BBName][&inst] = callBlock;
        }
    }
    else {
        if (timesCalled == 1) {
            FunctionCall* prevCallBlock = funcGraph.getFunctionCallBlock(0);
            Fork* wrapForkControl = new Fork(nullptr, 0);
            changeConnection(prevCallBlock->getInputContPort(), make_pair(wrapForkControl, 0));
            funcGraph.addWrapperControlFork(wrapForkControl);
            Merge* wrapControlIn = new Merge(nullptr, 0);
            wrapForkControl->setConnectedPort(wrapControlIn, wrapControlIn->addDataInPort());
            funcGraph.setWrapperControlIn(wrapControlIn);
            unsigned int typeSize;
            Merge* wrapParam;
            for (unsigned int i = 0; i < funcGraph.getNumArguments(); ++i) {
                typeSize = DL.getTypeSizeInBits(callInst.getArgOperand(i)->getType());
                wrapParam = new Merge(nullptr, typeSize);
                changeConnection(prevCallBlock->getInputArgPort(i), 
                    make_pair(wrapParam, wrapParam->addDataInPort()));
                funcGraph.addWrapperCallArg(wrapParam);
            }
            Demux* wrapControlOut = new Demux(nullptr, 0);
            wrapForkControl->setConnectedPort(wrapControlOut, wrapControlOut->addControlInPort());
            funcGraph.setWrapperControlOut(wrapControlOut);
            if (!callInst.getType()->isVoidTy()) {
                typeSize = DL.getTypeAllocSize(callInst.getType());
                Demux* wrapResult = new Demux(nullptr, typeSize);
                wrapForkControl->setConnectedPort(wrapResult, wrapResult->addControlInPort());
                funcGraph.setWrapperResult(wrapResult);
            }
        }
        Merge* wrapControlIn = funcGraph.getWrapperControlIn();
        Fork* wrapForkControl = new Fork(nullptr, 0);
        funcGraph.addWrapperControlFork(wrapForkControl);
        blockVar = controlBlocks[BBName];
        connectBlocks(blockVar, wrapForkControl, 0);
        wrapForkControl->setConnectedPort(wrapControlIn, wrapControlIn->addDataInPort());
        Demux* wrapControlOut = funcGraph.getWrapperControlOut();
        wrapForkControl->setConnectedPort(wrapControlOut, wrapControlOut->addControlInPort());
        if (!callInst.getType()->isVoidTy()) {
            Demux* wrapResult = funcGraph.getWrapperResult();
            wrapForkControl->setConnectedPort(wrapResult, wrapResult->addControlInPort());
            varsMapping[BBName][&inst] = callBlock;
        }
        Merge* wrapParam;
        for (unsigned int i = 0; i < funcGraph.getNumArguments(); ++i) {
            value = callInst.getArgOperand(i);
            if (isa<llvm::Constant>(value)) {
                blockVar = createConstant(value, BB);
                graph->addBlockToBB(blockVar);
            }
            else {
                blockVar = varsMapping[BBName][callInst.getArgOperand(i)];
            }
            wrapParam = funcGraph.getWrapperCallArg(i);
            connectBlocks(blockVar, wrapParam, wrapParam->addDataInPort(), value);
        }
    }
    if (controlSynch == nullptr) {
        controlSynch = new DFGraphComp::Operator(OpType::Synchronization, BB, 0);
    }
    callBlock->setConnectedControlPort(controlSynch, controlSynch->addInputPort(0));
    funcGraph.increaseTimesCalled();
}



void DFGraphPass::connectFunctionCall(Function& F) {
    FunctionGraph& funcGraph = graphs[F.getName()];
    FunctionCall* callBlock;
    if (funcGraph.getTimesCalled() == 1) {
        callBlock = funcGraph.getFunctionCallBlock(0);
        changeConnection(callBlock->getInputContPort(), 
            make_pair(funcGraph.getFunctionControlIn(), 0));
        for (unsigned int i = 0; i < funcGraph.getNumArguments(); ++i) {
            changeConnection(callBlock->getInputArgPort(i), 
                make_pair(funcGraph.getArgument(i), 0));
        }
        Block* funcResult = funcGraph.getFunctionResult();
        if (funcResult != nullptr) {
            funcResult->setConnectedPort(callBlock->getConnecDataPort());
        }
        Block* funcControlOut = funcGraph.getFunctionControlOut();
        funcControlOut->setConnectedPort(callBlock->getConnecControlPort());
    }
    else if (funcGraph.getTimesCalled() > 1) {
        Merge* wrapControlIn = (Merge*)funcGraph.getWrapperControlIn();
        Entry* funcControlIn = funcGraph.getFunctionControlIn();
        wrapControlIn->setConnectedPort(funcControlIn, 0);
        for (unsigned int i = 0; i < funcGraph.getNumArguments(); ++i) {
            Merge* wrapParam = (Merge*)funcGraph.getWrapperCallArg(i);
            DFGraphComp::Argument* funcArg = funcGraph.getArgument(i);
            wrapParam->setConnectedPort(funcArg, 0);
        }
        Block* funcResult = funcGraph.getFunctionResult();
        if (funcResult != nullptr) {
            Demux* wrapResult = funcGraph.getWrapperResult();
            funcResult->setConnectedPort(wrapResult, 0);
            for (unsigned int i = 0; i < funcGraph.getTimesCalled(); ++i) {
                callBlock = funcGraph.getFunctionCallBlock(i);
                wrapResult->addDataOutPort();
                wrapResult->setConnectedPort(callBlock->getConnecDataPort());
            }
        }
        Block* funcControlOut = funcGraph.getFunctionControlOut();
        Demux* wrapControlOut = funcGraph.getWrapperControlOut();
        funcControlOut->setConnectedPort(wrapControlOut, 0);
        for (unsigned int i = 0; i < funcGraph.getTimesCalled(); ++i) {
            callBlock = funcGraph.getFunctionCallBlock(i);
            wrapControlOut->addDataOutPort();
            wrapControlOut->setConnectedPort(callBlock->getConnecControlPort());
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
            processOperator(value, branch, 0, BB);
            processOperator(condition, branch, 1, BB);
            graph->addBlockToBB(branch);
            varsMapping[BBName][value] = branch;
        }
    }
}



void DFGraphPass::processOperator(const Value* operand, 
    Block* connecBlock, int connecPort, const BasicBlock* BB) 
{
    if (isa<llvm::Constant>(operand)) {
        ConstantInterf* constant = createConstant(operand, BB);
        constant->setConnectedPort(connecBlock, connecPort);
        graph->addBlockToBB(constant);
    }
    else if (isa<Instruction>(operand) || isa<llvm::Argument>(operand)) {
        Block* block = varsMapping[BB->getName()][operand];
        connectBlocks(block, connecBlock, connecPort, operand);
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
    const PHINode* phi;
    const BasicBlock* phiBB;
    const Value* value;
    // Stored each phi instruction and the index of the operand from BB
    for (set <pair <const PHINode*, unsigned int> >::const_iterator it = 
        liveness->phiConstants[BBName].begin();
        it != liveness->phiConstants[BBName].end(); ++it)
    {
        phi = it->first;
        phiBB = phi->getParent();
        value = phi->getIncomingValue(it->second);
        ConstantInterf* cst = createConstant(value, BB);
        graph->addBlockToBB(cst);
        bool createdBB = false;
        /* Constants are only connected once, and we need to connect them here to input the
            control signals. Therefore, we create the merge representing the phi, it will 
            connect with */
        if (varsMerges.find(phiBB) == varsMerges.end()) {
            varsMapping.insert(make_pair(phiBB->getName(), map <const Value*, Block*>()));
            varsMerges.insert(make_pair(phiBB, map <const Value*, Merge*>()));
            graph->addBasicBlock(phiBB->getName(), BBNumber);
            createdBB = true;
        }
        Merge* phiMerge;
        if (varsMerges[phiBB].find(phi) == varsMerges[phiBB].end()) {
            phiMerge = new Merge(phiBB, DL.getTypeSizeInBits(phi->getType()));
            if (createdBB) {
                graph->addBlockToBB(phiMerge);
                graph->setCurrentBB(BBName); // current BB changed when we created the BB
            }
            else graph->addBlockToBB(phiBB->getName(), phiMerge);
            varsMapping[phiBB->getName()][phi] = phiMerge;
            varsMerges[phiBB][phi] = phiMerge;
        }
        else {
            phiMerge = varsMerges[phiBB][phi];
        }
        cst->setConnectedPort(phiMerge, phiMerge->addDataInPort());
    }
}


void DFGraphPass::processBBEntryControl(const BasicBlock* BB) 
{
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
    controlBlocks[BB->getName()] = controlEntry;
}



void DFGraphPass::processBBExitControl(const BasicBlock* BB) {
    StringRef BBName = BB->getName();
    Block* controlExit;
    Block* control = controlBlocks[BBName];
    if (controlSynch != nullptr) {
        connectBlocks(control, controlSynch, controlSynch->addInputPort(0));
        graph->addControlBlockToBB(controlSynch);
        control = controlSynch;
    }
    if (succ_empty(BB)) {
        Exit* exitBlock = new Exit(BB);
        connectBlocks(control, exitBlock, 0);
        graph->addControlBlockToBB(exitBlock);
        controlExit = exitBlock;
        Block* controlOut = graph->getFunctionControlOut();
        if (controlOut != nullptr) {
            Merge* mergeControlOut;
            if (controlOut->getBlockType() == BlockType::Exit_Block) {
                mergeControlOut = new Merge(nullptr, 0);
                controlOut->setConnectedPort(mergeControlOut, mergeControlOut->addDataInPort());
                graph->setFunctionControlOut(mergeControlOut);
            }
            else {
                mergeControlOut = (Merge*)controlOut;
            }
            controlExit->setConnectedPort(mergeControlOut, mergeControlOut->addDataInPort());
        }
        else {
            graph->setFunctionControlOut(controlExit);
        }
    }
    else if (succ_size(BB) > 1) {
        const BranchInst* branchInst = cast<BranchInst>(BB->getTerminator());
        Branch* branch = new Branch(BB, 0);
        controlExit = branch;
        processOperator(branchInst->getCondition(), branch, 1, BB);
        connectBlocks(control, branch, 0);
        graph->addControlBlockToBB(branch);
    }
    else controlExit = controlSynch;
    if (controlExit != nullptr) controlBlocks[BBName] = controlExit;
}



void DFGraphPass::connectOrphanCst(ConstantInterf* connecBlock) 
{
    const BasicBlock* parentBB = connecBlock->getParentBB();
    StringRef parentBBName = parentBB->getName();
    Block* control = controlBlocks[parentBBName];
    connectBlocks(control, connecBlock, 0);
}



void DFGraphPass::connectBlocks(Block* block, Block* connecBlock,
    int connecPort, const Value* value) 
{
    if (block->getBlockType() == BlockType::Branch_Block) {
        const BasicBlock* branchBB = block->getParentBB();
        Branch* branch = (Branch*)block;
        const BranchInst* branchInst = cast<BranchInst>(branchBB->getTerminator());
        const BasicBlock* BBFalse = branchInst->getSuccessor(1);
        if (connecBlock->getBlockType() != BlockType::Merge_Block) {
            if (varsMapping.find(BBFalse->getName()) == varsMapping.end()) {
                branch->setCurrentPort(true);
            }
            else branch->setCurrentPort(false);
        }
    }
    if (block->connectionAvailable()) {
        block->setConnectedPort(connecBlock, connecPort);
    }
    else {
        pair <Block*, int> prevConnection = block->getConnectedPort();
        const BasicBlock* prevBB = prevConnection.first->getParentBB();
        const BasicBlock* currBB = connecBlock->getParentBB();
        int portWidth = 0;
        if (value != nullptr) portWidth = DL.getTypeSizeInBits(value->getType());
        Fork* fork;
        if (prevBB == nullptr) fork = new Fork(currBB, portWidth);
        else fork = new Fork(prevBB, portWidth);
        fork->setConnectedPort(prevConnection);
        fork->setConnectedPort(connecBlock, connecPort);
        block->setConnectedPort(fork, 0);
        if (value != nullptr) {
            if (prevBB != nullptr) {
                StringRef prevBBName = prevBB->getName();
                if (prevBB == currBB) graph->addBlockToBB(fork);
                else graph->addBlockToBB(prevBBName, fork);
                varsMapping[prevBBName][value] = fork;
            }
            else {
                graph->addBlockToBB(fork);
                varsMapping[currBB->getName()][value] = fork;
            }
        }
        else {
            if (prevBB != nullptr) {
                StringRef prevBBName = prevBB->getName();
                if (prevBB == currBB) graph->addControlBlockToBB(fork);
                else graph->addControlBlockToBB(prevBBName, fork);
                controlBlocks[prevBBName] = fork;
            }
            else {
                graph->addControlBlockToBB(fork);
                controlBlocks[currBB->getName()] = fork;
            }
        }
    }
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
            branch->setCurrentPort(true);
        }
        else if ((idPredBB >= idBBFalse and idPredBB < idBBMerge) or idBBMerge == idBBFalse) {
            branch->setCurrentPort(false);
        }
        else assert(0 && "Cannot find branch output to merge");
    }
    connectBlocks(block, merge, merge->addDataInPort(), value);
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
                    predValue = phi->getIncomingValue(i);
                    if (isa<Instruction>(predValue) || isa<llvm::Argument>(predValue)) {
                        predBB = phi->getIncomingBlock(i);
                        predBlock = varsMapping[predBB->getName()][predValue];
                        connectMerge(merge, predBlock, predBB, predValue);
                    }
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



void DFGraphPass::connectControlMerges() 
{
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



ConstantInterf* DFGraphPass::createConstant(const Value* operand, const BasicBlock* BB) 
{
    ConstantInterf* constant;
    Type* type = operand->getType();
    if (type->isIntegerTy()) {
        const ConstantInt* cst = cast<ConstantInt>(operand);
        if (cst->getBitWidth() <= 32) {
            constant = new DFGraphComp::Constant<int>((int)cst->getSExtValue(), BB);
        }
        else {
            constant = new DFGraphComp::Constant<long>(cst->getSExtValue(), BB);
        }
    }
    else if (type->isPointerTy()) { 
        // Created as string to print the value nullptr, but with the correct width
        constant = new DFGraphComp::Constant<string>("nullptr", BB, 32);
    }
    else if (type->isFloatTy()) {
        const ConstantFP* cst = cast<ConstantFP>(operand);
        constant = new DFGraphComp::Constant<float>(cst->getValueAPF().convertToFloat(), BB);
    }
    else if (type->isDoubleTy()) {
        const ConstantFP* cst = cast<ConstantFP>(operand);
        constant = new DFGraphComp::Constant<double>(cst->getValueAPF().convertToFloat(), BB);
    }
    else {
        assert(0 && "Constant type not supported");
    }
    connectOrphanCst(constant);
    return constant;
}


void DFGraphPass::printGraph(Module& M) 
{
    file << "digraph \"DataFlow Graph for '" << M.getModuleIdentifier() << "' file\" {" << endl;
    file << "\tlabel=\"DataFlow Graph for '" << M.getModuleIdentifier() << "' file\";" << endl;
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        Function& F = *it;
        file << endl;
        graphs[F.getName()].printNodes(file, F);
    }
    for (Module::iterator it = M.begin(); it != M.end(); ++it) {
        Function& F = *it;
        file << endl;
        graphs[F.getName()].printEdges(file, F);
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