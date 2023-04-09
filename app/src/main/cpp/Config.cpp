//
// Created by chic on 2023/4/10.
//

#include "Config.h"
#include "libevent/include/event2/event.h"

Config::Config() {
    pEventBase = event_base_new();
}
