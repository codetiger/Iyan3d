//
//  MTLMaterial.m
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//
#ifdef IOS
#import <Foundation/Foundation.h>
#endif
#include "MTLMaterial.h"
#include "MetalHandler.h"

MTLMaterial::MTLMaterial(){
    
}
MTLMaterial::~MTLMaterial(){
    uniforms.clear();
}
void MTLMaterial::AddProperty(string propertyName,NODE_PROPERTY property,DATA_TYPE type,u16 paramIndex,u16 count,uint32_t location,int nodeIndex,short renderTargetIndex){
    MTLUniform uni;
    uni.name = propertyName;
    uni.type = type;
    uni.property = property;
    uni.index = this->uniforms.size();
    uni.count = count;
    uni.parameterIndex = paramIndex;
    uni.nodeIndex = nodeIndex;
    uni.buf = nil;
    uni.renderTargetIndex = renderTargetIndex;
    this->uniforms.push_back(uni);
}
short MTLMaterial::setPropertyValue(string name,float *values,DATA_TYPE type,u16 count,u16 paramIndex,int nodeIndex,int renderTargetIndex){
    short uIndex = NOT_EXISTS;
    for(int i = 0; i < uniforms.size();i++){ //ToDo search optimisation
        if(uniforms[i].name == name && uniforms[i].nodeIndex == nodeIndex && uniforms[i].renderTargetIndex == renderTargetIndex && uniforms[i].count == count){
            uniforms[i].values = values;
            uIndex = i;
            break;
        }
    }
    if(uIndex == NOT_EXISTS){
        AddProperty(name,NODE_PROPERTY_USER_DEFINED,type,paramIndex,count,0,nodeIndex,renderTargetIndex);
        uIndex = uniforms.size()-1;
        int bufSize = count * sizeof(float);
        uniforms[uIndex].values = values;
        uniforms[uIndex].buf = [MetalHandler::getMTLDevice() newBufferWithLength:bufSize options:0];
    }
    if(uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");
    return uIndex;
}
short MTLMaterial::setPropertyValue(string name,int *values,DATA_TYPE type,u16 count,u16 paramIndex,int nodeIndex,int renderTargetIndex){
    
    short uIndex = NOT_EXISTS;
    for(int i = 0; i < uniforms.size();i++){
        if(uniforms[i].name == name && uniforms[i].nodeIndex == nodeIndex && uniforms[i].renderTargetIndex == renderTargetIndex){
            uniforms[i].values = values;
            uIndex = i;
            break;
        }
    }
    if(uIndex == NOT_EXISTS){
        AddProperty(name,NODE_PROPERTY_USER_DEFINED,type,paramIndex,count,0,nodeIndex,renderTargetIndex);
        uIndex = uniforms.size()-1;
        int bufSize = count * sizeof(int);
        uniforms[uIndex].values = values;
        uniforms[uIndex].buf = [MetalHandler::getMTLDevice() newBufferWithLength:bufSize options:0];
    }
    if(uIndex == NOT_EXISTS)
        Logger::log(ERROR, "MTLMaterial", "Error in setting" + name + "Property");
    return uIndex;
}
