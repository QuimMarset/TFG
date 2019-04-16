
#include "Block.h"
#include <iostream>


/*
 * =================================
 *  Class Block
 * =================================
*/

// Public functions

Block::Block(const string &blockName, BlockType blockType, int defaultPortWidth,
        int blockDelay) {
    assert(defaultPortWidth >= -1 and blockDelay >= 0);
    this->blockName = blockName;
    this->blockType = blockType;
    this->defaultPortWidth = defaultPortWidth;
    this->blockDelay = blockDelay;
}

Block::~Block() {}

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
}

void Block::setBlockDelay(int blockDelay) {
    assert(blockDelay >= 0);
    this->blockDelay = blockDelay;
}

void Block::printBlock(ostream &file) {
    unsigned int counterPortsNoWidth = getNumPortNoWidth();
    assert(counterPortsNoWidth == inputPorts.size() + outputPorts.size() or 
        counterPortsNoWidth == 0 or defaultPortWidth >= 0);
        
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

void Block::closeBlock(ostream &file) {
    file << "];" << endl;
}

// ostream &operator << (ostream& out, const Block &block) {
//     block.printBlock(out);
//     block.closeBlock(out);
//     return out;
// }

// Protected functions

string Block::getInPortName(int index) {
    return inputPorts[index].getName();
}

string Block::getOutPortName(int index) {
    return outputPorts[index].getName();
}

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

// Private Functons

unsigned int Block::getNumPortNoWidth() {
    unsigned int counter = 0;
    for (unsigned int i = 0; i < inputPorts.size(); ++i) {
        counter += (inputPorts[i].getWidth() == -1);
    }
    for (unsigned int i = 0; i < outputPorts.size(); ++i) {
        counter += (outputPorts[i].getWidth() == -1);
    }
    return counter;
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
    assert(latency >= 0 and II >= 0);
    ++instanceCounter[opType];
    this->opType = opType;
    this->latency = latency;
    this->II = II;
    if (opType == OperatorType::Not) {
        Block::addInputPort(Port("in"));
    }
    else {
        Block::addInputPort(Port("in1"));
        Block::addInputPort(Port("in2"));
    }
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

string Operator::getInDataPortName(int index) {
    assert(index >= 0 and index < Block::getNumInPorts());
    return Block::getInPortName(index);
}

string Operator::getOutDataPortName() {
    return Block::getOutPortName(0);
}

int Operator::getInDataPortWidth(int index) {
    assert(index >= 0 and index < Block::getNumInPorts());
    return Block::getInDataPortWidth(index);
}

int Operator::getOutDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

void Operator::setInDataPortWidth(int index, int width) {
    assert(index >= 0 and index < Block::getNumInPorts());
    Block::setInDataPortWidth(index, width);
}

void Operator::setOutDataPortWidth(int width) {
    Block::setOutDataPortWidth(0, width);
}

int Operator::getInDataPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumInPorts());
    return Block::getInPortDelay(index);
}

int Operator::getOutDataPortDelay() {
    return Block::getOutPortDelay(0);
}

void Operator::setInDataPortDelay(int index, int delay) {
    assert(index >= 0 and index < Block::getNumInPorts());
    Block::setInPortDelay(index, delay);
}

