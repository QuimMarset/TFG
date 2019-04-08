#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include "Attribute.h"
#include "AttributeUniValue.h"
#include "AttributeMultiValue.h"
using namespace std;

class Component {

public:

    struct Port {

        enum PortType {
            Base = 0,
            Condition,
            True,
            False
        };

        string name;
        int delay;
        PortType type;

        string getName();
        int getDelay();
        PortType getType();
        void setName(string name);
        void setDelay(int delay);
        void setType(PortType type);

    };

    enum BlockType {
        Operator = 0,
        Buffer,
        Constant,
        Fork,
        Merge,
        Select,
        Branch,
        Demux,
        Entry,
        Exit
    };

    Component();
    Component(const string &blockName, BlockType type);
    ~Component();

    string getBlockName();
    void setBlockName(const string &blockName);
    BlockType getBlockType();
    void setBlockType(BlockType blockType);
    void addInputPort(const Port &inPort);
    void addOutputPort(const Port &outPort);

    virtual void printBlock(ofstream &file);
    void closeBlock(ofstream &file);

    friend ofstream &operator << (ofstream &out, const Component::Port &p); 
    friend ofstream &operator << (ofstream &out, Component::BlockType blockType);
    friend bool operator == (const Component::Port &p1, const Component::Port &p2);
    
private:

    string blockName;
    AttributeUniValue <BlockType> blockType;
    AttributeMultiValue <Port> inputPorts;
    AttributeMultiValue <Port> outputPorts; 
    
};

class Operator : public Component {

public:
    
    Operator();
    Operator(const string &name, int numInPorts, int latency, int II);
    ~Operator();

    void printBlock(ofstream &file) override;

private:

    AttributeUniValue<int> latency;
    AttributeUniValue<int> II;

};


class Buffer : public Component {

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
class Constant : public Component {

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
Constant<T>::Constant() : Component(), constant("value") {}

template <typename T>
Constant<T>::Constant(const string &name, T constant)
                        : Component(name, BlockType::Constant), constant("value") {
    Port out = {"out", 0, Port::PortType::Base};
    addOutputPort(out);
    this->constant.setValue(constant);
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
    Component::printBlock(file);
    file << ", ";
    constant.printAttribute(file);
}





class Fork : public Component {

public:

    Fork();
    Fork(const string &name, int numOutPorts);
    ~Fork();

private:

};


class Merge : public Component {

public:

    Merge();
    Merge(const string &name, int numInPorts);
    ~Merge();

private:

};


class Select : public Component {

public:

    Select();
    Select(const string &name);
    ~Select();

private:

};

class Branch : public Component {

public:

    Branch();
    Branch(const string &name);
    ~Branch();

private:

};


class Demux : public Component {

public:

    Demux();
    Demux(const string &name, int numControlPorts);
    ~Demux();

private:

};


class Entry : public Component {

public:

    Entry();
    Entry(const string &name);
    ~Entry();

private:

};


class Exit : public Component {

public:

    Exit();
    Exit(const string &name);
    ~Exit();

private:

};


#endif // COMPONENTS_H