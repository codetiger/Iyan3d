//
//  Logger.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Logger.h"
#include <string>

using std::string;
logger_mode_t Logger::mode = LOGGER_MODE_DEV;

void Logger::setMode(logger_mode_t m) {
    Logger::mode = m;
}

void Logger::log(logger_message_t type, string module, string message) {
    string messageTypes[] = {"ERROR", "DEBUG", "INFO"};
    if (type == ERROR)
        printf("[SGE2 - %s - %s] - %s\n", module.c_str(), messageTypes[type].c_str(), message.c_str());
    else if (type == DEBUGGING && mode >= LOGGER_MODE_TEST)
        printf("[SGE2 - %s - %s] - %s\n", module.c_str(), messageTypes[type].c_str(), message.c_str());
    else if (type == INFO && mode >= LOGGER_MODE_DEV)
        printf("[SGE2 - %s - %s] - %s\n", module.c_str(), messageTypes[type].c_str(), message.c_str());
}
