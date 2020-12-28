#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <zconf.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t __write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(void)
{
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    const char* LOGIN_DATA_FORMAT="{\n"
                            "    \"username\":\"%s\",\n"
                            "    \"password\":\"%s\"\n"
                            "}";
    char* result=malloc(1024);

    printf("main\n");
//    char* ip="115.28.20.171";
    char* ip="192.168.78.95";
//    int port=9011;
    int port=7001;
//    char* method="api/v1/istar/login/app";
    char* method="login";
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {

        char *buffer = calloc(256, sizeof(char));
        sprintf(buffer, "http://%s:%d/%s", ip, port,method);
        printf("ip=[%s]\n",buffer);
        curl_easy_setopt(curl, CURLOPT_URL, buffer); //提交表单的URL地址
//        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");


        struct curl_slist *pList = NULL;
        // pList = curl_slist_append(pList,"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        // pList = curl_slist_append(pList,"Accept-Language:zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
        // pList = curl_slist_append(pList,"Accept-Encoding:gzip, deflate");
        pList = curl_slist_append(pList,"Connection:keep-alive");
        // 删除后续charset=UTF-8，会导致服务端415错误
        // pList = curl_slist_append(pList,"Content-Type:application/xml; charset=UTF-8");
        pList = curl_slist_append(pList,"Content-Type:application/json");
        pList = curl_slist_append(pList,"Upgrade-Insecure-Requests:1");

        printf("curl_easy_setopt set header\n");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pList);

        sprintf(buffer, LOGIN_DATA_FORMAT, "huangjiasheng","8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(buffer));
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        printf("curl_easy_perform start\n");

        res = curl_easy_perform(curl);

        printf("curl_easy_perform finished\n");
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        long res_code = 0;
        res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
        if (CURLE_OK == res && 200 == res_code)
        {
            printf("[response]:%s\n", chunk.memory);
        }

        free(buffer);
        free(chunk.memory);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
