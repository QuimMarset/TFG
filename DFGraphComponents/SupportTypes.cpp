
#include "SupportTypes.h"


namespace DFGraphComp
{


/*
 * =================================
 *  Enum BlockType
 * =================================
*/


ostream &operator << (ostream &out, BlockType blockType) {
    switch (blockType)
    {
        case BlockType::Operator_Block:
            out << "Operator";
            break;
        case BlockType::Buffer_Block:
            out << "Buffer";
            break;
        case BlockType::Constant_Block:
            out << "Constant";
            break;
        case BlockType::Fork_Block:
            out << "Fork";
            break;
        case BlockType::Merge_Block:
            out << "Merge";
            break;
        case BlockType::Select_Block:
            out << "Select";
            break;
        case BlockType::Branch_Block:
            out << "Branch";
            break;
        case BlockType::Demux_Block:
            out << "Demux";
            break;
        case BlockType::Entry_Block:
            out << "Entry";
            break;
        case BlockType::Exit_Block:
            out << "Exit";
            break;
        default:
            break;
    }
    return out;
}


/*
 * =================================
 *  Enum OperatorType
 * =================================
*/

int numberOperators = 47;

string getOpName(OpType op) {
    switch (op)
    {
        case Add:
            return "Add";
            break;
        case FAdd:
            return "FAdd";
            break;
        case Sub:
            return "Sub";
            break;
        case FSub:
            return "FSub";
            break;
        case Mul:
            return "Mul";
            break;
        case FMul:
            return "FMul";
            break;
        case Div:
            return "Div";
            break;
        case FDiv:
            return "FDiv";
            break;
        case Rem:
            return "Rem";
            break;
        case FRem:
            return "FRem";
            break;
        case And:
            return "And";
            break;
        case Or:
            return "Or";
            break;
        case Xor:
            return "Xor";
            break;
        case ShiftL:
            return "Shl";
            break;
        case ShiftR:
            return "Shr";
            break;
        case Eq:
            return "Eq";
            break;
        case FEq:
            return "FEq";
            break;
        case NE:
            return "Ne";
            break;
        case FNE:
            return "FNE";
            break;
        case GT:
            return "Gt";
            break;
        case FGT:
            return "FGt";
            break;
        case LT:
            return "Lt";
            break;
        case FLT:
            return "FLt";
            break;
        case GE:
            return "Ge";
            break;
        case FGE:
            return "FGe";
            break;
        case LE:
            return "Le";
            break;
        case FLE:
            return "FLe";
            break;
        case True:
            return "True";
            break;
        case False:
            return "False";
            break;
        case Store:
            return "Store";
            break;
        case Load:
            return "Load";
            break;
        case Alloca:
            return "Alloca";
            break;
        case FNeg:
            return "FNeg";
            break;
        case IntTrunc:
            return "IntTrunc";
            break;
        case IntZExt:
            return "IntZExt";
            break;
        case IntSExt:
            return "IntSExt";
            break;
        case FPointToUInt:
            return "FPointToUInt";
            break;
        case FPointToSInt:
            return "FPointToSInt";
            break;
        case UIntToFPoint:
            return "UIntToFPoint";
            break;
        case SIntToFPoint:
            return "SIntToFPoint";
            break;
        case FPointTrunc:
            return "FPointTrunc";
            break;
        case FPointExt:
            return "FPointExt";
            break;
        case PtrToInt:
            return "PtrToInt";
            break;
        case IntToPtr:
            return "IntToPtr";
            break;
        case BitCast:
            return "BitCast";
            break;
        case AddrSpaceCast:
            return "AddrSpaceCast";
            break;
        case Synchronization:
            return "Synchronization";
            break;
        default:
            break;
    }
    return "";
}


bool isUnary(OpType op) {
    return (op >= OpType::Load and op < OpType::Synchronization);
}


bool isBinary(OpType op) {
    return (op >= OpType::Add and op < OpType::Load);
}



ostream &operator << (ostream& out, OpType op) {
    switch (op)
    {
        case Add:
            out << "add";
            break;
        case FAdd:
            out << "fadd";
            break;
        case Sub:
            out << "sub";
            break;
        case FSub:
            out << "fsub";
            break;
        case Mul:
            out << "mul";
            break;
        case FMul:
            out << "fmul";
            break;
        case Div:
            out << "div";
            break;
        case FDiv:
            out << "fdiv";
            break;
        case Rem:
            out << "rem";
            break;
        case FRem:
            out << "frem";
            break;
        case ShiftL:
            out << "shl";
            break;
        case ShiftR:
            out << "shr";
            break;
        case And:
            out << "and";
            break;
        case Or:
            out << "or";
            break;
        case Xor:
            out << "xor";
            break;
        case Eq:
            out << "eq";
            break;
        case FEq:
            out << "feq";
            break;
        case NE:
            out << "ne";
            break;
        case FNE:
            out << "fne";
            break;
        case GT:
            out << "gt";
            break;
        case FGT:
            out << "fgt";
            break;
        case LT:
            out << "lt";
            break;
        case FLT:
            out << "flt";
            break;
        case GE:
            out << "ge";
            break;
        case FGE:
            out << "fge";
            break;
        case LE:
            out << "le";
            break;
        case FLE:
            out << "fle";
            break;
        case True:
            out << "true";
            break;
        case False:
            out << "false";
            break;        
        case Store:
            out << "store";
            break;
        case Load:
            out << "load";
            break;
        case Alloca:
            out << "alloca";
            break;
        case FNeg:
            out << "fneg";
            break;
        case IntTrunc:
            out << "inttrunc";
            break;
        case IntZExt:
            out << "intzext";
            break;
        case IntSExt:
            out << "intsext";
            break;
        case FPointToUInt:
            out << "fpointtouint";
            break;
        case FPointToSInt:
            out << "fpointtosint";
            break;
        case UIntToFPoint:
            out << "uinttofpoint";
            break;
        case SIntToFPoint:
            out << "sinttofpoint";
            break;
        case FPointTrunc:
            out << "fpointtrunc";
            break;
        case FPointExt:
            out << "fpointext";
            break;
        case PtrToInt:
            out << "ptrtoint";
            break;
        case IntToPtr:
            out << "inttoptr";
            break;
        case BitCast:
            out << "bitcast";
            break;
        case AddrSpaceCast:
            out << "addrspacecast";
            break;
        case Synchronization:
            out << "synchronization";
            break;
        default:
            break;
    }
    return out;
}


/*
 * =================================
 *  Class Port
 * =================================
*/


Port::Port() {}

Port::Port(const string &name, int width, Port::PortType type, 
    unsigned int delay) {
    assert(width >= -1);
    this->name = name;
    this->type = type;
    this->width = width;
    this->delay = delay;
}

Port::Port(const Port &port) {
    name = port.name;
    width = port.width;
    type = port.type;
    delay = port.delay;
}

Port::~Port() {}

string Port::getName() const {
    return name;
}

Port::PortType Port::getType() const {
    return type;
}

int Port::getWidth() const {
    return width;
}

int Port::getDelay() const {
    return delay;
}

void Port::setName(string name) {
    this->name = name;
}

void Port::setType(Port::PortType type) {
    this->type = type;
}

void Port::setWidth(unsigned int width) {
    this->width = width;
}

void Port::setDelay(unsigned int delay) {
    this->delay = delay;
}

ostream &operator << (ostream &out, const Port &p) {
    out << p.name;
    switch (p.type)
    {
        case Port::PortType::Condition:
            out << "?";
            break;
        case Port::PortType::True:
            out << "+";
            break;
        case Port::PortType::False:
            out << "-";
            break;
        default:
            break;
    }
    if (p.width > -1) out << ":" << p.width;
    return out;
}


} // Close namespace
