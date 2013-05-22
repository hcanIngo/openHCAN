#ifndef FRAME_MESSAGE_DESCRIPTION_H
#define FRAME_MESSAGE_DESCRIPTION_H

#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

struct frame_message_description_t {
    uint16_t        proto;
    uint16_t        service;
    string          service_name;
    uint16_t        command;
    string          command_name;
    string          details;
                    vector < string > param_names;
};

extern vector < frame_message_description_t > frame_message_description;

void init_frame_message_description();

#endif

