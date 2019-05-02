#ifndef BLOCKS_H
#define BLOCKS_H

#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include "SupportTypes.h"

using namespace std;

namespace DFGraphComp
{


class Block {

public:

    Block(const string &blockName, BlockType type, int defaultPortWidth, 
        int blockDelay);
    ~Block();

    string getBlockName();
    BlockType getBlockType();
    int getDefaultPortWidth();
    int getBlockDelay();
    void setBlockName(const string &blockName);
    void setBlockType(BlockType blockType);
    void setDefaultPortWidth(int defaultPortWidth);
    void setBlockDelay(int blockDelay);

    string getInPortName(int index);
    string getOutPortName(int index);  

    int getNextInPort();
    pair <Block*, int> getChannelEnd(int index = 0);
    void setChannelEnd(Block* block, int portIdx, int index = 0);
    bool channelEndAvailable();

    virtual void printBlock(ostream &file);
    void closeBlock(ostream &file);

protected:

    int getInDataPortWidth(int index);
    int getOutDataPortWidth(int index);
    void setInDataPortWidth(int index, int width);
    void setOutDataPortWidth(int index, int width);
    
    int getInPortDelay(int index);
    int getOutPortDelay(int index);
    void setInPortDelay(int index, int delay);
    void setOutPortDelay(int index, int delay);

    void addInputPort(const Port &inPort);
    void addOutputPort(const Port &outPort);

    int getNumInPorts();
    int getNumOutPorts();

    void addChannelEnd(Block* block = nullptr, int portIdx = -1);

private:

    string blockName;
    BlockType blockType;
    int defaultPortWidth;
    int blockDelay;
    vector <Port> inputPorts;
    vector <Port> outputPorts;

    int nextInPort;
    vector <pair <Block*, int> > channelEnd;

    unsigned int getNumPortNoWidth();
    
};


class Operator : public Block {

public:
    Operator(OperatorType opType, int defaultPortWidth = -1, int blockDelay = 0, 
        int latency = 0, int II = 0);
    ~Operator();

    int getLatency();
    int getII();
    OperatorType getOpType();
    void setLatency(int latency);
    void setII(int II);
    void setOpType(OperatorType opType);

    string getInDataPortName(int index);
    string getOutDataPortName();

    int getInDataPortWidth(int index);
    int getOutDataPortWidth();
    void setInDataPortWidth(int index, int width);
    void setOutDataPortWidth(int width);

    int getInDataPortDelay(int index);
    int getOutDataPortDelay();
    void setInDataPortDelay(int index, int width);
    void setOutDataPortDelay(int width);

    void printBlock(ostream &file) override;

    static void resetCounter();

private:

    static vector<int> instanceCounter;
    OperatorType opType;
    int latency;
    int II;

};


class Buffer : public Block {

public:

    Buffer(int slots = 2, bool transparent = false, 
        int defaultPortWidth = -1, int blockDelay = 0);
    ~Buffer();

    int getNumSlots();
    bool getTransparent();
    void setNumSlots(int slots);
    void setTransparent(bool transparent);

    string getInDataPortName();
    string getOutDataPortName();

    int getInDataPortWidth();
    int getOutDataPortWidth();
    void setInDataPortWidth(int width);
    void setOutDataPortWidth(int width);

    int getInDataPortDelay();
    int getOutDataPortDelay();
    void setInDataPortDelay(int delay);
    void setOutDataPortDelay(int delay);

    void printBlock(ostream &file) override;

    static void resetCounter();

private:

    static int instanceCounter;
    int slots;
    bool transparent;

};


template <typename T>
class Constant : public Block {

public:

    Constant(const string &name, T value, int defaultPortWidth = -1, 
        int blockDelay = 0);
    ~Constant();

    T getConstant();
    void setConstant(T constant);

    string getControlPortName();
    string getDataPortName();

    int getDataPortWidth();
    void setDataPortWidth(int width);

    int getControlPortDelay();
    int getDataPortDelay();
    void setControlPortDelay(int delay);
    void setDataPortDelay(int delay);

    void printBlock(ostream &file) override;

    static void resetCounter();

private:

