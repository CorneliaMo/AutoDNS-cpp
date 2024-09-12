#include "DataContainer.h"

DataContainer::DataContainer(){
    keys = NULL;
    values = NULL;
    length = 0;
    type = ' ';
}

Data DataContainer::getNullData(){
    Data result;
    result.data = NULL;
    result.type = "NULL";
    return result;
}

bool DataContainer::isObject(){
    if (this==NULL) return false;
    return (type=='O'? true : false);
}

bool DataContainer::isArray(){
    if (this==NULL) return false;
    return (type=='A'? true : false);
}

int DataContainer::getLength(){
    if (this==NULL) return -1;
    return length;
}

Data DataContainer::getElementByIndex(int i){
    //Index start from 0
    if (!isArray()){
        return getNullData();
    }else{
        if (i>=length){
            return getNullData();
        }else{
            return values[i];
        }
    }
}

Data DataContainer::operator[](int i){
    //Index start from 0
    if (!isArray()){
        return getNullData();
    }else{
        if (i>=length){
            return getNullData();
        }else{
            return values[i];
        }
    }
}

Data DataContainer::getElementByKey(std::string key){
    if (!isObject()){
        return getNullData();
    }else{
        for (int loop=0;loop<length;loop++){
            if (keys[loop]==key){
                return values[loop];
            }
        }
        return getNullData();
    }
}

Data DataContainer::operator[](std::string key){
    if (!isObject()){
        return getNullData();
    }else{
        for (int loop=0;loop<length;loop++){
            if (keys[loop]==key){
                return values[loop];
            }
        }
        return getNullData();
    }
}

std::string* DataContainer::getListOfKey(){
    if (this==NULL) return NULL;
    if (!isObject()){
        return NULL;
    }else{
        std::string* result = new std::string[length];
        for (int loop=0;loop<length;loop++){
            result[loop] = keys[loop];
        }
        return result;
    }
}