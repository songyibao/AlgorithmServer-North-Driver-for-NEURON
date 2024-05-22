//
// Created by root on 5/22/24.
//

#ifndef NEURON_HTTP_CLIENT_H
#define NEURON_HTTP_CLIENT_H
#include <stdio.h>
#include <string.h>
size_t on_response_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
void send_post_request(const char *url, const char *json_data, size_t (*callback)(char *, size_t, size_t, void *),void *arg);
#endif // NEURON_HTTP_CLIENT_H
