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

void MTLMaterial::AddProperty(string propertyName, NODE_PROPERTY property, DATA_TYPE type, u16 paramIndex, u16 count, uint32_t location, int nodeIndex)
{
    MTLUniform uni;
    uni.name = propertyName;
    uni.type = type;
    uni.property = property;
    uni.index = this->uniforms.size();
    uni.count = count;
    uni.parameterIndex = paramIndex;
    uni.nodeIndex = nodeIndex;
    uni.buf = nil;
    this->uniforms.push_back(uni);
}

short MTLMaterial::setPropertyValue(string name, float *values, DATA_TYPE type, u16 count, u16 paramIndex, int nodeIndex)
{
    short uIndex = NOT_EXISTS;
    for(int i = 0; i < uniforms.size(); i++) { //ToDo search optimisation
        if(uniforms[i].name == name && uniforms[i].nodeIndex == nodeIndex) {
            if(uniforms[i].values == NULL || uniforms[i].count != count)
                uniforms[i].values = new float[count];

            if(memcmp(uniforms[i].values, values, count * sizeof(float)) != 0) {
                memcpy(uniforms[i].values, values, count * sizeof(float));
                uniforms[i].count = count;
            }

            uIndex = i;
            break;
        }
    }
    
    if(uIndex == NOT_EXISTS) {
        AddProperty(name, NODE_PROPERTY_USER_DEFINED, type, paramIndex, count, 0, nodeIndex);
        uIndex = uniforms.size()-1;
        uniforms[uIndex].values = new float[count];
        memcpy(uniforms[uIndex].values, values, count * sizeof(float));
        uniforms[uIndex].count = count;

        int bufSize = count * sizeof(float);
        uniforms[uIndex].buf = [MetalHandler::getMTLDevice() newBufferWithLength:bufSize options:0];
    }
    
    if(uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");

    return uIndex;
}

short MTLMaterial::setPropertyValue(string name, int *values, DATA_TYPE type, u16 count, u16 paramIndex, int nodeIndex)
{
    
    short uIndex = NOT_EXISTS;
    for(int i = 0; i < uniforms.size(); i++) {
        if(uniforms[i].name == name && uniforms[i].nodeIndex == nodeIndex) {
            if(uniforms[i].values == NULL || uniforms[i].count != count)
                uniforms[i].values = new int[count];
            
            if(memcmp(uniforms[i].values, values, count * sizeof(int)) != 0) {
                memcpy(uniforms[i].values, values, count * sizeof(int));
                uniforms[i].count = count;
            }
            
            uIndex = i;
            break;
        }
    }
    
    if(uIndex == NOT_EXISTS) {
        AddProperty(name, NODE_PROPERTY_USER_DEFINED, type, paramIndex, count, 0, nodeIndex);
        uIndex = uniforms.size()-1;
        uniforms[uIndex].values = new int[count];
        memcpy(uniforms[uIndex].values, values, count * sizeof(int));
        uniforms[uIndex].count = count;
        
        int bufSize = count * sizeof(int);
        uniforms[uIndex].buf = [MetalHandler::getMTLDevice() newBufferWithLength:bufSize options:0];
    }
    
    if(uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");

    return uIndex;
}
