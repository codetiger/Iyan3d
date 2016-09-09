//
//  Material.h
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__OGLMaterial__
#define __SGEngine2__OGLMaterial__

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif
#include "Material.h"
#include "../../Utilities/Helper.h"

#define MAX_SHADERS_PER_MATERIAL 8

struct uniform {
    std::string name;
    uint32_t location;
    DATA_TYPE type;
    NODE_PROPERTY property;
    unsigned short index;
    unsigned short count;
    void *values;
    short nodeIndex;
    bool isUpdated;
};

struct attribute{
    std::string name;
    uint32_t location;
    DATA_TYPE type;
};

class OGLMaterial : public Material{
    
public:
    vector<uniform> uniforms;
    vector<attribute> attributes;
    uint32_t shaderProgram;
    
    OGLMaterial();
    ~OGLMaterial();
    void AddAttributes(string name,DATA_TYPE type,uint32_t location,u16 bufIndex);
    void AddProperty(string propertyName,NODE_PROPERTY property,DATA_TYPE type,u16 paramIndex = 0,u16 count = 1,uint32_t location = 0,short nodeIndex = NOT_EXISTS);
    virtual short setPropertyValue(string name,float *values,DATA_TYPE type,u16 count,u16 paramIndex = 0,int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS, int renderTargetIndex = NOT_EXISTS);
    virtual short setPropertyValue(string name,int *values,DATA_TYPE type,u16 count,u16 paramIndex = 0,int nodeIndex = NOT_EXISTS, int materialIndex = NOT_EXISTS, int renderTargetIndex = NOT_EXISTS);
    bool LoadShaders(string vShaderName,string fShaderName, std::map< string, string > shadersStr);
    GLuint CompileShader(string shaderName,GLenum shaderType, std::map< string, string > shadersStr);
    GLuint LinkShaders(GLuint vShaderHandle,GLuint fShaderHandle);
    GLuint CreateAttribute(GLuint programHandle,string attribName);
    GLuint CreateUniform(GLuint programHandle,string uniName);
    string getShaderAttributeNameByIndex(int i);
    int getMaterialAttribIndexByName(string name);
};

#endif /* defined(__SGEngine2__OGLMaterial__) */
