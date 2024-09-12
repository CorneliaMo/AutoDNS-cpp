#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <string>
#include <cstring>

// Define a common container for object, array
// Object is a list indexed by string keys
// Array is a list indexed by serial number 

typedef struct Data{
    void* data;
    std::string type;
}Data;

class DataContainer {
    private:
        std::string* keys;    //valid if is object
        Data* values;  //always valid
        int length;
        char type;      //'O' for object, 'A' for array
        friend class JsonAnalyze;
        Data getNullData();
    public:
        DataContainer();
        bool isObject();
        bool isArray();
        int getLength();
        std::string* getListOfKey();
        Data getElementByKey(std::string key);
        Data getElementByIndex(int i);
        Data operator[](std::string key);
        Data operator[](int i);
};

#endif //DATASTORAGE_H
