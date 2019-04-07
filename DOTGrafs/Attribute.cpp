
#include "Attribute.h"

Attribute::Attribute() {
    name = "";
}

Attribute::Attribute(const string &name) {
    this->name = name;
}

Attribute::~Attribute() {}

string Attribute::getName() {
    return name;
}

void Attribute::setName(const string &name) {
    this->name = name;
}

void Attribute::printName(ofstream &file) {
    file << name;
}

void Attribute::printAttribute(ofstream &file) {
    printName(file);
    file << " = \"";
    printValue(file);
    file << "\"";
}