#ifndef BLOCKS_H
#define BLOCKS_H

#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include "SupportTypes.h"
#include <iostream>
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

    virtual pair <Block*, const Port*> getConnectedPort() = 0;
    virtual void setConnectedPort(pair <Block*, const Port*> connection) = 0;
    virtual bool connectionAvailable() = 0;

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

    Operator(OpType opType, const BasicBlock* parentBB, 
        int portWidth = -1, unsigned int blockDelay = 0, 
        unsigned int latency = 0, unsigned int II = 0);
    ~Operator();

    OpType getOpType();

    const Port* addInputPort(int portWidth = -1, unsigned int portDelay = 0);

    void setLatency(unsigned int latency);
    void setII(unsigned int II);

    void setDataInPortWidth(unsigned int index, int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int index, unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    const Port* getDataInPort(unsigned int index);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    void printBlock(ostream& file) override;
    void printChannels(ostream& file) override;

private:

    OpType opType;
    vector <Port> dataIn;
    Port dataOut;
    unsigned int latency;
    unsigned int II;
    pair <Block*, const Port*> connectedPort;
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

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInPort();

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataIn;
    Port dataOut;
    unsigned int slots;
    bool transparent;
    pair <Block*, const Port*> connectedPort;
    static unsigned int instanceCounter;

};


class ConstantInterf : public Block {

public:

    void setDataPortWidth(int width);

    void setControlPortDelay(unsigned int delay);
    void setDataPortDelay(unsigned int delay);
 
    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getControlInPort();

    void printChannels(ostream& file) override;

protected:

    ConstantInterf();
    ConstantInterf(const BasicBlock* parentBB, int portWidth, 
        unsigned int blockDelay);
    virtual ~ConstantInterf();
    Port control;
    Port data;
    pair <Block*, const Port*> connectedPort;
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
    file << ", in = \"" << control << "\"";
    file << ", out = \"" << data << "\"";
    if (control.getDelay() > 0) {
        file << ", delay = \"" ;
        file << control.getName() << ":" << control.getDelay();
        if (blockDelay > 0) file << " " << blockDelay;
        if (data.getDelay() > 0) file << " " << data.getName() << 
            ":" << data.getDelay();
        file << "\"";
    }
    else if (data.getDelay() > 0) {
        file << ", delay = \"";
        if (blockDelay > 0) file << blockDelay;
        file << " " << data.getName() << 
            ":" << data.getDelay() << "\""; 
    }
    else if (blockDelay > 0) file << ", delay = " << blockDelay;
    file << ", value = " << value;
    file << "];" << endl;
}


class Fork : public Block {

public:

