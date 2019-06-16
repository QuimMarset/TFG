#ifndef BLOCKS_H
#define BLOCKS_H

#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include "SupportTypes.h"
#include "llvm/IR/BasicBlock.h"

using namespace std;
using namespace llvm;

namespace DFGraphComp
{


class Block {

public:

    virtual ~Block();

    const BasicBlock* getParentBB();

    string getBlockName();
    
    BlockType getBlockType();
    
    void setBlockDelay(unsigned int blockDelay);

    virtual pair <Block*, int> getConnectedPort() = 0;
    virtual void setConnectedPort(pair <Block*, int> connection) = 0;
    virtual bool connectionAvailable() = 0;
    virtual unsigned int getConnectedPortIndex() = 0;

    virtual const Port& getInputPort(unsigned int index) = 0;

    virtual void printBlock(ostream& file) = 0;
    virtual void printChannels(ostream& file) = 0;

protected:

    Block();
    Block(const string &blockName, const BasicBlock* parentBB,
        BlockType type, unsigned int blockDelay);
    string blockName;
    BlockType blockType;
    unsigned int blockDelay;
    const BasicBlock* parentBB;

};


class Operator : public Block {

public:

    Operator(OpType opType, const BasicBlock* parentBB = nullptr, 
        int portWidth = -1, unsigned int blockDelay = 0, 
        unsigned int latency = 0, unsigned int II = 0);
    ~Operator();

    OpType getOpType();

    unsigned int addInputPort(int portWidth = -1, unsigned int portDelay = 0);

    void setLatency(unsigned int latency);
    void setII(unsigned int II);

    void setDataInPortWidth(unsigned int index, int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int index, unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void printBlock(ostream& file) override;
    void printChannels(ostream& file) override;

private:

    OpType opType;
    vector <Port> dataIn;
    Port dataOut;
    unsigned int latency;
    unsigned int II;
    pair <Block*, int> connectedPort;
    static vector<unsigned int> instanceCounter;

};


class Buffer : public Block {

public:

