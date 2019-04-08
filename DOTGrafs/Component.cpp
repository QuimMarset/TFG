
#include "Component.h"


string Component::Port::getName() {
    return name;
}
int Component::Port::getDelay() {
    return delay;
}
Component::Port::PortType Component::Port::getType() {
    return type;
}
void Component::Port::setName(string name) {
    this->name = name;
}
void Component::Port::setDelay(int delay) {
    this->delay = delay;
}
void Component::Port::setType(Port::PortType type) {
    this->type = type;
}


Component::Component() : blockType("type"), inputPorts("in"), outputPorts("out")  {
    blockName = "";
}

Component::Component(const string &blockName, BlockType type) 
                    : blockType("type", type), inputPorts("in"), outputPorts("out") {
    this->blockName = blockName;
}

Component::~Component() {
    blockName = "";
    inputPorts.clearValues();
    outputPorts.clearValues();
}

string Component::getBlockName() {
    return blockName;
}

void Component::setBlockName(const string &blockName) {
    this->blockName = blockName;
}

Component::BlockType Component::getBlockType() {
    return blockType.getValue();
}

void Component::setBlockType(BlockType blockType) {
    this->blockType.setValue(blockType);
}

void Component::addInputPort(const Port &inPort) {
    inputPorts.addValue(inPort);
}

void Component::addOutputPort(const Port &outPort) {
    outputPorts.addValue(outPort);
}


void Component::printBlock(ofstream &file) {
    file << blockName + "[shape=oval, label=\"" + blockName + "\", ";
    blockType.printAttribute(file);
    file << ", ";
    inputPorts.printAttribute(file);
    file << ", ";
    outputPorts.printAttribute(file);
}

void Component::closeBlock(ofstream &file) {
    file << "];" << endl;
}

ofstream &operator << (ofstream &out, const Component::Port &p) {
    out << p.name;
    switch (p.type)
    {
        case Component::Port::PortType::Condition:
            out << "?";
            break;
        case Component::Port::PortType::True:
            out << "+";
            break;
        case Component::Port::PortType::False:
            out << "-";
            break;
        default:
            break;
    }
    if (p.delay > 0) {
        out << ":" << p.delay;
    }
    return out;
}

ofstream &operator << (ofstream &out, Component::BlockType blockType) {
    switch (blockType)
    {
        case Component::BlockType::Operator:
            out << "Operator";
            break;
        case Component::BlockType::Buffer:
            out << "Buffer";
            break;
        case Component::BlockType::Constant:
            out << "Constant";
            break;
        case Component::BlockType::Fork:
            out << "Fork";
            break;
        case Component::BlockType::Merge:
            out << "Merge";
            break;
        case Component::BlockType::Select:
            out << "Select";
            break;
        case Component::BlockType::Branch:
            out << "Branch";
            break;
        case Component::BlockType::Demux:
            out << "Demux";
            break;
        case Component::BlockType::Entry:
            out << "Entry";
            break;
        case Component::BlockType::Exit:
            out << "Exit";
            break;
        default:
            break;
    }
    return out;
}

bool operator == (const Component::Port &p1, const Component::Port &p2) {
    return (p1.name == p2.name);
}


Operator::Operator() : Component() {}

Operator::Operator(const string &name, int numInPorts, int latency, int II) : 
                Component(name, BlockType::Operator), 
                latency("slots"), II("transparent") {
    for ( int i = 1; i <= numInPorts; ++i) {
        Port in_i = {"in" + to_string(i), 0, Port::PortType::Base};
        addInputPort(in_i);
    }
    Port outPort = {"out", 0, Port::PortType::Base};
    addOutputPort(outPort);
    this->latency.setValue(latency);
    this->II.setValue(II);
}

Operator::~Operator() {}

void Operator::printBlock(ofstream &file) {
    Component::printBlock(file);
    file << ", ";
    latency.printAttribute(file);
    file << ", ";
    II.printAttribute(file);
}


Buffer::Buffer() : Component(), slots("slots"), transparent("transparent") {}

Buffer::Buffer(const string &name, int slots, bool transparent) : 
                Component(name, BlockType::Buffer), slots("slots"), 
                transparent("transparent") {
    Port inPort = {"in", 0, Port::PortType::Base};
    Port outPort = {"out", 0, Port::PortType::Base};
    addInputPort(inPort);
    addOutputPort(outPort);
    this->slots.setValue(slots);
    this->transparent.setValue(transparent);
}

Buffer::~Buffer() {}

void Buffer::printBlock(ofstream &file) {
    Component::printBlock(file);
    file << ", ";
    slots.printAttribute(file);
    file << ", ";
    transparent.printAttribute(file);
}


Fork::Fork() : Component() {}

Fork::Fork(const string &name, int numOutPorts) : Component(name, BlockType::Fork) {
    Port inPort = {"in", 0, Port::PortType::Base};
    addInputPort(inPort);
    for ( int i = 1;i <= numOutPorts; ++i) {
        Port out_i = {"out" + to_string(i), 0, Port::PortType::Base};
        addOutputPort(out_i);
    }
}

Fork::~Fork() {}


Merge::Merge() : Component() {}

Merge::Merge(const string &name, int numInPorts) : Component(name, BlockType::Merge) {
    Port outPort = {"out", 0, Port::PortType::Base};
    addOutputPort(outPort);
    for ( int i = 1;i <= numInPorts; ++i) {
        Port in_i = {"in" + to_string(i), 0, Port::PortType::Base};
        addInputPort(in_i);
    }
}

Merge::~Merge() {}


Select::Select() : Component() {}

Select::Select(const string &name) : Component(name, BlockType::Select) {
    Port inTrue = {"inTrue", 0, Port::PortType::True};
    Port inFalse = {"inFalse", 0, Port::PortType::False};
    Port inCondition = {"inCondition", 0, Port::PortType::Condition};
    Port out = {"out", 0, Port::PortType::Base};
    addInputPort(inTrue);
    addInputPort(inFalse);
    addInputPort(inCondition);
    addInputPort(out);
}

Select::~Select() {}


Branch::Branch() : Component() {}

Branch::Branch(const string &name) : Component(name, BlockType::Branch) {
    Port in = {"in", 0, Port::PortType::Base};
    Port inCondition = {"inCondition", 0, Port::PortType::Condition};
    Port outTrue = {"outTrue", 0, Port::PortType::True};
    Port outFalse = {"outFalse", 0, Port::PortType::False};
    addInputPort(in);
    addInputPort(inCondition);
    addInputPort(outTrue);
    addInputPort(outFalse);
}

Branch::~Branch() {}


Demux::Demux() : Component() {}

Demux::Demux(const string &name, int numControlPorts) : Component(name, BlockType::Demux) {
    Port dataIn = {"data", 0, Port::PortType::Base};
    for ( int i = 1; i <= numControlPorts; ++i) {
        Port control_i = {"control" + to_string(i), 0, Port::PortType::Base};
        Port dataOut_i = {"data" + to_string(i), 0, Port::PortType::Base};
        addInputPort(control_i);
        addOutputPort(control_i);
    }
    addInputPort(dataIn);
}

Demux::~Demux() {}


Entry::Entry() : Component() {}

Entry::Entry(const string &name) : Component(name, BlockType::Entry) {}

Entry::~Entry() {}


Exit::Exit() : Component() {}

Exit::Exit(const string &name) : Component(name, BlockType::Exit) {}

Exit::~Exit() {}