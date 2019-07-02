
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
    if (portWidth == -1 and dataIn.size() > 0) {
        portWidth = dataIn[0].getWidth();
    }
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
    assert(index < dataIn.size() && "Wrong input port");
    dataIn[index].setWidth(width);
}

void Operator::setDataOutPortWidth(int width) {
    dataOut.setWidth(width);
}

void Operator::setDataPortWidth(int width) {
    dataOut.setWidth(width);
    for (unsigned int i = 0; i < dataIn.size(); ++i) {
        dataIn[i].setWidth(width);
    }
}

void Operator::setDataInPortDelay(unsigned int index, unsigned int delay) {
    assert(index < dataIn.size() && "Wrong input port");
    dataIn[index].setDelay(delay);
}

void Operator::setDataOutPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
}

pair <Block*, int> Operator::getConnectedPort() {
    return connectedPort;
}

void Operator::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void Operator::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Operator::connectionAvailable()  {
    return (connectedPort.first == nullptr and
        connectedPort.second == -1);
}

unsigned int Operator::getOutputPortIndex() {
    return 0;
}

const Port& Operator::getInputPort(unsigned int index) {
    assert(index < dataIn.size() && "Wrong input port");
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
            }
            else file << " ";
            file << dataIn[i].getName() << ":" << dataIn[i].getDelay();
        }
    }
    if (opType != OpType::Store and dataOut.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " ";
        file << dataOut.getName() << ":" << dataOut.getDelay() << "\"";
    }
    else if (blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << ", op = " << opType;
    file << ", latency = " << latency;
    file << ", II = " << II;
    file << "];" << endl;
}

void Operator::printChannels(ostream& file) {
    if (opType != OpType::Store) {
        assert(connectedPort.first != nullptr and connectedPort.second != -1 &&
            "Operator output port disconnected");
        file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
            " [from = " << dataOut.getName() << ", to = " << 
            connectedPort.first->getInputPort(connectedPort.second).getName();
        unsigned int width = dataOut.getWidth();
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
    }
}


/*
 * =================================
 *  Class Buffer
 * =================================
*/


unsigned int Buffer::instanceCounter = 1;

Buffer::Buffer(const BasicBlock* parentBB, int portWidth, unsigned int blockDelay,
    unsigned int slots, bool transparent) : 
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

void Buffer::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void Buffer::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Buffer::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int Buffer::getOutputPortIndex() {
    return 0;
}

const Port& Buffer::getInputPort(unsigned int index) {
    assert(index == 0 && "Wrong input port");
    return dataIn;
}

void Buffer::printBlock(ostream &file) {
    file << blockName << "[type = Buffer";
    file << ", in = \"" << dataIn << "\"";
    file << ", out = \"" << dataOut << "\"";
    bool first = true;
    if (dataIn.getDelay() > 0) {
        first = false;
        file << ", delay = \"";
        file << dataIn.getName() << ":" << dataIn.getDelay();
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
    file << ", slots = " << slots;
    file << ", transparent = ";
    if (transparent) file << "true";
    else file << "false";
    file << "];" << endl;
}

void Buffer::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1 &&
        "Buffer output port disconnected");
    file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
        " [from = " << dataOut.getName() << ", to = " << 
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

void ConstantInterf::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void ConstantInterf::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool ConstantInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int ConstantInterf::getOutputPortIndex() {
    return 0;
}

const Port& ConstantInterf::getInputPort(unsigned int index) {
    assert(index == 0 && "Wrong input port");
    return controlIn;
}

void ConstantInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1 &&
        "Constant output port disconnected");
    file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
        " [from = " << dataOut.getName() << ", to = " << 
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
    assert(index < dataOut.size() && "Wrong output port");
    dataOut[index].setDelay(delay);
}

pair <Block*, int> Fork::getConnectedPort() {
    assert(connectedPorts.size() > 0 && "No output ports");
    return connectedPorts.back();
}

