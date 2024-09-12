#include "JsonAnalyze.h"
#include <iostream>

std::string JsonAnalyze::getKey(std::string json, int i){
    //search a key start from json[i]
    #ifdef DEBUG
    std::cout << "Getting key from " << json << " begins at index " << i << std::endl;
    #endif
    std::string key = "";
    int index = i;
    while(json[index]==' ') index++;
    while(json[index]!=':'){
        key += json[index];
        index++;
    }
    if (key[0]==key[key.length()-1]&&key[0]=='"') key = key.substr(1, key.length()-2);
    #ifdef DEBUG
    std::cout << "Find key \" " << key << " \"\n"; 
    #endif
    return key;
}

JsonAnalyze::JsonAnalyze(){
    mainContainer = NULL;
    success = 0;
}

std::string JsonAnalyze::clearSpace(std::string string){
    #ifdef DEBUG
    std::cout << "Clearing space from " << string << std::endl;
    #endif
    int left = 0, right = string.length()-1;
    while(string[left]==' ') left++;
    while(string[right]==' ') right--;
    return string.substr(left, right-left+1);
}

bool JsonAnalyze::isNumber(std::string string){
    int index = string.length()-1;
    while (index>=0){
        if (string[index]>'9'||string[index]<'0') return false;
        index--;
    }
    return true;
}

int JsonAnalyze::convertToData(std::string sep_json, Data &data){
    if (sep_json.length()==0) return ERROR;
    std::string json = clearSpace(sep_json);
    #ifdef DEBUG
    std::cout << "Converting string " << sep_json << std::endl;
    #endif
    //Data can be int, double, true, false, string, null
    if (json[0]=='"'&&json[json.length()-1]=='"'&&json.length()>=2){
        //is string
        #ifdef DEBUG
        std::cout << "Find string\n";
        #endif
        data.type = "string";
        if (json.length()==2){
            //empty string
            data.data = new std::string("");
        }else{
            data.data = new std::string(json.substr(1, json.length()-2));
        }
        return OK;
    }else if (isNumber(json)){
        //is int
        #ifdef DEBUG
        std::cout << "Find int\n";
        #endif
        data.type = "int";
        std::istringstream is(json);
        data.data = new int;
        is >> *(int*)(data.data);
        return OK;
    }else if (json.find('.')==json.rfind('.',json.length()-1)&&json.find('.')!=-1){
        int dot_pos = json.find('.');
        if (isNumber(json.substr(0, dot_pos))&&isNumber(json.substr(dot_pos+1, json.length()-dot_pos))){
            //is double
            #ifdef DEBUG
            std::cout << "Find double\n";
            #endif
            data.type = "double";
            std::istringstream is(json);
            data.data = new double;
            is >> *(double*)(data.data);
            return OK;
        }
    }
    if (json=="true"||json=="True"){
        //is true
        #ifdef DEBUG
        std::cout << "Find bool\n";
        #endif
        data.type = "bool";
        data.data = new bool;
        *(bool*)(data.data) = true;
        return OK;
    }else if (json=="false"||json=="False"){
        //is false
        #ifdef DEBUG
        std::cout << "Find bool\n";
        #endif
        data.type = "bool";
        data.data = new bool;
        *(bool*)(data.data) = false;
        return OK;
    }else if (json=="null"||json=="NULL"){
        //is null
        #ifdef DEBUG
        std::cout << "Find null\n";
        #endif
        data.type = "null";
        data.data = NULL;
        return OK;
    }
    return ERROR;
}

