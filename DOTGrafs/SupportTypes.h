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

class Port {

public:

    enum PortType {
            Base = 0,
            Condition,
            True,
            False
    };

    Port();
    Port(const string &name, PortType type = Base, int delay = 0);
    ~Port();

    string getName();
    int getDelay();
    PortType getType();
    void setName(string name);
    void setDelay(int delay);
    void setType(PortType type);

    friend bool operator == (const Port &p1, const Port &p2);
    friend ostream &operator << (ostream &out, const Port &p); 

private:

    string name;
    int delay;
    PortType type;

};


#endif // SUPPORTTYPES_H