    Buffer(const BasicBlock* parentBB = nullptr, unsigned int slots = 2, 
        bool transparent = false, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Buffer();

    void setNumSlots(unsigned int slots);
    void setTransparent(bool transparent);

    void setDataInPortWidth(int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataIn;
    Port dataOut;
    unsigned int slots;
    bool transparent;
    pair <Block*, int> connectedPort;
    static unsigned int instanceCounter;

};


class ConstantInterf : public Block {

public:

    void setDataPortWidth(int width);

    void setControlPortDelay(unsigned int delay);
    void setDataPortDelay(unsigned int delay);
 
    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void printChannels(ostream& file) override;

protected:

    ConstantInterf();
    ConstantInterf(const BasicBlock* parentBB, int portWidth, 
        unsigned int blockDelay);
    virtual ~ConstantInterf();
    Port controlIn;
    Port dataOut;
    pair <Block*, int> connectedPort;
    static unsigned int instanceCounter;

};

template <typename T>
class Constant : public ConstantInterf {

public:

    Constant(T value, const BasicBlock* parentBB = nullptr, 
        int portWidth = -1, unsigned int blockDelay = 0);
    ~Constant();

    void setValue(T value);

    void printBlock(ostream &file) override;

private:

    T value;
};

template <typename T>
Constant<T>::Constant(T value, const BasicBlock* parentBB, 
    int portWidth, unsigned int blockDelay)
    : ConstantInterf(parentBB, portWidth, blockDelay)
{
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
void Constant<T>::printBlock(ostream &file) {
    file << blockName << "[type = Constant";
    file << ", in = \"" << controlIn << "\"";
    file << ", out = \"" << dataOut << "\"";
    bool first = true;
    if (control.getDelay() > 0) {
        file << ", delay = \"";
        first = false;
        file << controlIn.getName() << ":" << controlIn.getDelay();
    }
    if (dataOut.getDelay() > 0) {
        if (first) {
            first = false;
            file << ", delay = \"";
        }
        else file << " ";
        if (blockDelay > 0) file << blockDelay << " "; 
        file << " " << dataOut.getName() << ":" << dataOut.getDelay() << "\""; 
    }
    if (first and blockDelay > 0) file << ", delay = " << blockDelay;
    else if (!first) file << "\"";
    file << ", value = " << value;
    file << "];" << endl;
}


class Fork : public Block {

public:

    Fork(const BasicBlock* parentBB = nullptr, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Fork();

    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int index, unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;
    void setOutPort(unsigned int index, pair <Block*, int> connection);

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataIn;
    vector <Port> dataOut;
    static unsigned int instanceCounter;
    vector <pair <Block*, int> > connectedPorts;

};


class Merge : public Block {

public:

    Merge(const BasicBlock* parentBB = nullptr, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Merge();

    unsigned int addDataInPort(unsigned int delay = 0);

    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int index, unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    vector <Port> dataIn;
    Port dataOut;
    static unsigned int instanceCounter;
    pair <Block*, int> connectedPort;
};


class Select : public Block {

public:

    Select(const BasicBlock* parentBB = nullptr, int portWidth = -1, 
        unsigned int blockDelay = 0);
    ~Select();

    void setDataPortWidth(int width);

    void setDataTruePortDelay(unsigned int delay);
    void setDataFalsePortDelay(unsigned int delay);
    void setConditionPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataTrue;
    Port dataFalse;
    Port condition;
    Port dataOut;
    static unsigned int instanceCounter;
    pair <Block*, int> connectedPort;

};

class Branch : public Block {

public:

    Branch(const BasicBlock* parentBB = nullptr, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Branch();

    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int delay);
    void setConditionPortDelay(unsigned int delay);
    void setDataTruePortDelay(unsigned int delay);
    void setDataFalsePortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void setCurrentPort(bool currentPort);

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataIn;
    Port condition;
    Port dataTrue;
    Port dataFalse;
    static unsigned int instanceCounter;
    pair <Block*, int> connectedPortTrue;
    pair <Block*, int> connectedPortFalse;
    bool currentPort;
    
};


class Demux : public Block {

public:

    Demux(const BasicBlock* parentBB = nullptr, int portWidth = -1, 
        unsigned int blockDelay = 0);
    ~Demux();

    unsigned int addControlInPort(unsigned int delay = 0);
    void addDataOutPort(unsigned int delay = 0);
    
    void setDataPortWidth(int width);

    void setControlPortDelay(unsigned int index, unsigned int delay);
    void setDataInPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int index, unsigned int delay);

    void setCurrentConnectedPort(int current);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    vector <Port> control;
    Port dataIn;
    vector <Port> dataOut;
    static unsigned int instanceCounter;
    vector <pair <Block*, int> > connectedPorts;
    int currentConnected;

};

class EntryInterf : public Block {

public:

    void setInPortDelay(unsigned int delay);
    void setOutPortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    virtual void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;


protected:
    EntryInterf();
    EntryInterf(const string& blockName, const BasicBlock* parentBB, 
        int portWidth = -1, unsigned int blockDelay = 0);
    virtual ~EntryInterf();
    Port inPort;
    Port outPort;
    pair <Block*, int> connectedPort;

};


class Entry : public EntryInterf {

public:

    Entry(const BasicBlock* parentBB = nullptr, unsigned int blockDelay = 0);
    ~Entry();
  
private:

    static unsigned int instanceCounter;

};


class Argument : public EntryInterf {

public:

    Argument(const BasicBlock* parentBB = nullptr, int portWidth = -1, 
        unsigned int blockDelay = 0);
    ~Argument();

    void setDataPortWidth(int width);

private:

    static unsigned int instanceCounter;

};


class ExitInterf : public Block {

public:

    void setInPortDelay(unsigned int delay);
    void setOutPortDelay(unsigned int delay);

    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;
    const Port& getInputPort(unsigned int index) override;

    virtual void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;


protected:
    ExitInterf();
    ExitInterf(const string& blockName, const BasicBlock* parentBB, 
        int portWidth = -1, unsigned  int blockDelay = 0);
    virtual ~ExitInterf();
    Port inPort;
    Port outPort;
    pair <Block*, int> connectedPort;

};


class Exit : public ExitInterf {

public:

    Exit(const BasicBlock* parentBB = nullptr, unsigned int blockDelay = 0);
    ~Exit();

private:

    static unsigned int instanceCounter;

};


class Return : public ExitInterf {

public:

    Return(const BasicBlock* parentBB = nullptr, int portWidth = -1, 
        unsigned int blockDelay = 0);
    ~Return();
    
    void setDataPortWidth(int width);

private:

    static unsigned int instanceCounter;

};


class FunctionCall : public Block {

public:

    FunctionCall(const BasicBlock* parentBB = nullptr);
    ~FunctionCall();

    void setConnectedPortResult(pair <Block*, int> connection);
    void setConnectedPortControl(pair <Block*, int> connection);
    
    pair <Block*, int> getConnectedPort() override;
    void setConnectedPort(pair <Block*, int> connection) override;
    bool connectionAvailable() override;
    unsigned int getConnectedPortIndex() override;

    void addInputArgPort(pair <Block*, int> connection);
    pair <Block*, int> getInputArgPort(unsigned int index);

    void setInputContPort(pair <Block*, int> connection);
    pair <Block*, int> getInputContPort();

    pair <Block*, int> getConnecDataPort();
    pair <Block*, int> getConnecControlPort();

    const Port& getInputPort(unsigned int index) override;

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:
    
    vector <pair <Block*, int> > inputArgumentPorts;
    pair <Block*, int> inputControlPort;
    pair <Block*, int> connectedResultPort;
    pair <Block*, int> connectedControlPort;

};


} // Close namespace


#endif // BLOCKS_H