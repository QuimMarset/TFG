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
    Component(string &name, const BlockType &type);
    Component(string &name, const BlockType &type,
                 const vector<Port> &input, const vector <Port> &output);
    ~Component();

    string getBlockName();
    void setBlockName(string &blockName);
    BlockType getBlockType();
    void setBlockType(BlockType &blockType);
    void addInputPort(Port &inPort);
    void addOutputPort(Port &outPort);

    virtual void printBlock(ofstream &file);
    void closeBlock(ofstream &file);

    friend ofstream &operator << (ofstream &out, const Component::Port &p); 
    friend ofstream &operator << (ofstream &out, const Component::BlockType &blockType);
    friend bool operator == (Component::Port p1, Component::Port p2);
    
private:

    string blockName;
    AttributeUniValue <BlockType> blockType;
    AttributeMultiValue <Port> inputPorts;
    AttributeMultiValue <Port> outputPorts; 
    
};

class Operator : public Component {

public:
    
    Operator();
    Operator(string &name, const vector<Port> &input, 
        const vector <Port> &output, int latency, int II);
    ~Operator();

    void printBlock(ofstream &file) override;

private:

    AttributeUniValue<int> latency;
    AttributeUniValue<int> II;

};


class Buffer : public Component {

public:

    Buffer();
    Buffer(string &name, const vector<Port> &input, 
        const vector <Port> &output, int slots, bool transparent);
    ~Buffer();
    void printBlock(ofstream &file) override;

private:

    AttributeUniValue<int> slots;
    AttributeUniValue<bool> transparent;

};


class Select : public Component {

public:

    Select();
    Select(string &name);
    ~Select();

private:

};

class Branch : public Component {

public:

    Branch();
    Branch(string &name);
    ~Branch();

private:

};


template <typename T>
class Constant : public Component {

public:

    Constant();
    Constant(string &name, int slots, bool transparent, T constant);
    ~Constant();

    T getConstant();
    void setConstant(T constant);

    void printBlock(ofstream &file) override;

private:

    AttributeUniValue<T> constant;

};


#endif // COMPONENTS_H