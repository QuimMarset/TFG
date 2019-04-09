#ifndef BlockS_H
#define BlockS_H

#include <vector>
#include <string>
#include <fstream>
#include <map>
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
    void setBlockName(const string &blockName);
    BlockType getBlockType();
    void setBlockType(BlockType blockType);
    void addInputPort(const Port &inPort);
    void addOutputPort(const Port &outPort);

    virtual void printBlock(ofstream &file);
    void closeBlock(ofstream &file);

private:

    string blockName;
    AttributeUniValue <BlockType> blockType;
    AttributeMultiValue <Port> inputPorts;
    AttributeMultiValue <Port> outputPorts; 
    
};


class Operator : public Block {

public:
    
    Operator();
    Operator(const string &name, int numInPorts, int latency, int II);
    ~Operator();

    void printBlock(ofstream &file) override;

private:

    AttributeUniValue<int> latency;
    AttributeUniValue<int> II;

};


class Buffer : public Block {

public:

    Buffer();
    Buffer(const string &name, int slots, bool transparent);
    ~Buffer();
    void printBlock(ofstream &file) override;

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

    void printBlock(ofstream &file) override;

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
    return constant;
}

template <typename T>
void Constant<T>::setConstant(T constant) {
    this->constant = constant;
}

template <typename T>
void Constant<T>::printBlock(ofstream &file) {
    Block::printBlock(file);
    file << ", ";
    constant.printAttribute(file);
}


class Fork : public Block {

public:

    Fork();
    Fork(const string &name, int numOutPorts);
    ~Fork();

private:

};


class Merge : public Block {

public:

    Merge();
    Merge(const string &name, int numInPorts);
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
    Demux(const string &name, int numControlPorts);
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