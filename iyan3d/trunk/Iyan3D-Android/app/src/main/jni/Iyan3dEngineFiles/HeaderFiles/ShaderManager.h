//
//  ShaderManager.h
//  FatMan
//
//  Created by Vivek on 12/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#ifndef __Iyan3D__ShaderManager__
#define __Iyan3D__ShaderManager__

#include <iostream>
#include "ShaderParamOrder.h"
#include "SGNode.h"
#include "Constants.h"


// Functio Name Should be Shader Material Name With Parameter of the Node Index in SMGR.

class ShaderManager{
public:
    Mat4 ortho2d_oc(float left,float right,float bottom,float top,float near,float far);
    static bool isRenderingDepthPass,isRendering , renderingPreview, sceneLighting;
    static bool shadowsOff, lightChanged;
    static float shadowDensity,shadowTextureSize;
    static int maxIntsances;
    static Vector3 camPos;
    static vector<Vector3> lightPosition,lightColor;
    static vector<float> lightFadeDistances;
    static vector< int > lightTypes;
    static Mat4 lighCamProjMatrix;
    static Mat4 lighCamViewMatrix;
    static string BundlePath;
    static DEVICE_TYPE deviceType;
    SceneManager *smgr;
    
    ShaderManager(SceneManager *smgr,DEVICE_TYPE deviceType, int maxUniforms);
    void setUniforms(SGNode *node,string matName);
    void loadAllShaders(SceneManager *smgr,DEVICE_TYPE deviceType, int maxUniforms);
    static std::map<string, string> getShaderStringsToReplace(int maxUniforms);
    
    static bool LoadShader(SceneManager* smgr, DEVICE_TYPE deviceType,string materialName,string vShaderName,string fShaderName, std::map< string, string > shadersStr);
    ~ShaderManager();
    
    
    void setMeshProps(SGNode *sgnNode, bool isVColored, int paramIndex);
    void setIsVertexColored(SGNode *sgNode,bool status, int paramIndex, bool isFragmentData);
    void setNumberOfLights(SGNode *sgNode , int paramIndex);
    void setLightsProperties(SGNode *sgNode, int paramIndex1 , int paramIndex2);
    void setJointTransform(SGNode *node,SkinMesh *SMesh,int paramIndex);
    void setModelViewProjMatrix(SGNode *node,u16 paramIndex, bool isDepthPass = false);
    void setMVPForParticles(SGNode *node, u16 paramIndex);
    void setViewProjMatrix(SGNode *node,u16 paramIndex);
    void setVertexColorUniform(SGNode *sgNode , Vector3 color,int paramIndex,int nodeIndex);
    void setNodeLighting(SGNode *sgNode,int paramIndex);
    void setViewProjMatrix(Material *material);
    void setSceneDataUniforms(SGNode *node,u16 paramIndex);
    void setNodeTransparency(SGNode *node,u16 paramIndex);
    void setModelMatrix(SGNode *node,u16 paramIndex);
    void setProjectionMatrix(SGNode *node,u16 paramIndex);
    void setViewMatrix(SGNode *node,u16 paramIndex);
    void setLightViewProjMatrix(SGNode *node,u16 paramIndex);
    void setVertexColorUniforms(SGNode *node,u16 paramIndex);
    void setTexturesUniforms(SGNode *node,u16 paramIndex);
    void copyMat(float* pointer,Mat4& mat);
    void setJointTransform(SGNode *node,int paramIndex,SceneManager *smgr);
    void setLightViewProjMatrix(SGNode *sgNode,int paramIndex);
    void setShadowDakness(SGNode *node,int paramIndex);
    void setReflectionValue(SGNode *node,int paramIndex);
    void setLightsPosition(SGNode *sgNode , float *lightPositions , int paramIndex);
    void setLightsColors(SGNode *sgNode, float *lightColors, int paramIndex);
    void setEyePos(SGNode *sgNode,int paramIndex);
};
#endif /* defined(__FatMan__ShaderManager__) */
