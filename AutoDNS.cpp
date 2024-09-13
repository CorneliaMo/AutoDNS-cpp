/*百度公网api https://qifu-api.baidubce.com/ip/local/geo/v1/district
返回值
{
    "code": "Success",
    "data": {
        "continent": "亚洲",
        "country": "中国",
        "zipcode": "518051",
        "timezone": "UTC+8",
        "accuracy": "区县",
        "owner": "中国电信",
        "isp": "中国电信",
        "source": "数据挖掘",
        "areacode": "CN",
        "adcode": "440305",
        "asnumber": "4134",
        "lat": "22.556244",
        "lng": "113.939291",
        "radius": "25.3318",
        "prov": "广东省",
        "city": "深圳市",
        "district": "南山区"
    },
    "charge": true,
    "msg": "查询成功",
    "ip": "218.17.207.60",  
    "coordsys": "WGS84"
}
*/

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include "JsonParser/JsonAnalyze.h"

size_t getRequestText(char* ptr, size_t size, size_t nmemb, void* stream){
    #ifdef DEBUG
    std::cout << "Get content\n";
    #endif
    std::string* data = (std::string*)stream;
    *data = "";
    size_t length = size*nmemb;
    char* buffer = (char*)ptr;
    for (int loop=0;loop<length;loop+=size){
        *data += *(buffer+loop);
    }
    return length;
}

int request_baiduapi(std::string json){
    CURL* curl;
    CURLcode re;
    std::string content;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://qifu-api.baidubce.com/ip/local/geo/v1/district");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getRequestText);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
    re = curl_easy_perform(curl);
    if (re==CURLE_OK){
        curl_easy_cleanup(curl);
        #ifdef DEBUG
        std::cout << content << std::endl;
        #endif
        json = content;
        return OK;
    }else{
        curl_easy_cleanup(curl);
        json = "";
        return ERROR;
    }
}

int query_ip(std::string &ip){
    ip = "";
    JsonAnalyze analyzer;
    DataContainer* json_data_p;
    DataContainer json_data;
    std::string json;
    int re;

    re = request_baiduapi(json);
    if (re!=OK) return ERROR;
    analyzer.AnalyzeString(json);
    json_data_p = analyzer.successAnalyze();
    if (json_data_p==NULL) return ERROR;
    json_data = *json_data_p;

    std::string* request_code = (std::string*)(json_data["code"].data);
    if (request_code==NULL||*request_code!="Success"){
        #ifdef DUBUG
        std::cout << "Query public ip failed\n";
        #endif
        return ERROR;
    }else{
        #ifdef DEBUG
        std::cout << "Queried public ip " << ip << std::endl;
        #endif
        ip = *(std::string*)(json_data["ip"].data);
        return OK;
    }
}

int loadConfig(std::string &SECRET_ID, std::string &SECRET_KEY, DataContainer* &Domains){
    int re;
    JsonAnalyze analyzer;
    DataContainer* json_data_p;
    DataContainer json_data;
    std::string buffer="", json="";
    std::ifstream configfile("DDNS-config.json");

    while (std::getline(configfile, buffer)) json += buffer;
    #ifdef DEBUG
    std::cout << "Load config: " << json << std::endl;
    #endif

    re = analyzer.AnalyzeString(json);
    if (re!=OK) return ERROR;
    json_data_p = analyzer.successAnalyze();
    if (json_data_p==NULL) return ERROR;
    json_data = *json_data_p;

    if (json_data["SECRET_ID"].data==NULL) return ERROR;
    else SECRET_ID = *(std::string*)(json_data["SECRET_ID"].data);

    if (json_data["SECRET_KEY"].data==NULL) return ERROR;
    else SECRET_KEY = *(std::string*)(json_data["SECRET_KEY"].data);

    if (json_data["Domains"].data==NULL) return ERROR;
    else Domains = (DataContainer*)(json_data["Domains"].data);
    
    return OK;
}

int main(){ 
    int re;
    std::string SECRET_ID, SECRET_KEY;
    DataContainer* Domains;
    re = loadConfig(SECRET_ID, SECRET_KEY, Domains);
    if (re!=OK){
        std::cout << "Wrong config\n";
        return 0; 
    }
}