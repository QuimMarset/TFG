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

    Block(const string &blockName, BlockType type, int blockDelay);
    ~Block();

    string getBlockName();
    void setBlockName(const string &blockName);
    
    BlockType getBlockType();
    void setBlockType(BlockType blockType);
    
    void setBlockDelay(int blockDelay);

    virtual pair <Block*, int> getConnectedPort() = 0;
    virtual void setConnectedPort(pair <Block*, int> connection) = 0;
    virtual bool connectionAvailable() = 0;

    virtual string getInPortName(int index) = 0;

    virtual void printBlock(ostream &file) = 0;

protected:

    string blockName;
    BlockType blockType;
    int blockDelay;

};


class Operator : public Block {

public:

    Operator(const string& blockName, int blockDelay = 0, 
        int latency = 0, int II = 0);
    ~Operator();

    void setLatency(int latency);
    void setII(int II);

    void setDataOutPortWidth(int widt);

    void setDataOutPortDelay(int delay);

    virtual void setDataPortWidth(int width) = 0;

    pair <Block*, int> getConnectedPort();
    void setConnectedPort(pair <Block*, int> connection);
    bool connectionAvailable();

protected:

    Port dataOut;
    int latency;
    int II;
    pair <Block*, int> connectedPort;
};

class UnaryOperator : public Operator {

public:

    UnaryOperator(UnaryOpType opType, int blockDelay = 0,
        int latency = 0, int II = 0);
    ~UnaryOperator();

    void setOpType(UnaryOpType type);

    void setDataInPortWidth(int width);

    void setDataPortWidth(int width) override;

    void setDataInPortDelay(int delay);

    static void resetCounter();

    string getInPortName(int index) override;

    void printBlock(ostream& file) override;

private:

    Port dataIn;
    UnaryOpType opType;
    static vector<int> instanceCounter;

};

class BinaryOperator : public Operator {

public:

    BinaryOperator(BinaryOpType opType, int blockDelay = 0,
        int latency = 0, int II = 0);
    ~BinaryOperator();

    void setOpType(BinaryOpType type);

    void setDataIn1PortWidth(int width);
    void setDataIn2PortWidth(int width);
    void setDataPortWidth(int width) override;

    void setDataIn1PortDelay(int delay);
    void setDataIn2PortDelay(int delay);

    static void resetCounter();

    void printBlock(ostream& file) override;

    string getInPortName(int index) override;

private:

    Port dataIn1;
    Port dataIn2;
    BinaryOpType opType;
    static vector<int> instanceCounter;

};


class Buffer : public Block {

public:

    Buffer(int slots = 2, bool transparent = false, int blockDelay = 0);
    ~Buffer();

    void setNumSlots(int slots);
    void setTransparent(bool transparent);

    void setDataInPortWidth(int width);
    void setDataOutPortWidth(int width);

    void setDataInPortDelay(int delay);
    void setDataOutPortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port dataIn;
    Port dataOut;
    static int instanceCounter;
    int slots;
    bool transparent;
    pair <Block*, int> connectedPort;

};


template <typename T>
class Constant : public Block {

public:

    Constant(T value, int blockDelay = 0);
    ~Constant();

    void setConstant(T constant);

    void setDataPortWidth(int width);

    void setControlPortDelay(int delay);
    void setDataPortDelay(int delay);
 
    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port control;
    Port data;
    static int instanceCounter;
    T constant;
    pair <Block*, int> connectedPort;

};

template <typename T>
int Constant<T>::instanceCounter = 1;

template <typename T>
Constant<T>::Constant(T constant, int blockDelay)
        : Block("Constant" + to_string(instanceCounter), 
        BlockType::Constant_Block, defaultPortWidth, blockDelay), 
        control("control", Port::Base, 0), data("out"),
        connectedPort(nullptr, -1) 
{
    ++instanceCounter;
    this->constant = constant;
}

template <typename T>
Constant<T>::~Constant() {}

template <typename T>
void Constant<T>::setConstant(T constant) {
    this->constant = constant;
}

template <typename T>
void Constant<T>::setDataPortWidth(int width) {
    assert(width >= 0);
    data.setWidth(width);
}

template <typename T>
void Constant<T>::setDataPortDelay(int delay) {
    assert(delay >= 0);
    data.setDelay(delay);
}

template <typename T>
void Constant<T>::setControlPortDelay(int delay) {
    assert(delay >= 0);
    control.setDelay(delay);
}

template <typename T>
pair <Block*, int> Constant<T>::getConnectedPort() {
    return connectedPort;
}

template <typename T>
void Constant<T>::setConnectedPort(pair <Block*, int> connection) {
    connectedPort = connection;
}

template <typename T>
bool Constant<T>::connectionAvailable() {
    return (connectedPort.first == nullptr and
        connectedPort.second == -1);
}

