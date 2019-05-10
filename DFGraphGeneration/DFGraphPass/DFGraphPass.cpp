
#include "DFGraphPass.h"

char DFGraphPass::ID = 0;

DFGraphPass::DFGraphPass() : FunctionPass(ID) {}

DFGraphPass::~DFGraphPass() {}

void DFGraphPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LiveVarsPass>();
    AU.setPreservesAll();
}

bool DFGraphPass::runOnFunction(Function &F) {
    LiveVarsPass &liveness = getAnalysis<LiveVarsPass>();
    DFGraph graph(F.getName());
    map <StringRef, map <StringRef, Block*> > varsMapping;
    DataLayout dl(F.getParent());
    for (Function::const_iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        StringRef name = bb_it->getName();
        varsMapping.insert(make_pair(name, map <StringRef, Block*>()));
        graph.addBasicBlock(BBGraph(name.str()));
        for (BasicBlock::const_iterator inst_it = bb_it->begin(); inst_it != bb_it->end(); 
            ++inst_it) 
        {
            if (isa <llvm::BinaryOperator>(inst_it) || isa<ICmpInst>(inst_it) ||
                isa<FCmpInst>(inst_it)) 
            {
                processBinaryInst(*inst_it, varsMapping[name], graph, dl);
            }
            else if (isa<AllocaInst>(inst_it)) {
                processAllocaInst(*inst_it, varsMapping[name], graph, dl);
            }
            else if (isa<LoadInst>(inst_it)) {
                processLoadInst(*inst_it, varsMapping[name], graph, dl);
            }
            else if (isa<StoreInst>(inst_it)) {
                processStoreInst(*inst_it, varsMapping[name], graph, dl);
            }
            else if (isa<PHINode>(inst_it)) {
                processPhiInst(*inst_it, varsMapping, graph, dl);
            }
            else if (isa<CastInst>(inst_it)) {
                processCastInst(*inst_it, varsMapping[name], graph, dl);
            }
            
            else if (isa<GetElementPtrInst>(inst_it)) {
                processGetElementPtrInst(*inst_it, varsMapping[name], graph, dl);
            }
        }
        //processLiveVars(liveness, *bb_it);
        //processControlBlocks();
    }
    // printGraph();

    return false;
}


void processBinaryInst(const Instruction &inst, 
    map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    unsigned int resultTypeSize = dl.getTypeSizeInBits(inst.getType());
    unsigned int op1TypeSize = dl.getTypeSizeInBits(inst.getOperand(0)->getType());
    unsigned int op2TypeSize = dl.getTypeSizeInBits(inst.getOperand(1)->getType());
    BinaryOpType opType;
    unsigned int opCode = inst.getOpcode();
    if (opCode == Instruction::Add || opCode == Instruction::FAdd) {
        opType = BinaryOpType::Add;
    }
    else if (opCode == Instruction::Sub || opCode == Instruction::FSub) {
        opType = BinaryOpType::Sub;
    }
    else if (opCode == Instruction::Mul || opCode == Instruction::FMul) {
        opType = BinaryOpType::Mul;
    }
    else if (opCode == Instruction::UDiv || opCode == Instruction::FDiv ||
        opCode == Instruction::SDiv) 
    {
        opType = BinaryOpType::Div;
    }
    else if (opCode == Instruction::URem || opCode == Instruction::FRem ||
        opCode == Instruction::SRem) 
    {
        opType = BinaryOpType::Rem;
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
        FCmpInst::Predicate pred = cmp->getPredicate();
        if (pred == FCmpInst::FCMP_OEQ || pred == FCmpInst::FCMP_UEQ) {
            opType = BinaryOpType::Eq;
        }
        else if (pred == FCmpInst::FCMP_ONE || pred == FCmpInst::FCMP_UNE) {
            opType = BinaryOpType::NE;
        }
        else if (pred == FCmpInst::FCMP_OGT || pred == FCmpInst::FCMP_UGT) {
            opType = BinaryOpType::GT;
        }
        else if (pred == FCmpInst::FCMP_OGE || pred == FCmpInst::FCMP_UGE) {
            opType = BinaryOpType::GE;
        }
        else if (pred == FCmpInst::FCMP_OLT || pred == FCmpInst::FCMP_ULT) {
            opType = BinaryOpType::LT;
        }
        else if (pred == FCmpInst::FCMP_OLE || pred == FCmpInst::FCMP_ULE) {
            opType = BinaryOpType::LE;
        }
    }
    DFGraphComp::BinaryOperator* op = new DFGraphComp::BinaryOperator(opType);
    op->setDataIn1PortWidth(op1TypeSize);
    op->setDataIn2PortWidth(op2TypeSize);
    op->setDataOutPortWidth(resultTypeSize);
    processOperator(inst.getOperand(0), make_pair(op, op->getDataIn1Port()), bbVars, graph);
    processOperator(inst.getOperand(0), make_pair(op, op->getDataIn2Port()), bbVars, graph);
    graph.addBlockToBB(op);   
}

