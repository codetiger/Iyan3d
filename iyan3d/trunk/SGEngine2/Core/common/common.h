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

#include "Vector2.h"
#include "Vector3.h"
#include "Mat4.h"
#include "Quaternion.h"

#include "../../Utilities/Logger.h"
#include "../../Utilities/Maths.h"

#include<iostream>
using namespace std;

#define MAX_VERTEX_DATA 6
#define MAX_VERTEX_DATA_SKINNED 10

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

static const int totalAttributes = MAX_VERTEX_DATA;
static const string attributesName[MAX_VERTEX_DATA] = { "vertPosition", "vertNormal", "texCoord1", "vertTangent", "vertBitangent", "vertColor" };
static const DATA_TYPE attributesType[MAX_VERTEX_DATA] = { DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT };
static const u_int32_t attributesTotalValues[MAX_VERTEX_DATA] = { 3, 3, 2, 3, 3, 4 };

static const int totalAttributesSkinned = MAX_VERTEX_DATA_SKINNED;
static const string attributesNameSkinned[MAX_VERTEX_DATA_SKINNED] = { "vertPosition", "vertNormal", "texCoord1", "vertTangent", "vertBitangent", "vertColor", "optionalData1", "optionalData2", "optionalData3", "optionalData4" };
static const DATA_TYPE attributesTypeSkinned[MAX_VERTEX_DATA_SKINNED] = { DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT, DATA_FLOAT };
static const u_int32_t attributesTotalValuesSkinned[MAX_VERTEX_DATA_SKINNED] = { 3, 3, 2, 3, 3, 4, 4, 4, 4, 4 };

class common {
public:
};

#endif
