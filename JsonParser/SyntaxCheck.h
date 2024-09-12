#ifndef SYNTAXCHECK_H
#define SYNTAXCHECK_H

#include "status_code.h"
#include "Stack.h"
#include <iostream>
#include <string>
//#include  "Stack.cpp"           //不知为何，在同时使用模板类和头文件分离时，需要include对应cpp文件

typedef struct SymbolCount
{
    int colon;  //number of : in a structure
    int comma;  //number of , in a structure
}SymbolCount;

class SyntaxCheck {
    private:
        Stack<char> charStack;
        Stack<SymbolCount> symbolCountStack;
        int isInString();
    public:
        SyntaxCheck();
        int checkSyntax(std::string json);
        int emptyStacks();
};

#endif //SYNTAXCHECK_H
