
#include "Block.h"
#include <iostream>


namespace DFGraphComp
{


/*
 * =================================
 *  Class Block
 * =================================
*/

// Public functions

Block::Block(const string &blockName, BlockType blockType, int blockDelay) {
    assert(blockDelay >= 0);
    this->blockName = blockName;
    this->blockType = blockType;
    this->blockDelay = blockDelay;
}

Block::~Block() {}

string Block::getBlockName() {
    return blockName;
}

BlockType Block::getBlockType() {
    return blockType;
}

void Block::setBlockName(const string &blockName) {
    this->blockName = blockName;
}

void Block::setBlockType(BlockType blockType) {
    this->blockType = blockType;
}

void Block::setBlockDelay(int blockDelay) {
    assert(blockDelay >= 0);
    this->blockDelay = blockDelay;
}


/*
 * =================================
 *  Class Operator
 * =================================
*/


Operator::Operator(const string& blockName, int blockDelay, int latency, int II) : 
    Block(blockName, BlockType::Operator_Block, blockDelay), dataOut("out"), 
    connectedPort(nullptr, nullptr)
{
    assert(latency >= 0 and II >= 0);
    this->latency = latency;
    this->II = II;
}

Operator::~Operator() {}

void Operator::setLatency(int latency) {
    assert(latency >= 0);
    this->latency = latency;
}

void Operator::setII(int II) {
    assert(II >= 0);
    this->II = II;
}

void Operator::setDataOutPortWidth(int width) {
    dataOut.setWidth(width);
}

void Operator::setDataOutPortDelay(int delay) {
    dataOut.setDelay(delay);
}

pair <Block*, const Port*> Operator::getConnectedPort() {
    return connectedPort;
}

void Operator::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Operator::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}
    
// Class Unary Operator

vector<int> UnaryOperator::instanceCounter(numberUnary, 1);

UnaryOperator::UnaryOperator(UnaryOpType opType, int blockDelay = 0,
    int latency = 0, int II = 0) : Operator(getUnaryOpName(opType) + 
    to_string(instanceCounter[opType]), blockDelay, latency, II),
    dataIn("in")
{
    ++instanceCounter[opType];
}

UnaryOperator::~UnaryOperator() {}

void UnaryOperator::setOpType(UnaryOpType type) {
    opType = type;
}

void UnaryOperator::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void UnaryOperator::setDataPortWidth(int width) {
    assert(width >= 0);
    dataIn.setWidth(width);
    dataOut.setWidth(width);
}

void UnaryOperator::setDataInPortDelay(int delay) {
    dataIn.setDelay(delay);
}

void UnaryOperator::resetCounter() {
    for (unsigned int i = 0; i < instanceCounter.size(); ++i) {
        instanceCounter[i] = 1;
    }
}

const Port* UnaryOperator::getDataInPort() {
    return &dataIn;
}

