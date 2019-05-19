
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

int numberUnary = 13;
int numberBinary = 30;

string getUnaryOpName(UnaryOpType op) {
    switch (op)
    {
    case Not:
        return "Not";
        break;
    case Load:
        return "Load";
        break;
    case IntTrunc:
        return "IntTrunc";
        break;
    case FPointTrunc:
        return "FPointTrunc";
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
    case IntToPtr:
        return "IntToPtr";
        break;
    case PtrToInt:
        return "PtrToInt";
        break;
    case TypeCast:
        return "TypeCast";
        break;
    case AddrSpaceCast:
        return "AddrSpaceCast";
        break;
    default:
        break;
    }
    return "";
}

string getBinaryOpName(BinaryOpType op) {
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
        case Alloca: 
            return "Alloca";
            break;
        default:
            break;
    }
    return "";
}

ostream &operator << (ostream& out, UnaryOpType op) {
    switch (op)
    {
        case Not:
            out << "not";
            break;
        case Load:
            out << "load";
            break;
        case IntTrunc:
            out << "intTrunc";
            break;
        case FPointTrunc:
            out << "fPointTrunc";
            break;
        case IntZExt:
            out << "intZExt";
            break;
        case IntSExt:
            out << "intSExt";
            break;
        case FPointToUInt:
            out << "fPointToUInt";
            break;
        case FPointToSInt:
            out << "fPointToSInt";
            break;
        case UIntToFPoint:
            out << "uIntToFPoint";
            break;
        case SIntToFPoint:
            out << "sIntToFPoint";
            break;
        case IntToPtr:
            out << "intToPtr";
            break;
        case PtrToInt:
            out << "ptrToInt";
            break;
        case TypeCast:
            out << "typeCast";
            break;
        case AddrSpaceCast:
            out << "addrSpaceCast";
            break;
        default:
            break;
    }
    return out;
}

ostream &operator << (ostream& out, BinaryOpType op) {
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
        case Alloca:
            out << "alloca";
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


Port::Port() {
    name = "";
    type = Base;
    width = 0;
}

Port::Port(const string &name, Port::PortType type, int width, int delay) {
    this->name = name;
    this->type = type;
    this->width = width;
    this->delay = delay;
}

Port::Port(const Port &port) {
    name = port.name;
    type = port.type;
    width = port.width;
    delay = port.delay;
}

Port::~Port() {
    name = "";
    type = Base;
    width = 0;
    delay = 0;
}

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

void Port::setWidth(int width) {
    this->width = width;
}

void Port::setDelay(int delay) {
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