void processPhiInst(const Instruction &inst,
    map <StringRef, map <StringRef, Block*> >& vars, DFGraph& graph, 
    const DataLayout &dl)
{
    const PHINode* phi = cast<PHINode>(&inst);
    int numInputs = phi->getNumIncomingValues();
    Merge* merge = new Merge(numInputs);
    for (unsigned int i = 0; i < numInputs; ++i) {
        processOperator(phi->getIncomingValue(i), make_pair(merge, merge->getDataInPort(i)), 
            vars[phi->getIncomingBlock(i)->getName()], graph);
    }
    graph.addBlockToBB(merge);
}

void processAllocaInst(const Instruction &inst, 
    map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    const AllocaInst* allocaInst = cast<AllocaInst>(&inst);
    unsigned int allocSize = dl.getTypeSizeInBits(allocaInst->getAllocatedType());
    DFGraphComp::Constant<int>* cstAlloc = new DFGraphComp::Constant<int>(allocSize);
    if (allocaInst->isArrayAllocation()) {
        const ConstantInt* cstElem = cast<ConstantInt>(allocaInst->getArraySize());
        DFGraphComp::Constant<int>* nElems = new DFGraphComp::Constant<int>(cstElem->getZExtValue());
        DFGraphComp::BinaryOperator* allocaBlock = new DFGraphComp::BinaryOperator(BinaryOpType::AllocaVector);
        cstAlloc->setConnectedPort(make_pair(allocaBlock, allocaBlock->getDataIn1Port()));
        nElems->setConnectedPort(make_pair(allocaBlock, allocaBlock->getDataIn2Port()));
        graph.addBlockToBB(nElems);
        graph.addBlockToBB(allocaBlock);
        bbVars[inst.getName()] = allocaBlock;
    }
    else {
        UnaryOperator* allocaBlock = new UnaryOperator(UnaryOpType::Alloca);
        cstAlloc->setConnectedPort(make_pair(allocaBlock, allocaBlock->getDataInPort()));
        graph.addBlockToBB(allocaBlock);
        bbVars[inst.getName()] = allocaBlock;
    }
    graph.addBlockToBB(cstAlloc);
}

void processLoadInst(const Instruction &inst, 
    map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    const LoadInst* loadInst = cast<LoadInst>(&inst);
    unsigned int pointerSize = dl.getTypeSizeInBits(loadInst->getPointerOperandType());
    unsigned int valueTypeSize = dl.getTypeSizeInBits(loadInst->getType());
    UnaryOperator* loadOp = new UnaryOperator(UnaryOpType::Load);
    loadOp->setDataInPortWidth(pointerSize);
    loadOp->setDataOutPortWidth(valueTypeSize);
    processOperator(loadInst->getPointerOperand(), make_pair(loadOp, loadOp->getDataInPort()),
        bbVars, graph);
    bbVars[loadInst->getName()] = loadOp;
    graph.addBlockToBB(loadOp);
}

void processStoreInst(const Instruction &inst, 
    map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    const StoreInst* storeInst = cast<StoreInst>(&inst);
    unsigned int storeValueSize = dl.getTypeSizeInBits(storeInst->getValueOperand()->getType());
    unsigned int pointerSize = dl.getTypeSizeInBits(storeInst->getPointerOperandType());
    // create block
    // set width
    // processOperator();
    // processOperator();
    // graph.addBlockToBB()
}