void UnaryOperator::printBlock(ostream& file) {
    file << blockName << "[type = Operator";
    file << ", in = \"" << dataIn << "\"";
    file << ", out = \"" << dataOut << "\"";
    if (dataIn.getDelay() > 0) {
        file << ", delay = \"" << dataIn.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        if (dataOut.getDelay() > 0) file << " " << dataOut.getName() << 
            ":" << dataOut.getDelay();
        file << "\"";
    }
    else if (dataOut.getDelay > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay;
        file << " " << dataOut.getName() << 
            ":" <<dataOut.getDelay() << "\""; 
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << ", op = " << opType;
    file << ", latency = " << latency;
    file << ", II = " << II;
    file << "];" << endl;
}

// Class Binary Operator

vector<int> UnaryOperator::instanceCounter(numberBinary, 1);

BinaryOperator::BinaryOperator(BinaryOpType opType, int blockDelay = 0,
    int latency = 0, int II = 0) : Operator(getBinaryOpName(opType) + 
    to_string(instanceCounter[opType]), blockDelay, latency, II),
    dataIn1("in1"), dataIn2("in2")
{
    ++instanceCounter[opType];
}

BinaryOperator::~BinaryOperator() {}

void BinaryOperator::setOpType(BinaryOpType type) {
    opType = type;
}

void BinaryOperator::setDataIn1PortWidth(int width) {
    dataIn1.setWidth(width);
}

void BinaryOperator::setDataIn2PortWidth(int width) {
    dataIn2.setWidth(width);
}

void BinaryOperator::setDataPortWidth(int width) {
    assert(width >= 0);
    dataIn1.setWidth(width);
    dataIn2.setWidth(width);
    dataOut.setWidth(width);
}

void BinaryOperator::setDataIn1PortDelay(int delay) {
    dataIn1.setDelay(delay);
}

void BinaryOperator::setDataIn2PortDelay(int delay) {
    dataIn2.setDelay(delay);
}

void BinaryOperator::resetCounter() {
    for (unsigned int i = 0; i < instanceCounter.size(); ++i) {
        instanceCounter[i] = 1;
    }
}

const Port* BinaryOperator::getDataIn1Port() {
    return &dataIn1;
}

const Port* BinaryOperator::getDataIn2Port() {
    return &dataIn2;
}

void BinaryOperator::printBlock(ostream& file) {
    file << blockName << "[type = Operator";
    file << ", in = \"" << dataIn1 << " " << dataIn2 << "\"";
    file << ", out = \"" << dataOut << "\"";
    if (dataIn1.getDelay() > 0 or dataIn2.getDelay() > 0 or dataOut.getDelay() > 0) {
        file << ", delay = \"";
        bool first = true;
        if (dataIn1.getDelay() > 0) {
            file << dataIn1.getName() << ":" << dataIn1.getDelay();
            first = false;
        }
        if (dataIn2.getDelay() > 0) {
            if (!first) file << " ";
            else first = false;
            file << dataIn2.getName() << ":" << dataIn2.getDelay(); 
        }
        if (blockDelay > 0) {
            if (!first) file << " ";
            else first = false;
            file << blockDelay;
        } 
        if (dataOut.getDelay() > 0) {
            if (!first) file << " ";
            else first = false;
            file << dataOut.getName() << ":" << dataOut.getDelay(); 
        }
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << ", op = " << opType;
    file << ", latency = " << latency;
    file << ", II = " << II;
    file << "];" << endl;
}


/*
 * =================================
 *  Class Buffer
 * =================================
*/


int Buffer::instanceCounter = 1;

Buffer::Buffer(int slots, bool transparent, int blockDelay) : 
    Block("Buffer" + to_string(instanceCounter), BlockType::Buffer_Block, blockDelay),
    dataIn("in"), dataOut("out"), 
    connectedPort(nullptr, nullptr)
{
    assert(slots > 0);
    ++instanceCounter;
    this->slots = slots;
    this->transparent = transparent;
}

Buffer::~Buffer() {}

void Buffer::setNumSlots(int slots) {
    assert(slots > 0);
    this->slots = slots;
}

void Buffer::setTransparent(bool transparent) {
    this->transparent = transparent;
}

void Buffer::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void Buffer::setDataOutPortWidth(int width) {
    dataOut.setWidth(width);
}

void Buffer::setDataInPortDelay(int delay) {
    dataIn.setDelay(delay);
}

void Buffer::setDataInPortDelay(int delay) {
    dataOut.setDelay(delay);
}

void Buffer::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Buffer::getConnectedPort() {
    return connectedPort;
}

void Buffer::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Buffer::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

const Port* Buffer::getDataInPort() {
    return &dataIn;
}

void Buffer::printBlock(ostream &file) {
    file << blockName << "[type = Buffer";
    file << ", in = \"" << dataIn << "\"";
    file << ", out = \"" << dataOut << "\"";
    if (dataIn.getDelay() > 0) {
        file << ", delay = \"" << dataIn.getName() 
            << ":" << dataIn.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        if (dataOut.getDelay > 0) file << " " << dataOut.getName() << 
            ":" << dataOut.getDelay();
        file << "\"";
    }
    else if (dataOut.getDelay > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay;
        file << " " << dataOut.getName() << 
            ":" << dataOut.getDelay() << "\""; 
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << ", slots = " << slots;
    file << ", transparent = ";
    if (transparent) file << "true";
    else file << "false";
    file << "];" << endl;
}


/*
 * =================================
 *  Class Fork
 * =================================
*/


int Fork::instanceCounter = 1;

Fork::Fork(int numOutPorts, int blockDelay) : 
    Block("Fork" + to_string(instanceCounter), BlockType::Fork_Block,
    blockDelay), dataIn("in")
{
    assert(numOutPorts > 0);
    ++instanceCounter;
    for (int i = 1; i <= numOutPorts; ++i) {
        dataOut.push_back(Port("out" + to_string(i)));
        connectedPorts.push_back(make_pair(nullptr, nullptr));
    }
}

Fork::~Fork() {}

void Fork::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void Fork::setDataOutPortWidth(int index, int width) {
    assert(index >= 0 and index < dataOut.size());
    dataOut[index].setWidth(width);
}

void Fork::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        dataOut[i].setWidth(width);
    }
}

void Fork::setDataInPortDelay(int delay) {
    dataIn.setDelay(delay);
}

void Fork::setDataOutPortDelay(int index, int delay) {
    assert(index >= 0 and index < dataOut.size());
    dataOut[index].setDelay(delay);
}

void Fork::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Fork::getConnectedPort() {
    assert(0);
    return make_pair(nullptr, nullptr);
}

void Fork::setConnectedPort(pair <Block*, const Port*> connection) {
    addOutPort();
    connectedPorts.push_back(connection);
}

bool Fork::connectionAvailable() {
    return true;    
}

const Port* Fork::getDataInPort() {
    return &dataIn;
}

void Fork::printBlock(ostream& file ) {
    file << blockName << "[type = Fork";
    file << ", in = \"" << dataIn << "\"";
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (i == 0) file << ", out = \"";
        else file << " ";
        file << dataOut[i];
        if (i == dataOut.size()-1) file << "\"";
    }
    if (dataIn.getDelay() > 0) {
        file << ", delay = \"" << dataIn.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        for (unsigned int i = 0; i < dataOut.size(); ++i) {
            if (dataOut[i].getDelay > 0) {
                file << " " << dataOut[i].getName() << ":" << dataOut[i].getDelay();
            }
        }
        file << "\"";
    }
    else {
        bool first = true;
        for (unsigned int i = 0; i < dataOut.size(); ++i) {
            if (dataOut[i].getDelay() > 0) {
                if (first) {
                    first = false;
                    file << ", delay = \"";
                    if (blockDelay > 0) file << blockDelay << " ";
                    file << dataOut[i].getName() << ":" << dataOut[i].getDelay();
                }
                else { 
                    file << " " << dataOut[i].getName() << ":" << dataOut[i].getDelay();
                }
            }
        }
        if (!first) file << "\"";
        else if (blockDelay > 0) file << ", delay = " << blockDelay; 
    }
    file << "];" << endl;
}