    Fork(const BasicBlock* parentBB = nullptr, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Fork();

    void setDataInPortWidth(int width);
    void setDataOutPortWidth(unsigned int index, int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int index, unsigned int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInPort();

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataIn;
    vector <Port> dataOut;
    static unsigned int instanceCounter;
    vector <pair <Block*, const Port*> > connectedPorts;

};


class Merge : public Block {

public:

    Merge(const BasicBlock* parentBB = nullptr, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Merge();

    const Port* addDataInPort(unsigned int delay = 0);

    void setDataInPortWidth(unsigned int index, int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int index, unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInPort(unsigned int index);

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    vector <Port> dataIn;
    Port dataOut;
    static unsigned int instanceCounter;
    pair <Block*, const Port*> connectedPort;
};


class Select : public Block {

public:

    Select(const BasicBlock* parentBB = nullptr, int portWidth = -1, 
        unsigned int blockDelay = 0);
    ~Select();

    void setDataTruePortWidth(int width);
    void setDataFalsePortWidth(int width);
    void setDataOutPortWidth(int width);
    void setDataPortWidth(int width);

    void setDataTruePortDelay(unsigned int delay);
    void setDataFalsePortDelay(unsigned int delay);
    void setConditionPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getDataInTruePort();
    const Port* getDataInFalsePort();
    const Port* getConditionInPort();

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataTrue;
    Port dataFalse;
    Port condition;
    Port dataOut;
    static unsigned int instanceCounter;
    pair <Block*, const Port*> connectedPort;

};

class Branch : public Block {

public:

    enum BranchCurrentPort {None = -1, False, True};

    Branch(const BasicBlock* parentBB = nullptr, int portWidth = -1,
        unsigned int blockDelay = 0);
    ~Branch();

    void setDataInPortWidth(int width);
    void setDataTruePortWidth(int width);
    void setDataFalsePortWidth(int width);  
    void setDataPortWidth(int width);

    void setDataInPortDelay(unsigned int delay);
    void setConditionPortDelay(unsigned int delay);
    void setDataTruePortDelay(unsigned int delay);
    void setDataFalsePortDelay(unsigned int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    bool isCurrentPortSet();
    void setCurrentPort(BranchCurrentPort currentPort);

    const Port* getDataInPort();
    const Port* getConditionInPort();

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    Port dataIn;
    Port condition;
    Port dataTrue;
    Port dataFalse;
    static unsigned int instanceCounter;
    pair <Block*, const Port*> connectedPortTrue;
    pair <Block*, const Port*> connectedPortFalse;
    BranchCurrentPort currentPort;
    
};


class Demux : public Block {

public:

    Demux(const BasicBlock* parentBB = nullptr, int portWidth = -1, 
        unsigned int blockDelay = 0);
    ~Demux();

    const Port* addControlInPort(unsigned int delay = 0);
    void addDataOutPort(unsigned int delay = 0);
    
    void setDataInPortWidth(int width);
    void setDataOutPortWidth(unsigned int index, int width);
    void setDataPortWidth(int width);

    void setControlPortDelay(unsigned int index, unsigned int delay);
    void setDataInPortDelay(unsigned int delay);
    void setDataOutPortDelay(unsigned int index, unsigned int delay);

    void setCurrentConnectedPort(int current);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getControlInPort(unsigned int index);
    const Port* getDataInPort();

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:

    vector <Port> control;
    Port dataIn;
    vector <Port> dataOut;
    static unsigned int instanceCounter;
    vector <pair <Block*, const Port*> > connectedPorts;
    int currentConnected;

};

class EntryInterf : public Block {

public:

    void setInPortDelay(unsigned int delay);
    void setOutPortDelay(unsigned int delay);

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getControlInPort();

    virtual void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;


protected:
    EntryInterf();
    EntryInterf(const string& blockName, const BasicBlock* parentBB, 
        int portWidth = -1, unsigned int blockDelay = 0);
    virtual ~EntryInterf();
    Port inPort;
    Port outPort;
    pair <Block*, const Port*> connectedPort;

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

    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    const Port* getInPort();

    virtual void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;


protected:
    ExitInterf();
    ExitInterf(const string& blockName, const BasicBlock* parentBB, 
        int portWidth = -1, unsigned  int blockDelay = 0);
    virtual ~ExitInterf();
    Port inPort;
    Port outPort;
    pair <Block*, const Port*> connectedPort;

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

    FunctionCall();
    ~FunctionCall();

    void setConnectedPortResult(pair <Block*, const Port*> connection);
    void setConnectedPortControl(pair <Block*, const Port*> connection);
    
    pair <Block*, const Port*> getConnectedPort() override;
    void setConnectedPort(pair <Block*, const Port*> connection) override;
    bool connectionAvailable() override;

    pair <Block*, const Port*> getConnecDataPort();
    pair <Block*, const Port*> getConnecControlPort();

    void printBlock(ostream &file) override;
    void printChannels(ostream& file) override;

private:
    
    pair <Block*, const Port*> connectedResultPort;
    pair <Block*, const Port*> connectedControlPort;

};


} // Close namespace


#endif // BLOCKS_H