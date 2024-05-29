//
// Created by root on 5/22/24.
//

#ifndef NEURON_UTILS_H
#define NEURON_UTILS_H
#include "neuron.h"
#include "../algorithm.h"
int tag_ut_array_to_neu_json_read_resp_t(UT_array *tags, neu_json_read_resp_t *json);
float ab_read_float(neu_plugin_t *plugin, const char *address);
int ab_write_float(neu_plugin_t *plugin, const char *address,float value);
#endif // NEURON_UTILS_H