void Fork::addOutPort() {
    int width = -1;
    int delay = 0;
    bool found = false;
    for (unsigned int i = 0; i < dataOut.size() and !found; ++i) {
        if (dataOut[i].getWidth() >= 0 and dataOut[i].getDelay() > 0) {
            width = dataOut[i].getWidth();
            delay = dataOut[i].getDelay();
            found = true;
        }
    }
    if (!found and dataIn.getWidth() >= 0 and dataIn.getDelay() > 0) {
        width = dataIn.getWidth();
        delay = dataIn.getDelay();
    }
    dataOut.push_back(Port("out" + to_string(dataOut.size()),
        Port::Base, width, delay));
}


/*
 * =================================
 *  Class Merge
 * =================================
*/


int Merge::instanceCounter = 1;

Merge::Merge(int numInPorts, int blockDelay) : 
    Block("Merge" + to_string(instanceCounter), BlockType::Merge_Block,
    blockDelay), dataOut("out"),
    connectedPort(nullptr, nullptr)
{
    assert(numInPorts > 0);
    ++instanceCounter;
    for (unsigned int i = 1;i <= numInPorts; ++i) {
        dataIn.push_back(Port("in" + to_string(i)));
    }
}

Merge::~Merge() {}

void Merge::setDataInPortWidth(int index, int width) {
    assert(index >= 0 and index < dataIn.size());
    dataIn[index].setWidth(width);
}