void processCastInst(const Instruction &inst, 
    map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    const CastInst* castInst = cast<CastInst>(&inst);
    unsigned int operandTypeSize = dl.getTypeSizeInBits(castInst->getSrcTy());
    unsigned int castTypeSize = dl.getTypeSizeInBits(castInst->getDestTy());
    Value* operand = castInst->getOperand(0);
    BinaryOpType castOpType;
    unsigned int opCode = castInst->getOpcode();
    if (opCode == Instruction::Trunc) {
        opCode = BinaryOpType::IntTrunc;
    }
    else if (opCode == Instruction::FPTrunc) {
        opCode == BinaryOpType::FPointTrunc;
    }
    else if (opCode == Instruction::ZExt) {
        opCode = BinaryOpType::IntZExt;
    }
    else if (opCode == Instruction::FPExt) {
        opCode = BinaryOpType::IntSExt;
    }
    else if (opCode == Instruction::FPToUI) {
        opCode = BinaryOpType::FPointToUInt;
    }
    else if (opCode == Instruction::FPToSI) {
        opCode = BinaryOpType::FPointToSInt;
    }
    else if (opCode == Instruction::UIToFP) {
        opCode = BinaryOpType::UIntToFPoint;
    }
    else if (opCode == Instruction::SIToFP) {
        opCode = BinaryOpType::SIntToFPoint;
    }
    else if (opCode == Instruction::IntToPtr) {
        opCode = BinaryOpType::IntToPtr;
    }
    else if (opCode == Instruction::PtrToInt) {
        opCode = BinaryOpType::PtrToInt;
    }
    else if (opCode == Instruction::BitCast) {
        opCode = BinaryOpType::TypeCast;
    }
    else if (opCode == Instruction::AddrSpaceCast) {
        opCode = BinaryOpType::AddrSpaceCast;
    }
    DFGraphComp::BinaryOperator* castOp = new DFGraphComp::BinaryOperator(castOpType);
    if (isa<llvm::Constant>(operand)) {
        Type* type = operand->getType();
        Block* constant;
        if (type->isIntegerTy()) {
            const ConstantInt* cst = cast<ConstantInt>(operand);
            if (cst->getBitWidth <= 32) {
                constant = new DFGraphComp::Constant<int>((int)cst->getSExtValue());
            }
            else {
                constant = new DFGraphComp::Constant<int64_t>(cst->getSExtValue());
            }
        }
        else if (type->isFloatTy()) {
            const ConstantFP* cst = cast<ConstantFP>(operand);
            constant = new DFGraphComp::Constant<float>(cst->getValueAPF().convertToFloat());
        }
        else if (type->isDoubleTy()) {
            const ConstantFP* cst = cast<ConstantFP>(operand);
            constant = new DFGraphComp::Constant<double>(cst->getValueAPF().convertToFloat());
        }
        constant->setConnectedPort(make_pair(castOp, castOp->getDataIn1Port()));
        graph.addBlockToBB(constant);
    }
    else if (isa<Instruction>(operand) || isa<llvm::Argument>(operand)) {
        processOperator(operand, make_pair(castOp, castOp->getDataIn1Port()), bbVars, graph);
    }
    bbVars[inst.getName()] = castOp;
    graph.addBlockToBB(castOp);
}

void processGetElementPtrInst(const Instruction &inst, 
    map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    const GetElementPtrInst* gep = cast<GetElementPtrInst>(&inst);
    llvm::gep_type_begin
    for (unsigned int i = 1; i < gep->getNumOperands(); ++i) {

    }
}




void processOperator(const Value* operand, pair <Block*, const Port*> connection,
    map <StringRef, Block*>& bbVars, DFGraph& graph) 
{
    if (isa<llvm::Constant>(operand)) {
        Type* type = operand->getType();
        Block* constant;
        if (type->isIntegerTy()) {
            const ConstantInt* cst = cast<ConstantInt>(operand);
            if (cst->getBitWidth <= 32) {
                constant = new DFGraphComp::Constant<int>((int)cst->getSExtValue());
            }
            else {
                constant = new DFGraphComp::Constant<int64_t>(cst->getSExtValue());
            }
        }
        else if (type->isFloatTy()) {
            const ConstantFP* cst = cast<ConstantFP>(operand);
            constant = new DFGraphComp::Constant<float>(cst->getValueAPF().convertToFloat());
        }
        else if (type->isDoubleTy()) {
            const ConstantFP* cst = cast<ConstantFP>(operand);
            constant = new DFGraphComp::Constant<double>(cst->getValueAPF().convertToFloat());
        }
        constant->setConnectedPort(connection);
        graph.addBlockToBB(constant);
    }
    else if (isa<Instruction>(operand) || isa<llvm::Argument>(operand)) {
        Block* block = bbVars[operand->getName()];
        if (block->connectionAvailable()) {
            block->setConnectedPort(connection);
        }
        else {
            pair <Block*, const Port*> prevConnection = block->getConnectedPort();
            Fork* fork = new Fork(2);
            fork->setDataPortWidth(prevConnection.second->getWidth()); //TODO:
            fork->setConnectedPort(prevConnection);
            block->setConnectedPort(make_pair(fork, fork->getDataInPort()));
        }
    }
}


void processLiveVars(const LiveVarsPass& liveness, const BasicBlock& bb) {

}

void DFGraphPass::printGraph() {
    /*  if (not exists base directory) {
            create base directory
        }
        create current file directory
        printGraph()
        printLVA (?)
    */
}



static RegisterPass<DFGraphPass> registerDFGraphPass("dfGraphPass", 
    "Create Data Flow Graph from LLVM IR function Pass",
    false /* Only looks at CFG */,
    false /* Analysis Pass */);