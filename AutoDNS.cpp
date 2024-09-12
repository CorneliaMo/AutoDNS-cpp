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

std::string request_baiduapi(){
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
        return content;
    }else{
        curl_easy_cleanup(curl);
        return "";
    }
}

int main(){
    std::string json;
    JsonAnalyze analyzer;
    DataContainer* json_data_p;
    DataContainer json_data;

    json = request_baiduapi();
    analyzer.AnalyzeString(json);
    json_data_p = analyzer.successAnalyze();
    json_data = *json_data_p;

    std::string* request_code = (std::string*)(json_data["code"].data);
    if (request_code==NULL||*request_code!="Success"){
        std::cout << "Query public ip failed\n";
    }else{
        std::string ip = *(std::string*)(json_data["ip"].data);
        std::cout << "Queried public ip " << ip << std::endl;
    }
    return 0;
}