template <typename T>
string Constant<T>::getInPortName(int index) {
    return control.getName();
}

template <typename T>
void Constant<T>::printBlock(ostream &file) {
    file << blockName << "[type = Constant";
    if (defaultPortWidth >= 0) file << defaultPortWidth;
    file << ", in = \"" << control << "\"";
    file << ", out = \"" << data << "\"";
    if (control.getDelay() > 0) {
        file << ", delay = \"" ;
        file << control.getName() << ":" << control.getDelay() 
        if (blockDelay > 0) file << " " << blockDelay;
        if (dataOut.getDelay > 0) file << " " << data.getName() << 
            ":" << data.getDelay();
        file << "\"";
    }
    else if (data.getDelay > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay;
        file << " " << data.getName() << 
            ":" << data.getDelay() << "\""; 
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << ", ";
    file << "value = " << constant;
    file << "];" << endl;
}


class Fork : public Block {

public:

    Fork(int numOutPorts = 2, int blockDelay = 0);
    ~Fork();

    void setDataInPortWidth(int width);
    void setDataOutPortWidth(int index, int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(int delay);
    void setDataOutPortDelay(int index, int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port dataIn;
    vector <Port> dataOut;
    static int instanceCounter;
    vector <pair <Block*, int> > connectedPorts;
    void addOutPort();

};


class Merge : public Block {

public:

    Merge(int numInPorts = 2, int blockDelay = 0);
    ~Merge();

    void setDataInPortWidth(int index, int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(int index, int delay);
    void setDataOutPortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    vector <Port> dataIn;
    Port dataOut;
    static int instanceCounter;
    pair <Block*, int> connectedPort;
};


class Select : public Block {

public:

    Select(int blockDelay = 0);
    ~Select();

    void setDataTruePortWidth(int width);
    void setDataFalsePortWidth(int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataTruePortDelay(int delay);
    void setDataFalsePortDelay(int delay);
    void setConditionPortDelay(int delay);
    void setDataOutPortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port dataTrue;
    Port dataFalse;
    Port condition;
    Port dataOut;
    static int instanceCounter;
    pair <Block*, int> connectedPort;

};

class Branch : public Block {

public:

    Branch(int blockDelay = 0);
    ~Branch();

    void setDataInPortWidth(int width);
    void setDataTruePortWidth(int width);
    void setDataFalsePortWidth(int width);  
    void setDataPortWidth(int width);

    void setDataInPortDelay(int delay);
    void setConditionPortDelay(int delay);
    void setDataTruePortDelay(int delay);
    void setDataFalsePortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    pair <Block*, int> getConnectedPortTrue();
    pair <Block*, int> getConnectedPortFalse();
    void setConnectedPortTrue(pair <Block*, int> connection);
    void setConnectedPortFalse(pair <Block*, int> connection);
    bool connectionTrueAvailable();
    bool connectionFalseAvailable();

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port dataIn;
    Port condition;
    Port dataTrue;
    Port dataFalse;
    static int instanceCounter;
    pair <Block*, int> connectedPortTrue;
    pair <Block*, int> connectedPortFalse;
    int nextOutPort;

};


class Demux : public Block {

public:

    Demux(int numControlPorts = 1, int blockDelay = 0);
    ~Demux();

    void setDataInPortWidth(int width);
    void setDataOutPortWidth(int index, int width);
    void setDataPortWidth(int width);

    void setControlPortDelay(int index, int delay);
    void setDataInPortDelay(int delay);
    void setDataOutPortDelay(int index, int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    vector <Port> control;
    Port dataIn;
    vector <Port> dataOut;
    static int instanceCounter;
    vector <pair <Block*, int> > connectedPorts;
    int nextOutPort;

};


class Entry : public Block {

public:

    Entry(int blockDelay = 0);
    ~Entry();

    void setControlPortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;
  
private:

    Port control;
    static int instanceCounter;
    pair <Block*, int> connectedPort;

};


class Argument : public Block {

public:

    Argument(int blockDelay = 0);
    ~Argument();

    void setDataPortWidth(int width);

    void setDataPortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port data;
    static int instanceCounter;
    pair <Block*, int> connectedPort;

};


class Exit : public Block {

public:

    Exit(int blockDelay = 0);
    ~Exit();
    void setControlPortDelay(int delay);

    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;
    
    void printBlock(ostream &file) override;

private:

    Port control;
    static int instanceCounter;

};


class Return : public Block {

public:

    Return(int blockDelay = 0);
    ~Return();
    
    void setDataPortWidth(int width);

    void setDataPortDelay(int delay);
    
    static void resetCounter();

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;

    string getInPortName(int index) override;

    void printBlock(ostream &file) override;

private:

    Port data;
    static int instanceCounter;

};


} // Close namespace


#endif // BLOCKS_H