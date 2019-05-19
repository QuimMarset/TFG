
#include "DFGraphPass.h"

char DFGraphPass::ID = 0;

DFGraphPass::DFGraphPass() : FunctionPass(ID), DL("") {}

DFGraphPass::~DFGraphPass() {}

void DFGraphPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LiveVarsPass>();
    AU.setPreservesAll();
}

bool DFGraphPass::runOnFunction(Function &F) {
    liveness = &getAnalysis<LiveVarsPass>();
    graph = DFGraph(F.getName());
    DL = DataLayout(F.getParent());
    bool firstBB = true;
    for (const BasicBlock& BB : F.getBasicBlockList()) {
        StringRef BBName = BB.getName();
        varsMapping.insert(make_pair(BBName, map <const Value*, Block*>()));
        varsMerges.insert(make_pair(&BB, map <const BasicBlock*, set <pair <Merge*, const Value*> > >()));
        graph.addBasicBlock();
        processBBEntryControl(&BB);
        if (firstBB) { // Function arguments
            firstBB = false;
            for (Function::const_arg_iterator arg_it = F.arg_begin(); arg_it != F.arg_end(); ++arg_it) {
                unsigned int argTypeSize = DL.getTypeSizeInBits(arg_it->getType());
                DFGraphComp::Argument* argBlock = new DFGraphComp::Argument(argTypeSize);
                graph.addBlockToBB(argBlock);
                varsMapping[BBName][arg_it] = argBlock;
                connectOrphanBlock(make_pair(argBlock, argBlock->getControlInPort()), &BB);
            }
        }
        else {
            processLiveIn(&BB);
        }
        for (BasicBlock::const_iterator inst_it = BB.begin(); inst_it != BB.end(); 
            ++inst_it) 
        {
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
                assert(0 && "Not supported (currently)");
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
        }
        processPhiConstants(&BB);
        processBBExitControl(&BB);
    }//TODO: Globals vars (allocate memory)
    connectMerges();
    connectControlMerges();
    printGraph(F);
    return false;
}


