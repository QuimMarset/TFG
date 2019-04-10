
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

void Attribute::printName(ostream &file) const {
    file << name;
}

void Attribute::printAttribute(ostream &file) const {
    printName(file);
    file << " = \"";
    printValue(file);
    file << "\"";
}