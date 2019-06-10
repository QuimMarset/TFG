
#include "Block.h"
#include <iostream>


namespace DFGraphComp
{


/*
 * =================================
 *  Class Block
 * =================================
*/


Block::Block() {}

Block::Block(const string &blockName, const BasicBlock* parentBB,
    BlockType blockType, unsigned int blockDelay) 
{
    this->blockName = blockName;
    this->parentBB = parentBB;
    this->blockType = blockType;
    this->blockDelay = blockDelay;
}

Block::~Block() {}

const BasicBlock* Block::getParentBB() {
    return parentBB;
}

string Block::getBlockName() {
    return blockName;
}

BlockType Block::getBlockType() {
    return blockType;
}

void Block::setBlockDelay(unsigned int blockDelay) {
    this->blockDelay = blockDelay;
}


/*
 * =================================
 *  Class Operator
 * =================================
*/


vector <unsigned int> Operator::instanceCounter(numberOperators, 1);

Operator::Operator(OpType opType, const BasicBlock* parentBB, 
    int portWidth, unsigned int blockDelay, unsigned int latency, 
    unsigned int II) : 
    Block(getOpName(opType) + to_string(instanceCounter[opType]), 
    parentBB, BlockType::Operator_Block, blockDelay), 
    dataOut("out", portWidth), connectedPort(nullptr, nullptr)
{
    this->latency = latency;
    this->II = II;
    this->opType = opType;
    ++instanceCounter[opType];
    if (isUnary(opType)) {
        dataIn.push_back(Port("in", portWidth));
    }
    else if (isBinary(opType)) {
        dataIn.push_back(Port("in1", portWidth));
        dataIn.push_back(Port("in2", portWidth));
    }     
}

Operator::~Operator() {}

OpType Operator::getOpType() {
    return opType;
}

const Port* Operator::addInputPort(int portWidth, unsigned int portDelay) {
    assert(!isUnary(opType) and !isBinary(opType));
    dataIn.push_back(Port("in" + to_string(dataIn.size()), portWidth, 
        Port::Base, portDelay));
    return &dataIn.back();
}

void Operator::setLatency(unsigned int latency) {
    this->latency = latency;
}

void Operator::setII(unsigned int II) {
    this->II = II;
}

void Operator::setDataInPortWidth(unsigned int index, int width) {
    assert(index < dataIn.size());
    dataIn[index].setWidth(width);
}

void Operator::setDataOutPortWidth(int width) {
    dataOut.setWidth(width);
}

void Operator::setDataPortWidth(int width) {
    dataOut.setWidth(width);
    for (Port& port : dataIn) {
        port.setWidth(width);
    }
}

void Operator::setDataInPortDelay(unsigned int index, unsigned int delay) {
    assert(index < dataIn.size());
    dataIn[index].setDelay(delay);
}

void Operator::setDataOutPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
}

const Port* Operator::getDataInPort(unsigned int index) {
    assert(index < dataIn.size());
    return &dataIn[index];
}

pair <Block*, const Port*> Operator::getConnectedPort() {
    return connectedPort;
}

void Operator::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool Operator::connectionAvailable()  {
    return (connectedPort.first == nullptr and
        connectedPort.second == nullptr);
}

void Operator::printBlock(ostream& file) {
    file << blockName << "[type = Operator";
    file << ", in = \"";
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        if (i != 0) file << " ";
        file << dataIn[i];
    }
    file << "\"";
    if (opType != OpType::Store) file << ", out = \"" << dataOut << "\"";
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
        if (opType != OpType::Store and dataOut.getDelay() > 0) {
            file << ", delay = \"";
            if (blockDelay > 0) file << blockDelay << " ";
            file << dataOut.getName() << ":" << dataOut.getDelay() << "\"";
        }
        else if (blockDelay > 0) file << ", delay = " << blockDelay;
    }
    else {
        if (opType != OpType::Store and dataOut.getDelay() > 0) {
            if (blockDelay > 0) file << blockDelay << " ";
            file << dataOut.getName() << ":" << dataOut.getDelay();
        }
        else if (blockDelay > 0) file << blockDelay;
        file << "\"";
    }
    file << ", op = " << opType;
    file << ", latency = " << latency;
    file << ", II = " << II;
    file << "];" << endl;
}