void DFGraphPass::processBinaryInst(const Instruction &inst) 
{
    unsigned int resultTypeSize = DL.getTypeSizeInBits(inst.getType());
    unsigned int op1TypeSize = DL.getTypeSizeInBits(inst.getOperand(0)->getType());
    unsigned int op2TypeSize = DL.getTypeSizeInBits(inst.getOperand(1)->getType());
    BinaryOpType opType;
    unsigned int opCode = inst.getOpcode();
    if (opCode == Instruction::Add) {
        opType = BinaryOpType::Add;
    }
    else if (opCode == Instruction::FAdd) {
        opCode = BinaryOpType::FAdd;
    }
    else if (opCode == Instruction::Sub) {
        opType = BinaryOpType::Sub;
    }
    else if (opCode == Instruction::FSub) {
        opCode = BinaryOpType::FSub;
    }
    else if (opCode == Instruction::Mul) {
        opType = BinaryOpType::Mul;
    }
    else if (opCode == Instruction::FMul) {
        opCode = BinaryOpType::FMul;
    }
    else if (opCode == Instruction::UDiv || opCode == Instruction::SDiv) {
        opType = BinaryOpType::Div;
    }
    else if (opCode == Instruction::FDiv) {
        opCode = BinaryOpType::FDiv;
    }
    else if (opCode == Instruction::URem || opCode == Instruction::SRem) {
        opType = BinaryOpType::Rem;
    }
    else if (opCode == Instruction::FRem) {
        opType = BinaryOpType::FRem;
    }
    else if (opCode == Instruction::And) {
        opType = BinaryOpType::And;
    }
    else if (opCode == Instruction::Or) {
        opType = BinaryOpType::Or;
    }
    else if (opCode == Instruction::Xor) {
        opType = BinaryOpType::Xor;
    }
    else if (opCode == Instruction::Shl) {
        opType = BinaryOpType::ShiftL;
    }
    else if (opCode == Instruction::LShr or opCode == Instruction::AShr) {
        opType = BinaryOpType::ShiftR;
    }
    else if (opCode == Instruction::ICmp) {
        const ICmpInst* cmp = cast<ICmpInst> (&inst);
        ICmpInst::Predicate pred = cmp->getPredicate();
        if (pred == ICmpInst::ICMP_EQ) {
            opType = BinaryOpType::Eq;
        }
        else if (pred == ICmpInst::ICMP_NE) {
            opType = BinaryOpType::NE;
        }
        else if (pred == ICmpInst::ICMP_SGT || pred == ICmpInst::ICMP_UGT) {
            opType = BinaryOpType::GT;
        }
        else if (pred == ICmpInst::ICMP_SGE || pred == ICmpInst::ICMP_UGE) {
            opType = BinaryOpType::GE;
        }
        else if (pred == ICmpInst::ICMP_SLT || pred == ICmpInst::ICMP_ULT) {
            opType = BinaryOpType::LT;
        }
        else if (pred == ICmpInst::ICMP_SLE || pred == ICmpInst::ICMP_ULE) {
            opType = BinaryOpType::LE;
        }
    }
    else if (opCode == Instruction::FCmp) {
        const FCmpInst* cmp = cast<FCmpInst> (&inst);
        FCmpInst::Predicate predicate = cmp->getPredicate();
        if (predicate == FCmpInst::FCMP_OEQ || predicate == FCmpInst::FCMP_UEQ) {
            opType = BinaryOpType::FEq;
        }
        else if (predicate == FCmpInst::FCMP_ONE || predicate == FCmpInst::FCMP_UNE) {
            opType = BinaryOpType::FNE;
        }
        else if (predicate == FCmpInst::FCMP_OGT || predicate == FCmpInst::FCMP_UGT) {
            opType = BinaryOpType::FGT;
        }
        else if (predicate == FCmpInst::FCMP_OGE || predicate == FCmpInst::FCMP_UGE) {
            opType = BinaryOpType::FGE;
        }
        else if (predicate == FCmpInst::FCMP_OLT || predicate == FCmpInst::FCMP_ULT) {
            opType = BinaryOpType::FLT;
        }
        else if (predicate == FCmpInst::FCMP_OLE || predicate == FCmpInst::FCMP_ULE) {
            opType = BinaryOpType::FLE;
        }
        else if (predicate == FCmpInst::FCMP_TRUE) {
            opType = BinaryOpType::True;
        }
        else if (predicate == FCmpInst::FCMP_FALSE) {
            opType = BinaryOpType::False;
        }
    } 
    DFGraphComp::BinaryOperator* op = new DFGraphComp::BinaryOperator(opType);
    op->setDataIn1PortWidth(op1TypeSize);
    op->setDataIn2PortWidth(op2TypeSize);
    op->setDataOutPortWidth(resultTypeSize);
    processOperator(inst.getOperand(0), make_pair(op, op->getDataIn1Port()), inst.getParent());
    processOperator(inst.getOperand(1), make_pair(op, op->getDataIn2Port()), inst.getParent());
    graph.addBlockToBB(op);
    varsMapping[inst.getParent()->getName()][&inst] = op;
}

void DFGraphPass::processPhiInst(const Instruction &inst)
{
    const BasicBlock* BB = inst.getParent();
    const PHINode* phi = cast<PHINode>(&inst);
    unsigned int resultTypeSize = DL.getTypeSizeInBits(phi->getType());
    Merge* merge = new Merge();
    merge->setDataOutPortWidth(resultTypeSize);
    for (unsigned int i = 0; i < phi->getNumIncomingValues(); ++i) {
        varsMerges[BB][phi->getIncomingBlock(i)].insert(make_pair(merge, phi->getIncomingValue(i)));
    }
    graph.addBlockToBB(merge);
    varsMapping[BB->getName()][&inst] = merge;
}

