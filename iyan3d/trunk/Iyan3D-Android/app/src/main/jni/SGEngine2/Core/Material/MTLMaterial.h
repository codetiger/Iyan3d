//
//  MTLMaterial.h
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#ifndef SGEngine2_MTLMaterial_h
#define SGEngine2_MTLMaterial_h

#include <iostream>
#include "Material.h"
#ifdef ANDROID
#include "../../../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-x86_64/usr/include/stdint.h"
#endif
#ifdef IOS
#if !(TARGET_IPHONE_SIMULATOR)
    #import <QuartzCore/CAMetalLayer.h>
    #import <Metal/Metal.h>
#include <stdint.h>

#endif
#endif


using namespace std;

struct MTLUniform{
    std::string name;
    uint32_t location;
    DATA_TYPE type;
    NODE_PROPERTY property;
    u16 parameterIndex, index, count;
    void *values;
#if !(TARGET_IPHONE_SIMULATOR)
    id<MTLBuffer> buf;
#endif
    short nodeIndex, materialIndex;
};

class MTLMaterial : public Material{
    
public:
#if !(TARGET_IPHONE_SIMULATOR)
    vector<MTLUniform> uniforms;
    MTLMaterial();
    ~MTLMaterial();
    id <MTLRenderPipelineState> PipelineState;
    void AddProperty(string propertyName, NODE_PROPERTY property, DATA_TYPE type, u16 paramIndex = 0, u16 count = 1, uint32_t location = 0, int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS);
    virtual short setPropertyValue(string name, float *values, DATA_TYPE type, u16 count, u16 paramIndex = 0, int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS);
    virtual short setPropertyValue(string name, int *values, DATA_TYPE type, u16 count, u16 paramIndex = 0, int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS);
#endif

};
#endif