void Operator::printChannels(ostream& file) {
    assert(opType != OpType::Store and connectedPort.first != nullptr and 
        connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Buffer
 * =================================
*/


unsigned int Buffer::instanceCounter = 1;

Buffer::Buffer(const BasicBlock* parentBB, unsigned int slots, bool transparent, 
    int portWidth, unsigned int blockDelay) : 
    Block("Buffer" + to_string(instanceCounter), 
    parentBB, BlockType::Buffer_Block, blockDelay), 
    dataIn("in", portWidth), dataOut("out", portWidth), 
    connectedPort(nullptr, nullptr)
{
    ++instanceCounter;
    this->slots = slots;
    this->transparent = transparent;
}

Buffer::~Buffer() {}

void Buffer::setNumSlots(unsigned int slots) {
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

void Buffer::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    dataOut.setWidth(width);
}

void Buffer::setDataInPortDelay(unsigned int delay) {
    dataIn.setDelay(delay);
}

void Buffer::setDataOutPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
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
    if (dataIn.getDelay() > 0 or dataOut.getDelay() > 0) {
        bool first = true;
        file << ", delay = \"";
        if (dataIn.getDelay() > 0) {
            file << dataIn.getName() << ":" << dataIn.getDelay();
            first = false;
        }
        if (blockDelay > 0) {
            if (!first) file << " ";
            else first = false;
            file << blockDelay;
        }
        if (dataIn.getDelay() > 0) {
            if (!first) file << " ";
            file << dataOut.getName() << ":" << dataOut.getDelay();
        }
        file << "\"";
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
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class ConstantInterf
 * =================================
*/

unsigned int ConstantInterf::instanceCounter = 1;

ConstantInterf::ConstantInterf() {}

ConstantInterf::ConstantInterf(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    Block("Constant" + to_string(instanceCounter), parentBB,
    BlockType::Constant_Block, blockDelay), control("control", 0), 
    data("out", portWidth), connectedPort(nullptr, nullptr) 
{
    ++instanceCounter;
}

ConstantInterf::~ConstantInterf() {}

void ConstantInterf::setDataPortWidth(int width) {
    data.setWidth(width);
}

void ConstantInterf::setControlPortDelay(unsigned int delay) {
    control.setDelay(delay);
}

void ConstantInterf::setDataPortDelay(unsigned int delay) {
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

void ConstantInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        data.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Fork
 * =================================
*/


unsigned int Fork::instanceCounter = 1;

Fork::Fork(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    Block("Fork" + to_string(instanceCounter), parentBB,
    BlockType::Fork_Block, blockDelay), dataIn("in", portWidth)
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

void Fork::setDataInPortDelay(unsigned int delay) {
    dataIn.setDelay(delay);
}

void Fork::setDataOutPortDelay(unsigned int index, unsigned int delay) {
    assert(index >= 0 and index < dataOut.size());
    dataOut[index].setDelay(delay);
}

pair <Block*, const Port*> Fork::getConnectedPort() {
    assert(0);
    return make_pair(nullptr, nullptr);
}

void Fork::setConnectedPort(pair <Block*, const Port*> connection) {
    int width = dataIn.getWidth();
    int delay = dataIn.getDelay();
    dataOut.push_back(Port("out" + to_string(dataOut.size()),
        width, Port::Base, delay));
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
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != nullptr);
        file << blockName << " -> " << connectedPorts[i].first->getBlockName() << " [from = " <<
            dataOut[i].getName() << ", to = " << connectedPorts[i].second->getName() << "];"
            << endl;
    }
}


/*
 * =================================
 *  Class Merge
 * =================================
*/


unsigned int Merge::instanceCounter = 1;

Merge::Merge(const BasicBlock* parentBB, int portWidth,
    unsigned int blockDelay) : 
    Block("Merge" + to_string(instanceCounter), parentBB,
    BlockType::Merge_Block, blockDelay), dataOut("out", portWidth),
    connectedPort(nullptr, nullptr)
{
    ++instanceCounter;
}

Merge::~Merge() {}

const Port* Merge::addDataInPort(unsigned int delay) {
    int width = dataOut.getWidth();
    dataIn.push_back(Port("in" + to_string(dataIn.size()), width,
        Port::Base, delay));
    return &dataIn.back();
}

void Merge::setDataInPortWidth(unsigned int index, int width) {
    assert(index < dataIn.size());
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

void Merge::setDataInPortDelay(unsigned int index, unsigned int delay) {
    assert(index < dataIn.size());
    dataIn[index].setDelay(delay);
}

void Merge::setDataOutPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
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
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Select
 * =================================
*/


unsigned int Select::instanceCounter = 1;

Select::Select(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    Block("Select" + to_string(instanceCounter), parentBB,
    BlockType::Select_Block, blockDelay), dataTrue("inTrue", portWidth, Port::True),
    dataFalse("inFalse", portWidth, Port::False), condition("condition", 1, Port::Condition),
    dataOut("out", portWidth), connectedPort(nullptr, nullptr)
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

void Select::setDataTruePortDelay(unsigned int delay) {
    dataTrue.setDelay(delay);
}

void Select::setDataFalsePortDelay(unsigned int delay) {
    dataFalse.setDelay(delay);
}

void Select::setConditionPortDelay(unsigned int delay) {
    condition.setDelay(delay);
}

void Select::setDataOutPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
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
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Branch
 * =================================
*/


unsigned int Branch::instanceCounter = 1;

Branch::Branch(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    Block("Branch" + to_string(instanceCounter), parentBB,
    BlockType::Branch_Block, blockDelay), dataIn("in", portWidth), 
    condition("condition", 1, Port::Condition), dataTrue("outTrue", portWidth, 
    Port::True), dataFalse("outFalse", portWidth, Port::False), 
    connectedPortTrue(nullptr, nullptr), connectedPortFalse(nullptr, nullptr)
{
    ++instanceCounter;
    currentPort = None;
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

void Branch::setDataInPortDelay(unsigned int delay) {
    dataIn.setDelay(delay);
}

void Branch::setConditionPortDelay(unsigned int delay) {
    condition.setDelay(delay);
}

void Branch::setDataTruePortDelay(unsigned int delay) {
    dataTrue.setDelay(delay);
}

void Branch::setDataFalsePortDelay(unsigned int delay) {
    dataFalse.setDelay(delay);
}

pair <Block*, const Port*> Branch::getConnectedPort() {
    assert(currentPort != None);
    if (currentPort == True) return connectedPortTrue;
    return connectedPortFalse;
}

void Branch::setConnectedPort(pair <Block*, const Port*> connection) {
    assert(currentPort != None);
    if (currentPort == True) connectedPortTrue = connection;
    else connectedPortFalse = connection;
}

bool Branch::connectionAvailable() {
    assert(currentPort != None);
    if (currentPort == True) {
        return (connectedPortTrue.first == nullptr and
            connectedPortTrue.second == nullptr);
    }
    return (connectedPortFalse.first == nullptr and
        connectedPortFalse.second == nullptr);
    
}

void Branch::setCurrentPort(BranchCurrentPort currentPort) {
    this->currentPort = currentPort;
}

bool Branch::isCurrentPortSet() {
    return (currentPort != None);
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
        file << blockName << " -> " << connectedPortTrue.first->getBlockName() << " [from = " <<
            dataTrue.getName() << ", to = " << connectedPortTrue.second->getName() << "];"
            << endl;
    }
    if (connectedPortFalse.first != nullptr) {
        file << blockName << " -> " << connectedPortFalse.first->getBlockName() << " [from = " <<
            dataFalse.getName() << ", to = " << connectedPortFalse.second->getName() << "];"
            << endl;    
    }
}


/*
 * =================================
 *  Class Demux
 * =================================
*/


unsigned int Demux::instanceCounter = 1;

Demux::Demux(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    Block("Demux" + to_string(instanceCounter), parentBB,
    BlockType::Demux_Block, blockDelay), dataIn("in", portWidth)
{
    ++instanceCounter;
    currentConnected = -1;
}

Demux::~Demux() {}

const Port* Demux::addControlInPort(unsigned int delay) {
    control.push_back(Port("control" + to_string(control.size()),
        0, Port::Base, delay));
    return &control.back();
}

void Demux::addDataOutPort(unsigned int delay) {
    dataOut.push_back(Port("out" + to_string(dataOut.size()), dataIn.getWidth(), 
        Port::Base, delay));
    connectedPorts.push_back(make_pair(nullptr, nullptr));
    
}

void Demux::setDataInPortWidth(int width) {
    dataIn.setWidth(width);
}

void Demux::setDataOutPortWidth(unsigned int index, int width) {
    assert(index < dataOut.size());
    dataOut[index].setWidth(width);
}

void Demux::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        dataOut[i].setWidth(width);
    }
}

void Demux::setControlPortDelay(unsigned int index, unsigned int delay) {
    assert(index < control.size());
    control[index].setDelay(delay);
}

void Demux::setDataInPortDelay(unsigned int delay) {
    dataIn.setDelay(delay);
}

void Demux::setDataOutPortDelay(unsigned int index, unsigned int delay) {
    assert(index < dataOut.size());
    dataOut[index].setDelay(delay);
}

void Demux::setCurrentConnectedPort(int current) {
    assert(current >= -1 and (unsigned int)current < connectedPorts.size());
    currentConnected = current;
}

pair <Block*, const Port*> Demux::getConnectedPort() {
    if (currentConnected > -1) {
        return connectedPorts[currentConnected];
    }
    else {
        assert(connectedPorts.size() > 0);
        return connectedPorts.back();
    }
}

void Demux::setConnectedPort(pair <Block*, const Port*> connection) {
    if (currentConnected > -1) {
        connectedPorts[currentConnected] = connection;
    }
    else {
        assert(connectedPorts.size() > 0);
        connectedPorts.back() = connection;
    }
}

bool Demux::connectionAvailable() {
    if (currentConnected > -1) {
        return (connectedPorts[currentConnected].first == nullptr and
            connectedPorts[currentConnected].second == nullptr);
    }
    else {
        assert(connectedPorts.size() > 0);
        pair <Block*, const Port*>& last = connectedPorts.back();
        return (last.first == nullptr and last.second == nullptr);
    }
}

const Port* Demux::getControlInPort(unsigned int index) {
    assert(index < control.size());
    return &control[index];
}

const Port* Demux::getDataInPort() {
    return &dataIn;
}

void Demux::printBlock(ostream& file) {
    assert(control.size() == dataOut.size());
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
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != nullptr);
        file << blockName << " -> " << connectedPorts[i].first->getBlockName() << " [from = " <<
            dataOut[i].getName() << ", to = " << connectedPorts[i].second->getName() << "];"
            << endl;
    }
}


/*
 * =================================
 *  Class EntryInterf
 * =================================
*/


EntryInterf::EntryInterf() : Block() {}

EntryInterf::EntryInterf(const string& blockName, const BasicBlock* parentBB,
    int portWidth, unsigned int blockDelay) : 
    Block(blockName, parentBB, BlockType::Entry_Block, blockDelay),
    inPort("control", 0), outPort("out", portWidth), 
    connectedPort(nullptr, nullptr) {}

EntryInterf::~EntryInterf() {}

void EntryInterf::setInPortDelay(unsigned int delay) {
    inPort.setDelay(delay);
}

void EntryInterf::setOutPortDelay(unsigned int delay) {
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

const Port* EntryInterf::getControlInPort() {
    return &inPort;
}

void EntryInterf::printBlock(ostream &file) {
    file << blockName << "[type = Entry";
    file << ", out = \"" << outPort << "\"";
    if (inPort.getDelay() > 0 or outPort.getDelay() > 0) {
        bool first = true;
        file << ", delay = \"";
        if (inPort.getDelay() > 0) {
            file << inPort.getName() << ":" << inPort.getDelay();
            first = false;
        }
        if (blockDelay > 0) {
            if (first) first = false;
            else file << " ";
            file << blockDelay;
        }
        if (outPort.getDelay() > 0) {
            if (!first) file << " ";
            file << inPort.getName() << ":" << inPort.getDelay();
        }
        file << "\"";
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << "];" << endl;
}

void EntryInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != nullptr);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        outPort.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Entry
 * =================================
*/


unsigned int Entry::instanceCounter = 1;

Entry::Entry(const BasicBlock* parentBB, unsigned int blockDelay) : 
    EntryInterf("Entry" + to_string(instanceCounter), parentBB,
    0, blockDelay)
{
    ++instanceCounter;
    inPort.setName("controlIn");
    outPort.setName("controlOut");
}

Entry::~Entry() {}


/*
 * =================================
 *  Class Argument
 * =================================
*/


unsigned int Argument::instanceCounter = 1;

Argument::Argument(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    EntryInterf("Argument" + to_string(instanceCounter), parentBB,
    portWidth, blockDelay)
{
    ++instanceCounter;
    outPort.setName("data");
}

Argument::~Argument() {}

void Argument::setDataPortWidth(int width) {
    outPort.setWidth(width);
}


/*
 * =================================
 *  Class ExitInterf
 * =================================
*/


ExitInterf::ExitInterf() : Block() {}

ExitInterf::ExitInterf(const string& blockName, const BasicBlock* parentBB,
    int portWidth, unsigned int blockDelay) :
    Block(blockName, parentBB, BlockType::Exit_Block, blockDelay),
    inPort("in", portWidth), outPort("controlOut", 0),
    connectedPort(nullptr, nullptr) {}

ExitInterf::~ExitInterf() {}

void ExitInterf::setInPortDelay(unsigned int delay) {
    inPort.setDelay(delay);
}

void ExitInterf::setOutPortDelay(unsigned int delay) {
    outPort.setDelay(delay);
}

pair <Block*, const Port*> ExitInterf::getConnectedPort()  {
    return connectedPort;
}

void ExitInterf::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedPort = connection;
}

bool ExitInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and
        connectedPort.second == nullptr);
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

void ExitInterf::printChannels(ostream& file) {
    if(connectedPort.first != nullptr and connectedPort.second != nullptr)
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        outPort.getName() << ", to = " << connectedPort.second->getName() << "];"
        << endl;
}


/*
 * =================================
 *  Class Exit
 * =================================
*/


unsigned int Exit::instanceCounter = 1;

Exit::Exit(const BasicBlock* parentBB, unsigned int blockDelay) : 
    ExitInterf("Exit" + to_string(instanceCounter), 
    parentBB, 0, blockDelay)
{
    ++instanceCounter;
    inPort.setName("controlIn");
}

Exit::~Exit() {}


/*
 * =================================
 *  Class Return
 * =================================
*/


unsigned int Return::instanceCounter = 1;

Return::Return(const BasicBlock* parentBB, int portWidth, 
    unsigned int blockDelay) : 
    ExitInterf("Return" + to_string(instanceCounter), parentBB,
    portWidth, blockDelay)
{
    ++instanceCounter;
    inPort.setName("data");
}

Return::~Return() {}

void Return::setDataPortWidth(int width) {
    inPort.setWidth(width);
}


/*
 * =================================
 *  Class FunctionCall (Dummy block)
 * =================================
*/


FunctionCall::FunctionCall() : 
    Block("", nullptr, BlockType::FunctionCall_Block, 0) {}

FunctionCall::~FunctionCall() {}

void FunctionCall::setConnectedPortResult(pair <Block*, const Port*> connection) {
    connectedResultPort = connection;
}

void FunctionCall::setConnectedPortControl(pair <Block*, const Port*> connection) {
    connectedControlPort = connection;
}

pair <Block*, const Port*> FunctionCall::getConnectedPort() {
    return connectedResultPort;
}

void FunctionCall::setConnectedPort(pair <Block*, const Port*> connection) {
    connectedResultPort = connection;
}

bool FunctionCall::connectionAvailable() {
    return (connectedResultPort.first == nullptr and
        connectedResultPort.second == nullptr);
}

pair <Block*, const Port*> FunctionCall::getConnecDataPort() {
    return connectedResultPort;
}

pair <Block*, const Port*> FunctionCall::getConnecControlPort() {
    return connectedControlPort;
}

void FunctionCall::printBlock(ostream &file) {
    assert(0);
}

void FunctionCall::printChannels(ostream& file) {
    assert(0);
}


} // Close namespace