void Merge::setDataOutPortWidth(int width) {
    dataOut.setWidth(width);
}

void Merge::setDataPortWidth(int width) {
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        dataIn[i].setWidth(width);
    }
    dataOut.setWidth(width);
}

void Merge::setDataInPortDelay(int index, int delay) {
    assert(index >= 0 and index < dataIn.size());
    dataIn[index].setDelay(delay);
}

void Merge::setDataOutPortDelay(int delay) {
    dataOut.setDelay(delay);
}

void Merge::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Merge::getConnectedPort() {
    return connectedPort;
}

void Merge::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Merge::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

const Port* Merge::getDataInPort(int index) {
    assert(index >= 0 and index < dataIn.size());
    return &dataIn[index];
}

void Merge::printBlock(ostream& file) {
    file << blockName << "[type = Merge";
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        if (i == 0) file << ", in = \"";
        else file << " ";
        file << dataIn[i];
        if (i == dataIn.size()-1) file << "\"";
    }
    file << ", out = \"" << dataOut << "\"";
    bool first = true;
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        if (dataIn[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
                file << dataIn[i].getName() << ":" << dataIn[i].getDelay();
            }
            else {
                file << " " << dataIn[i].getName() << ":" << dataIn[i].getDelay();
            }
        }
    }
    if (first) {
        if (dataOut.getDelay() > 0) {
            file << ", delay = \"";
            if (blockDelay > 0) file << blockDelay << " ";
            file << dataOut.getName() << ":" << dataOut.getDelay() << "\"";
        }
        else if (blockDelay > 0) file << ", delay = " << blockDelay;
    }
    else {
        if (dataOut.getDelay() > 0) {
            if (blockDelay > 0) file << blockDelay << " ";
            file << dataOut.getName() << ":" << dataOut.getDelay();
        }
        else if (blockDelay > 0) file << blockDelay;
        file << "\"";
    }
    file << "];" << endl;
}


/*
 * =================================
 *  Class Select
 * =================================
*/


int Select::instanceCounter = 1;

Select::Select(int blockDelay) : Block("Select" + to_string(instanceCounter), 
    BlockType::Select_Block, blockDelay), dataTrue("inTrue", Port::True),
    dataFalse("inFalse", Port::False), condition("condition", Port::Condition, 1),
    dataOut("out"), connectedPort(nullptr, nullptr)
{
    ++instanceCounter;
}

Select::~Select() {}

void Select::setDataTruePortWidth(int width) { 
    dataTrue.setWidth(width);
}

void Select::setDataFalsePortWidth(int width) {
    dataFalse.setWidth(width);
}

void Select::setDataOutPortWidth(int width) { 
    dataOut.setWidth(width);
}

void Select::setDataPortWidth(int width) {
    dataTrue.setWidth(width);
    dataFalse.setWidth(width);
    dataOut.setWidth(width);
}

void Select::setDataTruePortDelay(int delay) {
    dataTrue.setDelay(delay);
}

void Select::setDataFalsePortDelay(int delay) {
    dataFalse.setDelay(delay);
}

void Select::setConditionPortDelay(int delay) {
    condition.setDelay(delay);
}

void Select::setDataOutPortDelay(int delay) {
    dataOut.setDelay(delay);
}

void Select::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Select::getConnectedPort() {
    return connectedPort;
}

void Select::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Select::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

const Port* Select::getDataInTruePort() {
    return &dataTrue;
}

const Port* Select::getDataInFalsePort() {
    return &dataFalse;
}

const Port* Select::getConditionInPort() {
    return &condition;
}

