//
//  MTLMaterial.m
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//
#ifdef IOS
#import <Foundation/Foundation.h>
#include "MetalHandler.h"
#endif
#include "MTLMaterial.h"

MTLMaterial::MTLMaterial()
{
    
}

MTLMaterial::~MTLMaterial()
{
    uniforms.clear();
}

void MTLMaterial::AddProperty(string propertyName, NODE_PROPERTY property, DATA_TYPE type, u16 paramIndex, u16 count, uint32_t location, int nodeIndex, int materialIndex, int renderTargetIndex)
{
    MTLUniform uni;
    uni.name = propertyName;
    uni.type = type;
    uni.property = property;
    uni.index = this->uniforms.size();
    uni.count = count;
    uni.parameterIndex = paramIndex;
    uni.nodeIndex = nodeIndex;
    uni.materialIndex = materialIndex;
    uni.buf = nil;
    uni.renderTargetIndex = renderTargetIndex;
    this->uniforms.push_back(uni);
}

short MTLMaterial::setPropertyValue(string name, float *values, DATA_TYPE type, u16 count, u16 paramIndex, int nodeIndex, int materialIndex, int renderTargetIndex)
{
    short uIndex = NOT_EXISTS;
    for(int i = 0; i < uniforms.size(); i++) { //ToDo search optimisation
        if(uniforms[i].name == name && uniforms[i].nodeIndex == nodeIndex && uniforms[i].materialIndex == materialIndex && uniforms[i].renderTargetIndex == renderTargetIndex) {
            if(uniforms[i].buf == 0 || uniforms[i].count != count) {
                uniforms[i].buf = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:0];
            }

            if(uniforms[i].count != count || memcmp((uint8_t *)[uniforms[i].buf contents], values, count * sizeof(float)) != 0) {
                memcpy((uint8_t *)[uniforms[i].buf contents], values, count * sizeof(float));
                uniforms[i].count = count;
            }

            uIndex = i;
            break;
        }
    }
    
    if(uIndex == NOT_EXISTS) {
        AddProperty(name, NODE_PROPERTY_USER_DEFINED, type, paramIndex, count, 0, nodeIndex, materialIndex, renderTargetIndex);
        uIndex = uniforms.size()-1;
        uniforms[uIndex].count = count;
        uniforms[uIndex].buf = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:0];
        memcpy((uint8_t *)[uniforms[uIndex].buf contents], values, count * sizeof(float));
    }
    
    if(uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");

    return uIndex;
}

short MTLMaterial::setPropertyValue(string name, int *values, DATA_TYPE type, u16 count, u16 paramIndex, int nodeIndex, int materialIndex, int renderTargetIndex)
{
    
    short uIndex = NOT_EXISTS;
    for(int i = 0; i < uniforms.size(); i++) {
        if(uniforms[i].name == name && uniforms[i].nodeIndex == nodeIndex && uniforms[i].materialIndex == materialIndex && uniforms[i].renderTargetIndex == renderTargetIndex) {
            if(uniforms[i].buf == 0 || uniforms[i].count != count) {
                uniforms[i].buf = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:0];
            }
            
            if(uniforms[i].count != count || memcmp((uint8_t *)[uniforms[i].buf contents], values, count * sizeof(int)) != 0) {
                memcpy((uint8_t *)[uniforms[i].buf contents], values, count * sizeof(int));
                uniforms[i].count = count;
            }
            
            uIndex = i;
            break;
        }
    }
    
    if(uIndex == NOT_EXISTS) {
        AddProperty(name, NODE_PROPERTY_USER_DEFINED, type, paramIndex, count, 0, nodeIndex, materialIndex, renderTargetIndex);
        uIndex = uniforms.size()-1;
        uniforms[uIndex].count = count;
        uniforms[uIndex].buf = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:0];
        memcpy((uint8_t *)[uniforms[uIndex].buf contents], values, count * sizeof(int));
    }
    
    if(uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");

    return uIndex;
}
