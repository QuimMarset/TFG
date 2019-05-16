#ifndef SUPPORTTYPES_H
#define SUPPORTTYPES_H

#include <string>
#include <fstream>
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
    Exit_Block
};

ostream &operator << (ostream &out, BlockType blockType);


enum UnaryOpType {
    Not = 0,
    Load,
    IntTrunc,
    FPointTrunc,
    IntZExt,
    IntSExt,
    FPointToUInt,
    FPointToSInt,
    UIntToFPoint,
    SIntToFPoint,
    IntToPtr,
    PtrToInt,
    TypeCast,
    AddrSpaceCast
};

enum BinaryOpType {
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
    Alloca
};



extern int numberUnary;
extern int numberBinary;

string getUnaryOpName(UnaryOpType op);
string getBinaryOpName(BinaryOpType op);

ostream &operator << (ostream& out, UnaryOpType op);
ostream &operator << (ostream& out, BinaryOpType op);



class Port {

public:

    enum PortType {
        Base = 0,
        Condition,
        True,
        False
    };

    Port();
    Port(const string &name, PortType type = Base, int width = -1, int delay = 0);
    Port(const Port &port);
    ~Port();

    string getName() const;
    PortType getType() const;
    int getWidth() const;
    int getDelay() const;
    void setName(string name);
    void setType(PortType type);
    void setWidth(int width);
    void setDelay(int delay);
    
    friend ostream &operator << (ostream &out, const Port &p); 

private:

    string name;
    PortType type;
    int width;
    int delay;

};


} // Close namespace

#endif // SUPPORTTYPES_H