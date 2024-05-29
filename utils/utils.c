//
// Created by root on 5/22/24.
//
#include "utils.h"
#include <libplctag.h>
int tag_ut_array_to_neu_json_read_resp_t(UT_array *tags, neu_json_read_resp_t *json)
{
    int index = 0;

    if (0 == utarray_len(tags)) {
        return 0;
    }

    json->n_tag = utarray_len(tags);
    json->tags  = (neu_json_read_resp_tag_t *) calloc(json->n_tag, sizeof(neu_json_read_resp_tag_t));
    if (NULL == json->tags) {
        return -1;
    }

    utarray_foreach(tags, neu_resp_tag_value_meta_t *, tag_value)
    {
        neu_tag_value_to_json(tag_value, &json->tags[index]);
        index += 1;
    }

    return 0;
}
float ab_read_float(neu_plugin_t *plugin, const char *address)
{
    float res = 0.0;
    char template[100]="protocol=ab-eip&gateway=%s&path=1,0&cpu=controllogix&name=%s";
    // 用 plugin->host 和 plugin->plc_val_address 替换模板中的 %s
    char *data = malloc(strlen(template) + strlen(plugin->host) + strlen(address));
    sprintf(data, template, plugin->host, address);
    plog_debug(plugin,"控制反馈地址: %s",data);
    int32_t plc_tag = plc_tag_create(data,5000);
    if (plc_tag < 0) {
        plog_error(plugin,"创建PLC标签失败");
        free(data);
        return res;
    }
    free(data);
    int rc = plc_tag_read(plc_tag, 2000);
    if(rc != PLCTAG_STATUS_OK) {
        plog_error(plugin,"读取PLC标签失败");
        return res;
    }
    res = plc_tag_get_float32(plc_tag, 0);
    return res;
}
int ab_write_float(neu_plugin_t *plugin, const char *address,float value)
{
    char template[100]="protocol=ab-eip&gateway=%s&path=1,0&cpu=controllogix&name=%s";
    // 用 plugin->host 和 plugin->plc_val_address 替换模板中的 %s
    char *data = malloc(strlen(template) + strlen(plugin->host) + strlen(address));
    sprintf(data, template, plugin->host, address);
    plog_debug(plugin,"写入地址: %s",data);
    int32_t plc_tag = plc_tag_create(data,5000);
    if (plc_tag < 0) {
        plog_error(plugin,"创建PLC标签失败");
        free(data);
        return -1;
    }
    free(data);
    plc_tag_set_float32(plc_tag,0,value);
    int rc = plc_tag_write(plc_tag,5000);
    if(rc==PLCTAG_STATUS_OK)
    {
        return 0;
    }
    else
    {
        plog_error(plugin,"写入PLC标签失败");
        return -1;
    }
}