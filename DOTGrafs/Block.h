#ifndef Blocks_H
#define Blocks_H

#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include "AttributeUniValue.h"
#include "AttributeMultiValue.h"
#include "SupportTypes.h"
using namespace std;

class Block {

public:

    Block();
    Block(const string &blockName, BlockType type, int defaultPortWidth = 0, 
        int blockDelay = 0);
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
    
    Operator();
    Operator(const string &name, int numInPorts = 2, bool hasOutput = true, 
        int latency = 0, int II = 0);
    ~Operator();

    int getLatency();
    int getII();
    void setLatency(int latency);
    void setII(int II);

    int getInDataPortDelay(int index);
    int getOutDataPortDelay();
    void setInDataPortDelay(int index, int delay);
    void setOutDataPortDelay(int delay);

    void printBlock(ostream &file) const override;

private:

    int latency;
    int II;

};


class Buffer : public Block {

public:

    Buffer();
    Buffer(const string &name, int slots = 2, bool transparent = false);
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

private:

    int slots;
    bool transparent;
};


template <typename T>
class Constant : public Block {

public:

    Constant();
    Constant(const string &name, T value);
    ~Constant();

    T getConstant();
    void setConstant(T constant);

    int getDataPortsWidth() override;
    void setDataPortsWidth(int width) override;

    int getOutDataPortDelay();
    void setOutDataPortDelay(int delay);

    void printBlock(ostream &file) const override;

private:

    T constant;

};

template <typename T>
Constant<T>::Constant() : Block() {}

template <typename T>
Constant<T>::Constant(const string &name, T constant)
                        : Block(name, BlockType::Constant_Block) {
    this->constant = constant;
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
void Constant<T>::setOutDataPortDelay(int delay) {
    Block::setOutPortDelay(0, delay);
}

template <typename T>
void Constant<T>::printBlock(ostream &file) const {
    Block::printBlock(file);
    file << ", ";
    file << "value = " << constant;
}


class Fork : public Block {

public:

    Fork();
    Fork(const string &name, int numOutPorts = 2);
    ~Fork();

    int getInDataPortDelay();
    int getOutDataPortDelay(int index);
    void setInDataPortDelay(int delay);
    void setOutDataPortDelay(int index, int delay);

private:

};


class Merge : public Block {

public:

    Merge();
    Merge(const string &name, int numInPorts = 2);
    ~Merge();

    int getInDataPortDelay(int index);
    int getOutDataPortDelay();
    void setInDataPortDelay(int index, int delay);
    void setOutDataPortDelay(int delay);

private:

};


class Select : public Block {

public:

    Select();
    Select(const string &name);
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


private:

};

class Branch : public Block {

public:

    Branch();
    Branch(const string &name);
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

private:

};


class Demux : public Block {

public:

    Demux();
    Demux(const string &name, int numControlPorts = 1);
    ~Demux();

    int getDataPortsWidth() override;
    void setDataPortsWidth(int width) override;

    int getControlPortDelay(int index);
    int getDataInPortDelay();
    int getDataOutPortDelay(int index);
    void setControlPortDelay(int index, int delay);
    void setDataInPortDelay(int delay);
    void setDataOutPortDelay(int index, int delay);


private:

};


class Entry : public Block {

public:

    Entry();
    Entry(const string &name);
    ~Entry();

private:

};


class Exit : public Block {

public:

    Exit();
    Exit(const string &name);
    ~Exit();

private:

};


#endif // BlockS_H