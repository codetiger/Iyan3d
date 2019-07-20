//
//  Helper.cpp
//  Iyan3D
//
//  Created by Vivek on 19/03/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#include "Helper.h"

GLenum Helper::getOGLES2DataType(DATA_TYPE type) {
    switch (type) {
        case DATA_BOOL:
            return GL_BOOL;
            break;
        case DATA_FLOAT:
            return GL_FLOAT;
            break;
        case DATA_INTEGER:
            return GL_INT;
            break;
        case DATA_FLOAT_MAT4:
            break;
        default:
            break;
    }
    return 0;
}

DATA_TYPE Helper::getSGEngineDataType(GLenum type) {
    switch (type) {
        case GL_BOOL:
            return DATA_BOOL;
            break;
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC4:
        case GL_FLOAT:
            return DATA_FLOAT;
            break;
        case GL_INT:
            return DATA_INTEGER;
            break;
        case DATA_FLOAT_MAT4:
            break;
        default:
            break;
    }
    Logger::log(ERROR, "Helper", "Mismatched OGLDataType");
    return DATA_FLOAT;
}

Vector2 Helper::screenToOpenglCoords(Vector2 coord, float screenWidth, float screenHeight) {
    float sWMid        = screenWidth / 2.0;
    float sHMid        = screenHeight / 2.0;
    float difFromsWMid = (coord.x - sWMid);
    float difFromsHMid = (sHMid - coord.y);
    return Vector2(difFromsWMid / sWMid, difFromsHMid / sHMid);
}
