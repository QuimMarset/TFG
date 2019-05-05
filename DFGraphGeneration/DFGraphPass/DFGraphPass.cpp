
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

            }
            else if (isa<LoadInst>(inst_it)) {

            }
            else if (isa<StoreInst>(inst_it)) {
                
            }
            else if (isa<PHINode>(inst_it)) {
                processPhiInst(*inst_it, varsMapping, graph, dl);
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
        //TODO: True and False ?
    }
    DFGraphComp::BinaryOperator* op = new DFGraphComp::BinaryOperator(opType);
    op->setDataIn1PortWidth(op1TypeSize);
    op->setDataIn2PortWidth(op2TypeSize);
    op->setDataOutPortWidth(resultTypeSize);
    processOperator(inst.getOperand(0), make_pair(op, 0), bbVars, graph);
    processOperator(inst.getOperand(0), make_pair(op, 1), bbVars, graph);
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
        processOperator(phi->getIncomingValue(i), make_pair(merge, i), 
            vars[phi->getIncomingBlock(i)->getName()], graph);
    }
    graph.addBlockToBB(merge);
}


void processOperator(Value* operand, pair <Block*, int> connection,
    map <StringRef, Block*>& bbVars, DFGraph& graph) 
{
    if (isa<llvm::Constant>(operand)) {
        Type* type = operand->getType();
        Block* constant;
        if (type->isIntegerTy()) {
            ConstantInt* cst = cast<ConstantInt>(operand);
            if (cst->getBitWidth <= 32) {
                constant = new DFGraphComp::Constant<int>((int)cst->getSExtValue());
            }
            else {
                constant = new DFGraphComp::Constant<int64_t>(cst->getSExtValue());
            }
        }
        else if (type->isFloatTy()) {
            ConstantFP* cst = cast<ConstantFP>(operand);
            constant = new DFGraphComp::Constant<float>(cst->getValueAPF().convertToFloat());
        }
        else if (type->isDoubleTy()) {
            ConstantFP* cst = cast<ConstantFP>(operand);
            constant = new DFGraphComp::Constant<double>(cst->getValueAPF().convertToFloat());
        }
        constant->setConnectedPort(connection);
        graph.addBlockToBB(constant);
    }
    if (isa<Instruction>(operand) || isa<llvm::Argument>(operand)) {
        Block* block = bbVars[operand->getName()];
        if (block->connectionAvailable()) {
            block->setConnectedPort(connection);
        }
        else {
            pair <Block*, int> prevConnection = block->getConnectedPort();
            Fork* fork = new Fork(2);
            fork->setDataPortWidth(0); //TODO:
            fork->setConnectedPort(prevConnection);
            block->setConnectedPort(make_pair(fork, 0));
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