
#include "SupportTypes.h"


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

int numberOperators = 17;

ostream &operator << (ostream& out, OperatorType op) {
    switch (op)
    {
        case Add:
            out << "add";
            break;
        case Sub:
            out << "sub";
            break;
        case Mul:
            out << "mul";
            break;
        case Div:
            out << "div";
            break;
        case Rem:
            out << "rem";
            break;
        case ShiftL:
            out << "shiftl";
            break;
        case ShiftR:
            out << "shiftr";
            break;
        case And:
            out << "and";
            break;
        case Or:
            out << "or";
            break;
        case Not:
            out << "not";
            break;
        case Xor:
            out << "xor";
            break;
        case Eq:
            out << "eq";
            break;
        case NEq:
            out << "neq";
            break;
        case GT:
            out << "gt";
            break;
        case LT:
            out << "lt";
            break;
        case GET:
            out << "geq";
            break;
        case LET:
            out << "leq";
            break;
        default:
            break;
    }
    return out;
}


string getOperatorName(OperatorType op) {
    switch (op)
    {
        case Add:
            return "Add";
            break;
        case Sub:
            return "Sub";
            break;
        case Mul:
            return "Mul";
            break;
        case Div:
            return "Div";
            break;
        case Rem:
            return "Rem";
            break;
        case ShiftL:
            return "ShiftL";
            break;
        case ShiftR:
            return "ShiftR";
            break;
        case And:
            return "And";
            break;
        case Or:
            return "Or";
            break;
        case Not:
            return "Not";
            break;
        case Xor:
            return "Xor";
            break;
        case Eq:
            return "Eq";
            break;
        case NEq:
            return "NEq";
            break;
        case GT:
            return "GT";
            break;
        case LT:
            return "LT";
            break;
        case GET:
            return "GET";
            break;
        case LET:
            return "LET";
            break;
        default:
            break;
    }
    return "";
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

bool operator == (const Port &p1, const Port &p2) {
    return (p1.name == p2.name);
}