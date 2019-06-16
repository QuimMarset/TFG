
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
    dataOut("out", portWidth), connectedPort(nullptr, -1)
{
    this->latency = latency;
    this->II = II;
    this->opType = opType;
    ++instanceCounter[opType];
    if (isUnary(opType)) {
        dataIn.push_back(Port("in", portWidth));
    }
    else if (isBinary(opType)) {
        dataIn.push_back(Port("in0", portWidth));
        dataIn.push_back(Port("in1", portWidth));
    }     
}

Operator::~Operator() {}

OpType Operator::getOpType() {
    return opType;
}

unsigned int Operator::addInputPort(int portWidth, unsigned int portDelay) {
    assert(!isUnary(opType) and !isBinary(opType));
    dataIn.push_back(Port("in" + to_string(dataIn.size()), portWidth, 
        Port::Base, portDelay));
    return dataIn.size()-1;
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

pair <Block*, int> Operator::getConnectedPort() {
    return connectedPort;
}

void Operator::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Operator::connectionAvailable()  {
    return (connectedPort.first == nullptr and
        connectedPort.second == -1);
}

unsigned int Operator::getConnectedPortIndex() {
    return 0;
}

const Port& Operator::getInputPort(unsigned int index) {
    return dataIn[index];
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
        connectedPort.second != -1);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
    unsigned int width = dataOut.getWidth();
    file << ", color = ";
    if (width == 0) file << "red";
    else if (width == 1) file << "magenta";
    else file << "blue";
    file << "];" << endl;
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
    connectedPort(nullptr, -1)
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

pair <Block*, int> Buffer::getConnectedPort() {
    return connectedPort;
}

void Buffer::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Buffer::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int Buffer::getConnectedPortIndex() {
    return 0;
}

const Port& Buffer::getInputPort(unsigned int index) {
    return dataIn;
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
    assert(connectedPort.first != nullptr and connectedPort.second != -1);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
    unsigned int width = dataOut.getWidth();
    file << ", color = ";
    if (width == 0) file << "red";
    else if (width == 1) file << "magenta";
    else file << "blue";
    file << "];" << endl;
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
    BlockType::Constant_Block, blockDelay), controlIn("in", 0), 
    dataOut("out", portWidth), connectedPort(nullptr, -1) 
{
    ++instanceCounter;
}

ConstantInterf::~ConstantInterf() {}

void ConstantInterf::setDataPortWidth(int width) {
    dataOut.setWidth(width);
}

void ConstantInterf::setControlPortDelay(unsigned int delay) {
    controlIn.setDelay(delay);
}

void ConstantInterf::setDataPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
}

pair <Block*, int> ConstantInterf::getConnectedPort() {
    return connectedPort;
}

void ConstantInterf::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool ConstantInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int ConstantInterf::getConnectedPortIndex() {
    return 0;
}

const Port& ConstantInterf::getInputPort(unsigned int index) {
    return controlIn;
}

void ConstantInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
    unsigned int width = dataOut.getWidth();
    file << ", color = ";
    if (width == 0) file << "red";
    else if (width == 1) file << "magenta";
    else file << "blue";
    file << "];" << endl;
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
    assert(index < dataOut.size());
    dataOut[index].setDelay(delay);
}

pair <Block*, int> Fork::getConnectedPort() {
    assert(connectedPorts.size() > 0);
    return connectedPorts.back();
}

void Fork::setConnectedPort(pair <Block*, int> connection) {
    int width = dataIn.getWidth();
    int delay = dataIn.getDelay();
    dataOut.push_back(Port("out" + to_string(dataOut.size()),
        width, Port::Base, delay));
    connectedPorts.push_back(connection);
}

bool Fork::connectionAvailable() {
    return true;    
}

unsigned int Fork::getConnectedPortIndex() {
    return connectedPorts.size()-1;
}

