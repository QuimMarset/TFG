#ifndef ATTRIBUTEUNIVALUE_H
#define ATTIRBUTEUNIVALUE_H

#include "Attribute.h"

template <typename T>
class AttributeUniValue : public Attribute {

public:

    T value;

    AttributeUniValue();
    AttributeUniValue(const string &name);
    AttributeUniValue(const string &name, T value);
    ~AttributeUniValue();

    T& getValue();
    void setValue(T value);
    
    void printValue(ostream &file) const;

private:

}; 


template <typename T>
AttributeUniValue<T>::AttributeUniValue() : Attribute() {}

template <typename T>
AttributeUniValue<T>::AttributeUniValue(const string &name) : Attribute(name) {}

template <typename T>
AttributeUniValue<T>::AttributeUniValue(const string &name, T value) : Attribute(name) {
    this->value = value;
}

template <typename T>
AttributeUniValue<T>::~AttributeUniValue() {}

template <typename T>
T& AttributeUniValue<T>::getValue() {
    return value;
}

template <typename T>
void AttributeUniValue<T>::setValue(T value) {
    this->value = value;
}

template <typename T>
void AttributeUniValue<T>::printValue(ostream &file) const {
    file << value;
}


#endif // ATTRIBUTEUNIVALUE_H