void Fork::setConnectedPort(Block* block, int idxPort) {
    int width = dataIn.getWidth();
    int delay = dataIn.getDelay();
    dataOut.push_back(Port("out" + to_string(dataOut.size()),
        width, Port::Base, delay));
    connectedPorts.push_back(make_pair(block, idxPort));
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

unsigned int Fork::getOutputPortIndex() {
    return connectedPorts.size()-1;
}

void Fork::setOutPort(unsigned int index, pair <Block*, int> connection) {
    assert(index < connectedPorts.size() && "Wrong output port");
    connectedPorts[index] = connection;
}

const Port& Fork::getInputPort(unsigned int index) {
    assert(index == 0 && "Wrong input port");
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
        first = false;
        file << ", delay = \"";
        file << dataIn.getName() << ":" << dataIn.getDelay();
    }
    bool first2 = true;
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (dataOut[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            else file << " ";
            if (first2) {
                first2 = false;
                if (blockDelay > 0) file << blockDelay << " ";
            }
            file << dataOut[i].getName() << ":" << dataOut[i].getDelay();
        }
    }
    if (first2 and blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void Fork::printChannels(ostream& file) {
    unsigned int width = dataIn.getWidth();
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != -1 &&
            "Fork has some output port disconnected");
        file << '\t' << blockName << " -> " << connectedPorts[i].first->getBlockName() << 
            " [from = " << dataOut[i].getName() << ", to = " << 
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
    assert(index < dataIn.size() && "Wrong input port");
    dataIn[index].setDelay(delay);
}

void Merge::setDataOutPortDelay(unsigned int delay) {
    dataOut.setDelay(delay);
}

pair <Block*, int> Merge::getConnectedPort() {
    return connectedPort;
}

void Merge::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void Merge::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Merge::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int Merge::getOutputPortIndex() {
    return 0;
}

const Port& Merge::getInputPort(unsigned int index) {
    assert(index < dataIn.size() && "Wrong input port");
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
    assert(connectedPort.first != nullptr and connectedPort.second != -1 &&
        "Merge output port disconnected");
    file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
        " [from = " << dataOut.getName() << ", to = " << 
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
    dataFalse("inFalse", portWidth, Port::False), condition("inCondition", 1, Port::Condition),
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

void Select::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void Select::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool Select::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int Select::getOutputPortIndex() {
    return 0;
}

const Port& Select::getInputPort(unsigned int index) {
    assert(index < 3 && "Wrong input port");
    if (index == 0) return dataTrue;
    else if (index == 1) return dataFalse;
    else return condition;
}

void Select::printBlock(ostream& file) {
    file << blockName << "[type = Select";
    file << ", in = \"" << dataTrue << " " << dataFalse
        << " " << condition << "\"";
    file << ", out = \"" << dataOut << "\"";
    bool first = true;
    if (dataTrue.getDelay() > 0) {
        first = false;
        file << ", delay = \"";
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
    assert(connectedPort.first != nullptr and connectedPort.second != -1 &&
        "Select output port disconnected");
    file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
        " [from = " << dataOut.getName() << ", to = " << 
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
    condition("inCondition", 1, Port::Condition), dataTrue("outTrue", portWidth, 
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

void Branch::setConnectedPort(Block* block, int idxPort) {
    if (currentPort) connectedPortTrue = make_pair(block, idxPort);
    else connectedPortFalse = make_pair(block, idxPort);
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

unsigned int Branch::getOutputPortIndex() {
    if (currentPort) return 1;
    else return 0;
}

const Port& Branch::getInputPort(unsigned int index) {
    assert(index < 2 && "Wrong input port");
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
        first = false;
        file << ", delay = \""; 
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
        if (dataTrue.getDelay() == 0 and blockDelay > 0) file << blockDelay << " ";
        file << dataFalse.getName() << ":" << dataFalse.getDelay();
    }
    else if (dataTrue.getDelay() == 0 and blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void Branch::printChannels(ostream& file) {
    assert(((connectedPortTrue.first != nullptr and connectedPortTrue.second != -1) or
        (connectedPortFalse.first != nullptr and connectedPortFalse.second != -1)) &&
        "Branch has some output port disconnected");
    unsigned int width = dataIn.getWidth();
    if (connectedPortTrue.first != nullptr) {
        file << '\t' << blockName << " -> " << connectedPortTrue.first->getBlockName() << 
            " [from = " << dataTrue.getName() << ", to = " << 
            connectedPortTrue.first->getInputPort(connectedPortTrue.second).getName();        
        file << ", color = ";
        if (width == 0) file << "red";
        else if (width == 1) file << "magenta";
        else file << "blue";
        file << "];" << endl;
    }
    if (connectedPortFalse.first != nullptr) {
        file << '\t' << blockName << " -> " << connectedPortFalse.first->getBlockName() << 
            " [from = " << dataFalse.getName() << ", to = " << 
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
    currentConnected = 0;
}

Demux::~Demux() {}

unsigned int Demux::addControlInPort(unsigned int delay) {
    control.push_back(Port("inControl" + to_string(control.size()),
        0, Port::Base, delay));
    return control.size();
}

void Demux::addDataOutPort(unsigned int delay) {
    dataOut.push_back(Port("out" + to_string(dataOut.size()), dataIn.getWidth(), 
        Port::Base, delay));
    currentConnected = connectedPorts.size();
    connectedPorts.push_back(make_pair(nullptr, -1));
}

void Demux::setDataPortWidth(int width) {
    dataIn.setWidth(width);
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        dataOut[i].setWidth(width);
    }
}

void Demux::setControlPortDelay(unsigned int index, unsigned int delay) {
    assert(index < control.size() && "Wrong input port");
    control[index].setDelay(delay);
}

void Demux::setDataInPortDelay(unsigned int delay) {
    dataIn.setDelay(delay);
}

void Demux::setDataOutPortDelay(unsigned int index, unsigned int delay) {
    assert(index < dataOut.size() && "Wrong output port");
    dataOut[index].setDelay(delay);
}

void Demux::setCurrentConnectedPort(unsigned int current) {
    assert(current < connectedPorts.size() && "Wrong output port");
    currentConnected = current;
}

pair <Block*, int> Demux::getConnectedPort() {
    return connectedPorts[currentConnected];
}

void Demux::setConnectedPort(Block* block, int idxPort) {
    connectedPorts[currentConnected] = make_pair(block, idxPort);
}

void Demux::setConnectedPort(pair <Block*, int> connection) {
    connectedPorts[currentConnected] = connection;
}

bool Demux::connectionAvailable() {
    return (connectedPorts[currentConnected].first == nullptr and
        connectedPorts[currentConnected].second == -1);
}

unsigned int Demux::getOutputPortIndex() {
    return currentConnected;
}

const Port& Demux::getInputPort(unsigned int index) {
    assert(index <= control.size() && "Wrong input port");
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
    }
    bool first2 = true;
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        if (dataOut[i].getDelay() > 0) {
            if (first) {
                first = false;
                file << ", delay = \"";
            }
            else file << " ";
            if (first2) {
                first2 = false;
                if (blockDelay > 0) file << blockDelay << " ";
            }
            file << dataOut[i].getName() << ":" << dataOut[i].getDelay();
        }
    }
    if (first2 and blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void Demux::printChannels(ostream& file) {
    unsigned int width = dataIn.getWidth();
    for (unsigned int i = 0; i < dataOut.size(); ++i) {
        assert(connectedPorts[i].first != nullptr and connectedPorts[i].second != -1 &&
            "Demux has some output port disconnected");
        file << '\t' << blockName << " -> " << connectedPorts[i].first->getBlockName() << 
            " [from = " << dataOut[i].getName() << ", to = " << 
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
    inPort("in", portWidth), outPort("out", portWidth), 
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

void EntryInterf::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void EntryInterf::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool EntryInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and 
        connectedPort.second == -1);
}

unsigned int EntryInterf::getOutputPortIndex() {
    return 0;
}

const Port& EntryInterf::getInputPort(unsigned int index) {
    assert(index == 0 && "Wrong input port");
    return inPort;
}

void EntryInterf::printBlock(ostream &file) {
    file << blockName << "[type = Entry";
    file << ", in = \"" << inPort << "\"";
    file << ", out = \"" << outPort << "\"";
    bool first = true;
    if (inPort.getDelay() > 0) {
        first = false;
        file << ", delay = \"";
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
    else if (blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void EntryInterf::printChannels(ostream& file) {
    assert(connectedPort.first != nullptr and connectedPort.second != -1 &&
        "Entry output port disconnected");
    file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
        " [from = " << outPort.getName() << ", to = " << 
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
    inPort.setWidth(width);
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
    inPort("in", portWidth), outPort("out", portWidth),
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

void ExitInterf::setConnectedPort(Block* block, int idxPort) {
    connectedPort = make_pair(block, idxPort);
}

void ExitInterf::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

bool ExitInterf::connectionAvailable() {
    return (connectedPort.first == nullptr and
        connectedPort.second == -1);
}

unsigned int ExitInterf::getOutputPortIndex() {
    return 0;
}

const Port& ExitInterf::getInputPort(unsigned int index) {
    assert(index == 0 && "Wrong input port");
    return inPort;
}

void ExitInterf::printBlock(ostream &file) {
    file << blockName << "[type = Exit";
    file << ", in = \"" << inPort << "\"";
    file << ", out = \"" << outPort << "\"";
    bool first = true;
    if (inPort.getDelay() > 0) {
        first = false;
        file << ", delay = \"";
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
    else if (blockDelay > 0) {
        if (first) file << ", delay = ";
        else file << " ";
        file << blockDelay;
    }
    if (!first) file << "\"";
    file << "];" << endl;
}

void ExitInterf::printChannels(ostream& file) {
    if (connectedPort.first != nullptr and connectedPort.second != -1) {
        file << '\t' << blockName << " -> " << connectedPort.first->getBlockName() << 
        " [from = " << outPort.getName() << ", to = " << 
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
    outPort.setWidth(width);
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


void FunctionCall::setConnectedControlPort(Block* block, int idxPort) {
    connectedControlPort = make_pair(block, idxPort);
}

pair <Block*, int> FunctionCall::getConnectedPort() {
    return connectedResultPort;
}

void FunctionCall::setConnectedPort(Block* block, int idxPort) {
    connectedResultPort = make_pair(block, idxPort);
}

void FunctionCall::setConnectedPort(pair <Block*, int> connection) {
    connectedResultPort = connection;
}

bool FunctionCall::connectionAvailable() {
    return (connectedResultPort.first == nullptr and
        connectedResultPort.second == -1);
}

unsigned int FunctionCall::getOutputPortIndex() {
    assert(0 && "Not should be called");
}

const Port& FunctionCall::getInputPort(unsigned int index) {
    assert(0 && "Not should be called");
}

void FunctionCall::addInputArgPort(Block* block, int idxPort) {
    inputArgumentPorts.push_back(make_pair(block, idxPort));
}

void FunctionCall::setInputContPort(Block* block, int idxPort) {
    inputControlPort = make_pair(block, idxPort);
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
    assert(0 && "Not should be called");
}

void FunctionCall::printChannels(ostream& file) {
    assert(0 && "Not should be called");
}


} // Close namespace