int JsonAnalyze::Analyze(std::string json){
    //先确定给定的字符串是一个对象或数组
    //如果是，通过搜索逗号的方法确定同级元素的数量（不包含子序列中的元素）
    //创建DataContainer，将length、type填入容器中
    //如果是对象，初始化keys和values的空间，否则只初始化values的空间
    //将Container的指针压入栈
    //
    //作为对象
    //通过当前字符串位置到冒号前，确定key的内容
    //通过冒号后到逗号或}前确定value的内容，放入对应解析器
    //通过循环解析每一个元素
    //
    //作为数组
    //通过当前字符串位置到逗号或]前确定一个元素的内容，放入对应解析器
    //通过循环解析每一个元素
    success = 0;
    if (json[0]=='{'||json[0]=='['){
        int length = 0, lengthB = 1;

        //calculate how many elements in this object or array
        int tmp_cal_length = 0;
        int endPos = 0;
        for (int loop=1;loop<json.length();loop++){
            //scan the number of object
            if (json[loop]=='{'||json[loop]=='['){
                tmp_cal_length++;
            }else if (json[loop]=='}'||json[loop]==']'){
                tmp_cal_length--;
            }else if (json[loop]==':'&&tmp_cal_length==0){
                length++;
            }else if (json[loop]==','&&tmp_cal_length==0){
                lengthB++;
            }
            if (tmp_cal_length<0){
                endPos = loop;
                break;
            }
        }

        #ifdef DEBUG
        if (json[0]=='{') std::cout << "Find " << "object" << " which length is " << length << std::endl; 
        else std::cout << "Find " << "array" << " which length is " << lengthB << std::endl;
        #endif
        
        //create a container for this object or array
        DataContainer* pContainer = new DataContainer();
        pContainer->type = (json[0]=='{'? 'O':'A');
        datacontainerStack.push(pContainer);
        if (json[0]=='{'){
            //is an object
            pContainer->length = length;
            pContainer->keys = new std::string[length];
            pContainer->values = new Data[length];
            //add each element into the object
            int added = 0, nowIndex = 1;
            while(added<length&&nowIndex<endPos){
                pContainer->keys[added] = getKey(json, nowIndex);
                //Search the pos of value
                while(json[nowIndex]!=':') nowIndex++;
                nowIndex++;
                while(json[nowIndex]==' ') nowIndex++;
                if (json[nowIndex]=='{'||json[nowIndex]=='['){
                    //if the value is an object or an array
                    //analyze the object or array of the value
                    Analyze(std::string(json, nowIndex));
                    //put the analyzed element into container
                    DataContainer** tmpPtr = NULL;
                    int re = datacontainerStack.pop(tmpPtr);
                    if (re!=OK||tmpPtr==NULL) return ERROR;
                    pContainer->values[added].data = *tmpPtr;
                    pContainer->values[added].type = json[nowIndex]=='{'? "object" : "array";
                    delete tmpPtr;
                    //set new index
                    tmp_cal_length = 0;
                    for (int loop=nowIndex+1;loop<json.length();loop++){
                        //scan the number of object
                        if (json[loop]=='{'||json[loop]=='['){
                            tmp_cal_length++;
                        }else if (json[loop]=='}'||json[loop]==']'){
                            tmp_cal_length--;
                        }
                        if (tmp_cal_length<0){
                            nowIndex = loop+1;
                            break;
                        }
                    }
                    while(json[nowIndex]!=','&&json[nowIndex]!='}') nowIndex++;
                    nowIndex++;
                    while(json[nowIndex]==' ') nowIndex++;
                    added++;
                }else{
                    //the value is int, double, string, bool or null
                    int value_end = nowIndex;
                    //search the end pos of the value
                    while(json[value_end]!=','&&json[value_end]!='}') value_end++;
                    value_end--;
                    int re = convertToData(json.substr(nowIndex, value_end-nowIndex+1), pContainer->values[added]);
                    if (re!=OK) return ERROR;
                    //set new index
                    nowIndex = value_end+2;
                    while(json[nowIndex]==' ') nowIndex++;
                    added++;
                }
            }//finish add object's elements
        }else{
            //is an array
            pContainer->length = lengthB;
            pContainer->keys = NULL;
            pContainer->values = new Data[lengthB];
            //add each element into container
            int added = 0, nowIndex = 1;
            while(added<lengthB&&nowIndex<endPos){
                if (json[nowIndex]=='{'||json[nowIndex]=='['){
                    //if the value is an object or an array
                    //analyze the object or array of the value
                    Analyze(std::string(json, nowIndex));
                    //put the analyzed element into container
                    DataContainer** tmpPtr = NULL;
                    int re = datacontainerStack.pop(tmpPtr);
                    if (re!=OK||tmpPtr==NULL) return ERROR;
                    pContainer->values[added].data = *tmpPtr;
                    pContainer->values[added].type = json[nowIndex]=='{'? "object" : "array";
                    delete tmpPtr;
                    //set new index
                    tmp_cal_length = 0;
                    for (int loop=nowIndex+1;loop<json.length();loop++){
                        //scan the number of object
                        if (json[loop]=='{'||json[loop]=='['){
                            tmp_cal_length++;
                        }else if (json[loop]=='}'||json[loop]==']'){
                            tmp_cal_length--;
                        }
                        if (tmp_cal_length<0){
                            nowIndex = loop+1;
                            break;
                        }
                    }
                    while(json[nowIndex]!=','&&json[nowIndex]!='}') nowIndex++;
                    nowIndex++;
                    added++;
                }else{
                    //is common value
                    int value_end = nowIndex;
                    //search the end pos of the value
                    while(json[value_end]!=','&&json[value_end]!=']') value_end++;
                    value_end--;
                    int re = convertToData(json.substr(nowIndex,value_end-nowIndex+1), pContainer->values[added]);
                    if (re!=OK) return ERROR;
                    //set new index
                    nowIndex = value_end+2;
                    added++;
                }
            }
        }
    }else{
        return ERROR;
    }
    success = 1;
    return OK;
}

DataContainer* JsonAnalyze::successAnalyze(){
    if (success==1&&datacontainerStack.getLength()==1){
        DataContainer** result = NULL;
        datacontainerStack.pop(result);
        mainContainer = *result;
        return mainContainer;
    }else if(success==1&&mainContainer!=NULL){
        return mainContainer;
    }else{
        return NULL;
    }
}

int JsonAnalyze::AnalyzeString(std::string json){
    SyntaxCheck checker;
    if (checker.checkSyntax(json)!=OK) return WRONG;
    if (Analyze(json)!=OK) return ERROR;
    return OK;
}