
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

Block::Block() {}

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

void Operator::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from =" <<
        dataOut.getName() << ", to=" << connectedPort.second->getName() << "];"
        << endl;
}
    
// Class Unary Operator

vector<int> UnaryOperator::instanceCounter(numberUnary, 1);

UnaryOperator::UnaryOperator(UnaryOpType opType, int blockDelay,
    int latency, int II) : Operator(getUnaryOpName(opType) + 
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
    else if (dataOut.getDelay() > 0) {
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

vector<int> BinaryOperator::instanceCounter(numberBinary, 1);

BinaryOperator::BinaryOperator(BinaryOpType opType, int blockDelay,
    int latency, int II) : Operator(getBinaryOpName(opType) + 
    to_string(instanceCounter[opType]), blockDelay, latency, II),
    dataIn1("in1"), dataIn2("in2")
{
    ++instanceCounter[opType];
    this->opType = opType;
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
        if (dataOut.getDelay() > 0) file << " " << dataOut.getName() << 
            ":" << dataOut.getDelay();
        file << "\"";
    }
    else if (dataOut.getDelay() > 0) {
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

void Buffer::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from =" <<
        dataOut.getName() << ", to=" << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class ConstantInterf
 * =================================
*/

int ConstantInterf::instanceCounter = 1;

ConstantInterf::ConstantInterf() {}

ConstantInterf::ConstantInterf(int porWidth, int blockDelay) : 
    Block("Constant" + to_string(instanceCounter), BlockType::Constant_Block, blockDelay), 
    control("control", Port::Base, 0), data("out"),
    connectedPort(nullptr, nullptr) 
{
    ++instanceCounter;
}

ConstantInterf::~ConstantInterf() {}

void ConstantInterf::setDataPortWidth(int width) {
    data.setWidth(width);
}

void ConstantInterf::setControlPortDelay(int delay) {
    control.setDelay(delay);
}

void ConstantInterf::setDataPortDelay(int delay){
    data.setDelay(delay);
}

pair <Block*, const Port*> ConstantInterf::getConnectedPort() {
    return connectedPort;
}

void ConstantInterf::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool ConstantInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

const Port* ConstantInterf::getControlInPort() {
    return &control;
}

void ConstantInterf::resetCounter() {
    instanceCounter = 1;
}

void ConstantInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from =" <<
        data.getName() << ", to=" << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Fork
 * =================================
*/


int Fork::instanceCounter = 1;

Fork::Fork(int blockDelay) : Block("Fork" + to_string(instanceCounter), 
    BlockType::Fork_Block, blockDelay), dataIn("in")
{
    ++instanceCounter;
}

Fork::~Fork() {}

void Fork::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void Fork::setDataOutPortWidth(unsigned int index, int width) {
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

void Fork::setDataOutPortDelay(unsigned int index, int delay) {
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
    int width = -1;
    int delay = 0;
    bool found = false;
    for (unsigned int i = 0; i < dataOut.size() and !found; ++i) {
        if (dataOut[i].getWidth() >= 0 and dataOut[i].getDelay() > 0) {
            width = dataOut[i].getWidth();
            delay = dataOut[i].getDelay();
            found = true;
            break;
        }
    }
    if (!found and dataIn.getWidth() >= 0 and dataIn.getDelay() > 0) {
        width = dataIn.getWidth();
        delay = dataIn.getDelay();
    }
    dataOut.push_back(Port("out" + to_string(dataOut.size()),
        Port::Base, width, delay));
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
            if (dataOut[i].getDelay() > 0) {
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

void Fork::printChannels(ostream& file) {
    cout << dataOut.size() << '\n';
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != nullptr);
        file << blockName << " -> " << connectedPorts[i].first->getBlockName() << " [from =" <<
            dataOut[i].getName() << ", to=" << connectedPorts[i].second->getName() << "];"
            << endl;
    }
}


/*
 * =================================
 *  Class Merge
 * =================================
*/


int Merge::instanceCounter = 1;

Merge::Merge(int blockDelay) : 
    Block("Merge" + to_string(instanceCounter), BlockType::Merge_Block,
    blockDelay), dataOut("out"),
    connectedPort(nullptr, nullptr)
{
    ++instanceCounter;
}

Merge::~Merge() {}

const Port* Merge::addDataInPort(int width, int delay) {
    int portWidth;
    if (width != -1) portWidth = width;
    else portWidth = dataOut.getWidth();
    dataIn.push_back(Port("in" + to_string(dataIn.size()), Port::Base, 
        portWidth, delay));
    return &dataIn[dataIn.size()-1];
}

void Merge::setDataInPortWidth(unsigned int index, int width) {
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

void Merge::setDataInPortDelay(unsigned int index, int delay) {
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

const Port* Merge::getDataInPort(unsigned int index) {
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

void Merge::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from =" <<
        dataOut.getName() << ", to=" << connectedPort.second->getName() << "];"
        << endl;
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
    if (dataTrue.getDelay() > 0 or dataFalse.getDelay() > 0 or 
        condition.getDelay() > 0 or dataOut.getDelay() > 0) 
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

void Select::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from =" <<
        dataOut.getName() << ", to=" << connectedPort.second->getName() << "];"
        << endl;
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

void Branch::printChannels(ostream& file) {
    assert((connectedPortTrue.first != nullptr and connectedPortTrue.second != nullptr) or
        (connectedPortFalse.first != nullptr and connectedPortFalse.second != nullptr));
    if (connectedPortTrue.first != nullptr) {
        file << blockName << " -> " << connectedPortTrue.first->getBlockName() << " [from =" <<
            dataTrue.getName() << ", to=" << connectedPortTrue.second->getName() << "];"
            << endl;
    }
    if (connectedPortFalse.first != nullptr) {
        file << blockName << " -> " << connectedPortFalse.first->getBlockName() << " [from =" <<
            dataFalse.getName() << ", to=" << connectedPortFalse.second->getName() << "];"
            << endl;    
    }
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

void Demux::setDataOutPortWidth(unsigned int index, int width) {
    assert(index >= 0 and index < dataOut.size());
    dataOut[index].setWidth(width);
}

void Demux::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        dataOut[i].setWidth(width);
    }
}

void Demux::setControlPortDelay(unsigned int index, int delay) {
    assert(index >= 0 and index < control.size());
    control[index].setDelay(delay);
}

void Demux::setDataInPortDelay(int delay) {
    dataIn.setDelay(delay);
}

void Demux::setDataOutPortDelay(unsigned int index, int delay) {
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

const Port* Demux::getControlInPort(unsigned int index) {
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
            if (dataOut[i].getDelay() > 0) {
                file << " " << dataOut[i].getName() << ":" << dataOut[i].getDelay();
            }
        }
        file << "\"";
    }
    file << "];" << endl;
}

void Demux::printChannels(ostream& file) {
    for (unsigned int i = 0;i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != nullptr);
        file << blockName << " -> " << connectedPorts[i].first->getBlockName() << " [from =" <<
            dataOut[i].getName() << ", to=" << connectedPorts[i].second->getName() << "];"
            << endl;
    }
}


/*
 * =================================
 *  Class EntryInterf
 * =================================
*/


EntryInterf::EntryInterf() : Block() {}

EntryInterf::EntryInterf(const string& blockName, int portWidth, int blockDelay) : 
    Block(blockName, BlockType::Entry_Block, blockDelay), 
    connectedPort(nullptr, nullptr) {}

EntryInterf::~EntryInterf() {}

void EntryInterf::setInPortDelay(int delay) {
    outPort.setDelay(delay);
}

pair <Block*, const Port*> EntryInterf::getConnectedPort() {
    return connectedPort;
}

void EntryInterf::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool EntryInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == nullptr);
}

void EntryInterf::printBlock(ostream &file) {
    file << blockName << "[type = Entry";
    file << ", out = \"" << outPort << "\"";
    if (outPort.getDelay() > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay << " ";
        file << outPort.getName() << ":" << outPort.getDelay() << "\"";
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << "];" << endl;
}

void EntryInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from =" <<
        outPort.getName() << ", to=" << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Entry
 * =================================
*/


int Entry::instanceCounter = 1;

Entry::Entry(int blockDelay) : EntryInterf("Entry" + to_string(instanceCounter), 
    0, blockDelay)
{
    ++instanceCounter;
    outPort.setName("control");
}

Entry::~Entry() {}

void Entry::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Argument
 * =================================
*/


int Argument::instanceCounter = 1;

Argument::Argument(int portWidth, int blockDelay) : 
    EntryInterf("Argument" + to_string(instanceCounter), portWidth, blockDelay),
    inControl("control", Port::Base, 0)
{
    ++instanceCounter;
    outPort.setName("data");
}

Argument::~Argument() {}

void Argument::setDataPortWidth(int width) {
    outPort.setWidth(width);
}

void Argument::setControlPortDelay(int delay) {
    inControl.setDelay(delay);
}

const Port* Argument::getControlInPort() {
    return &inControl;
}

void Argument::resetCounter() {
    instanceCounter = 1;
}

void Argument::printBlock(ostream& file) {
    file << blockName << "[type = Entry";
    file << ", in = \"" << inControl << "\"";
    file << ", out = \"" << outPort << "\"";
    bool first = true;
    if (inControl.getDelay() > 0 || outPort.getDelay() > 0) {
        file << ", delay = \"";
        if (inControl.getDelay() > 0) {
            first = false;
            file << inControl.getName() << ":" << inControl.getDelay();
        }
        if (blockDelay > 0) {
            if (first) first = false;
            else file << " ";
            file << blockDelay;
        }
        if (outPort.getDelay() > 0) {
            if (first) first = false;
            else file << " ";
            file << outPort.getName() << ":" << outPort.getDelay();
        }
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << "];" << endl;
}

/*
 * =================================
 *  Class ExitInterf
 * =================================
*/


ExitInterf::ExitInterf() : Block() {}

ExitInterf::ExitInterf(const string& blockName, int portWidth, int blockDelay) :
    Block(blockName, BlockType::Exit_Block, blockDelay) {}

ExitInterf::~ExitInterf() {}

void ExitInterf::setInPortDelay(int delay) {
    inPort.setDelay(delay);
}

pair <Block*, const Port*> ExitInterf::getConnectedPort()  {
    assert(0);
    return make_pair(nullptr, nullptr);
}

void ExitInterf::setConnectedPort(pair <Block*, const Port*> connection) {
    assert(0);
}

bool ExitInterf::connectionAvailable() {
    assert(0);
    return false;
}

const Port* ExitInterf::getInPort() {
    return &inPort;
}

void ExitInterf::printBlock(ostream &file) {
    file << blockName << "[type = Exit";
    file << ", in = \"" << inPort << "\"";
    if (inPort.getDelay() > 0) {
        file << ", delay = \"";
        file << inPort.getName() << ":" << inPort.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay " << blockDelay;
    file << "];" << endl;
}

void ExitInterf::printChannels(ostream& file) {}


/*
 * =================================
 *  Class Exit
 * =================================
*/


int Exit::instanceCounter = 1;

Exit::Exit(int blockDelay) : ExitInterf("Exit" + to_string(instanceCounter), 
    0, blockDelay)
{
    ++instanceCounter;
    inPort.setName("control");
}

Exit::~Exit() {}

void Exit::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Return
 * =================================
*/


int Return::instanceCounter = 1;

Return::Return(int portWidth, int blockDelay) : ExitInterf("Return" + 
    to_string(instanceCounter), portWidth, blockDelay)
{
    ++instanceCounter;
    inPort.setName("data");
}

Return::~Return() {}

void Return::setDataPortWidth(int width) {
    inPort.setWidth(width);
}

void Return::resetCounter() {
    instanceCounter = 1;
}


/*
 * =================================
 *  Class Store
 * =================================
*/

int Store::instanceCounter = 1;

Store::Store(int blockDelay) : ExitInterf("Store" + to_string(instanceCounter)),
    inPortAddr("pointer"), inPortAlign("align", Port::Base, 32)
{
    ++instanceCounter;
    inPort.setName("value");
}

Store::~Store() {}

void Store::setDataPortWidth(int width) {
    inPort.setWidth(width);
}

void Store::setAddrPortWidth(int width) {
    inPortAddr.setWidth(width);
}

const Port* Store::getAddrPort() {
    return &inPortAddr;
}

const Port* Store::getAlignPort() {
    return &inPortAlign;
}

void Store::resetCounter() {
    instanceCounter = 1;
}

void Store::printBlock(ostream& file) {
    file << blockName << "[type = Exit";
    file << ", in = \"" << inPort << " " << inPortAddr << " " << inPortAlign << "\"";
    if (inPort.getDelay() > 0 || inPortAddr.getDelay() > 0 || inPortAlign.getDelay() > 0) {
        file << ", delay = \"";
        bool first = true; 
        if (inPort.getDelay() > 0) {
            first = false;
            file << inPort.getName() << ":" << inPort.getDelay();
        }
        if (inPortAddr.getDelay() > 0) {
            if (!first) file << " ";
            else first = false;
            file << inPortAddr.getName() << ":" << inPortAddr.getDelay();
        }
        if (inPortAlign.getDelay() > 0) {
            if (!first) file << " ";
            else first = false;
            file << inPortAlign.getName() << ":" << inPortAlign.getDelay();
        }
        if (blockDelay > 0) file << " " << blockDelay << "\"";
    }
    else if (blockDelay > 0) file << ", delay " << blockDelay;
    file << "];" << endl;
}



} // Close namespace