    static int instanceCounter;
    T constant;

};

template <typename T>
int Constant<T>::instanceCounter = 1;

template <typename T>
Constant<T>::Constant(const string &name, T constant, int defaultPortWidth,
        int blockDelay)
        : Block("Constant" + to_string(instanceCounter) + name, 
        BlockType::Constant_Block, defaultPortWidth, blockDelay) {
    ++instanceCounter;
    this->constant = constant;
    Block::addInputPort(Port("control"));
    Block::addOutputPort(Port("out"));
    channelEnd.second = -1;
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
string Constant<T>::getControlPortName() {
    return Block::getInPortName(0);
}

template <typename T>
string Constant<T>::getDataPortName() {
    return Block::getOutPortName(0);
}

template <typename T>
int Constant<T>::getDataPortWidth() {
    return Block::getOutDataPortWidth(0);
}

template <typename T>
void Constant<T>::setDataPortWidth(int width) {
    Block::setOutDataPortWidth(0, width);
}

template <typename T>
int Constant<T>::getDataPortDelay() {
    return Block::getOutPortDelay(0);
}

template <typename T>
int Constant<T>::getControlPortDelay() {
    return Block::getInPortDelay(0);
}

template <typename T>
void Constant<T>::setDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

template <typename T>
void Constant<T>::setControlPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

template <typename T>
void Constant<T>::printBlock(ostream &file) {
    Block::printBlock(file);
    file << ", ";
    file << "value = " << constant;
}


class Fork : public Block {

public:

    Fork(int numOutPorts = 2, int defaultPortWidth = -1, int blockDelay = 0);
    ~Fork();

    void addOutPort(int width = -1);

    string getInDataPortName();
    string getOutDataPortName(int index);

    int getInDataPortWidth();
    int getOutDataPortWidth(int index);
    void setInDataPortWidth(int width);
    void setOutDataPortWidth(int index, int width);

    int getInDataPortDelay();
    int getOutDataPortDelay(int index);
    void setInDataPortDelay(int delay);
    void setOutDataPortDelay(int index, int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Merge : public Block {

public:

    Merge(int numInPorts = 2, int defaultPortWidth = -1, int blockDelay = 0);
    ~Merge();

    string getInDataPortName(int index);
    string getOutDataPortName();

    int getInDataPortWidth(int index);
    int getOutDataPortWidth();
    void setInDataPortWidth(int index, int width);
    void setOutDataPortWidth(int width);

    int getInDataPortDelay(int index);
    int getOutDataPortDelay();
    void setInDataPortDelay(int index, int delay);
    void setOutDataPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Select : public Block {

public:

    Select(int defaultPortWidth = -1, int blockDelay = 0);
    ~Select();

    string getTrueDataPortName();
    string getFalseDataPortName();
    string getConditionPortName();
    string getOutDataPortName();

    int getTrueDataPortWidth();
    int getFalseDataPortWidth();
    int getOutDataPortWidth();
    void setTrueDataPortWidth(int width);
    void setFalseDataPortWidth(int width);
    void setOutDataPortWidth(int width);

    int getTrueDataPortDelay();
    int getFalseDataPortDelay();
    int getConditionPortDelay();
    int getOutDataPortDelay();
    void setTrueDataPortDelay(int delay);
    void setFalseDataPortDelay(int delay);
    void setConditionPortDelay(int delay);
    void setOutDataPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};

class Branch : public Block {

public:

    Branch(int defaultPortWidth = -1, int blockDelay = 0);
    ~Branch();

    string getInDataPortName();
    string getConditionPortName();
    string getTrueDataPortName();
    string getFalseDataPortName();

    int getInDataPortWidth();
    int getTrueDataPortWidth();
    int getFalseDataPortWidth(); 
    void setInDataPortWidth(int width);
    void setTrueDataPortWidth(int width);
    void setFalseDataPortWidth(int width);  

    int getInDataPortDelay();
    int getConditionPortDelay();
    int getTrueDataPortDelay();
    int getFalseDataPortDelay();    
    void setInDataPortDelay(int delay);
    void setConditionPortDelay(int delay);
    void setTrueDataPortDelay(int delay);
    void setFalseDataPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Demux : public Block {

public:

    Demux(int numControlPorts = 1, int defaultPortWidth = -1, int blockDelay = 0);
    ~Demux();

    string getInDataPortName();
    string getControlPortName(int index);
    string getOutDataPortName(int index);

    int getInDataPortWidth();
    int getOutDataPortWidth(int index);
    void setInDataPortWidth(int width);
    void setOutDataPortWidth(int index, int width);

    int getControlPortDelay(int index);
    int getInDataPortDelay();
    int getOutDataPortDelay(int index);
    void setControlPortDelay(int index, int delay);
    void setInDataPortDelay(int delay);
    void setOutDataPortDelay(int index, int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Entry : public Block {

public:

    Entry(int defaultPortWidth = -1, int blockDelay = 0);
    ~Entry();

    string getControlPortName();

    int getControlPortDelay();
    void setControlPortDelay(int delay);
  
    static void resetCounter();
    
private:

    static int instanceCounter;

};


class Argument : public Block {

public:

    Argument(int defaultPortWidth = -1, int blockDelay = 0);
    ~Argument();

    string getDataPortName();

    int getDataPortWidth();
    void setDataPortWidth(int width);

    int getDataPortDelay();
    void setDataPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

    pair <Block*, int> channelEnd;

};


class Exit : public Block {

public:

    Exit(int defaultPortWidth = -1, int blockDelay = 0);
    ~Exit();

    string getControlPortName();

    int getControlPortDelay();
    void setControlPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Return : public Block {

public:

    Return(int defaultPortWidth = -1, int blockDelay = 0);
    ~Return();

    string getDataPortName();

    int getDataPortWidth();
    void setDataPortWidth(int width);

    int getDataPortDelay();
    void setDataPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


} // Close namespace


#endif // BLOCKS_H