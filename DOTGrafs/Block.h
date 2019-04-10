#ifndef BlockS_H
#define BlockS_H

#include <vector>
#include <string>
#include <fstream>
#include "AttributeUniValue.h"
#include "AttributeMultiValue.h"
#include "SupportTypes.h"
using namespace std;

class Block {

public:

    Block();
    Block(const string &blockName, BlockType type);
    ~Block();

    string getBlockName();
    BlockType getBlockType();
    void setBlockName(const string &blockName);
    void setBlockType(BlockType blockType);

    string getInPortName(int index);
    int getInPortDelay(int index);
    Port::PortType getInPortType(int index);
    void setInPortName(int index, const string &name);
    void setInPortDelay(int index, int delay);
    void setInPortType(int index, Port::PortType type);

    string getOutPortName(int index);
    int getOutPortDelay(int index);
    Port::PortType getOutPortType(int index);
    void setOutPortName(int index, const string &name);
    void setOutPortDelay(int index, int delay);
    void setOutPortType(int index, Port::PortType type);

    virtual void printBlock(ostream &file) const;
    void closeBlock(ostream &file) const;
    friend ostream &operator << (ostream& out, const Block &block);

protected:

    void addInputPort(const Port &inPort);
    void addOutputPort(const Port &outPort);

private:

    string blockName;
    AttributeUniValue <BlockType> blockType;
    AttributeMultiValue <Port> inputPorts;
    AttributeMultiValue <Port> outputPorts; 
    
};


class Operator : public Block {

public:
    
    Operator();
    Operator(const string &name, int latency, int II, int numInPorts = 2);
    ~Operator();

    int getLatency();
    int getII();
    void setLatency(int latency);
    void setII(int II);

    void printBlock(ostream &file) const override;

private:

    AttributeUniValue<int> latency;
    AttributeUniValue<int> II;

};


class Buffer : public Block {

public:

    Buffer();
    Buffer(const string &name, int slots, bool transparent);
    ~Buffer();

    int getNumSlots();
    bool getTransparent();
    void setNumSlots(int slots);
    void setTransparent(bool transparent);

    void printBlock(ostream &file) const override;

private:

    AttributeUniValue<int> slots;
    AttributeUniValue<bool> transparent;

};


template <typename T>
class Constant : public Block {

public:

    Constant();
    Constant(const string &name, T value);
    ~Constant();

    T getConstant();
    void setConstant(T constant);

    void printBlock(ostream &file) const override;

private:

    AttributeUniValue<T> constant;

};

template <typename T>
Constant<T>::Constant() : Block(), constant("value") {}

template <typename T>
Constant<T>::Constant(const string &name, T constant)
                        : Block(name, BlockType::Constant_Block), 
                        constant("value", constant) {
    addOutputPort(Port("out"));
}

template <typename T>
Constant<T>::~Constant() {}

template <typename T>
T Constant<T>::getConstant() {
    return constant.getValue();
}

template <typename T>
void Constant<T>::setConstant(T constant) {
    this->constant.setValue(constant);
}

template <typename T>
void Constant<T>::printBlock(ostream &file) const {
    Block::printBlock(file);
    file << ", ";
    constant.printAttribute(file);
}


class Fork : public Block {

public:

    Fork();
    Fork(const string &name, int numOutPorts = 2);
    ~Fork();

private:

};


class Merge : public Block {

public:

    Merge();
    Merge(const string &name, int numInPorts = 2);
    ~Merge();

private:

};


class Select : public Block {

public:

    Select();
    Select(const string &name);
    ~Select();

private:

};

class Branch : public Block {

public:

    Branch();
    Branch(const string &name);
    ~Branch();

private:

};


class Demux : public Block {

public:

    Demux();
    Demux(const string &name, int numControlPorts = 1);
    ~Demux();

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