void Fork::setOutPort(unsigned int index, pair <Block*, int> connection) {
    assert(index < connectedPorts.size());
    connectedPorts[index] = connection;
}

const Port& Fork::getInputPort(unsigned int index) {
    return dataIn;
}

void Fork::printBlock(ostream& file ) {
    file << blockName << "[type = Fork";
    file << ", in = \"" << dataIn << "\"";
    file << ", out = \"";
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (i > 0) file << " ";
        file << dataOut[i];
    }
    file << "\"";
    bool first = true;
    if (dataIn.getDelay() > 0) {
        file << ", delay = \"" << dataIn.getName() << ":" << 
            dataIn.getDelay();
        first = false;
    }
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (dataOut[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            else file << " ";
            if (blockDelay > 0) file << blockDelay << " ";
            file << dataOut[i].getName() << ":" << dataOut[i].getDelay();
        }
    }
    if (first and blockDelay > 0) file << ", delay = " << blockDelay;
    else if (!first) file << "\"";
    file << "];" << endl;
}

void Fork::printChannels(ostream& file) {
    unsigned int width = dataIn.getWidth();
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != -1);
        file << blockName << " -> " << connectedPorts[i].first->getBlockName() << " [from = " <<
            dataOut[i].getName() << ", to = " << 
            connectedPorts[i].first->getInputPort(connectedPorts[i].second).getName();
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
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
    connectedPort(nullptr, -1)
{
    ++instanceCounter;
}

Merge::~Merge() {}