void DFGraphPass::processAllocaInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    const AllocaInst* allocaInst = cast<AllocaInst>(&inst);
    unsigned int allocBytes = DL.getTypeAllocSize(allocaInst->getAllocatedType());
    if (allocaInst->isArrayAllocation()) {
        const ConstantInt* cstSize = cast<ConstantInt>(allocaInst->getArraySize());
        unsigned int nElems = cstSize->getZExtValue();
        allocBytes *= nElems;
    }
    unsigned int ptrSize = DL.getTypeAllocSizeInBits(allocaInst->getType());
    DFGraphComp::Constant<unsigned int>* allocBytesBlock = 
        new DFGraphComp::Constant<unsigned int>(allocBytes);
    connectOrphanBlock(make_pair(allocBytesBlock, allocBytesBlock->getControlInPort()), BB);
    DFGraphComp::Constant<unsigned int>* alignBlock =
        new DFGraphComp::Constant<unsigned int>(allocaInst->getAlignment());
    connectOrphanBlock(make_pair(alignBlock, alignBlock->getControlInPort()), BB);
    DFGraphComp::BinaryOperator* allocaBlock = 
        new DFGraphComp::BinaryOperator(BinaryOpType::Alloca);
    allocaBlock->setDataIn1PortWidth(sizeof(unsigned int));
    allocaBlock->setDataIn2PortWidth(sizeof(unsigned int));
    allocaBlock->setDataOutPortWidth(ptrSize);
    allocBytesBlock->setConnectedPort(make_pair(allocaBlock, allocaBlock->getDataIn1Port()));
    alignBlock->setConnectedPort(make_pair(allocaBlock, allocaBlock->getDataIn2Port()));
    graph.addBlockToBB(allocBytesBlock);
    graph.addBlockToBB(alignBlock);
    graph.addBlockToBB(allocaBlock);
    varsMapping[inst.getParent()->getName()][&inst] = allocaBlock;
}

void DFGraphPass::processLoadInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    const LoadInst* loadInst = cast<LoadInst>(&inst);
    unsigned int pointerSize = DL.getTypeSizeInBits(loadInst->getPointerOperandType());
    unsigned int valueTypeSize = DL.getTypeSizeInBits(loadInst->getType());
    unsigned int align = loadInst->getAlignment();
    UnaryOperator* loadOp = new UnaryOperator(UnaryOpType::Load); //TODO:
    loadOp->setDataInPortWidth(pointerSize);
    loadOp->setDataOutPortWidth(valueTypeSize);
    DFGraphComp::Constant<unsigned int>* cstAlign = new DFGraphComp::Constant<unsigned int>(align);
    //cstAlign->setConnectedPort(make_pair(loadOp, loadOp->getDataIn1Port()));
    //connectOrphanBlock(make_pair(cstAlign, cstAlign->getControlInPort()), BB);
    processOperator(loadInst->getPointerOperand(), make_pair(loadOp, loadOp->getDataInPort()),
        inst.getParent());
    varsMapping[BB->getName()][&inst] = loadOp;
    graph.addBlockToBB(loadOp);
}

void DFGraphPass::processStoreInst(const Instruction &inst) 
{
    const StoreInst* storeInst = cast<StoreInst>(&inst);
    unsigned int storeValueSize = DL.getTypeSizeInBits(storeInst->getValueOperand()->getType());
    unsigned int pointerSize = DL.getTypeSizeInBits(storeInst->getPointerOperandType());
    Store* store = new Store();
    store->setDataPortWidth(storeValueSize);
    store->setAddrPortWidth(pointerSize);
    processOperator(storeInst->getValueOperand(), make_pair(store, store->getInPort()), 
        inst.getParent());
    processOperator(storeInst->getPointerOperand(), make_pair(store, store->getAddrPort()), 
        inst.getParent());
    DFGraphComp::Constant<int>* cstAlign = new DFGraphComp::Constant<int>(storeInst->getAlignment());
    cstAlign->setConnectedPort(make_pair(store, store->getAlignPort()));
    connectOrphanBlock(make_pair(cstAlign, cstAlign->getControlInPort()), inst.getParent());
    graph.addBlockToBB(cstAlign);
    graph.addBlockToBB(store);
}

