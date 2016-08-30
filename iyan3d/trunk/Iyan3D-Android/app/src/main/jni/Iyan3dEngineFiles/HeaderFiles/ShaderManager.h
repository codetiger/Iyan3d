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

class ShaderManager {
public:
    Mat4 ortho2d_oc(float left,float right,float bottom,float top,float near,float far);
    static bool isRenderingDepthPass,isRendering , renderingPreview, sceneLighting;
    static bool shadowsOff, lightChanged;
    static float shadowDensity,shadowTextureSize;
    static int maxIntsances;
    static int maxJoints;
    static Vector3 camPos;
    static vector<Vector3> lightPosition,lightColor;
    static vector<float> lightFadeDistances;
    static vector< int > lightTypes;
    static Mat4 lighCamProjMatrix;
    static Mat4 lighCamViewMatrix;
    static string BundlePath;
    static DEVICE_TYPE deviceType;
    static float ambientLight;
    
    Texture* shadowTexture;
    Texture* environmentTex;
    SceneManager *smgr;
    
    std::map< PROP_INDEX, Property > sceneProps;
    
    ShaderManager(SceneManager *smgr,DEVICE_TYPE deviceType, int maxUniforms, int maxJoints);
    void setUniforms(SGNode *node, string matName, int materialIndex = 0);
    void loadAllShaders(SceneManager *smgr,DEVICE_TYPE deviceType, int maxUniforms, int maxJoints);
    static std::map<string, string> getShaderStringsToReplace(int maxUniforms);
    static std::map<string, string> getStringsForRiggedObjects(int maxJoints);
    
    static bool LoadShader(SceneManager* smgr, DEVICE_TYPE deviceType,string materialName,string vShaderName,string fShaderName, std::map< string, string > shadersStr, bool isTest = false);
    ~ShaderManager();
    
    
    void setMeshProps(SGNode *sgnNode, bool isVColored, int paramIndex);
    void setHasMeshColor(SGNode *sgNode, bool status, int paramIndex, bool isFragmentData, int materialIndex);
    void setNumberOfLights(SGNode *sgNode, int paramIndex);
    void setLightsProperties(SGNode *sgNode, int param1, int param2, int param3, int param4, int param5);
    void setJointTransform(SGNode *node, SkinMesh *SMesh, int paramIndex);
    void setModelViewProjMatrix(SGNode *node, u16 paramIndex, bool isDepthPass = false);
    void setMVPForParticles(SGNode *node, u16 paramIndex);
    void setViewProjMatrix(SGNode *node, u16 paramIndex);
    void setVertexColorUniform(SGNode *sgNode, Vector4 color, int paramIndex, int nodeIndex, int materialIndex);
    void setNodeLighting(SGNode *sgNode, int paramIndex, int materialIndex);
    void setViewProjMatrix(Material *material);
    void setSceneDataUniforms(SGNode *node, u16 paramIndex);
    void setNodeTransparency(SGNode *node, u16 paramIndex, int materialIndex);
    void setUVScaleValue(SGNode *sgNode, u16 paramIndex, int materialIndex);
    void setModelMatrix(SGNode *node, u16 paramIndex);
    void setProjectionMatrix(SGNode *node, u16 paramIndex);
    void setViewMatrix(SGNode *node, u16 paramIndex);
    void setTextureForNode(SGNode* sgNode, Texture* texture, string textureName, int paramIndex, int userValue, int materialIndex);
    void setLightViewProjMatrix(SGNode *node, u16 paramIndex);
    void setVertexColorUniforms(SGNode *node, u16 paramIndex);
    void setTexturesUniforms(SGNode *node, u16 paramIndex, int materialIndex);
    void setSamplerType(SGNode *sgNode, u16 paramIndex);
    void copyMat(float* pointer, Mat4& mat);
    void setJointTransform(SGNode *node, int paramIndex, SceneManager *smgr);
    void setLightViewProjMatrix(SGNode *sgNode, int paramIndex);
    void setShadowDakness(SGNode *node, int paramIndex);
    void setReflectionValue(SGNode *node, int paramIndex, int materialIndex);
    void setLightsPosition(SGNode *sgNode, float *lightPositions, int paramIndex);
    void setLightsColors(SGNode *sgNode, float *lightColors, int paramIndex);
    void setEyePos(SGNode *sgNode, int paramIndex);
    
    void addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE type = TYPE_NONE, string title = "", string groupName = " ", string fileName = "", ICON_INDEX iconId = NO_ICON);
    void checkAndUpdatePropsMap(std::map < PROP_INDEX, Property > &propsMap, Property property);
    Property& getProperty(PROP_INDEX pIndex);

};
#endif /* defined(__FatMan__ShaderManager__) */
