//
//  Logger.h
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Logger__
#define __SGEngine2__Logger__

#include <cstdio>
#include <iostream>

using namespace std;

enum logger_message_t {
    ERROR = 0,
    DEBUGGING,
    INFO,
};

enum logger_mode_t {
    LOGGER_MODE_PROD = 0,
    LOGGER_MODE_TEST,
    LOGGER_MODE_DEV,
};

class Logger {
    static logger_mode_t mode;

public:
    static void setMode(logger_mode_t m);
    static void log(logger_message_t type, string module, string message);
};

#endif /* defined(__SGEngine2__Logger__) */
