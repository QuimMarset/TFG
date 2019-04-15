#ifndef BLOCKS_H
#define BLOCKS_H

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <assert.h>
#include "AttributeUniValue.h"
#include "AttributeMultiValue.h"
#include "SupportTypes.h"
using namespace std;

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

    virtual int getDataPortsWidth();
    virtual void setDataPortsWidth(int width);

    virtual void printBlock(ostream &file) const;
    void closeBlock(ostream &file) const;
    friend ostream &operator << (ostream& out, const Block &block);


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

private:

    string blockName;
    BlockType blockType;
    int defaultPortWidth;
    int blockDelay;
    vector <Port> inputPorts;
    vector <Port> outputPorts;
    
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

    int getInDataPortDelay(int index);
    int getOutDataPortDelay();
    void setInDataPortDelay(int index, int delay);
    void setOutDataPortDelay(int delay);

    void printBlock(ostream &file) const override;

    static void resetCounter();

private:

    static vector<int> instanceCounter;
    OperatorType opType;
    int latency;
    int II;

};


class Buffer : public Block {

public:

    Buffer(int defaultPortWidth = -1, int blockDelay = 0, int slots = 2, 
        bool transparent = false);
    ~Buffer();

    int getNumSlots();
    bool getTransparent();
    void setNumSlots(int slots);
    void setTransparent(bool transparent);

    int getInDataPortDelay();
    int getOutDataPortDelay();
    void setInDataPortDelay(int delay);
    void setOutDataPortDelay(int delay);

    void printBlock(ostream &file) const override;

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

    int getDataPortsWidth() override;
    void setDataPortsWidth(int width) override;

    int getInControlPortDelay();
    int getOutDataPortDelay();
    void setInControlPortDelay(int delay);
    void setOutDataPortDelay(int delay);

    void printBlock(ostream &file) const override;

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
    Block::addInputPort(Port("inControl"));
    Block::addOutputPort(Port("out"));
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
int Constant<T>::getDataPortsWidth() {
    return Block::getOutDataPortWidth(0);
}

template <typename T>
void Constant<T>::setDataPortsWidth(int width) {
    Block::setOutDataPortWidth(0, width);
}

template <typename T>
int Constant<T>::getOutDataPortDelay() {
    return Block::getOutPortDelay(0);
}

template <typename T>
int Constant<T>::getInControlPortDelay() {
    return Block::getInPortDelay(0);
}

template <typename T>
void Constant<T>::setOutDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

template <typename T>
void Constant<T>::setInControlPortDelay(int delay) {
    Block::setInPortDelay(0, delay);
}

template <typename T>
void Constant<T>::printBlock(ostream &file) const {
    Block::printBlock(file);
    file << ", ";
    file << "value = " << constant;
}


class Fork : public Block {

public:

    Fork(int defaultPortWidth = -1, int blockDelay = 0, int numOutPorts = 2);
    ~Fork();

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

    Merge(int defaultPortWidth = -1, int blockDelay = 0, int numInPorts = 2);
    ~Merge();

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

    void setDataPortsWidth(int width) override;

    int getDataTruePortDelay();
    int getDataFalsePortDelay();
    int getConditionPortDelay();
    int getDataOutPortDelay();
    void setDataTruePortDelay(int delay);
    void setDataFalsePortDelay(int delay);
    void setConditionPortDelay(int delay);
    void setDataOutPortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};

class Branch : public Block {

public:

    Branch(int defaultPortWidth = -1, int blockDelay = 0);
    ~Branch();

    void setDataPortsWidth(int width) override;

    int getDataInPortDelay();
    int getConditionPortDelay();
    int getDataTruePortDelay();
    int getDataFalsePortDelay();    
    void setDataInPortDelay(int delay);
    void setConditionPortDelay(int delay);
    void setDataTruePortDelay(int delay);
    void setDataFalsePortDelay(int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Demux : public Block {

public:

    Demux(int defaultPortWidth = -1, int blockDelay = 0, int numControlPorts = 1);
    ~Demux();

    int getDataPortsWidth() override;
    void setDataPortsWidth(int width) override;

    int getControlPortDelay(int index);
    int getDataInPortDelay();
    int getDataOutPortDelay(int index);
    void setControlPortDelay(int index, int delay);
    void setDataInPortDelay(int delay);
    void setDataOutPortDelay(int index, int delay);

    static void resetCounter();

private:

    static int instanceCounter;

};


class Entry : public Block {

public:

    Entry(int defaultPortWidth = -1, int blockDelay = 0);
    ~Entry();
  
    static void resetCounter();

private:

    static int instanceCounter;

};


class Argument : public Block {

public:

    Argument(int defaultPortWidth = -1, int blockDelay = 0);
    ~Argument();

    static void resetCounter();

private:

    static int instanceCounter;

};


class Exit : public Block {

public:

    Exit(int defaultPortWidth = -1, int blockDelay = 0);
    ~Exit();

    static void resetCounter();

private:

    static int instanceCounter;

};


class Return : public Block {

public:

    Return(int defaultPortWidth = -1, int blockDelay = 0);
    ~Return();

    static void resetCounter();

private:

    static int instanceCounter;

};


#endif // BLOCKS_H