unsigned int Merge::addDataInPort(unsigned int delay) {
    int width = dataOut.getWidth();
    dataIn.push_back(Port("in" + to_string(dataIn.size()), width,
        Port::Base, delay));
    return dataIn.size()-1;
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

pair <Block*, int> Merge::getConnectedPort() {
    return connectedPort;
}

void Merge::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Merge::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int Merge::getConnectedPortIndex() {
    return 0;
}

const Port& Merge::getInputPort(unsigned int index) {
    assert(index < dataIn.size());
    return dataIn[index];
}

void Merge::printBlock(ostream& file) {
    file << blockName << "[type = Merge";
    file << ", in = \"";
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        if (i > 0) file << " ";
        file << dataIn[i];
    }
    file << "\", out = \"" << dataOut << "\"";
    bool first = true;
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        if (dataIn[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            else file << " ";
            file << dataIn[i].getName() << ":" << dataIn[i].getDelay();
        }
    }
    if (dataOut.getDelay() > 0) {
        if (first) {
            file << ", delay = \"";
            first = false;
        }
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " ";
        file << dataOut.getName() << ":" << dataOut.getDelay();
    }
    else if (blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void Merge::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
    unsigned int width = dataOut.getWidth();
    file << ", color = ";
    if (width == 0) file << "red";
    else if (width == 1) file << "magenta";
    else file << "blue";
    file << "];" << endl;
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
    dataOut("out", portWidth), connectedPort(nullptr, -1)
{
    ++instanceCounter;
}

Select::~Select() {}

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

pair <Block*, int> Select::getConnectedPort() {
    return connectedPort;
}

void Select::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Select::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int Select::getConnectedPortIndex() {
    return 0;
}

const Port& Select::getInputPort(unsigned int index) {
    assert(index < 3);
    if (index == 0) return dataTrue;
    else if (index == 1) return dataFalse;
    else return condition;
}

void Select::printBlock(ostream& file) {
    file << blockName << "[type = Merge";
    file << ", in = \"" << dataTrue << " " << dataFalse
        << " " << condition << "\"";
    file << ", out = \"" << dataOut << "\"";
    bool first = true;
    if (dataTrue.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }     
        else file << " ";
        file << dataTrue.getName() << ":" << dataTrue.getDelay();
    }
    if (dataFalse.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        } 
        else file << " ";
        file << dataFalse.getName() << ":" << dataFalse.getDelay();
    }
    if (condition.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        } 
        else file << " ";
        file << condition.getName() << ":" << condition.getDelay();
    }
    if (dataOut.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        } 
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " ";
        file << dataOut.getName() << ":" << dataOut.getDelay();
    }
    else if (blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void Select::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        dataOut.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
    unsigned int width = dataOut.getWidth();
    file << ", color = ";
    if (width == 0) file << "red";
    else if (width == 1) file << "magenta";
    else file << "blue";
    file << "];" << endl;
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
    connectedPortTrue(nullptr, -1), connectedPortFalse(nullptr, -1)
{
    ++instanceCounter;
    currentPort = false;
}

Branch::~Branch() {}

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

pair <Block*, int> Branch::getConnectedPort() {
    if (currentPort) return connectedPortTrue;
    return connectedPortFalse;
}

void Branch::setConnectedPort(pair <Block*, int> connection) {
    if (currentPort) connectedPortTrue = connection;
    else connectedPortFalse = connection;
}

bool Branch::connectionAvailable() {
    if (currentPort) {
        return (connectedPortTrue.first == nullptr and
            connectedPortTrue.second == -1);
    }
    return (connectedPortFalse.first == nullptr and
        connectedPortFalse.second == -1);
}

unsigned int Branch::getConnectedPortIndex() {
    if (currentPort) return 1;
    else return 0;
}

const Port& Branch::getInputPort(unsigned int index) {
    if (index == 0) return dataIn;
    else return condition;
}

void Branch::setCurrentPort(bool currentPort) {
    this->currentPort = currentPort;
}

void Branch::printBlock(ostream& file) {
    file << blockName << "[type = Branch";
    file << ", in = \"" << dataIn << " " << condition << "\"";
    file << ", out = \"" << dataTrue << " " << dataFalse << "\"";
    bool first = true;
    if (dataIn.getDelay() > 0) {
        if (first) {
           first = false;
           file << ", delay = \""; 
        }
        else file << " ";
        file << dataIn.getName() << ":" << dataIn.getDelay();
    }
    if (condition.getDelay() > 0) {
        if (first) {
           first = false;
           file << ", delay = \""; 
        }
        else file << " ";
        file << condition.getName() << ":" << condition.getDelay();
    }
    if (dataTrue.getDelay() > 0) {
        if (first) {
           first = false;
           file << ", delay = \""; 
        }
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " ";
        file << dataTrue.getName() << ":" << dataTrue.getDelay();
    }
    if (dataFalse.getDelay() > 0) {
        if (first) {
           first = false;
           file << ", delay = \""; 
        }
        else file << " ";
        if (dataFalse.getDelay() == 0 and blockDelay > 0) file << blockDelay << " ";
        file << dataFalse.getName() << ":" << dataFalse.getDelay();
    }
    if (first and blockDelay > 0) file << ", delay = " << blockDelay;
    else if (!first) file << "\"";
    file << "];" << endl;
}

void Branch::printChannels(ostream& file) {
    assert((connectedPortTrue.first != nullptr and connectedPortTrue.second != -1) or
        (connectedPortFalse.first != nullptr and connectedPortFalse.second != -1));
    unsigned int width = dataIn.getWidth();
    if (connectedPortTrue.first != nullptr) {
        file << blockName << " -> " << connectedPortTrue.first->getBlockName() << " [from = " <<
            dataTrue.getName() << ", to = " << 
            connectedPortTrue.first->getInputPort(connectedPortTrue.second).getName();        
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
    }
    if (connectedPortFalse.first != nullptr) {
        file << blockName << " -> " << connectedPortFalse.first->getBlockName() << " [from = " <<
            dataFalse.getName() << ", to = " << 
            connectedPortFalse.first->getInputPort(connectedPortFalse.second).getName();
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
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

unsigned int Demux::addControlInPort(unsigned int delay) {
    control.push_back(Port("control" + to_string(control.size()),
        0, Port::Base, delay));
    return control.size();
}

void Demux::addDataOutPort(unsigned int delay) {
    dataOut.push_back(Port("out" + to_string(dataOut.size()), dataIn.getWidth(), 
        Port::Base, delay));
    connectedPorts.push_back(make_pair(nullptr, -1));
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

pair <Block*, int> Demux::getConnectedPort() {
    if (currentConnected > -1) {
        return connectedPorts[currentConnected];
    }
    else {
        assert(connectedPorts.size() > 0);
        return connectedPorts.back();
    }
}

void Demux::setConnectedPort(pair <Block*, int> connection) {
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
            connectedPorts[currentConnected].second == -1);
    }
    else {
        assert(connectedPorts.size() > 0);
        pair <Block*, int>& last = connectedPorts.back();
        return (last.first == nullptr and last.second == -1);
    }
}

unsigned int Demux::getConnectedPortIndex() {
    if (currentConnected > -1)
        return currentConnected;
    else return connectedPorts.size()-1;
}

const Port& Demux::getInputPort(unsigned int index) {
    if (index == 0) return dataIn;
    else return control[index-1];
}

void Demux::printBlock(ostream& file) {
    assert(control.size() == dataOut.size());
    file << blockName << "[type = Demux";
    file << ", in = \"";
    for (unsigned int i = 0; i < control.size(); ++i) {
        if (i > 0) file << " ";
        file << control[i];
    }
    file << dataIn << "\", out = \"";
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (i > 0) file << " ";
        file << dataOut[i];
    }
    file << "\"";
    bool first = true;
    for (unsigned int i = 0; i < control.size(); ++i) {
        if (control[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            else file << " ";
            file << control[i].getName() << ":" << control[i].getDelay();
        }
    }
    if (dataIn.getDelay() > 0) {
        if (first) {
            file << ", delay = \"";
            first = false;
        }
        else file << " ";
        file << dataIn.getName() << ":" << dataIn.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
    }
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (dataOut[i].getDelay() > 0) {
            if (first) {
                file << ", delay = \"";
                first = false;
            }
            else file << " ";
            if (dataIn.getDelay() == 0 and blockDelay > 0) file << blockDelay << " ";
            file << dataOut[i].getName() << ":" << dataOut[i].getDelay();
        }
    }
    if (first and blockDelay > 0) file << ", delay = " << blockDelay;
    else if (!first) file << "\"";
    file << "];" << endl;
}

void Demux::printChannels(ostream& file) {
    unsigned int width = dataIn.getWidth();
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != -1);
        file << blockName << " -> " << connectedPorts[i].first->getBlockName() << " [from = " <<
            dataOut[i].getName() << ", to = " << 
            connectedPorts[i].first->getInputPort(connectedPorts[i].second).getName();
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
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
    inPort("in", 0), outPort("out", portWidth), 
    connectedPort(nullptr, -1) {}

EntryInterf::~EntryInterf() {}

void EntryInterf::setInPortDelay(unsigned int delay) {
    inPort.setDelay(delay);
}

void EntryInterf::setOutPortDelay(unsigned int delay) {
    outPort.setDelay(delay);
}

pair <Block*, int> EntryInterf::getConnectedPort() {
    return connectedPort;
}

void EntryInterf::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool EntryInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int EntryInterf::getConnectedPortIndex() {
    return 0;
}

const Port& EntryInterf::getInputPort(unsigned int index) {
    return inPort;
}

void EntryInterf::printBlock(ostream &file) {
    file << blockName << "[type = Entry";
    file << ", in = \"" << inPort << "\"";
    file << ", out = \"" << outPort << "\"";
    bool first = true;
    if (inPort.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }
        file << inPort.getName() << ":" << inPort.getDelay();
    }
    if (outPort.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " ";
        file << inPort.getName() << ":" << inPort.getDelay();
    }
    if (first and blockDelay > 0) file << ", delay = " << blockDelay;
    else if (!first) file << "\"";
    file << "];" << endl;
}

void EntryInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1);
    file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        outPort.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
    unsigned int width = outPort.getWidth();
    file << ", color = ";
    if (width == 0) file << "red";
    else if (width == 1) file << "magenta";
    else file << "blue";
    file << "];" << endl;
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
    inPort("in", portWidth), outPort("out", 0),
    connectedPort(nullptr, -1) {}

ExitInterf::~ExitInterf() {}

void ExitInterf::setInPortDelay(unsigned int delay) {
    inPort.setDelay(delay);
}

void ExitInterf::setOutPortDelay(unsigned int delay) {
    outPort.setDelay(delay);
}

pair <Block*, int> ExitInterf::getConnectedPort()  {
    return connectedPort;
}

void ExitInterf::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool ExitInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and
        connectedPort.second == -1);
}

unsigned int ExitInterf::getConnectedPortIndex() {
    return 0;
}

const Port& ExitInterf::getInputPort(unsigned int index) {
    return inPort;
}

void ExitInterf::printBlock(ostream &file) {
    file << blockName << "[type = Exit";
    file << ", in = \"" << inPort << "\"";
    file << ", out = \"" << outPort << "\"";
    bool first = true;
    if (inPort.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }
        file << inPort.getName() << ":" << inPort.getDelay();
    }
    if (outPort.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " ";
        file << inPort.getName() << ":" << inPort.getDelay();
    }
    if (first and blockDelay > 0) file << ", delay = " << blockDelay;
    else if (!first) file << "\"";
    file << "];" << endl;
}

