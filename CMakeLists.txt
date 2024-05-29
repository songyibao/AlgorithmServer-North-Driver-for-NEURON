#
# Copyright 2018 Capitar IT Group BV <info@capitar.com>
# Copyright 2018 Staysail Systems, Inc. <info@staysail.tech>
#
# This software is supplied under the terms of the MIT License, a
# copy of which should be located in the distribution where this
# file was obtained (LICENSE.txt).  A copy of the license may also be
# found online at https://opensource.org/licenses/MIT.

cmake_minimum_required(VERSION 3.13)
set(LIBRARY_OUTPUT_PATH "./")
file(COPY /software/neuron/plugins/algorithm/algorithm.json DESTINATION
        /software/neuron/build/plugins/schema)
project(algorithm)

# Call this from your own project's makefile.

find_package(Threads)


add_library(plugin-algorithm SHARED algorithm.c
        http_client/http_client.c
        utils/utils.c
        service/service.c)

target_include_directories(plugin-algorithm PRIVATE
        /software/neuron/include/neuron
)

target_link_libraries(plugin-algorithm PRIVATE z ev)
target_link_libraries(plugin-algorithm PRIVATE /usr/local/lib/neuron/libneuron-base.so curl plctag)