void Operator::setOutDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Operator::printBlock(ostream &file) {
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

Buffer::Buffer(int slots, bool transparent, int defaultPortWidth, int blockDelay) : 
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

string Buffer::getInDataPortName() {
    return Block::getInPortName(0);
}

string Buffer::getOutDataPortName() {
    return Block::getOutPortName(0);
}

int Buffer::getInDataPortWidth() {
    return Block::getInDataPortWidth(0);
}

int Buffer::getOutDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

void Buffer::setInDataPortWidth(int width) {
    Block::setInDataPortWidth(0, width);
}

void Buffer::setOutDataPortWidth(int width) {
    Block::setOutDataPortWidth(0, width);
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

void Buffer::printBlock(ostream &file) {
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

Fork::Fork(int numOutPorts, int defaultPortWidth, int blockDelay) : 
        Block("Fork" + to_string(instanceCounter), BlockType::Fork_Block,
        defaultPortWidth, blockDelay) {
    assert(numOutPorts > 0);
    ++instanceCounter;
    Block::addInputPort(Port("in"));
    for (int i = 1; i <= numOutPorts; ++i) {
        Block::addOutputPort(Port("out" + to_string(i)));
    }
}

Fork::~Fork() {}

string Fork::getInDataPortName() {
    return Block::getInPortName(0);
}
string Fork::getOutDataPortName(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutPortName(index);
}

int Fork::getInDataPortWidth() {
    return Block::getInDataPortWidth(0);
}

int Fork::getOutDataPortWidth(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutDataPortWidth(index);
}

void Fork::setInDataPortWidth(int width) {
    Block::setInDataPortWidth(0, width);
}

void Fork::setOutDataPortWidth(int index, int width) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    Block::setOutDataPortWidth(index, width);
}

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

Merge::Merge(int numInPorts, int defaultPortWidth, int blockDelay) : 
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

string Merge::getInDataPortName(int index) {
    assert(index >= 0 and index < Block::getNumInPorts());
    return Block::getInPortName(index);
}

string Merge::getOutDataPortName() {
    return Block::getOutPortName(0);
}

int Merge::getInDataPortWidth(int index) {
    assert(index >= 0 and index < Block::getNumInPorts());
    return Block::getInDataPortWidth(index);
}

int Merge::getOutDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

void Merge::setInDataPortWidth(int index, int width) {
    assert(index >= 0 and index < Block::getNumInPorts());
    Block::setInDataPortWidth(index, width);
}

void Merge::setOutDataPortWidth(int width) {
    Block::setOutDataPortWidth(0, width);
}

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
    Block::addInputPort(Port("condition", Port::PortType::Condition, 1));
    Block::addOutputPort(Port("out"));
}

Select::~Select() {}

string Select::getTrueDataPortName() {
    return Block::getInPortName(0);
}

string Select::getFalseDataPortName() {
    return Block::getInPortName(1);
}

string Select::getConditionPortName() {
    return Block::getInPortName(2);
}

string Select::getOutDataPortName() {
    return Block::getOutPortName(0);
}

int Select::getTrueDataPortWidth() {
    return Block::getInDataPortWidth(0);
}

int Select::getFalseDataPortWidth() {
    return Block::getInDataPortWidth(1);
}

int Select::getOutDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

void Select::setTrueDataPortWidth(int width) { 
    Block::setInDataPortWidth(0, width);
}

void Select::setFalseDataPortWidth(int width) {
    Block::setInDataPortWidth(1, width);
}

void Select::setOutDataPortWidth(int width) { 
    Block::setOutDataPortWidth(0, width);
}

int Select::getTrueDataPortDelay() {
    return Block::getInPortDelay(0);
}

int Select::getFalseDataPortDelay() { 
    return Block::getInPortDelay(1);
}

int Select::getConditionPortDelay() {
    return Block::getInPortDelay(2);
}

int Select::getOutDataPortDelay() {
    return Block::getOutPortDelay(0);
}

void Select::setTrueDataPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Select::setFalseDataPortDelay(int delay) {
    Block::setInPortDelay(1, delay);
}

void Select::setConditionPortDelay(int delay) {
    Block::setInPortDelay(2, delay);
}

void Select::setOutDataPortDelay(int delay) {
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
    Block::addInputPort(Port("condition", Port::PortType::Condition, 1));
    Block::addOutputPort(Port("outTrue", Port::PortType::True));
    Block::addOutputPort(Port("outFalse", Port::PortType::False));
}

Branch::~Branch() {}

string Branch::getInDataPortName() {
    return Block::getInPortName(0);
}
string Branch::getConditionPortName() {
    return Block::getInPortName(1);
}

string Branch::getTrueDataPortName() {
    return Block::getOutPortName(0);
}

string Branch::getFalseDataPortName() {
    return Block::getOutPortName(1);
}

int Branch::getInDataPortWidth() {
    return Block::getInDataPortWidth(0);
}

int Branch::getTrueDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

int Branch::getFalseDataPortWidth() {
    return Block::getOutDataPortWidth(1);
}

void Branch::setInDataPortWidth(int width) {
    Block::setInDataPortWidth(0, width);
}

void Branch::setTrueDataPortWidth(int width) {
    Block::setOutDataPortWidth(0, width);
}

void Branch::setFalseDataPortWidth(int width) {
    Block::setOutDataPortWidth(1, width);
}

int Branch::getInDataPortDelay() {
    return Block::getInPortDelay(0);
}

int Branch::getConditionPortDelay() {
    return Block::getInPortDelay(1);
}

int Branch::getTrueDataPortDelay() {
    return Block::getOutPortDelay(0);
}

int Branch::getFalseDataPortDelay() {
    return Block::getOutPortDelay(1);
}

void Branch::setInDataPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Branch::setConditionPortDelay(int delay) {
    Block::setInPortDelay(1, delay);
}
void Branch::setTrueDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Branch::setFalseDataPortDelay(int delay) {
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

Demux::Demux(int numControlPorts, int defaultPortWidth, int blockDelay) : 
        Block("Demux" + to_string(instanceCounter), BlockType::Demux_Block, 
        defaultPortWidth, blockDelay) {
    ++instanceCounter;
    for (int i = 1; i <= numControlPorts; ++i) {
        Block::addInputPort(Port("control" + to_string(i)));
        Block::addOutputPort(Port("in" + to_string(i)));
    }
    Block::addInputPort(Port("out"));
}

Demux::~Demux() {}

string Demux::getInDataPortName() {
    return Block::getInPortName(Block::getNumInPorts()-1);
}

string Demux::getControlPortName(int index) {
    assert(index >= 0 and index < Block::getNumInPorts()-1);
    return Block::getInPortName(index);
}

string Demux::getOutDataPortName(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutPortName(index);
}

int Demux::getInDataPortWidth() {
    return Block::getInDataPortWidth(Block::getNumInPorts()-1);
}

int Demux::getOutDataPortWidth(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutDataPortWidth(index);
}

void Demux::setInDataPortWidth(int width) {
    Block::setInDataPortWidth(Block::getNumInPorts()-1, width);
}

void Demux::setOutDataPortWidth(int index, int width) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    Block::setOutDataPortWidth(index, width);
}

int Demux::getControlPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumInPorts()-1);
    return Block::getInPortDelay(index);
}