void DFGraphPass::processCastInst(const Instruction &inst) 
{
    const CastInst* castInst = cast<CastInst>(&inst);
    unsigned int operandTypeSize = DL.getTypeSizeInBits(castInst->getSrcTy());
    unsigned int castTypeSize = DL.getTypeSizeInBits(castInst->getDestTy());
    Value* operand = castInst->getOperand(0);
    UnaryOpType castOpType;
    unsigned int opCode = castInst->getOpcode();
    if (opCode == Instruction::Trunc) {
        castOpType = UnaryOpType::IntTrunc;
    }
    else if (opCode == Instruction::FPTrunc) {
        castOpType = UnaryOpType::FPointTrunc;
    }
    else if (opCode == Instruction::ZExt) {
        castOpType = UnaryOpType::IntZExt;
    }
    else if (opCode == Instruction::FPExt) {
        castOpType = UnaryOpType::IntSExt;
    }
    else if (opCode == Instruction::FPToUI) {
        castOpType = UnaryOpType::FPointToUInt;
    }
    else if (opCode == Instruction::FPToSI) {
        castOpType = UnaryOpType::FPointToSInt;
    }
    else if (opCode == Instruction::UIToFP) {
        castOpType = UnaryOpType::UIntToFPoint;
    }
    else if (opCode == Instruction::SIToFP) {
        castOpType = UnaryOpType::SIntToFPoint;
    }
    else if (opCode == Instruction::IntToPtr) {
        castOpType = UnaryOpType::IntToPtr;
    }
    else if (opCode == Instruction::PtrToInt) {
        castOpType = UnaryOpType::PtrToInt;
    }
    else if (opCode == Instruction::BitCast) { //TODO:
        castOpType = UnaryOpType::TypeCast;
    }
    else if (opCode == Instruction::AddrSpaceCast) {
        castOpType = UnaryOpType::AddrSpaceCast;
    }
    UnaryOperator* castOp = new UnaryOperator(castOpType);
    castOp->setDataInPortWidth(operandTypeSize);
    castOp->setDataOutPortWidth(castTypeSize);
    processOperator(operand, make_pair(castOp, castOp->getDataInPort()), inst.getParent());
    varsMapping[inst.getParent()->getName()][&inst] = castOp;
    graph.addBlockToBB(castOp);
}

void DFGraphPass::processSelectInst(const Instruction &inst) 
{
    const SelectInst* selectInst = cast<SelectInst>(&inst);
    Select* selectBlock = new Select();
    unsigned int width = DL.getTypeSizeInBits(selectInst->getTrueValue()->getType());
    selectBlock->setDataTruePortWidth(width);
    selectBlock->setDataFalsePortWidth(width);
    selectBlock->setDataOutPortWidth(width);
    processOperator(selectInst->getTrueValue(), make_pair(selectBlock, 
        selectBlock->getDataInTruePort()), inst.getParent());
    processOperator(selectInst->getFalseValue(), make_pair(selectBlock, 
        selectBlock->getDataInFalsePort()), inst.getParent());
    processOperator(selectInst->getCondition(), make_pair(selectBlock, 
        selectBlock->getConditionInPort()), inst.getParent());
    graph.addBlockToBB(selectBlock);
    varsMapping[inst.getParent()->getName()][&inst] = selectBlock;
}

void DFGraphPass::processReturnInst(const Instruction &inst) 
{
    const BasicBlock* BB = inst.getParent();
    Return* retBlock = new Return;
    if (inst.getNumOperands() > 0) {
        Value* operand = inst.getOperand(0);
        unsigned int size = DL.getTypeSizeInBits(operand->getType());
        retBlock->setDataPortWidth(size);
        processOperator(operand, make_pair(retBlock, retBlock->getInPort()), BB);
    }
    else {
        retBlock->setDataPortWidth(0);
        connectOrphanBlock(make_pair(retBlock, retBlock->getInPort()), BB);
    }
    graph.addBlockToBB(retBlock);
    varsMapping[BB->getName()][&inst] = retBlock;
}

