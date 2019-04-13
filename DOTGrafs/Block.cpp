
#include "Block.h"
#include <iostream>


/*
 * =================================
 *  Class Block (base class)
 * =================================
*/

// Public function

Block::Block(const string &blockName, BlockType blockType, int defaultPortWidth,
        int blockDelay) {
    assert(blockDelay >= 0);
    assert(defaultPortWidth >= -1);
    this->blockName = blockName;
    this->blockType = blockType;
    this->defaultPortWidth = defaultPortWidth;
    this->blockDelay = blockDelay;
}

Block::~Block() {
    blockName = "";
    defaultPortWidth = 0;
    blockDelay = 0;
    inputPorts.clear();
    outputPorts.clear();
}

string Block::getBlockName() {
    return blockName;
}

BlockType Block::getBlockType() {
    return blockType;
}

int Block::getDefaultPortWidth() {
    return defaultPortWidth;
}

int Block::getBlockDelay() {
    return blockDelay;
}

void Block::setBlockName(const string &blockName) {
    this->blockName = blockName;
}

void Block::setBlockType(BlockType blockType) {
    this->blockType = blockType;
}

void Block::setDefaultPortWidth(int defaultPortWidth) {
    assert(defaultPortWidth >= 0);
    this->defaultPortWidth = defaultPortWidth;
    setDataPortsWidth(defaultPortWidth);
}

void Block::setBlockDelay(int blockDelay) {
    assert(blockDelay >= 0);
    this->blockDelay = blockDelay;
}

int Block::getDataPortsWidth() {
    return inputPorts[0].getWidth();
}

void Block::setDataPortsWidth(int width) {
    for (Port& port : inputPorts) {
        port.setWidth(width);
    }
    for (Port& port : outputPorts) {
        port.setWidth(width);
    }
}