void Select::printBlock(ostream& file) {
    file << blockName << "[type = Merge";
    file << ", in = \"" << dataTrue << " " << dataFalse
        << " " << condition << "\"";
    file << ", out = \"" << dataOut << "\"";
    if (dataTrue.getDelay() > 0 or dataFalse.getDelay() > 0 or condition.getDelay() > 0
        or dataOut.getDelay() > 0) 
    {
        file << ", delay = \"";
        bool first = true;
        if (dataTrue.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << dataTrue.getName() << ":" << dataTrue.getDelay();
        }
        if (dataFalse.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << dataFalse.getName() << ":" << dataFalse.getDelay();
        }
        if (condition.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << condition.getName() << ":" << condition.getDelay();
        }
        if (blockDelay > 0) {
            if (first) first = false;
            else file << " ";
            file << blockDelay;
        }
        if (dataOut.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << dataOut.getName() << ":" << dataOut.getDelay();
        }
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << "];" << endl;
}


/*
 * =================================
 *  Class Branch
 * =================================
*/


int Branch::instanceCounter = 1;

Branch::Branch(int blockDelay) : Block("Branch" + to_string(instanceCounter), 
    BlockType::Branch_Block, blockDelay), dataIn("in"), condition("condition",
    Port::Condition, 1), dataTrue("outTrue", Port::True), dataFalse("outFalse",
    Port::False), connectedPortTrue(nullptr, nullptr), 
    connectedPortFalse(nullptr, nullptr)
{
    ++instanceCounter;
    nextOutPort = 0;
}

Branch::~Branch() {}

void Branch::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void Branch::setDataTruePortWidth(int width) {
    dataTrue.setWidth(width);
}

void Branch::setDataFalsePortWidth(int width) {
    dataFalse.setWidth(width);
}

void Branch::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    dataTrue.setWidth(width);
    dataFalse.setWidth(width);
}

void Branch::setDataInPortDelay(int delay) {
    dataIn.setDelay(delay);
}

void Branch::setConditionPortDelay(int delay) {
    condition.setDelay(delay);
}
void Branch::setDataTruePortDelay(int delay) {
    dataTrue.setDelay(delay);
}

void Branch::setDataFalsePortDelay(int delay) {
    dataFalse.setDelay(delay);
}

void Branch::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Branch::getConnectedPort() {
    assert(0);
    if (nextOutPort == 0) return connectedPortTrue;
    return connectedPortFalse;
}

void Branch::setConnectedPort(pair <Block*, const Port*> connection) {
    assert(nextOutPort < 2);
    if (nextOutPort == 0) connectedPortTrue = connection;
    else connectedPortFalse = connection;
    ++nextOutPort;
}

bool Branch::connectionAvailable() {
    return (nextOutPort < 2);
}

pair <Block*, const Port*> Branch::getConnectedPortTrue() {
    return connectedPortTrue;
}

pair <Block*, const Port*> Branch::getConnectedPortFalse() {
    return connectedPortFalse;
}

void Branch::setConnectedPortTrue(pair <Block*, const Port*> connection) {
    connectedPortTrue = connection;
}

void Branch::setConnectedPortFalse(pair <Block*, const Port*> connection) {
    connectedPortFalse = connection;
}

bool Branch::connectionTrueAvailable() {
    return (connectedPortTrue.first == nullptr and
        connectedPortTrue.second == nullptr);
}

bool Branch::connectionFalseAvailable() {
    return (connectedPortFalse.first == nullptr and
        connectedPortFalse.second == nullptr);
}

const Port* Branch::getDataInPort() {
    return &dataIn;
}

const Port* Branch::getConditionInPort() {
    return &condition;
}

void Branch::printBlock(ostream& file) {
    file << blockName << "[type = Branch";
    file << ", in = \"" << dataIn << " " << condition << "\"";
    file << ", out = \"" << dataTrue << " " << dataFalse << "\"";
    if (dataIn.getDelay() > 0 or condition.getDelay() > 0 or
        dataTrue.getDelay() > 0 or dataFalse.getDelay() > 0) 
    {
        file << ", delay = \"";
        bool first = true;
        if (dataIn.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << dataIn.getName() << ":" << dataIn.getDelay();
        }
        if (condition.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << condition.getName() << ":" << condition.getDelay();
        }
        if (blockDelay > 0) {
            if (first) first = false;
            else file << " ";
            file << blockDelay;
        }
        if (dataTrue.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << dataTrue.getName() << ":" << dataTrue.getDelay();
        }
        if (dataFalse.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << dataFalse.getName() << ":" << dataFalse.getDelay();
        }
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << "];" << endl;
}

/*
 * =================================
 *  Class Demux
 * =================================
*/


int Demux::instanceCounter = 1;

Demux::Demux(int numControlPorts, int blockDelay) : 
    Block("Demux" + to_string(instanceCounter), BlockType::Demux_Block, 
    blockDelay), dataIn("in")
{
    ++instanceCounter;
    for (int i = 1; i <= numControlPorts; ++i) {
        control.push_back(Port("control" + to_string(i)));
        dataOut.push_back(Port("out" + to_string(i)));
        connectedPorts.push_back(make_pair(nullptr, nullptr));
    }
    nextOutPort = 0;
}

Demux::~Demux() {}

void Demux::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void Demux::setDataOutPortWidth(int index, int width) {
    assert(index >= 0 and index < dataOut.size());
    dataOut[index].setWidth(width);
}

void Demux::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        dataOut[i].setWidth(width);
    }
}

