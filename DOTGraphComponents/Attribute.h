#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <fstream>
using namespace std;

class Attribute {

public:

    string name;

    Attribute();
    Attribute(const string &name);
    ~Attribute();

    string getName();
    void setName(const string &name);
    void printName(ostream &file) const;

    virtual void printValue(ostream &file) const = 0;

    void printAttribute(ostream &file) const;

private:


};

#endif // ATTRIBUTE_H