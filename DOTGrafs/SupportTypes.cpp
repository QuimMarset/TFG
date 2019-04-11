
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
    out << ":" << p.width;
    return out;
}

bool operator == (const Port &p1, const Port &p2) {
    return (p1.name == p2.name);
}