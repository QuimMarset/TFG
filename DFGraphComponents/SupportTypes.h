#ifndef SUPPORTTYPES_H
#define SUPPORTTYPES_H

#include <string>
#include <fstream>
#include <assert.h>
using namespace std;


namespace DFGraphComp
{


enum BlockType {
    Operator_Block = 0,
    Buffer_Block,
    Constant_Block,
    Fork_Block,
    Merge_Block,
    Select_Block,
    Branch_Block,
    Demux_Block,
    Entry_Block,
    Exit_Block,
    FunctionCall_Block // Dummy block
};

ostream &operator << (ostream &out, BlockType blockType);



enum OpType {
    // Binary
    Add = 0,
    FAdd,
    Sub,
    FSub,
    Mul,
    FMul,
    Div,
    FDiv,
    Rem,
    FRem,
    And,
    Or,
    Xor,
    ShiftL,
    ShiftR,
    Eq,
    FEq,
    NE,
    FNE,
    GT,
    FGT,
    LT,
    FLT,
    GE,
    FGE,
    LE,
    FLE,
    True,
    False,
    Store,

    // Unary
    Load,
    Alloca,
    FNeg,
    IntTrunc,
    IntZExt,
    IntSExt,
    FPointToUInt,
    FPointToSInt,
    UIntToFPoint,
    SIntToFPoint,
    FPointTrunc,
    FPointExt,
    PtrToInt,
    IntToPtr,
    BitCast,
    AddrSpaceCast,

    // More than two inputs
    Synchronization
};

extern int numberOperators;

string getOpName(OpType op);

bool isUnary(OpType op);
bool isBinary(OpType op);

ostream &operator << (ostream& out, OpType op);



class Port {

public:

    enum PortType {
        Base = 0,
        Condition,
        True,
        False
    };

    Port();
    Port(const string &name, int width = -1, PortType type = Base,  
        unsigned int delay = 0);
    Port(const Port &port);
    ~Port();

    string getName() const;
    PortType getType() const;
    int getWidth() const;
    int getDelay() const;
    void setName(string name);
    void setType(PortType type);
    void setWidth(unsigned int width);
    void setDelay(unsigned int delay);
    
    friend ostream &operator << (ostream &out, const Port &p); 

private:

    string name;
    PortType type;
    int width;
    unsigned int delay;

};


} // Close namespace

#endif // SUPPORTTYPES_H