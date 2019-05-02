
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
                ++inst_it) {
            
            processInstruction(*inst_it, varsMapping[name], graph, dl);
        }
        //processLiveVars(liveness, *bb_it);
        //processControlBlocks();
    }
    // printGraph();

    return false;
}


void DFGraphPass::processInstruction(const Instruction &inst, 
        map <StringRef, Block*>& bbVars, DFGraph& graph, const DataLayout &dl) 
{
    unsigned int opCode = inst.getOpcode();
    Block* block;
    unsigned int instTypeSize = -1;
    if (inst.getType()->isSized()) {
        instTypeSize = dl.getTypeSizeInBits(inst.getType());
    }

    // Arithmetic Operations
    
    if (opCode == Instruction::Add || opCode == Instruction::FAdd) {
        block = new Operator(OperatorType::Add, instTypeSize);
    }
    else if (opCode == Instruction::Sub || opCode == Instruction::FSub) {
        block = new Operator(OperatorType::Sub, instTypeSize);
    }
    else if (opCode == Instruction::Mul || opCode == Instruction::FMul) {
        block = new Operator(OperatorType::Mul, instTypeSize);
    }
    else if (opCode == Instruction::UDiv || opCode == Instruction::FDiv ||
        opCode == Instruction::SDiv) {
        block = new Operator(OperatorType::Div, instTypeSize);
    }
    else if (opCode == Instruction::URem || opCode == Instruction::FRem ||
        opCode == Instruction::SRem) {
        block = new Operator(OperatorType::Rem, instTypeSize);
    }

    // Bitwise Operations

    else if (opCode == Instruction::And) {
        block = new Operator(OperatorType::And, instTypeSize);
    }
    else if (opCode == Instruction::Or) {
        block = new Operator(OperatorType::Or, instTypeSize);
    }
    else if (opCode == Instruction::Xor) {
        block = new Operator(OperatorType::Xor, instTypeSize);
    }
    else if (opCode == Instruction::Shl) {
        block = new Operator(OperatorType::ShiftL, instTypeSize);
    }
    else if (opCode == Instruction::LShr or opCode == Instruction::AShr) {
        block = new Operator(OperatorType::ShiftR, instTypeSize);
    }

    // Logic Operations

    else if (opCode == Instruction::ICmp) {
        const ICmpInst* cmp = cast<ICmpInst> (&inst);
        ICmpInst::Predicate pred = cmp->getPredicate();
        if (pred == ICmpInst::ICMP_EQ) {
            block = new Operator(OperatorType::Eq, instTypeSize);
        }
        else if (pred == ICmpInst::ICMP_NE) {
            block = new Operator(OperatorType::NE, instTypeSize);
        }
        else if (pred == ICmpInst::ICMP_SGT || pred == ICmpInst::ICMP_UGT) {
            block = new Operator(OperatorType::GT, instTypeSize);
        }
        else if (pred == ICmpInst::ICMP_SGE || pred == ICmpInst::ICMP_UGE) {
            block = new Operator(OperatorType::GE, instTypeSize);
        }
        else if (pred == ICmpInst::ICMP_SLT || pred == ICmpInst::ICMP_ULT) {
            block = new Operator(OperatorType::LT, instTypeSize);
        }
        else if (pred == ICmpInst::ICMP_SLE || pred == ICmpInst::ICMP_ULE) {
            block = new Operator(OperatorType::LE, instTypeSize);
        }
    }

    else if (opCode == Instruction::FCmp) {
        const FCmpInst* cmp = cast<FCmpInst> (&inst);
        FCmpInst::Predicate pred = cmp->getPredicate();
        if (pred == FCmpInst::FCMP_OEQ || pred == FCmpInst::FCMP_UEQ) {
            block = new Operator(OperatorType::Eq, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_ONE || pred == FCmpInst::FCMP_UNE) {
            block = new Operator(OperatorType::NE, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_OGT || pred == FCmpInst::FCMP_UGT) {
            block = new Operator(OperatorType::GT, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_OGE || pred == FCmpInst::FCMP_UGE) {
            block = new Operator(OperatorType::GE, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_OLT || pred == FCmpInst::FCMP_ULT) {
            block = new Operator(OperatorType::LT, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_OLE || pred == FCmpInst::FCMP_ULE) {
            block = new Operator(OperatorType::LE, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_TRUE) {
            block = new Operator(OperatorType::True, instTypeSize);
        }
        else if (pred == FCmpInst::FCMP_FALSE) {
            block = new Operator(OperatorType::False, instTypeSize);
        }
    }

    // Phi

    else if (opCode == Instruction::PHI) {
        const PHINode* phi = cast<PHINode> (&inst);
        block = new Merge(phi->getNumIncomingValues(), instTypeSize);
    }




    graph.addBlockToBB(block);
    Value* op_i;
    Block* block_op;
    for (unsigned int i = 0; i < inst.getNumOperands(); ++i) {
        op_i = inst.getOperand(i);
        if (isa<Instruction>(op_i) || isa<llvm::Argument>(op_i)) {
            block_op = bbVars[op_i->getName()];

            if (!block_op->outPortAvailable()) {
                pair <Block*, int> prevEnd = block_op->getChannelEnd();
                Fork* fork = new Fork();
                fork->setChannelEnd(prevEnd.first, prevEnd.second);
                block_op->setChannelEnd(fork, 0);
                bbVars[op_i->getName()] = fork;
                block_op = fork;
            }
        }
        else if (isa<llvm::Constant>(op_i)) {
            // DFGraphComp::Constant<int> constant();
            // graph.addBlockToCluster(constant);
            // block_op = &constant;
        }
        block->setChannelEnd(block_op, 0);
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