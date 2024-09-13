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
#include <tencentcloud/core/TencentCloud.h>
#include <tencentcloud/core/Credential.h>
#include <tencentcloud/dnspod/v20210323/DnspodClient.h>
#include <time.h>

void delay(int seconds) //参数为整型，表示延时多少秒
{
    clock_t start = clock();
    clock_t lay = (clock_t)seconds * CLOCKS_PER_SEC;
    while ((clock()-start) < lay);
}

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

int request_baiduapi(std::string &json){
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
    if (re!=OK){
        std::cout << "Request baidu api failed\n";
        return ERROR;
    }
    analyzer.AnalyzeString(json);
    json_data_p = analyzer.successAnalyze();
    if (json_data_p==NULL){
        std::cout << "Analyze baidu request json failed\n";
        return ERROR;
    }
    json_data = *json_data_p;

    std::string* request_code = (std::string*)(json_data["code"].data);
    if (request_code==NULL||*request_code!="Success"){
        std::cout << "Query public ip failed\n";
        return ERROR;
    }else{
        ip = *(std::string*)(json_data["ip"].data);
        std::cout << "Queried public ip " << ip << std::endl;
        return OK;
    }
}

int loadConfig(std::string &SECRET_ID, std::string &SECRET_KEY, std::string &region, int &query_interval, DataContainer* &Domains){
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

    if (json_data["region"].data==NULL) return ERROR;
    else region = *(std::string*)(json_data["region"].data);

    if (json_data["query_interval"].data==NULL) return ERROR;
    else query_interval = *(int*)(json_data["query_interval"].data);

    if (json_data["Domains"].data==NULL) return ERROR;
    else Domains = (DataContainer*)(json_data["Domains"].data);
    
    return OK;
}

int updateDomain(std::string ip, TencentCloud::Dnspod::V20210323::DnspodClient &dnspod_client, std::string Domain, std::string SubDomain, int RecordId, std::string RecordType){
    TencentCloud::Dnspod::V20210323::Model::DescribeRecordRequest describe_req;
    TencentCloud::Dnspod::V20210323::DnspodClient::DescribeRecordOutcome describe_out;
    TencentCloud::Dnspod::V20210323::Model::DescribeRecordResponse describe_response;
    TencentCloud::Dnspod::V20210323::Model::ModifyRecordRequest modify_req;
    TencentCloud::Dnspod::V20210323::DnspodClient::ModifyRecordOutcome modify_out;
    TencentCloud::Dnspod::V20210323::Model::RecordInfo recordinfo;
    std::string prev_ip, RecordLine;
    describe_req.SetDomain(Domain);
    describe_req.SetRecordId(RecordId);
    describe_out = dnspod_client.DescribeRecord(describe_req);

    if (!describe_out.IsSuccess()){
        std::cout << "Describe " << RecordId << " of " << Domain << " error\n";
        std::cout << describe_out.GetError().GetErrorMessage() << std::endl;
        return OK;
    }else{
        describe_response = describe_out.GetResult();
        recordinfo = describe_response.GetRecordInfo();
        if (recordinfo.GetSubDomain()!=SubDomain){
            std::cout << "RecordId " << RecordId << "'s SubDomain should be " << recordinfo.GetSubDomain() << " but not " << SubDomain << std::endl;
            return OK;
        }
        if (recordinfo.GetRecordType()!=RecordType){
            std::cout << "SubDomain " << SubDomain << " doesn't have record " << RecordType << std::endl;
            return OK;
        }
        prev_ip = recordinfo.GetValue();
        if (prev_ip==ip){
            std::cout << "Record " << SubDomain << " of " << Domain << " haven't change\n";
            return OK;
        }
        RecordLine = recordinfo.GetRecordLine();
    }

    modify_req.SetDomain(Domain);
    modify_req.SetRecordId(RecordId);
    modify_req.SetRecordType(RecordType);
    modify_req.SetSubDomain(SubDomain);
    modify_req.SetValue(ip);
    modify_req.SetRecordLine(RecordLine);
    modify_out = dnspod_client.ModifyRecord(modify_req);
    if (!modify_out.IsSuccess()){
        std::cout << "Modiffy " << RecordId << " of " << Domain << " error\n";
        std::cout << modify_out.GetError().GetErrorMessage() << std::endl;
        return OK;
    }else{
        std::cout << "Successfully modify record " << SubDomain << " of " << Domain << " from " << prev_ip << " to " << ip << std::endl;
    }
    return OK;
}

int updateDomains(std::string ip, TencentCloud::Dnspod::V20210323::DnspodClient &dnspod_client, DataContainer* Domains_p){
    int count = Domains_p->getLength();
    DataContainer Domains = *Domains_p;
    std::cout << "Updating " << count << " domain(s)\n";
    for (int loop=0;loop<count;loop++){
        std::string Domain="", SubDomain="", RecordType="";
        int RecordId=0;
        DataContainer domain = *(DataContainer*)(Domains[loop].data);
        Domain = *(std::string*)(domain["Domain"].data);
        SubDomain = *(std::string*)(domain["SubDomain"].data);
        RecordId = *(int*)(domain["RecordId"].data);
        RecordType = *(std::string*)(domain["RecordType"].data);
        if (Domain==""||SubDomain==""||RecordId==0||RecordType=="") continue;
        updateDomain(ip, dnspod_client, Domain, SubDomain, RecordId, RecordType);
    }
    return OK;
}

int main(){ 
    time_t curtime;
    //load config
    int re, query_interval;
    std::string SECRET_ID, SECRET_KEY, region, now_ip;
    DataContainer* Domains;
    re = loadConfig(SECRET_ID, SECRET_KEY, region, query_interval, Domains);
    if (re!=OK||SECRET_ID==""||SECRET_KEY==""||region==""||query_interval<=0||Domains->getLength()==0){
        std::cout << "Wrong config\n";
        std::cout << "Please check file \"DDNS-config.json\"\n";
        return 0; 
    }

    //Init
    TencentCloud::InitAPI();
    TencentCloud::Credential cred = TencentCloud::Credential(SECRET_ID, SECRET_KEY);
    TencentCloud::Dnspod::V20210323::DnspodClient dnspod_client(cred, region);
    
    //main loop
    while (true){
        time(&curtime);
        std::cout << "Time now: " << ctime(&curtime);
        while (query_ip(now_ip)!=OK) delay(30);
        while (updateDomains(now_ip, dnspod_client, Domains)!=OK) delay(30);
        std::cout << "\n\n";

        delay(query_interval);
    }

    return 0;
}