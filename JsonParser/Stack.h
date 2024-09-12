#ifndef STACK_H
#define STACK_H
#include "status_code.h"
#include <cstddef>

template<class T>
class Stack{
    private:
        T* top;
        T* bottom;
        int size;               //allocated memory size
        int length;             //used space
        short size_increment;
        int mem_increase(){
            try
            {
                //allocate new memory
                T* tmp_pointer = new T[size+size_increment];
                //move the data
                for (int loop=0;loop<size;loop++){
                    tmp_pointer[loop] = bottom[loop];
                }
                //release memory
                delete[] bottom;
                //relocate the pointer
                bottom = tmp_pointer;
                top = tmp_pointer+length-1;
                size += size_increment;
                return OK;
            }
            catch(...)
            {
                return ERROR;
            }
        }
    public:
        Stack(int _size=50){
            size = _size;
            length = 0;
            size_increment = 20;
            top = bottom = new T[size];
        }
        int push(T data){
            if (size==length&&length!=0){
                //memory is full
                mem_increase();        
            }
            if (length==0||top==NULL){
                top = bottom;
            }else{
                top++;
            }
            *top = data;
            length++;
            return OK;
        }
        int pop(T* &result){
            if (top==NULL){
                //Memory error
                result == NULL;
                return ERROR;
            }else if(top==bottom){
                //reach the bottom
                //reset the memory
                T* tmp = new T;
                *tmp = *top;
                top = NULL;
                length = 0;
                result = tmp;
                return OK;
            }else{
                //default situation
                T* tmp = new T;
                *tmp = *top;
                length--;
                top--;
                result = tmp;
                return OK;
            }
        }         //result contain poped data
        int getSize(){
            return size;
        }             //return size of stack
        int getLength(){
            return length;
        }            //return length of stack
        int setSizeincrement(int i){
            size_increment = i;
            return size_increment;
        } //return current size_increment
        int emptyStack(){
            if (bottom!=NULL){
                delete[] bottom;
            }
            bottom = top = new T[size_increment];
            size = size_increment;
            length = 0;
            return OK;
        }
        ~Stack(){
            if (bottom!=NULL){
                delete[] bottom;
            }
        }
};

#endif