void Block::printBlock(ostream &file) const {
    file << blockName << "[type = " << blockType;
    if (defaultPortWidth >= 0) {
        file << ", channel_width = " << defaultPortWidth;
    }
    for (unsigned int i = 0; i < inputPorts.size(); ++i) {
        if (i == 0) file << ", in = \"";
        else file << " ";
        file << inputPorts[i];
        if (i == inputPorts.size()-1) file << "\"";
    }
    for (unsigned int i = 0; i < outputPorts.size(); ++i) {
        if (i == 0) file << ", out = \"";
        else file << " ";
        file << outputPorts[i];
        if (i == outputPorts.size()-1) file << "\"";
    }
    bool first = true;
    bool printIn = false;
    bool printOut = false;
    for (unsigned int i = 0; i < inputPorts.size(); ++i) {
        if (inputPorts[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            if (printIn) file << " ";
            else printIn = true;
            file << inputPorts[i].getName() << ":" << inputPorts[i].getDelay();
        }
    }
    for (unsigned int i = 0; i < outputPorts.size(); ++i) {
        if (outputPorts[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            if (blockDelay > 0) {
                if (printIn) file << " ";
                file << blockDelay << " ";
            }
            if (printOut) file << " ";
            else printOut = true;
            file << outputPorts[i].getName() << ":" << outputPorts[i].getDelay();
        }
    }
    if (printIn or printOut) {
        file << "\"";
    }
    else if (!printIn and !printOut and blockDelay > 0) {
        file << ", delay = " << blockDelay;
    }
}

void Block::closeBlock(ostream &file) const {
    file << "];" << endl;
}

ostream &operator << (ostream& out, const Block &block) {
    block.printBlock(out);
    block.closeBlock(out);
    return out;
}

// Protected functions

void Block::addInputPort(const Port &inPort) {
    inputPorts.push_back(inPort);
}

void Block::addOutputPort(const Port &outPort) {
    outputPorts.push_back(outPort);
}

int Block::getNumInPorts() { 
    return outputPorts.size();
}

int Block::getNumOutPorts() {
    return inputPorts.size();
}

int Block::getInDataPortWidth(int index) {
    return inputPorts[index].getWidth();
}

int Block::getOutDataPortWidth(int index) {
    return outputPorts[index].getWidth();
}

void Block::setInDataPortWidth(int index, int width) {
    inputPorts[index].setWidth(width);
}

void Block::setOutDataPortWidth(int index, int width) {
    outputPorts[index].setWidth(width);
}

int Block::getInPortDelay(int index) {
    return inputPorts[index].getDelay();
}

int Block::getOutPortDelay(int index) {
    return outputPorts[index].getDelay();
}

void Block::setInPortDelay(int index, int delay) {
    inputPorts[index].setDelay(delay);
}

void Block::setOutPortDelay(int index, int delay) {
    outputPorts[index].setDelay(delay);
}


/*
 * =================================
 *  Class Operator
 * =================================
*/

vector<int> Operator::instanceCounter(numberOperators, 1);

Operator::Operator(OperatorType opType, int defaultPortWidth, int blockDelay, 
        int latency, int II) : 
        Block(getOperatorName(opType) + to_string(instanceCounter[opType]), 
        BlockType::Operator_Block, defaultPortWidth, blockDelay) {
    assert(latency >= 0);
    assert(II >= 0);
    ++instanceCounter[opType];
    this->opType = opType;
    this->latency = latency;
    this->II = II;
    Block::addInputPort(Port("in1"));
    if (opType != OperatorType::Not) Block::addInputPort(Port("in2"));
    Block::addOutputPort(Port("out"));
}

Operator::~Operator() {}

int Operator::getLatency() {
    return latency;
}

int Operator::getII() {
    return II;
}

OperatorType Operator::getOpType() {
    return opType;
}

void Operator::setLatency(int latency) {
    assert(latency >= 0);
    this->latency = latency;
}

void Operator::setII(int II) {
    assert(II >= 0);
    this->II = II;
}

void Operator::setOpType(OperatorType opType) {
    this->opType = opType;
}

int Operator::getInDataPortDelay(int index) {
    assert(index >= 0 and index < 2);
    return Block::getInPortDelay(index);
}

int Operator::getOutDataPortDelay() {
    return Block::getOutPortDelay(0);
}

void Operator::setInDataPortDelay(int index, int delay) {
    assert(index >= 0 and index < 2);
    Block::setInPortDelay(index, delay);
}

void Operator::setOutDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Operator::printBlock(ostream &file) const {
    Block::printBlock(file);
    file << ", op = " << opType;
    file << ", latency = " << latency;
    file << ", II = " << II;
}

void Operator::resetCounter() {
    for (unsigned int i = 0; i < instanceCounter.size(); ++i) {
        instanceCounter[i] = 1;
    }
}


/*
 * =================================
 *  Class Buffer
 * =================================
*/


int Buffer::instanceCounter = 1;

Buffer::Buffer(int defaultPortWidth, int blockDelay, int slots, bool transparent) : 
        Block("Buffer" + to_string(instanceCounter), BlockType::Buffer_Block, 
        defaultPortWidth, blockDelay) {
    assert(slots > 0);
    ++instanceCounter;
    this->slots = slots;
    this->transparent = transparent;
    Block::addInputPort(Port("in"));
    Block::addOutputPort(Port("out"));
}

Buffer::~Buffer() {}

int Buffer::getNumSlots() {
    return slots;
}

bool Buffer::getTransparent() {
    return transparent;
}

void Buffer::setNumSlots(int slots) {
    assert(slots > 0);
    this->slots = slots;
}

void Buffer::setTransparent(bool transparent) {
    this->transparent = transparent;
}

int Buffer::getInDataPortDelay() {
    return Block::getInPortDelay(0);
}

int Buffer::getOutDataPortDelay() {
    return Block::getOutPortDelay(0);
}

void Buffer::setInDataPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Buffer::setOutDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Buffer::printBlock(ostream &file) const {
    Block::printBlock(file);
    file << ", ";
    file << "slots = " << slots << ", ";
    file << "transparent = ";
    if (transparent) file << "true";
    else file << "false";
}

void Buffer::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Fork
 * =================================
*/


int Fork::instanceCounter = 1;

Fork::Fork(int defaultPortWidth, int blockDelay, int numOutPorts) : 
        Block("Fork" + to_string(instanceCounter), BlockType::Fork_Block,
        defaultPortWidth, blockDelay) {
    assert(numOutPorts > 0);
    ++instanceCounter;
    Block::addInputPort(Port("in"));
    for ( int i = 1;i <= numOutPorts; ++i) {
        Block::addOutputPort(Port("out" + to_string(i)));
    }
}

Fork::~Fork() {}

int Fork::getInDataPortDelay() {
    return Block::getInPortDelay(0);
}

int Fork::getOutDataPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutPortDelay(index);
}

void Fork::setInDataPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Fork::setOutDataPortDelay(int index, int delay) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    Block::setOutPortDelay(index, delay);
}

void Fork::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Merge
 * =================================
*/


int Merge::instanceCounter = 1;

Merge::Merge(int defaultPortWidth, int blockDelay, int numInPorts) : 
        Block("Merge" + to_string(instanceCounter), BlockType::Merge_Block,
        defaultPortWidth, blockDelay) {
    assert(numInPorts > 0);
    ++instanceCounter;
    Block::addOutputPort(Port("out"));
    for ( int i = 1;i <= numInPorts; ++i) {
        Block::addInputPort(Port("in" + to_string(i)));
    }
}

Merge::~Merge() {}

int Merge::getInDataPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumInPorts());
    return Block::getInPortDelay(index);
}

int Merge::getOutDataPortDelay() {
    return Block::getOutPortDelay(0);
}

void Merge::setInDataPortDelay(int index, int delay) {
    assert(index >= 0 and index < Block::getNumInPorts());
    Block::setInPortDelay(index, delay);
}

void Merge::setOutDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Merge::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Select
 * =================================
*/


int Select::instanceCounter = 1;

Select::Select(int defaultPortWidth, int blockDelay) : 
        Block("Select" + to_string(instanceCounter), BlockType::Select_Block,
        defaultPortWidth, blockDelay) {
    ++instanceCounter;
    Block::addInputPort(Port("inTrue", Port::PortType::True));
    Block::addInputPort(Port("inFalse", Port::PortType::False));
    Block::addInputPort(Port("inConditions", Port::PortType::Condition, 1));
    Block::addOutputPort(Port("out"));
}

