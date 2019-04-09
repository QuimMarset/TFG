
#include "SupportTypes.h"


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


Port::Port() {
    name = "";
    delay = 0;
    type = Base;
}

Port::Port(const string &name, Port::PortType type, int delay) {
    this->name = name;
    this->delay = delay;
    this->type = type;
}

Port::~Port() {
    name = "";
    delay = 0;
    type = Base;
}

string Port::getName() {
    return name;
}
int Port::getDelay() {
    return delay;
}
Port::PortType Port::getType() {
    return type;
}
void Port::setName(string name) {
    this->name = name;
}
void Port::setDelay(int delay) {
    this->delay = delay;
}
void Port::setType(Port::PortType type) {
    this->type = type;
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
    out << ":" << p.delay;
    return out;
}

bool operator == (const Port &p1, const Port &p2) {
    return (p1.name == p2.name);
}