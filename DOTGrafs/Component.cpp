
#include "Component.h"

Component::Component() : blockType("type"), inputPorts("in"), outputPorts("out")  {
    blockName = "";
}

Component::Component(string &name, const BlockType &type) 
                    : blockType("type"), inputPorts("in"), outputPorts("out") {

    this->blockName = name;
    blockType.setValue(type);
}

Component::Component(string &name, const BlockType &type, 
                    const vector<Port> &input, const vector <Port> &output) 
                    : blockType("type"), inputPorts("in"), outputPorts("out")  {
    this->blockName = name;
    blockType.setValue(type);
    for (const Port &p : input) {
        inputPorts.addValue(p);
    }
    for (const Port &p : output) {
        outputPorts.addValue(p);
    }
}

Component::~Component() {
    blockName = "";
    inputPorts.clearValues();
    outputPorts.clearValues();
}


string Component::getBlockName() {
    return blockName;
}

void Component::setBlockName(string &blockName) {
    this->blockName = blockName;
}

Component::BlockType Component::getBlockType() {
    return blockType.getValue();
}

void Component::setBlockType(Component::BlockType &blockType) {
    this->blockType.setValue(blockType);
}

void Component::addInputPort(Component::Port &inPort) {
    inputPorts.addValue(inPort);
}

void Component::addOutputPort(Component::Port &outPort) {
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

ofstream &operator << (ofstream &out, const Component::BlockType &blockType) {
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

bool operator == (Component::Port p1, Component::Port p2) {
    return (p1.name == p2.name);
}


Operator::Operator() : Component() {}

Operator::Operator(string &name, const vector<Port> &input, const vector <Port> &output, 
                int latency, int II) : Component(name, BlockType::Operator, input, output), 
                latency("slots"), II("transparent") {
    
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

Buffer::Buffer(string &name, const vector<Port> &input, const vector <Port> &output, 
                int _slots, bool _transparent) : Component(name, BlockType::Buffer, input, output), 
                slots("slots"), transparent("transparent") {
    
    slots.setValue(_slots);
    transparent.setValue(_transparent);
}

Buffer::~Buffer() {}

void Buffer::printBlock(ofstream &file) {
    Component::printBlock(file);
    file << ", ";
    slots.printAttribute(file);
    file << ", ";
    transparent.printAttribute(file);
}


Select::Select() : Component() {}

Select::Select(string &name) : Component(name, BlockType::Select) {

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

Branch::Branch(string &name) : Component(name, BlockType::Select) {

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


template <typename T>
Constant<T>::Constant() : Component(), constant("value") {}

template <typename T>
Constant<T>::Constant(string &name, int slots, bool transparent, T constant)
                        : Component(name, BlockType::Constant), constant("value") {
    Port out = {"out", 0, Port::PortType::Base};
    addOutputPort(out);
    this->constant = constant;
}

template <typename T>
Constant<T>::~Constant() {}

template <typename T>
T Constant<T>::getConstant() {
    return constant;
}

template <typename T>
void Constant<T>::setConstant(T constant) {
    this->constant = constant;
}

template <typename T>
void Constant<T>::printBlock(ofstream &file) {
    Component::printBlock(file);
    file << ", ";
    constant.printAttribute(file);
}