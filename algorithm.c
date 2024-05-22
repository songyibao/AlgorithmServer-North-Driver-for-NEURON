//
// Created by root on 5/22/24.
//
#include "algorithm.h"
#include "service/service.h"
#define DESCRIPTION "Plugins for interacting with control algorithms"
#define DESCRIPTION_ZH "与控制算法交互的插件"
static const neu_plugin_intf_funs_t plugin_intf_funs = {
    .open    = driver_open,
    .close   = driver_close,
    .init    = driver_init,
    .uninit  = driver_uninit,
    .start   = driver_start,
    .stop    = driver_stop,
    .setting = driver_config,
    .request = driver_request,

    .driver.validate_tag = driver_validate_tag,
    .driver.group_timer  = driver_group_timer,
    .driver.write_tag    = driver_write,
};

const neu_plugin_module_t neu_plugin_module = {
    .version         = NEURON_PLUGIN_VER_1_0,
    .schema          = "algorithm",
    .module_name     = "Algorithm Server",
    .module_descr    = DESCRIPTION,
    .module_descr_zh = DESCRIPTION_ZH,
    .intf_funs       = &plugin_intf_funs,
    .kind            = NEU_PLUGIN_KIND_SYSTEM,
    .type            = NEU_NA_TYPE_APP,
    .display         = true,
    .single          = false,
};

static neu_plugin_t *driver_open(void)
{

    neu_plugin_t *plugin = calloc(1, sizeof(neu_plugin_t));

    neu_plugin_common_init(&plugin->common);
    plugin->common.link_state = NEU_NODE_LINK_STATE_DISCONNECTED;

    return plugin;
}

static int driver_close(neu_plugin_t *plugin)
{
    free(plugin);

    return 0;
}
// driver_init -> driver_config -> driver_start
static int driver_init(neu_plugin_t *plugin, bool load)
{
    (void) load;
    plog_notice(plugin,
                "============================================================"
                "\ninitialize "
                "plugin============================================================\n");
    plugin->connected = false;
    plugin->started   = false;
    return 0;
}

static int driver_config(neu_plugin_t *plugin, const char *setting)
{
    plog_notice(plugin,
                "============================================================\nconfig "
                "plugin============================================================\n");
    int   ret       = 0;
    char *err_param = NULL;

    neu_json_elem_t url = { .name = "url", .t = NEU_JSON_STR, .v.val_str = NULL };

    ret = neu_parse_param((char *) setting, &err_param, 1, &url);

    strcpy(plugin->url, url.v.val_str);
    plog_notice(plugin, "config: url: %s", plugin->url);
    return 0;
}

static int driver_start(neu_plugin_t *plugin)
{
    plog_notice(plugin,
                "============================================================\nstart "
                "plugin============================================================\n");
    plugin->started = true;
    return 0;
}

static int driver_stop(neu_plugin_t *plugin)
{

    plog_notice(plugin,
                "============================================================\nstop "
                "plugin============================================================\n");
    plugin->started = false;
    return 0;
}

static int driver_uninit(neu_plugin_t *plugin)
{
    plog_notice(plugin,
                "============================================================\nuninit "
                "plugin============================================================\n");

    return NEU_ERR_SUCCESS;
}

static int driver_request(neu_plugin_t *plugin, neu_reqresp_head_t *head, void *data)
{
    plog_notice(plugin,
                "============================================================request "
                "plugin============================================================\n");
    if (plugin->started == false) {
        return 0;
    }
    switch (head->type) {
    case NEU_REQRESP_TRANS_DATA:
        handle_trans_data(plugin, head, data);
    }
    return 0;
}

static int driver_validate_tag(neu_plugin_t *plugin, neu_datatag_t *tag)
{
    plog_notice(plugin, "validate tag: %s", tag->name);

    return 0;
}

static int driver_group_timer(neu_plugin_t *plugin, neu_plugin_group_t *group)
{
    (void) plugin;
    (void) group;

    return 0;
}

static int driver_write(neu_plugin_t *plugin, void *req, neu_datatag_t *tag, neu_value_u value)
{
    (void) plugin;
    (void) req;
    (void) tag;
    (void) value;

    return 0;
}