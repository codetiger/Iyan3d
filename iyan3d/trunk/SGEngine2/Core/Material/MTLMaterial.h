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
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#include <stdint.h>

using namespace std;

struct MTLUniform {
    std::string   name;
    uint32_t      location;
    DATA_TYPE     type;
    u16           parameterIndex, count;
    void*         values;
    id<MTLBuffer> buf;
    short         nodeIndex, materialIndex, renderTargetIndex;
};

class MTLMaterial : public Material {
public:
    vector<MTLUniform> uniforms;
    MTLMaterial();
    ~MTLMaterial();
    id<MTLRenderPipelineState> PipelineState;
    void                       AddProperty(string propertyName, DATA_TYPE type, u16 paramIndex = 0, u16 count = 1, uint32_t location = 0, int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS, int renderTargetIndex = NOT_EXISTS);
    virtual short              setPropertyValue(string name, float* values, DATA_TYPE type, u16 count, u16 paramIndex = 0, int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS, int renderTargetIndex = NOT_EXISTS);
    virtual short              setPropertyValue(string name, int* values, DATA_TYPE type, u16 count, u16 paramIndex = 0, int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS, int renderTargetIndex = NOT_EXISTS);
};
#endif
