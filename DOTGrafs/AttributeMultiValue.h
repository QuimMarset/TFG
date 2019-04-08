#ifndef ATTRIBUTEMULTIVALUE_H
#define ATTRIBUTEMULTIVALUE_H

#include "Attribute.h"
#include <vector>
#include <tuple>
using namespace std;

template <typename T>
class AttributeMultiValue : public Attribute {
 
public:

    vector <T> values;

    AttributeMultiValue();
    AttributeMultiValue(const string &name);
    AttributeMultiValue(const string &name, const vector <T> &values);
    ~AttributeMultiValue();

    vector <T> getValues();
    void setValues(const vector <T> &value);
    void addValue(T value);
    T &getValue(int index);
    void setValue(int index, T value);
    void clearValues();

    void printValue(ofstream &file);

private:

};


template <typename T>
AttributeMultiValue<T>::AttributeMultiValue() : Attribute() {}

template <typename T>
AttributeMultiValue<T>::AttributeMultiValue(const string &name) : Attribute(name) {}

template <typename T>
AttributeMultiValue<T>::AttributeMultiValue(const string &name, const vector <T> &values)
        : Attribute(name) {
    for (T value : values) {
        addValue(value);
    }
}

template <typename T>   
AttributeMultiValue<T>::~AttributeMultiValue() {
    clearValues();
}


template <typename T>
void AttributeMultiValue<T>::addValue(T value) {
    values.push_back(value);
}


template <typename T>
vector <T> AttributeMultiValue<T>::getValues() {
    return values;
}

template <typename T>
void AttributeMultiValue<T>::setValues(const vector <T> &value) {
    this->value = value;
}

template <typename T>   
void AttributeMultiValue<T>::clearValues() {
    values.clear();
}

template <typename T>   
T &AttributeMultiValue<T>::getValue(int index) {
    return values[index];
}

template <typename T>
void AttributeMultiValue<T>::setValue(int index, T value) {
    values[index] = value;
}

template <typename T>
void AttributeMultiValue<T>::printValue(ofstream &file) {
    for (unsigned int i = 0; i < values.size(); ++i) {
        file << values[i];
        if (i < values.size()-1) {
            file << " ";
        }
    }
}

#endif // ATTRIBUTEMULTIVALUE_H