void ExitInterf::printChannels(ostream& file) {
    if (connectedPort.first != nullptr and connectedPort.second != -1) {
        file << blockName << " -> " << connectedPort.first->getBlockName() << " [from = " <<
        outPort.getName() << ", to = " << 
        connectedPort.first->getInputPort(connectedPort.second).getName();
        unsigned int width = outPort.getWidth();
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
    }
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


FunctionCall::FunctionCall(const BasicBlock* parentBB) : 
    Block("", parentBB, BlockType::FunctionCall_Block, 0),
    inputControlPort(nullptr, -1), connectedResultPort(nullptr, -1),
    connectedControlPort(nullptr, -1) {}

FunctionCall::~FunctionCall() {}

void FunctionCall::setConnectedPortResult(pair <Block*, int> connection) {
    connectedResultPort = connection;
}

void FunctionCall::setConnectedPortControl(pair <Block*, int> connection) {
    connectedControlPort = connection;
}

pair <Block*, int> FunctionCall::getConnectedPort() {
    return connectedResultPort;
}

void FunctionCall::setConnectedPort(pair <Block*, int> connection) {
    connectedResultPort = connection;
}

bool FunctionCall::connectionAvailable() {
    return (connectedResultPort.first == nullptr and
        connectedResultPort.second == -1);
}

unsigned int FunctionCall::getConnectedPortIndex() {
    assert(0);
}

const Port& FunctionCall::getInputPort(unsigned int index) {
    assert(0);
}

void FunctionCall::addInputArgPort(pair <Block*, int> connection) {
    inputArgumentPorts.push_back(connection);
}

void FunctionCall::setInputContPort(pair <Block*, int> connection) {
    inputControlPort = connection;
}

pair <Block*, int> FunctionCall::getInputArgPort(unsigned int index) {
    assert(index < inputArgumentPorts.size());
    return inputArgumentPorts[index];
}

pair <Block*, int> FunctionCall::getInputContPort() {
    return inputControlPort;
}


pair <Block*, int> FunctionCall::getConnecDataPort() {
    return connectedResultPort;
}

pair <Block*, int> FunctionCall::getConnecControlPort() {
    return connectedControlPort;
}

void FunctionCall::printBlock(ostream &file) {
    assert(0);
}

void FunctionCall::printChannels(ostream& file) {
    assert(0);
}


} // Close namespace