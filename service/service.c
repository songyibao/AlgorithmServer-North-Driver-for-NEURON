//
// Created by root on 5/22/24.
//
#include "service.h"

#include "../http_client/http_client.h"
#include "../utils/utils.h"
#include "jansson.h"
#include "math.h"
#include "json/neu_json_rw.h"
int transform(neu_plugin_t *plugin, char *input_json_str, char **output_json_str)
{
    json_t      *input_root, *res_arr;
    json_error_t error;

    // 解析输入JSON字符串
    input_root = json_loads(input_json_str, 0, &error);

    if (!input_root) {
        plog_error(plugin, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }

    // 提取并重组数据
    res_arr = json_array();
    if (json_is_array(json_object_get(input_root, "tags"))) {
        size_t  index;
        json_t *value;

        json_t *tags_array = json_object_get(input_root, "tags");
        size_t  tags_size  = json_array_size(tags_array);
        // 0为heart 1为start_flag
        for (index = 2; index < tags_size && (value = json_array_get(tags_array, index)); index++) {
            json_t *real_value = json_object_get(value, "value");
            // 小数保留两位
            json_real_set(real_value, (round(json_real_value(real_value) * 1000000)) / 1000000);

            if (json_is_real(real_value) || json_is_integer(real_value)) {
                json_array_append_new(res_arr, real_value);
            }
        }
        for (int i = 0; i < 7; i++) {
            json_t *real_value = json_real(0);
            json_array_append_new(res_arr, real_value);
        }
        json_t *integer_value = json_integer(neu_time_ms());

        if (json_is_integer(integer_value)) {
            json_array_append_new(res_arr, integer_value);
        }
    }

    *output_json_str = json_dumps(res_arr, JSON_INDENT(0) | JSON_PRESERVE_ORDER);

    // 释放内存
    json_decref(input_root);
    json_decref(res_arr);

    return 0;
}
int handle_trans_data(neu_plugin_t *plugin, neu_reqresp_head_t *head, neu_reqresp_trans_data_t *trans_data)
{
    int                  ret             = 0;
    char                *json_str        = NULL;
    char                *transformed_str = NULL;
    neu_json_read_resp_t resp            = { 0 };

    tag_ut_array_to_neu_json_read_resp_t(trans_data->tags, &resp);
    for (int i = 0; i < resp.n_tag; i++) {
        if (resp.tags[i].error != 0) {
            plog_error(plugin, "tag %s error: %ld", resp.tags[i].name, resp.tags[i].error);
            return -1;
        }
    }
    ret = neu_json_encode_by_fn(&resp, neu_json_encode_read_resp, &json_str);
    if (ret != 0) {
        plog_notice(plugin, "parse json failed");
        return -1;
    }
    //    plog_debug(plugin, "parse json str succeed: %s", json_str);

    int res = transform(plugin, json_str, &transformed_str);
    if (res != 0) {
        plog_error(plugin, "transform json failed");
        return -1;
    }
    plog_debug(plugin, "transform json str succeed: %s", transformed_str);

    send_post_request(plugin->url, transformed_str, on_response_callback, plugin);
    free(json_str);
    free(transformed_str);
}