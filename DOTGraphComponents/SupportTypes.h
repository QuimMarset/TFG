#ifndef SUPPORTTYPES_H
#define SUPPORTTYPES_H

#include <string>
#include <fstream>
using namespace std;

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


enum OperatorType {
    Add = 0,
    Sub,
    Mul,
    Div,
    Rem,
    And, // Bitwise op
    Or,
    Not,
    Xor,
    ShiftL,
    ShiftR,
    Eq,
    NE,
    GT,
    LT,
    GE,
    LE
};

string getOperatorName(OperatorType op);
bool isUnaryOperator(OperatorType op);
ostream &operator << (ostream& out, OperatorType op);
extern int numberOperators;


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


#endif // SUPPORTTYPES_H