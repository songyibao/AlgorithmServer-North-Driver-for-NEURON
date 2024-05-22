//
// Created by root on 5/22/24.
//
#include "http_client.h"

#include "../algorithm.h"
#include <curl/curl.h>
size_t on_response_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    neu_plugin_t *plugin = (neu_plugin_t *) userdata;
    plog_debug(plugin, "http请求结束回调函数");
    return size * nmemb;
}
// 发送HTTP POST请求的函数，包括URL, JSON数据和回调函数
void send_post_request(const char *url, const char *json_data, size_t (*callback)(char *, size_t, size_t, void *),
                       void *arg)
{
    CURL    *curl;
    CURLcode res;
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *headers = NULL;
        headers                    = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        // 设置回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        // 设置回调函数的参数
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, arg);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        printf("已发送HTTP POST请求\n");
        printf("错误码: %s\n", curl_easy_strerror(res));
        if (res != CURLE_OK) {
            printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    } else {
        printf("Failed to initialize curl\n");
    }
}