void Demux::setControlPortDelay(int index, int delay) {
    assert(index >= 0 and index < control.size());
    control[index].setDelay(delay);
}

void Demux::setDataInPortDelay(int delay) {
    dataIn.setDelay(delay);
}

void Demux::setDataOutPortDelay(int index, int delay) {
    assert(index >= 0 and index < dataOut.size());
    dataOut[index].setDelay(delay);
}

void Demux::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Demux::getConnectedPort() {
    assert(nextOutPort < dataOut.size());
    return connectedPorts[nextOutPort];
}

void Demux::setConnectedPort(pair <Block*, const Port*> connection) {
    assert(nextOutPort < dataOut.size());
    connectedPorts[nextOutPort] = connection;
    ++nextOutPort;
}

bool Demux::connectionAvailable() {
    return (nextOutPort < dataOut.size());
}

const Port* Demux::getControlInPort(int index) {
    assert(index >= 0 and index < control.size());
    return &control[index];
}

const Port* Demux::getDataInPort() {
    return &dataIn;
}

void Demux::printBlock(ostream& file) {
    file << blockName << "[type = Demux";
    file << ", in = \"";
    for (unsigned int i = 0; i < control.size(); ++i) {
        if (i > 0) file << " ";
        file << control[i];
    }
    file << dataIn << "\"";
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (i == 0) file << ", out = \"";
        else file << " ";
        file << dataOut[i];
        if (i == dataOut.size()-1) file << "\"";
    }
    bool first = true;
    for (unsigned int i = 0; i < control.size(); ++i) {
        if (control[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
                file << control[i].getName() << ":" << control[i].getDelay();
            }
            else {
                file << " " << control[i].getName() << ":" << control[i].getDelay();
            }
        }
    }
    if (first) {
        if (dataIn.getDelay() > 0) {
            file << ", delay = \"";
            file << dataIn.getName() << ":" << dataIn.getDelay() << "\"";
            if (blockDelay > 0) file << " " << blockDelay;
            for (unsigned int i = 0; i < dataOut.size(); ++i) {
                if (dataOut[i].getDelay() > 0) {
                    file << " " << dataOut[i].getDelay();
                }
            }
            file << "\"";
        }
        else {
            bool first2 = true;
            for (unsigned int i = 0; i < dataOut.size(); ++i) {
                if (dataOut[i].getDelay() > 0) {
                    if (first2) {
                        first2 = false;
                        file << ", delay = \"";
                        if (blockDelay > 0) file << blockDelay << " ";
                        file << dataOut[i].getName() << ":" << dataOut[i].getDelay();
                    }
                    else {
                        file << " " << dataOut[i].getName() << ":" << dataOut[i].getDelay();
                    }
                }
            }
            if (first2 and blockDelay > 0) file << ", delay = " << blockDelay;
            else if (!first2) file << "\"";
        }
    }
    else {
        if (dataIn.getDelay() > 0) file << " " << dataIn.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        for (unsigned int i = 0; i < dataOut.size(); ++i) {
            if (dataOut[i].getDelay > 0) {
                file << " " << dataOut[i].getName() << ":" << dataOut[i].getDelay();
            }
        }
        file << "\"";
    }
    file << "];" << endl;
}


/*
 * =================================
 *  Class Entry
 * =================================
*/


int Entry::instanceCounter = 1;

