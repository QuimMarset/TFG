#ifndef ATTRIBUTEUNIVALUE_H
#define ATTIRBUTEUNIVALUE_H

#include "Attribute.h"

template <typename T>
class AttributeUniValue : public Attribute {

private:

    T value;

public:

    AttributeUniValue();
    AttributeUniValue(const string &name);
    AttributeUniValue(const string &name, const T &value);
    ~AttributeUniValue();

    T getValue();
    void setValue(const T& value);
    
    void printValue(ofstream &file);

}; 


template <typename T>
AttributeUniValue<T>::AttributeUniValue() : Attribute() {}

template <typename T>
AttributeUniValue<T>::AttributeUniValue(const string &name) : Attribute(name) {}

template <typename T>
AttributeUniValue<T>::AttributeUniValue(const string &name, const T &value) : Attribute(name) {
    this->value = value;
}

template <typename T>
AttributeUniValue<T>::~AttributeUniValue() {}

template <typename T>
T AttributeUniValue<T>::getValue() {
    return value;
}

template <typename T>
void AttributeUniValue<T>::setValue(const T& value) {
    this->value = value;
}

template <typename T>
void AttributeUniValue<T>::printValue(ofstream &file) {
    file << value;
}


#endif // ATTRIBUTEUNIVALUE_H