int Demux::getInDataPortDelay() {
    return Block::getInPortDelay(Block::getNumInPorts()-1);
}

int Demux::getOutDataPortDelay(int index) {
    assert(index >= 0 and index < Block::getNumOutPorts());
    return Block::getOutPortDelay(index);
}

void Demux::setControlPortDelay(int index, int delay) {
    assert(index >= 0 and index < Block::getNumInPorts()-1);
    Block::setInPortDelay(index, delay);
}

void Demux::setInDataPortDelay(int delay) {
    Block::setInPortDelay(Block::getNumInPorts()-1, delay);
}

void Demux::setOutDataPortDelay(int index, int delay) {
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
    Block::addOutputPort(Port("control", Port::PortType::Base, 0));
}

Entry::~Entry() {}

string Entry::getControlPortName() {
    return Block::getOutPortName(0);
}

int Entry::getControlPortDelay() {
    return Block::getOutPortDelay(0);
}

void Entry::setControlPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Entry::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Argument
 * =================================
*/


int Argument::instanceCounter = 1;

Argument::Argument(int defaultPortWdith, int blockDelay) : 
        Block("Arg" + to_string(instanceCounter), 
        BlockType::Entry_Block, defaultPortWdith, blockDelay) {
    ++instanceCounter;
    Block::addOutputPort(Port("out"));
}

Argument::~Argument() {}

string Argument::getDataPortName() {
    return Block::getOutPortName(0);
}

int Argument::getDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

void Argument::setDataPortWidth(int width) {
    Block::setOutDataPortWidth(0, width);
}

int Argument::getDataPortDelay() {
    return Block::getOutPortDelay(0);
}

void Argument::setDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

void Argument::resetCounter() {
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
    Block::addInputPort(Port("control", Port::PortType::Base, 0));
}

Exit::~Exit() {}

string Exit::getControlPortName() {
    return Block::getInPortName(0);
}

int Exit::getControlPortDelay() {
    return Block::getInPortDelay(0);
}

void Exit::setControlPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Exit::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Return
 * =================================
*/


int Return::instanceCounter = 1;

Return::Return(int defaultPortWdith, int blockDelay) : 
        Block("Ret" + to_string(instanceCounter), 
        BlockType::Exit_Block, defaultPortWdith, blockDelay) {
    ++instanceCounter;
    Block::addInputPort(Port("in"));
}

Return::~Return() {}

string Return::getDataPortName() {
    return Block::getInPortName(0);
}

int Return::getDataPortWidth() {
    return Block::getInDataPortWidth(0);
}

void Return::setDataPortWidth(int width) {
    Block::setInDataPortWidth(0, width);
}

int Return::getDataPortDelay() {
    return Block::getInPortDelay(0);
}

void Return::setDataPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

void Return::resetCounter() {
    instanceCounter = 1;
}