### （练手）C++ Json解析器 by CorneliaMo

just simply include JsonAnalyze.h

e.g.

    #include "JsonAnalyze.h"

    JsonAnalyze analyzer;
    int re = analyzer.AnalyzeString( "{a:123, b:2.34, c:false}" );
    
    if (re == WRONG){
            //The syntax of json string is wrong
    }else if (re == ERROR){
            //Other errors such as OutOfMemory
    }else if (re == OK){
            //Analyzer successfully ran
            //Create a container
        DataContainer* json_p = analyzer.successAnalyze();
        if (json==NULL) return 0;               //Check error again
        DataContainer json = *json_p;

        //Now you can read data from json container
        //for example

        Data a = json["a"];                     //Read using the key 
        Data a = json.getElementByKey("a");     //(Only if it's an object)
                                                

        Data a = json[0];                       //Read using the index
        Data a = json.getElementByIndex(0);     //(Only if it's an array)

        int length = json.getLength();
        std::string* keys = json.getListOfKey();//Get the list of keys
                                                //(Only if it's an object)
    }