void DFGraphPass::processBranchInst(const Instruction &inst)
{
    const BasicBlock* BB = inst.getParent();
    StringRef BBName = BB->getName();
    const BranchInst* branchInst = cast<BranchInst>(&inst);
    const set<const Value*>& BBLiveOut = liveness->liveOutVars[BBName];
    unsigned int numBranch = BBLiveOut.size();
    Value* condition = nullptr;
    Block* conditionTrue = nullptr;
    if (branchInst->isConditional()) {
        condition = branchInst->getCondition();
    }
    else {
        // DFGraphComp::Constant<bool>* cstTrue = new DFGraphComp::Constant<bool>(true, 1);
        // connectOrphanBlock(make_pair(cstTrue, cstTrue->getControlInPort()), BB);
        // graph.addBlockToBB(cstTrue);
        // if (numBranch > 1) {
        //     Fork* fork = new Fork();
        //     graph.addBlockToBB(fork);
        //     cstTrue->setConnectedPort(make_pair(fork, fork->getDataInPort()));
        //     conditionTrue = fork;
        // }
        // else conditionTrue = cstTrue;
    }
    for (set<const Value*>::const_iterator it = BBLiveOut.begin(); it != BBLiveOut.end(); ++it) {
        const Value* value = *it;
        Block* blockVar = varsMapping[BBName][value];
        if (varsMapping[BBName].find(value) != varsMapping[BBName].end()) {
            blockVar = varsMapping[BBName][value];
        }   
        else {
            errs() << "No hauria d'entrar aqui\n";
        }
        Branch* branchBlock = new Branch();
        if (condition) {
            processOperator(condition, make_pair(branchBlock, branchBlock->getConditionInPort()), 
                BB);
        }
        else { 
            DFGraphComp::Constant<bool>* cstTrue = new DFGraphComp::Constant<bool>(true, 1);
            connectOrphanBlock(make_pair(cstTrue, cstTrue->getControlInPort()), BB);
            graph.addBlockToBB(cstTrue);
            cstTrue->setConnectedPort(make_pair(branchBlock, branchBlock->getConditionInPort()));
            // connectBlocks(conditionTrue, make_pair(branchBlock, branchBlock->getConditionInPort()));
        }
        blockVar = varsMapping[BBName][value];
        connectBlocks(blockVar, make_pair(branchBlock, branchBlock->getDataInPort()));
        varsMapping[BBName][value] = branchBlock;
        graph.addBlockToBB(branchBlock);
    }
}


void DFGraphPass::processOperator(const Value* operand, pair<Block*, const Port*> connection,
    const BasicBlock* BB) 
{
    StringRef BBName = BB->getName();
    if (isa<llvm::Constant>(operand)) {
        ConstantInterf* constant = createConstant(operand, BB);
        constant->setConnectedPort(connection);
        graph.addBlockToBB(constant);
    }
    else if (isa<Instruction>(operand) || isa<llvm::Argument>(operand)) {
        const set<const Value*>& BBLiveIn = liveness->liveInVars[BBName];
        if (varsMapping[BBName].find(operand) != varsMapping[BBName].end()) {
            Block* block = varsMapping[BBName][operand];
            if (Fork* fork = connectBlocks(block, connection)) {
                varsMapping[BBName][operand] = fork;
            }
        }
        else if (BBLiveIn.find(operand) != BBLiveIn.end()) {
            errs() << "No hauria d'entrar aqui\n";
        }
    }
}

void DFGraphPass::processLiveIn(const BasicBlock* BB) {
    StringRef BBName = BB->getName();
    set <const Value*> liveIn = liveness->liveInVars[BBName];
    for (set <const Value*>::const_iterator it = liveIn.begin(); it != liveIn.end(); ++it)
    {
        const Value* value = *it;
        unsigned int size = DL.getTypeSizeInBits(value->getType());
        Merge* mergeLiveIn = new Merge();
        varsMapping[BBName][value] = mergeLiveIn;
        mergeLiveIn->setDataOutPortWidth(size);
        graph.addBlockToBB(mergeLiveIn);
        for (const_pred_iterator it = pred_begin(BB); it != pred_end(BB); ++it) {
            const BasicBlock* pred = (*it);
            StringRef predName = pred->getName();
            set <const Value*>& predLiveOut = liveness->liveOutVars[predName];
            if (predLiveOut.find(value) != predLiveOut.end()) {
                varsMerges[BB][pred].insert(make_pair(mergeLiveIn, value));
            }
        }
    }
}

