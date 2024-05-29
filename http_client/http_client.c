//
// Created by root on 5/22/24.
//
#include "http_client.h"

#include "../algorithm.h"
#include <curl/curl.h>
#include "../utils/utils.h"
size_t on_response_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    neu_plugin_t *plugin = (neu_plugin_t *) userdata;
    plugin->common.link_state = NEU_NODE_LINK_STATE_CONNECTED;
    plog_debug(plugin,"算法服务器返回数据: %s",ptr);
    float in_float = strtof(ptr,NULL);
    if(in_float==0){
        plog_error(plugin,"算法服务器反馈数据异常");
        return size * nmemb;
    }
    if(ab_write_float(plugin,plugin->plc_val_address,in_float)!=0){
        plog_error(plugin,"控制失败");
        return size * nmemb;
    }
    float out_float = ab_read_float(plugin,plugin->plc_val_address);
    if(in_float == out_float){
        plog_debug(plugin,"控制成功");
    }else{
        plog_error(plugin,"控制失败");
    }
    return size * nmemb;
}
// 发送HTTP POST请求的函数，包括URL, JSON数据和回调函数
void send_post_request(const char *url, const char *json_data, size_t (*callback)(char *, size_t, size_t, void *),
                       void *arg)
{
    neu_plugin_t *plugin = (neu_plugin_t *) arg;
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

        if (res != CURLE_OK) {
            plugin->common.link_state = NEU_NODE_LINK_STATE_DISCONNECTED;
            plog_debug(plugin,"请求错误: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    } else {
        plog_debug(plugin,"Failed to initialize curl\n");
    }
}