Select::~Select() {}

void Select::setDataPortsWidth(int width) {
    Block::setInDataPortWidth(0, width);
    Block::setInDataPortWidth(1, width);
    Block::setOutDataPortWidth(0, width);
}

int Select::getDataTruePortDelay() {
    return Block::getInPortDelay(0);
}

int Select::getDataFalsePortDelay() { 
    return Block::getInPortDelay(1);
}

int Select::getConditionPortDelay() {
    return Block::getInPortDelay(2);
}

int Select::getDataOutPortDelay() {
    return Block::getOutPortDelay(0);
}

void Select::setDataTruePortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Select::setDataFalsePortDelay(int delay) {
    Block::setInPortDelay(1, delay);
}

void Select::setConditionPortDelay(int delay) {
    Block::setInPortDelay(2, delay);
}

void Select::setDataOutPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Select::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Branch
 * =================================
*/


int Branch::instanceCounter = 1;

Branch::Branch(int defaultPortWidth, int blockDelay) : 
        Block("Branch" + to_string(instanceCounter), BlockType::Branch_Block, 
        defaultPortWidth, blockDelay) {
    ++instanceCounter;
    Block::addInputPort(Port("in"));
    Block::addInputPort(Port("inCondition", Port::PortType::Condition, 1));
    Block::addOutputPort(Port("outTrue", Port::PortType::True));
    Block::addOutputPort(Port("outFalse", Port::PortType::False));
}

Branch::~Branch() {}

void Branch::setDataPortsWidth(int width) {
    Block::setInDataPortWidth(0, width);
    Block::setOutDataPortWidth(0, width);
    Block::setOutDataPortWidth(1, width);
}

int Branch::getDataInPortDelay() {
    return Block::getInPortDelay(0);
}

int Branch::getConditionPortDelay() {
    return Block::getInPortDelay(1);
}

int Branch::getDataTruePortDelay() {
    return Block::getOutPortDelay(0);
}

int Branch::getDataFalsePortDelay() {
    return Block::getOutPortDelay(1);
}

void Branch::setDataInPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Branch::setConditionPortDelay(int delay) {
    Block::setInPortDelay(1, delay);
}
void Branch::setDataTruePortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Branch::setDataFalsePortDelay(int delay) {
    Block::setOutPortDelay(1, delay);
}

void Branch::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Demux
 * =================================
*/


int Demux::instanceCounter = 1;

Demux::Demux(int defaultPortWidth, int blockDelay, int numControlPorts) : 
        Block("Demux" + to_string(instanceCounter), BlockType::Demux_Block, 
        defaultPortWidth, blockDelay) {
    ++instanceCounter;
    for (int i = 1; i <= numControlPorts; ++i) {
        Block::addInputPort(Port("control" + to_string(i)));
        Block::addOutputPort(Port("data" + to_string(i)));
    }
    Block::addInputPort(Port("data"));
}

Demux::~Demux() {}

int Demux::getDataPortsWidth() {
    unsigned int numIn = Block::getNumInPorts();
    return Block::getInDataPortWidth(numIn-1);
}

void Demux::setDataPortsWidth(int width) {
    unsigned int numIn = Block::getNumInPorts();
    unsigned int numOut = Block::getNumOutPorts();
    Block::setInDataPortWidth(numIn-1, width);
    for (unsigned int i = 0;i < numOut; ++i) {
        Block::setOutDataPortWidth(i, width);
    }
}

int Demux::getControlPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumInPorts()-1);
    return Block::getInPortDelay(index);
}

int Demux::getDataInPortDelay() {
    return Block::getInPortDelay(Block::getNumInPorts()-1);
}

int Demux::getDataOutPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutPortDelay(index);
}

void Demux::setControlPortDelay(int index, int delay) {
    assert(index >= 0 and index < Block::getNumInPorts()-1);
    Block::setInPortDelay(index, delay);
}

void Demux::setDataInPortDelay(int delay) {
    Block::setInPortDelay(Block::getNumInPorts()-1, delay);
}

void Demux::setDataOutPortDelay(int index, int delay) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    Block::setInPortDelay(index, delay);
}

void Demux::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Entry
 * =================================
*/


int Entry::instanceCounter = 1;

Entry::Entry(int defaultPortWidth, int blockDelay) : 
        Block("Entry" + to_string(instanceCounter), 
        BlockType::Entry_Block, defaultPortWidth, blockDelay) {
    ++instanceCounter;
    Block::addOutputPort(Port("controlOut"));
}

Entry::~Entry() {}

void Entry::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Exit
 * =================================
*/


int Exit::instanceCounter = 1;

Exit::Exit(int defaultPortWdith, int blockDelay) : Block("Exit" + to_string(instanceCounter), 
        BlockType::Exit_Block, defaultPortWdith, blockDelay) {
    ++instanceCounter;
    Block::addInputPort(Port("controlIn"));
}

Exit::~Exit() {}

void Exit::resetCounter() {
    instanceCounter = 1;
}