Entry::Entry(int blockDelay) : Block("Entry" + to_string(instanceCounter), 
    BlockType::Entry_Block, blockDelay),
    control("control", Port::Base, 0), connectedPort(nullptr, nullptr)
{
    ++instanceCounter;
}

Entry::~Entry() {}

void Entry::setControlPortDelay(int delay) {
    control.setDelay(delay);
}

void Entry::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Entry::getConnectedPort() {
    return connectedPort;
}

void Entry::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Entry::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

void Entry::printBlock(ostream& file) {
    file << blockName << "[type = Entry";
    file << ", out = \"" << control << "\"";
    if (control.getDelay() > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay << " ";
        file << control.getName() << ":" << control.getDelay() << "\"";
    }
    else if (blockDelay > 0) file << ", delay " << blockDelay;
    file << "];" << endl;
}


/*
 * =================================
 *  Class Argument
 * =================================
*/


int Argument::instanceCounter = 1;

Argument::Argument(int blockDelay) : 
    Block("Arg" + to_string(instanceCounter), 
    BlockType::Entry_Block, blockDelay),
    data("out"), connectedPort(nullptr, nullptr)
{
    ++instanceCounter;
}

Argument::~Argument() {}

void Argument::setDataPortWidth(int width) {
    data.setWidth(width);
}

void Argument::setDataPortDelay(int delay) {
    data.setDelay(delay);
}

void Argument::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Argument::getConnectedPort() {
    return connectedPort;
}

void Argument::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Argument::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

void Argument::printBlock(ostream& file) {
    file << blockName << "[type = Entry";
    file << ", out = \"" << data << "\"";
    if (data.getDelay() > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay << " ";
        file << data.getName() << ":" << data.getDelay() << "\"";
    }
    else if (blockDelay > 0) file << ", delay " << blockDelay;
    file << "];" << endl;
}


/*
 * =================================
 *  Class Exit
 * =================================
*/


int Exit::instanceCounter = 1;

Exit::Exit(int blockDelay) : 
    Block("Exit" + to_string(instanceCounter), 
    BlockType::Exit_Block, blockDelay),
    control("control", Port::Base, 0) 
{
    ++instanceCounter;
}

Exit::~Exit() {}

void Exit::setControlPortDelay(int delay) {
    control.setDelay(delay);
}

void Exit::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Exit::getConnectedPort() {
    assert(0);
    return make_pair(nullptr, nullptr);
}

void Exit::setConnectedPort(pair <Block*, const Port*> connection) {
    assert(0);
}

bool Exit::connectionAvailable() {
    assert(0);
    return false;
}

const Port* Exit::getControlInPort() {
    return &control;
}

void Exit::printBlock(ostream& file) {
    file << blockName << "[type = Exit";
    file << ", in = \"" << control << "\"";
    if (control.getDelay() > 0) {
        file << ", delay = \"";
        file << control.getName() << ":" << control.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay " << blockDelay;
    file << "];" << endl;
}

/*
 * =================================
 *  Class Return
 * =================================
*/


int Return::instanceCounter = 1;

Return::Return(int blockDelay) : 
    Block("Ret" + to_string(instanceCounter), 
    BlockType::Exit_Block, blockDelay),
    data("in")
{
    ++instanceCounter;
}

Return::~Return() {}

void Return::setDataPortWidth(int width) {
    data.setWidth(width);
}

void Return::setDataPortDelay(int delay) {
    data.setDelay(delay);
}

void Return::resetCounter() {
    instanceCounter = 1;
}

pair <Block*, const Port*> Return::getConnectedPort() {
    assert(0);
    return make_pair(nullptr, nullptr);
}

void Return::setConnectedPort(pair <Block*, const Port*> connection) {
    assert(0);
}

bool Return::connectionAvailable() {
    assert(0);
    return false;
}

const Port* Return::getDataInPort() {
    return &data;
}

void Return::printBlock(ostream& file) {
    file << blockName << "[type = Exit";
    file << ", in = \"" << data << "\"";
    if (data.getDelay() > 0) {
        file << ", delay = \"";
        file << data.getName() << ":" << data.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay " << blockDelay;
    file << "];" << endl;
}


} // Close namespace