void DFGraphPass::processPhiConstants(const BasicBlock* BB) {
    StringRef BBName = BB->getName();
    for (set<const Value*>::const_iterator it = liveness->phiConstants[BBName].begin();
        it != liveness->phiConstants[BBName].end(); ++it)
    {
        Branch* branch = new Branch();
        ConstantInterf* cst = createConstant(*it, BB);
        cst->setConnectedPort(make_pair(branch, branch->getDataInPort()));
        varsMapping[BBName][*it] = branch;
        graph.addBlockToBB(cst);
        graph.addBlockToBB(branch);
        const BranchInst* branchInst = cast<BranchInst>(BB->getTerminator());
        if (branchInst->isConditional()) {
            processOperator(branchInst->getCondition(), make_pair(branch, 
                branch->getConditionInPort()), BB);
        }
        else {
            DFGraphComp::Constant<bool>* cstTrue = new DFGraphComp::Constant<bool>(true, 1);
            cstTrue->setConnectedPort(make_pair(branch, branch->getConditionInPort()));
            connectOrphanBlock(make_pair(cstTrue, cstTrue->getControlInPort()), BB);
            graph.addBlockToBB(cstTrue);
        }
    }
}

void DFGraphPass::processBBEntryControl(const BasicBlock* BB) 
{
    Block* controlEntry;
    if (pred_empty(BB)) {
        controlEntry = new Entry();
    }
    else {
        Merge* merge = new Merge();
        merge->setDataOutPortWidth(0);
        controlMerges[BB] = merge;
        controlEntry = merge;
    }
    graph.addBlockToBB(controlEntry);
    controlBlocks[BB->getName()] = controlEntry;
}


void DFGraphPass::connectOrphanBlock(pair <Block*, const Port*> connection, const BasicBlock* BB) {
    Block* control = controlBlocks[BB->getName()];
    if (Fork* fork = connectBlocks(control, connection)) {
        controlBlocks[BB->getName()] = fork;
    }
}

void DFGraphPass::processBBExitControl(const BasicBlock* BB) {
    Block* controlExit;
    Block* control = controlBlocks[BB->getName()];
    pair<Block*, const Port*> connection;
    if (succ_empty(BB)) {
        Exit* exitBlock = new Exit();
        connection = make_pair(exitBlock, exitBlock->getInPort());
        controlExit = exitBlock;
    }
    else {
        Branch* branch = new Branch();
        controlExit = branch;
        branch->setDataPortWidth(0);
        connection = make_pair(branch, branch->getDataInPort());
        pair <Block*, const Port*> connectionCond = make_pair(branch, branch->getConditionInPort());
        if (const BranchInst* branchInst = dyn_cast<BranchInst>(BB->getTerminator())) 
        {
            if (branchInst->isConditional()) {
                processOperator(branchInst->getCondition(), connectionCond, BB);
            }
            else {
                DFGraphComp::Constant<bool>* cstTrue = new DFGraphComp::Constant<bool>(true, 1);
                connectOrphanBlock(make_pair(cstTrue, cstTrue->getControlInPort()), BB);
                cstTrue->setConnectedPort(connectionCond);
                graph.addBlockToBB(cstTrue);
            }
        }
    }
    graph.addBlockToBB(controlExit);
    control = controlBlocks[BB->getName()];
    connectBlocks(control, connection);
    controlBlocks[BB->getName()] = controlExit;
}


Fork* DFGraphPass::connectBlocks(Block* block, pair<Block*, const Port*> connection) {
    if (block->connectionAvailable()) {
        block->setConnectedPort(connection);
    }
    else {
        pair <Block*, const Port*> prevConnection = block->getConnectedPort();
        Fork* fork = new Fork();
        fork->setDataInPortWidth(connection.second->getWidth());
        fork->setConnectedPort(prevConnection);
        fork->setConnectedPort(connection);
        block->setConnectedPort(make_pair(fork, fork->getDataInPort()));
        graph.addBlockToBB(fork);
        return fork;
    }
    return nullptr;
}


