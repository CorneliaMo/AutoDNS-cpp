#include "SyntaxCheck.h"

SyntaxCheck::SyntaxCheck(){
    charStack;
    symbolCountStack;
}

int SyntaxCheck::emptyStacks(){
    charStack.emptyStack();
    symbolCountStack.emptyStack();
    return OK;
}

int SyntaxCheck::isInString(){
    if (charStack.getLength()==0){
        return false;
    }else{
        char* prev_symbol = NULL;
        int re = charStack.pop(prev_symbol);
        charStack.push(*prev_symbol);
        if (re==ERROR||prev_symbol==NULL){
            return false;
        }
        if (*prev_symbol=='"'){
            return true;
        }else{
            return false;
        }
    }
}

int SyntaxCheck::checkSyntax(std::string json){
    if (json.length()<=1){
        #ifdef DEBUG
        std::cout << "Json string is too short\n";
        #endif
        return WRONG;
    }
    for (int loop=0;loop<json.length();loop++){
        char step = json[loop];
        if (step=='{'){
            if (isInString()) { continue; }
            charStack.push('{');
            SymbolCount tmp_symbolCount;
            tmp_symbolCount.comma = 0;
            tmp_symbolCount.colon = 0;
            symbolCountStack.push(tmp_symbolCount);
        }else if (step=='['){
            if (isInString()) { continue; }
            charStack.push('[');
            SymbolCount tmp_symbolCount;
            tmp_symbolCount.comma = 0;
            tmp_symbolCount.colon = 0;
            symbolCountStack.push(tmp_symbolCount);
        }else if (step==']'){
            if (isInString()) { continue; }
            if (charStack.getLength()==0){
                #ifdef DEBUG
                std::cout << "Can't find previous symbol\n";
                #endif
                emptyStacks();
                return WRONG;
            }
            char* prev_symbol = NULL;
            int re = charStack.pop(prev_symbol);
            if (re==ERROR||prev_symbol==NULL){
                #ifdef DEBUG
                std::cout << "Pop error\n";
                #endif
                emptyStacks();
                return ERROR;
            }
            if (*prev_symbol!='['){
                #ifdef DEBUG
                std::cout << "Previous symbol is wrong\n";
                #endif
                emptyStacks();
                delete prev_symbol;
                return WRONG;
            }
            delete prev_symbol;
            if (symbolCountStack.getLength()==0){
                #ifdef DEBUG
                std::cout << "Can't find previous symbol count\n";
                #endif
                emptyStacks();
                return WRONG;
            }
            SymbolCount* prev_symbolCount = NULL;
            re = symbolCountStack.pop(prev_symbolCount);
            if (re==ERROR||prev_symbolCount==NULL){
                #ifdef DEBUG
                std::cout << "Pop error\n";
                #endif
                emptyStacks();
                return ERROR;
            }
            if (prev_symbolCount->colon!=0){
                #ifdef DEBUG
                std::cout << "Symbol count is wrong\n";
                #endif
                emptyStacks();
                delete prev_symbolCount;
                return WRONG;
            }
            delete prev_symbolCount;
        }else if (step=='}'){
            if (isInString()) { continue; }
            if (charStack.getLength()==0){
                #ifdef DEBUG
                std::cout << "Can't find previous symbol\n";
                #endif
                emptyStacks();
                return ERROR;
            }
            char* prev_symbol = NULL;
            int re = charStack.pop(prev_symbol);
            if (re==ERROR||prev_symbol==NULL){
                #ifdef DEBUG
                std::cout << "Pop error\n";
                #endif
                emptyStacks();
                return ERROR;
            }
            if (*prev_symbol!='{'){
                #ifdef DEBUG
                std::cout << "Previous symbol is wrong\n";
                #endif
                emptyStacks();
                delete prev_symbol;
                return WRONG;
            }
            delete prev_symbol;
            if (symbolCountStack.getLength()==0){
                #ifdef DEBUG
                std::cout << "Can't find previous symbol count\n";
                #endif
                emptyStacks();
                return WRONG;
            }
            SymbolCount* prev_symbolCount = NULL;
            re = symbolCountStack.pop(prev_symbolCount);
            if (re==ERROR||prev_symbolCount==NULL){
                #ifdef DEBUG
                std::cout << "pop error\n";
                #endif
                emptyStacks();
                return ERROR;
            }
            if (!(prev_symbolCount->colon==0&&prev_symbolCount->comma==0)){
                if (prev_symbolCount->colon!=prev_symbolCount->comma+1){
                    #ifdef DEBUG
                    std::cout << "Symbol count is wrong\n";
                    #endif
                    emptyStacks();
                    delete prev_symbolCount;
                    return WRONG;
                }
            }
            delete prev_symbolCount;
        }else if (step==','){
            if (isInString()) { continue; }
            if (symbolCountStack.getLength()==0){
                emptyStacks();
                return WRONG;
            }else{
                SymbolCount* prev_symbolCount = NULL;
                int re = symbolCountStack.pop(prev_symbolCount);
                if (re==ERROR||prev_symbolCount==NULL){
                    #ifdef DEBUG
                    std::cout << "Pop error\n";
                    #endif
                    emptyStacks();
                    return ERROR;
                }
                prev_symbolCount->comma++;
                symbolCountStack.push(*prev_symbolCount);
                delete prev_symbolCount;
            }
        }else if (step==':'){
            if (isInString()) { continue; }
            if (symbolCountStack.getLength()==0){
                emptyStacks();
                return WRONG;
            }else{
                SymbolCount* prev_symbolCount = NULL;
                int re = symbolCountStack.pop(prev_symbolCount);
                if (re==ERROR||prev_symbolCount==NULL){
                    #ifdef DEBUG
                    std::cout << "Pop error\n";
                    #endif
                    emptyStacks();
                    return ERROR;
                }
                prev_symbolCount->colon++;
                symbolCountStack.push(*prev_symbolCount);
                delete prev_symbolCount;
            }
        }else if (step=='"'){
            if (charStack.getLength()==0){
                emptyStacks();
                return WRONG;
            }
            char* prev_symbol = NULL;
            int re = charStack.pop(prev_symbol);
            if (re==ERROR||prev_symbol==NULL){
                #ifdef DEBUG
                std::cout << "Pop error\n";
                #endif
                emptyStacks();
                return ERROR;
            }
            if (*prev_symbol!='"'){
                charStack.push(*prev_symbol);
                charStack.push('"');
            }
            delete prev_symbol;
        }
    }
    if (charStack.getLength()!=0){
        emptyStacks();
        #ifdef DEBUG
        std::cout << "charStack not empty\n";
        #endif
        return WRONG;
    }
    emptyStacks();
    return OK;
}