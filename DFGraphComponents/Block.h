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

    Block();
    Block(const string &blockName, BlockType type, int blockDelay);
    ~Block();

    string getBlockName();
    void setBlockName(const string &blockName);
    
    BlockType getBlockType();
    void setBlockType(BlockType blockType);
    
    void setBlockDelay(int blockDelay);

    virtual pair <Block*, const Port*> getConnectedPort() = 0;
    virtual void setConnectedPort(pair <Block*, const Port*> connection) = 0;
    virtual bool connectionAvailable() = 0;

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

    pair <Block*, const Port*> getConnectedPort();
    void setConnectedPort(pair <Block*, const Port*> connection);
    bool connectionAvailable();

protected:

    Port dataOut;
    int latency;
    int II;
    pair <Block*, const Port*> connectedPort;
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

    const Port* getDataInPort();

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

    const Port* getDataIn1Port();
    const Port* getDataIn2Port();

    void printBlock(ostream& file) override;

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

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInPort();

    void printBlock(ostream &file) override;

private:

    Port dataIn;
    Port dataOut;
    static int instanceCounter;
    int slots;
    bool transparent;
    pair <Block*, const Port*> connectedPort;

};

class ConstantInterf : public Block {

public:

    ~ConstantInterf();

    void setDataPortWidth(int width);

    void setControlPortDelay(int delay);
    void setDataPortDelay(int delay);
 
    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getControlInPort();

protected:

    ConstantInterf();
    ConstantInterf(const string& blockName, int porWidth = -1, int blockDelay = 0);
    Port control;
    Port data;
    pair <Block*, const Port*> connectedPort;

};

template <typename T>
class Constant : public ConstantInterf {

public:

    Constant(T value, int portWidth = -1, int blockDelay = 0);
    ~Constant();

    void setValue(T value);

    static void resetCounter();

    void printBlock(ostream &file) override;

private:

    static int instanceCounter;
    T value;
};

template <typename T>
int Constant<T>::instanceCounter = 1;

template <typename T>
Constant<T>::Constant(T value, int portWidth, int blockDelay)
        : ConstantInterf("Constant" + to_string(instanceCounter), 
        portWidth, blockDelay), control("control", Port::Base, 0), 
        data("out"), connectedPort(nullptr, nullptr) 
{
    ++instanceCounter;
    this->value = value;
    if (portWidth != -1) data.setWidth(portWidth);
    else data.setWidth(sizeof(T)*8);
}

template <typename T>
Constant<T>::~Constant() {}

template <typename T>
void Constant<T>::setValue(T value) {
    this->value = value;
}

template <typename T>
void Constant<T>::resetCounter() {
    instanceCounter = 1;
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
    file << "value = " << value;
    file << "];" << endl;
}


class Fork : public Block {

public:

    Fork(int blockDelay = 0);
    ~Fork();

    void setDataInPortWidth(int width);
    void setDataOutPortWidth(int index, int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(int delay);
    void setDataOutPortDelay(int index, int delay);

    static void resetCounter();

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInPort();

    void printBlock(ostream &file) override;

private:

    Port dataIn;
    vector <Port> dataOut;
    static int instanceCounter;
    vector <pair <Block*, const Port*> > connectedPorts;

};


class Merge : public Block {

public:

    Merge(int blockDelay = 0);
    ~Merge();

    const Port* addDataInPort(int width = -1, int delay = 0);

    void setDataInPortWidth(int index, int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(int index, int delay);
    void setDataOutPortDelay(int delay);

    static void resetCounter();

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInPort(int index);

    void printBlock(ostream &file) override;

private:

    vector <Port> dataIn;
    Port dataOut;
    static int instanceCounter;
    pair <Block*, const Port*> connectedPort;
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

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInTruePort();
    const Port* getDataInFalsePort();
    const Port* getConditionInPort();

    void printBlock(ostream &file) override;

private:

    Port dataTrue;
    Port dataFalse;
    Port condition;
    Port dataOut;
    static int instanceCounter;
    pair <Block*, const Port*> connectedPort;

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

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    pair <Block*, const Port*> getConnectedPortTrue();
    pair <Block*, const Port*> getConnectedPortFalse();
    void setConnectedPortTrue(pair <Block*, const Port*> connection);
    void setConnectedPortFalse(pair <Block*, const Port*> connection);
    bool connectionTrueAvailable();
    bool connectionFalseAvailable();

    const Port* getDataInPort();
    const Port* getConditionInPort();

    void printBlock(ostream &file) override;

private:

    Port dataIn;
    Port condition;
    Port dataTrue;
    Port dataFalse;
    static int instanceCounter;
    pair <Block*, const Port*> connectedPortTrue;
    pair <Block*, const Port*> connectedPortFalse;
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

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getControlInPort(int index);
    const Port* getDataInPort();

    void printBlock(ostream &file) override;

private:

    vector <Port> control;
    Port dataIn;
    vector <Port> dataOut;
    static int instanceCounter;
    vector <pair <Block*, const Port*> > connectedPorts;
    int nextOutPort;

};

class EntryInterf : public Block {

public:

    void setInPortDelay(int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    void printBlock(ostream &file) override;


protected:
    EntryInterf();
    EntryInterf(const string& blockName, int portWidth = -1, int blockDelay = 0);
    ~EntryInterf();
    Port outPort;
    pair <Block*, const Port*> connectedPort;

};


class Entry : public EntryInterf {

public:

    Entry(int blockDelay = 0);
    ~Entry();

    static void resetCounter();
  
private:

    static int instanceCounter;
};


class Argument : public EntryInterf {

public:

    Argument(int width, int blockDelay = 0);
    ~Argument();

    void setDataPortWidth(int width);

    void setControlPortDelay(int delay);

    const Port* getControlInPort();
    
    static void resetCounter();

private:

    Port inControl;
    static int instanceCounter;

};


class ExitInterf : public Block {

public:

    void setInPortDelay(int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getInPort();

    void printBlock(ostream &file) override;


protected:
    ExitInterf();
    ExitInterf(const string& blockName, int width = -1, int delay = 0);
    ~ExitInterf();
    Port inPort;

};


class Exit : public ExitInterf {

public:

    Exit(int blockDelay = 0);
    ~Exit();

    static void resetCounter();

private:

    static int instanceCounter;

};


class Return : public ExitInterf {

public:

    Return(int portWidth = -1, int blockDelay = 0);
    ~Return();
    
    void setDataPortWidth(int width);
    
    static void resetCounter();

private:

    static int instanceCounter;

};

class Store : public ExitInterf {

public:

    Store(int blockDelay = 0);
    ~Store();
    
    void setDataPortWidth(int width);
    void setAddrPortWidth(int width);

    const Port* getAddrPort();
    const Port* getAlignPort();
    
    static void resetCounter();

    virtual void printBlock(ostream& file) override;

private:

    static int instanceCounter;
    Port inPortAddr;
    Port inPortAlign;

};


} // Close namespace


#endif // BLOCKS_H