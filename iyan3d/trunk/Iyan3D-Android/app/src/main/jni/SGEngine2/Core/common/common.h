//
//  common.h
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//


#ifndef SGEngine2_common_h
#define SGEngine2_common_h

#define OPENGL_RENDERER
#undef METAL_RENDERER

#define MAX_TEXTURE_PER_NODE 4
#define ANTI_ALIASING_SAMPLE_COUNT 1

#include <math.h>
#include <string>
#include <vector>
#include <sstream>

#ifdef OPTIMGLKM
#include "Vector2GLK.h"
#include "Vector3GLK.h"
#include "Mat4GLK.h"
#include "QuaternionGLK.h"
#else
#include "Vector2.h"
#include "Vector3.h"
#include "Mat4.h"
#include "Quaternion.h"
#endif


#include "../../Utilities/Logger.h"
#include "../../Utilities/Maths.h"

#include<iostream>
using namespace std;

#define MAX_VERTEX_DATA 4
#define MAX_VERTEX_DATA_SKINNED 7

#define MAX_VERTICES_COUNT 65535

using std::string;
using std::vector;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char c8;
typedef float f32;

static float const PI = 3.141592653589793f;
static const double RADTODEG = (180.0 / PI);
static const double DEGTORAD = (PI / 180.0);
static short const NOT_EXISTS = -1;

static int maxInstanceCount = 100;

typedef enum {
    CPU_SKIN,
    GPU_SKIN
}skin_type;

enum DATA_TYPE {
    DATA_BOOL = 0,
    DATA_INTEGER,
    DATA_FLOAT,
    DATA_FLOAT_VEC2,
    DATA_FLOAT_VEC3,
    DATA_FLOAT_VEC4,
    DATA_FLOAT_MAT2,
    DATA_FLOAT_MAT3,
    DATA_FLOAT_MAT4,
    DATA_TEXTURE_2D,
    DATA_TEXTURE_CUBE

};

enum NODE_PROPERTY {
    NODE_PROPERTY_MODEL,
    NODE_PROPERTY_PROJECTION,
    NODE_PROPERTY_VIEW,
    NODE_PROPERTY_MVP,
    NODE_PROPERTY_VP,
    NODE_PROPERTY_UV1,
    NODE_PROPERTY_USER_DEFINED
};

enum TEXTURE_DATA_FORMAT {
    TEXTURE_RGBA8,
    TEXTURE_RGB8,
    TEXTURE_R8,
    TEXTURE_RG,
    TEXTURE_DEPTH32
};
enum TEXTURE_DATA_TYPE {
    TEXTURE_BYTE
};
enum DEVICE_TYPE {
    OPENGLES2,
    METAL
};

enum FPLANES {
    F_FAR_PLANE = 0,
    F_NEAR_PLANE = 1,
    F_LEFT_PLANE,
    F_RIGHT_PLANE,
    F_BOTTOM_PLANE,
    F_TOP_PLANE,
    F_PLANE_COUNT = 6
};
enum METAL_DEPTH_FUNCTION {
    CompareFunctionNever = 0,
    CompareFunctionLess = 1,
    CompareFunctionEqual = 2,
    CompareFunctionLessEqual = 3,
    CompareFunctionGreater = 4,
    CompareFunctionNotEqual = 5,
    CompareFunctionGreaterEqual = 6,
    CompareFunctionAlways = 7
};

enum IMAGE_FLIP {
    NO_FLIP,
    FLIP_HORIZONTAL,
    FLIP_VERTICAL
};

//template <typename T>
//std::string to_string(T value)
//{
//    std::ostringstream ostringstream1 ;
//    ostringstream1 << value ;
//    return ostringstream1.str() ;
//}

//template <typename T>
//int  stringToInt(T string1){
//    string buffer=string1;
//    int a = 0;
//
//    for(string::iterator it = buffer.begin(); it != buffer.end(); ++it)
//    {
//        a=(a*10) + (*it-48);
//    }
//    cout<<"the value"<<a;
//
//    return a;
//}
//
//template <typename T>
//float stringToFloat(T string1){
//    std::string num = string1;
//    double temp = ::atof(num.c_str());
//    return temp;
//
//}

static const int totalAttributes = MAX_VERTEX_DATA;
static const string attributesName[MAX_VERTEX_DATA] = { "vertPosition", "vertNormal", "texCoord1", "optionalData1" };
static const DATA_TYPE attributesType[MAX_VERTEX_DATA] = { DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT };
static const u_int32_t attributesTotalValues[MAX_VERTEX_DATA] = { 3, 3, 2, 4 };

static const int totalAttributesSkinned = MAX_VERTEX_DATA_SKINNED;
static const string attributesNameSkinned[MAX_VERTEX_DATA_SKINNED] = { "vertPosition", "vertNormal", "texCoord1", "optionalData1", "optionalData2", "optionalData3", "optionalData4" };
static const DATA_TYPE attributesTypeSkinned[MAX_VERTEX_DATA_SKINNED] = { DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT };
static const u_int32_t attributesTotalValuesSkinned[MAX_VERTEX_DATA_SKINNED] = { 3, 3, 2, 4, 4, 4, 4 };

class common {
public:
    static DEVICE_TYPE deviceType;
};

#endif