void DFGraphPass::connectMerges() {
    for (map <const BasicBlock*, map <const BasicBlock*, set <pair<Merge*, 
        const Value*> > > >::const_iterator it = varsMerges.begin();
        it != varsMerges.end(); ++it) 
    {
        const BasicBlock* BB = it->first;
        for (map <const BasicBlock*, set <pair <Merge*, const Value* > > >::const_iterator
            it2 = varsMerges[BB].begin(); it2 != varsMerges[BB].end(); ++it2)
        {
            const BasicBlock* pred = it2->first;
            const BranchInst* branchInst = cast<BranchInst>(pred->getTerminator());
            for (set <pair <Merge*, const Value*> >::const_iterator it3 = varsMerges[BB][pred].begin();
                it3 != varsMerges[BB][pred].end(); ++it3)
            {
                Merge* merge = it3->first;
                const Value* value = it3->second;
                Branch* branchBlock = (Branch*)varsMapping[pred->getName()][value];
                pair <Block*, const Port*> connection = make_pair(merge, merge->addDataInPort(0));
                if (branchInst->getSuccessor(0) == BB) {
                    if (branchInst->isConditional()) {
                        branchBlock->setConnectedPortFalse(connection);
                    }
                    else {
                        branchBlock->setConnectedPortTrue(connection);
                    }
                }
                else if (branchInst->isConditional() and branchInst->getSuccessor(1) == BB) {
                    branchBlock->setConnectedPortTrue(connection);
                }
            }
        }
    }
}


void DFGraphPass::connectControlMerges() {
    for(map<const BasicBlock*, Merge*>::const_iterator it = controlMerges.begin(); it != 
        controlMerges.end(); ++it) 
    {
        const BasicBlock* BB = it->first;
        Merge* merge = it->second;
        pair <Block*, const Port*> connection;
        for (const_pred_iterator it2 = pred_begin(BB); it2 != pred_end(BB); ++it2) {
            connection = make_pair(merge, merge->addDataInPort());
            const BasicBlock* pred = (*it2);
            const BranchInst* predBranchInst = cast<BranchInst>(pred->getTerminator());
            Branch* predControlBranch = (Branch*)controlBlocks[pred->getName()];
            if (predBranchInst->getSuccessor(0) == BB) {
                if (predBranchInst->isConditional()) {
                    predControlBranch->setConnectedPortFalse(connection);
                }
                else {
                    predControlBranch->setConnectedPortTrue(connection);
                }
            }
            else if (predBranchInst->isConditional() and predBranchInst->getSuccessor(1) == BB) {
                predControlBranch->setConnectedPortTrue(connection);
            }
        }
    }
}



ConstantInterf* DFGraphPass::createConstant(const Value* operand, const BasicBlock* BB) {
    ConstantInterf* constant;
    Type* type = operand->getType();
    if (type->isIntegerTy()) { //TODO: More types
        const ConstantInt* cst = cast<ConstantInt>(operand);
        if (cst->getBitWidth() <= 32) {
            constant = new DFGraphComp::Constant<int>((int)cst->getSExtValue());
        }
        else {
            constant = new DFGraphComp::Constant<long>(cst->getSExtValue());
        }
    }
    else if (type->isPointerTy()) {
        constant = new DFGraphComp::Constant<void*>(nullptr);
    }
    else if (type->isFloatTy()) {
        const ConstantFP* cst = cast<ConstantFP>(operand);
        constant = new DFGraphComp::Constant<float>(cst->getValueAPF().convertToFloat());
    }
    else if (type->isDoubleTy()) {
        const ConstantFP* cst = cast<ConstantFP>(operand);
        constant = new DFGraphComp::Constant<double>(cst->getValueAPF().convertToFloat());
    }
    connectOrphanBlock(make_pair(constant, constant->getControlInPort()), BB);
    return constant;
}


void DFGraphPass::printGraph(Function& F) {
    ofstream file;
    file.open(liveness->getInputFileName() + "_" + 
        graph.getFunctionName() + ".dot");
    graph.printGraph(file);
    graph.freeGraph();
    file.close();
}



static RegisterPass<DFGraphPass> registerDFGraphPass("dfGraphPass", 
    "Create Data Flow Graph from LLVM IR function Pass",
    false /* Only looks at CFG */,
    false /* Analysis Pass */);