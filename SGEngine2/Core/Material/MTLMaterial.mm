//
//  MTLMaterial.m
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "MetalHandler.h"
#include "MTLMaterial.h"

MTLMaterial::MTLMaterial() {
}

MTLMaterial::~MTLMaterial() {
    uniforms.clear();
}

void MTLMaterial::AddProperty(string propertyName, DATA_TYPE type, u16 paramIndex, u16 count, uint32_t location, int nodeIndex, int materialIndex, int renderTargetIndex) {
    MTLUniform uni;
    uni.name              = propertyName;
    uni.type              = type;
    uni.count             = count;
    uni.parameterIndex    = paramIndex;
    uni.nodeIndex         = nodeIndex;
    uni.materialIndex     = materialIndex;
    uni.buf               = nil;
    uni.renderTargetIndex = renderTargetIndex;
    this->uniforms.push_back(uni);
}

short MTLMaterial::setPropertyValue(string name, float* values, DATA_TYPE type, u16 count, u16 paramIndex, int nodeIndex, int materialIndex, int renderTargetIndex) {
    short uIndex = NOT_EXISTS;
    for (int i = 0; i < uniforms.size(); i++) {
        MTLUniform* uniform = &uniforms[i];
        if (uniform->nodeIndex == nodeIndex && uniform->materialIndex == materialIndex && uniform->renderTargetIndex == renderTargetIndex && uniform->name == name) {
            if (uniform->buf == 0 || uniform->count != count) {
                if (uniform->buf)
                    [uniform->buf setPurgeableState:MTLPurgeableStateEmpty];
                uniform->buf = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:MTLResourceCPUCacheModeWriteCombined];
            }

            if (uniform->count != count || memcmp((uint8_t*)[uniform->buf contents], values, count * sizeof(float)) != 0) {
                memcpy((uint8_t*)[uniform->buf contents], values, count * sizeof(float));
                uniform->count = count;
            }

            uIndex = i;
            break;
        }
    }

    if (uIndex == NOT_EXISTS) {
        AddProperty(name, type, paramIndex, count, 0, nodeIndex, materialIndex, renderTargetIndex);
        uIndex                 = uniforms.size() - 1;
        uniforms[uIndex].count = count;
        uniforms[uIndex].buf   = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:MTLResourceCPUCacheModeWriteCombined];
        memcpy((uint8_t*)[uniforms[uIndex].buf contents], values, count * sizeof(float));
    }

    if (uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");

    return uIndex;
}

short MTLMaterial::setPropertyValue(string name, int* values, DATA_TYPE type, u16 count, u16 paramIndex, int nodeIndex, int materialIndex, int renderTargetIndex) {
    short uIndex = NOT_EXISTS;
    for (int i = 0; i < uniforms.size(); i++) {
        MTLUniform* uniform = &uniforms[i];
        if (uniform->nodeIndex == nodeIndex && uniform->materialIndex == materialIndex && uniform->renderTargetIndex == renderTargetIndex && uniform->name == name) {
            if (uniform->buf == 0 || uniform->count != count) {
                if (uniform->buf)
                    [uniform->buf setPurgeableState:MTLPurgeableStateEmpty];
                uniform->buf = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:MTLResourceCPUCacheModeWriteCombined];
            }

            if (uniform->count != count || memcmp((uint8_t*)[uniform->buf contents], values, count * sizeof(int)) != 0) {
                memcpy((uint8_t*)[uniform->buf contents], values, count * sizeof(int));
                uniform->count = count;
            }

            uIndex = i;
            break;
        }
    }

    if (uIndex == NOT_EXISTS) {
        AddProperty(name, type, paramIndex, count, 0, nodeIndex, materialIndex, renderTargetIndex);
        uIndex                 = uniforms.size() - 1;
        uniforms[uIndex].count = count;
        uniforms[uIndex].buf   = [MetalHandler::getMTLDevice() newBufferWithLength:count * sizeof(float) options:MTLResourceCPUCacheModeWriteCombined];
        memcpy((uint8_t*)[uniforms[uIndex].buf contents], values, count * sizeof(int));
    }

    if (uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");

    return uIndex;
}
