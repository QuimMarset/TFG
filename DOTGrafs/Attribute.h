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
    void printName(ofstream &file);

    virtual void printValue(ofstream &file) = 0;

    void printAttribute(ofstream &file);

private:


};

#endif // ATTRIBUTE_H