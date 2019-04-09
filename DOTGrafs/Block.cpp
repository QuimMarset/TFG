
#include "Block.h"


Block::Block() : blockType("type"), inputPorts("in"), outputPorts("out")  {
    blockName = "";
}

Block::Block(const string &blockName, BlockType type) 
                    : blockType("type", type), inputPorts("in"), outputPorts("out") {
    this->blockName = blockName;
}

Block::~Block() {
    blockName = "";
    inputPorts.clearValues();
    outputPorts.clearValues();
}

string Block::getBlockName() {
    return blockName;
}

void Block::setBlockName(const string &blockName) {
    this->blockName = blockName;
}

BlockType Block::getBlockType() {
    return blockType.getValue();
}

void Block::setBlockType(BlockType blockType) {
    this->blockType.setValue(blockType);
}

void Block::addInputPort(const Port &inPort) {
    inputPorts.addValue(inPort);
}

void Block::addOutputPort(const Port &outPort) {
    outputPorts.addValue(outPort);
}

void Block::printBlock(ofstream &file) {
    file << blockName + "[shape=oval, label=\"" + blockName + "\", ";
    blockType.printAttribute(file);
    file << ", ";
    inputPorts.printAttribute(file);
    file << ", ";
    outputPorts.printAttribute(file);
}

void Block::closeBlock(ofstream &file) {
    file << "];" << endl;
}


Operator::Operator() : Block(), latency("latency"), II("II") {}

Operator::Operator(const string &name, int numInPorts, int latency, int II) : 
                Block(name, BlockType::Operator_Block), 
                latency("latency", latency), II("II", II) {
    for (int i = 1; i <= numInPorts; ++i) {
        addInputPort(Port("in" + to_string(i)));
    }
    addOutputPort(Port("out"));
}

Operator::~Operator() {}

void Operator::printBlock(ofstream &file) {
    Block::printBlock(file);
    file << ", ";
    latency.printAttribute(file);
    file << ", ";
    II.printAttribute(file);
}


Buffer::Buffer() : Block(), slots("slots"), transparent("transparent") {}

Buffer::Buffer(const string &name, int slots, bool transparent) : 
                Block(name, BlockType::Buffer_Block), slots("slots", slots), 
                transparent("transparent", transparent) {
    addInputPort(Port("in"));
    addOutputPort(Port("out"));
}

Buffer::~Buffer() {}

void Buffer::printBlock(ofstream &file) {
    Block::printBlock(file);
    file << ", ";
    slots.printAttribute(file);
    file << ", ";
    transparent.printAttribute(file);
}


Fork::Fork() : Block() {}

Fork::Fork(const string &name, int numOutPorts) : Block(name, BlockType::Fork_Block) {
    addInputPort(Port("in"));
    for ( int i = 1;i <= numOutPorts; ++i) {
        addOutputPort(Port("out" + to_string(i)));
    }
}

Fork::~Fork() {}


Merge::Merge() : Block() {}

Merge::Merge(const string &name, int numInPorts) : Block(name, BlockType::Merge_Block) {
    addOutputPort(Port("out"));
    for ( int i = 1;i <= numInPorts; ++i) {
        addInputPort(Port("in" + to_string(i)));
    }
}

Merge::~Merge() {}


Select::Select() : Block() {}

Select::Select(const string &name) : Block(name, BlockType::Select_Block) {
    addInputPort(Port("inTrue", Port::PortType::True));
    addInputPort(Port("inFalse", Port::PortType::False));
    addInputPort(Port("inConditions", Port::PortType::Condition));
    addOutputPort(Port("out"));
}

Select::~Select() {}


Branch::Branch() : Block() {}

Branch::Branch(const string &name) : Block(name, BlockType::Branch_Block) {
    addInputPort(Port("in"));
    addInputPort(Port("inCondition", Port::PortType::Condition));
    addOutputPort(Port("outTrue", Port::PortType::True));
    addOutputPort(Port("outFalse", Port::PortType::False));
}

Branch::~Branch() {}


Demux::Demux() : Block() {}

Demux::Demux(const string &name, int numControlPorts) : Block(name, BlockType::Demux_Block) {
    for (int i = 1; i <= numControlPorts; ++i) {
        addInputPort(Port("control" + to_string(i)));
        addOutputPort(Port("data" + to_string(i)));
    }
    addInputPort(Port("data"));
}

Demux::~Demux() {}


Entry::Entry() : Block() {}

Entry::Entry(const string &name) : Block(name, BlockType::Entry_Block) {}

Entry::~Entry() {}


Exit::Exit() : Block() {}

Exit::Exit(const string &name) : Block(name, BlockType::Exit_Block) {}

Exit::~Exit() {}