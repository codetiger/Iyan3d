//
//  ShaderManager.cpp
//  FatMan
//
//  Created by Vivek on 12/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#include "HeaderFiles/ShaderManager.h"

#ifdef ANDROID
#include "../opengl.h"
#endif

bool ShaderManager::isRenderingDepthPass = false;
bool ShaderManager::isRendering = false;
bool ShaderManager::sceneLighting = true;
float ShaderManager::shadowDensity = 0.0, ShaderManager::shadowTextureSize = 2048.0;
Vector3 ShaderManager::camPos = Vector3(0.0);
vector<Vector3> ShaderManager::lightPosition;// = Vector3(0.0);
vector<Vector3> ShaderManager::lightColor;// = Vector3(1.0);
vector<float> ShaderManager::lightFadeDistances;
Mat4 ShaderManager::lighCamProjMatrix;
Mat4 ShaderManager::lighCamViewMatrix;
string ShaderManager::BundlePath = " ";
DEVICE_TYPE ShaderManager::deviceType = OPENGLES2;

ShaderManager::~ShaderManager(){
    
}

ShaderManager::ShaderManager(SceneManager *smgr,DEVICE_TYPE deviceType){
    isRenderingDepthPass = false;
    shadowDensity = 0.0;
    shadowTextureSize = 2048.0;
    camPos = Vector3(0.0);
    //lightPosition[0] = Vector3(0.0);
    //lightColor[0] = Vector3(1.0);
    this->smgr = smgr;
    ShaderManager::BundlePath = constants::BundlePath;
    ShaderManager::deviceType = deviceType;
    loadAllShaders(smgr,deviceType);
}
void ShaderManager::loadAllShaders(SceneManager *smgr,DEVICE_TYPE deviceType){
    for(int i = 0; i < TOTAL_MATERIALS;i++){
        if(deviceType == OPENGLES2)
            LoadShader(deviceType,OGLMaterialAndShaderNames[i][0],OGLMaterialAndShaderNames[i][1],OGLMaterialAndShaderNames[i][2]);
        if(deviceType == METAL)
            LoadShader(deviceType,MTLMaterialAndShaderNames[i][0],MTLMaterialAndShaderNames[i][1],MTLMaterialAndShaderNames[i][2]);
    }
}
short ShaderManager::LoadShader(DEVICE_TYPE deviceType,string materialName,string vShaderName,string fShaderName){
    bool isDepthPass = (fShaderName=="")?true:false;
    if(deviceType == OPENGLES2) {
        return smgr->LoadShaders(materialName, BundlePath + "/" + vShaderName,
                                 BundlePath + "/" + fShaderName);
    }
    else if(deviceType == METAL)
        return smgr->LoadShaders(materialName,vShaderName,fShaderName,isDepthPass);
    return NOT_EXISTS;
}
void ShaderManager::setUniforms(SGNode *sgNode,string matName){
    if(matName == "SHADER_SHADOW_1ST_PASS"){
        
    }else if(matName.find("SHADER_VERTEX_COLOR_SHADOW_SKIN") !=  string::npos || matName.find("SHADER_VERTEX_COLOR_SKIN_TOON") !=  string::npos || matName.find("SHADER_VERTEX_COLOR_SKIN") !=  string::npos){
        setIsVertexColored(sgNode, true , SHADER_COMMON_isVertexColored, true);
        setJointTransform(sgNode, SHADER_PERVERTEXCOLOR_jointData, smgr);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode, SHADER_PERVERTEXCOLOR_mvp);
        setModelMatrix(sgNode, SHADER_PERVERTEXCOLOR_world);
        setNodeTransparency(sgNode,SHADER_PERVERTEXCOLOR_transparency);
        setNodeLighting(sgNode,SHADER_PERVERTEXCOLOR_isLighting);
        setShadowDakness(sgNode,SHADER_PERVERTEXCOLOR_shadowDarkness);
        setBrightnessValue(sgNode,SHADER_PERVERTEXCOLOR_brightness);
        setShininessValue(sgNode,SHADER_PERVERTEXCOLOR_shininess);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setEyePos(sgNode,SHADER_PERVERTEXCOLOR_eyePos);
        setJointTransform(sgNode,SHADER_COMMON_SKIN_jointData,smgr);
        setLightViewProjMatrix(sgNode,SHADER_PERVERTEXCOLOR_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_PERVERTEXCOLOR_viewMatrix);
    }else if(matName.find("SHADER_VERTEX_COLOR_SHADOW") !=  string::npos || matName.find("SHADER_VERTEX_COLOR") !=  string::npos){
        setIsVertexColored(sgNode, true , SHADER_COMMON_isVertexColored, true);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode, SHADER_PERVERTEXCOLOR_mvp);
        setModelMatrix(sgNode, SHADER_PERVERTEXCOLOR_world);
        setNodeTransparency(sgNode,SHADER_PERVERTEXCOLOR_transparency);
        setNodeLighting(sgNode,SHADER_PERVERTEXCOLOR_isLighting);
        setShadowDakness(sgNode,SHADER_PERVERTEXCOLOR_shadowDarkness);
        setBrightnessValue(sgNode,SHADER_PERVERTEXCOLOR_brightness);
        setShininessValue(sgNode,SHADER_PERVERTEXCOLOR_shininess);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setEyePos(sgNode,SHADER_PERVERTEXCOLOR_eyePos);
        setLightViewProjMatrix(sgNode,SHADER_PERVERTEXCOLOR_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_PERVERTEXCOLOR_viewMatrix);
    }else if(matName == "SHADER_COLOR"){
        setModelViewProjMatrix(sgNode, SHADER_COLOR_mvp);
        Vector3 vertexColor = sgNode->props.isSelected ? Vector3(0.0,1.0,0.0) : sgNode->props.vertexColor;
        setVertexColorUniform(sgNode->node->material,vertexColor,SHADER_COLOR_vertexColor,smgr->getNodeIndexByID(sgNode->node->getID()));
        setNodeTransparency(sgNode,SHADER_COLOR_transparency);
    }else if(matName == "SHADER_COLOR_SKIN" || matName == "SHADER_COLOR_TEXT"){
        setModelViewProjMatrix(sgNode,SHADER_COLOR_SKIN_mvp);
        Vector3 vertexColor = sgNode->props.isSelected ? Vector3(0.0,1.0,0.0) : sgNode->props.vertexColor;
        setVertexColorUniform(sgNode->node->material,vertexColor,SHADER_COLOR_SKIN_vertexColor,smgr->getNodeIndexByID(sgNode->node->getID()));
        setNodeTransparency(sgNode,SHADER_COLOR_SKIN_transparency);
        setJointTransform(sgNode,SHADER_COLOR_SKIN_jointData,smgr);
    }else if(matName.find("SHADER_COMMON_SKIN") !=  string::npos || matName.find("SHADER_TOON_SKIN") !=  string::npos){
        setVertexColorUniform(sgNode->node->material,sgNode->props.vertexColor,SHADER_COMMON_SKIN_VertexColor,smgr->getNodeIndexByID(sgNode->node->getID()));
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, false);
        
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, true);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode,SHADER_COMMON_SKIN_mvp);
        setModelMatrix(sgNode, SHADER_COMMON_SKIN_world);
        setNodeTransparency(sgNode,SHADER_COMMON_SKIN_transparency);
        setNodeLighting(sgNode,SHADER_COMMON_SKIN_isLighting);
        setShadowDakness(sgNode,SHADER_COMMON_SKIN_shadowDarkness);
        setBrightnessValue(sgNode,SHADER_COMMON_SKIN_brightness);
        setShininessValue(sgNode,SHADER_COMMON_SKIN_shininess);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setEyePos(sgNode,SHADER_COMMON_SKIN_eyePos);
        setJointTransform(sgNode,SHADER_COMMON_SKIN_jointData,smgr);
        setLightViewProjMatrix(sgNode,SHADER_COMMON_SKIN_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_COMMON_SKIN_ViewMatrix);
    }else if(matName.find("SHADER_COMMON") !=  string::npos || matName.find("SHADER_TOON") !=  string::npos){
        setIsVertexColored(sgNode, false , SHADER_COMMON_isVertexColored, true);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode,SHADER_COMMON_mvp);
        setModelMatrix(sgNode, SHADER_COMMON_world);
        setNodeTransparency(sgNode,SHADER_COMMON_transparency);
        setNodeLighting(sgNode,SHADER_COMMON_isLighting);
        setShadowDakness(sgNode,SHADER_COMMON_shadowDarkness);
        setBrightnessValue(sgNode,SHADER_COMMON_brightness);
        setShininessValue(sgNode,SHADER_COMMON_shininess);
        setEyePos(sgNode,SHADER_COMMON_eyePos);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setLightViewProjMatrix(sgNode,SHADER_COMMON_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_COMMON_viewMatrix);
    }else if(matName == "SHADER_2D_PLANE"){
        
    }else if(matName == "SHADER_DRAW_2D_IMAGE" || matName == "SHADER_DRAW_2D_IMAGE_DEPTH"){
        setTexturesUniforms(sgNode,SHADER_DRAW_2D_IMAGE_Texture1);
    }else if(matName == "SHADER_SHADOW_DEPTH_PASS_SKIN" || matName == "SHADER_SHADOW_DEPTH_PASS_TEXT"){
        setModelViewProjMatrix(sgNode,SHADER_DEPTH_PASS_SKIN_mvp);
        setJointTransform(sgNode,SHADER_DEPTH_PASS_SKIN_jointdata,smgr);
    }else if(matName == "SHADER_SHADOW_DEPTH_PASS"){
        setModelViewProjMatrix(sgNode,SHADER_DEPTH_PASS_mvp);
    }
}

void ShaderManager::setIsVertexColored(SGNode *sgNode,bool status, int paramIndex, bool isFragmentData)
{
    float isVertexColored = (status) ? 1.0 : 0.0;
    smgr->setPropertyValue(sgNode->node->material, "isVertexColored", &isVertexColored, DATA_FLOAT,1,isFragmentData,paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setNumberOfLights(SGNode *sgNode, int paramIndex)
{
    //TODO hardcoded number of lights
    float lights = (float)ShaderManager::lightPosition.size();
    smgr->setPropertyValue(sgNode->node->material, "numberOfLights", &lights, DATA_FLOAT,1,true,paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setLightsProperties(SGNode *sgNode, int paramIndex1 , int paramIndex2)
{
    int lightsCount = (int)ShaderManager::lightPosition.size();
    float * lightPositions = new float[lightsCount * 3];
    float *lightColors = new float[lightsCount * 3];
    float * fadeEndDistance = new float[lightsCount];
    
    int copyIncrement = 0;
    for(int i = 0; i < lightsCount; i++) {
        MathHelper::copyVector3ToPointer(lightPositions + copyIncrement, lightPosition[i]);
        MathHelper::copyVector3ToPointer(lightColors + copyIncrement, lightColor[i]);
        *(fadeEndDistance + i) = lightFadeDistances[i];
        copyIncrement += 3;
    }
    
    setLightsPosition(sgNode, lightPositions, paramIndex1);
    setLightsColors(sgNode, lightColors, paramIndex2);
    
    smgr->setPropertyValue(sgNode->node->material, "fadeEndDistance", fadeEndDistance, DATA_FLOAT, lightsCount, true ,SHADER_COMMON_lightFadeDistance,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    
    delete(fadeEndDistance);
    
}

void ShaderManager::setLightsPosition(SGNode *sgNode, float *lightPositions, int paramIndex)
{
    int lightsCount = (int)ShaderManager::lightPosition.size();
    smgr->setPropertyValue(sgNode->node->material, "lightPos", lightPositions, DATA_FLOAT_VEC3, lightsCount * 3, true, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete(lightPositions);
}

void ShaderManager::setLightsColors(SGNode *sgNode, float *lightColors, int paramIndex)
{
    int lightsCount = (int)ShaderManager::lightPosition.size();
        smgr->setPropertyValue(sgNode->node->material, "lightColor", lightColors, DATA_FLOAT_VEC3, lightsCount * 3, true, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete(lightColors);
}

void ShaderManager::setLightEndDistance(SGNode *sgNode, float *fadeDistances, int paramIndex)
{
    int lightsCount = (int)ShaderManager::lightPosition.size();
        smgr->setPropertyValue(sgNode->node->material, "fadeEndDistance", fadeDistances, DATA_FLOAT, lightsCount , true, SHADER_COMMON_lightFadeDistance,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete(fadeDistances);
}

void ShaderManager::setNodeLighting(SGNode *sgNode,int paramIndex)
{
    int lighting = (sgNode->getType() == NODE_LIGHT) ? 0:(int)sgNode->props.isLighting;
    lighting = (ShaderManager::sceneLighting) ? lighting : 0;
    smgr->setPropertyValue(sgNode->node->material, "isLighting", &lighting, DATA_INTEGER,1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setShadowDakness(SGNode *sgNode,int paramIndex){
    smgr->setPropertyValue(sgNode->node->material, "shadowDarkness", &shadowDensity, DATA_FLOAT, 1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setBrightnessValue(SGNode *sgNode,int paramIndex){
    float value = sgNode->props.brightness;
    smgr->setPropertyValue(sgNode->node->material, "brightness", &value, DATA_FLOAT, 1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setShininessValue(SGNode *sgNode,int paramIndex){
    float value = sgNode->props.shininess;
    smgr->setPropertyValue(sgNode->node->material, "shininess", &value, DATA_FLOAT, 1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setLightPos(SGNode *sgNode,int paramIndex){
    if(lightPosition.size()) {
    float *lightPos = new float[3];
    lightPos[0] = lightPosition[0].x;    lightPos[1] = lightPosition[0].y;    lightPos[2] = lightPosition[0].z;
    smgr->setPropertyValue(sgNode->node->material, "lightPos", lightPos, DATA_FLOAT_VEC3, 3, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete lightPos;
    }
}
void ShaderManager::setEyePos(SGNode *sgNode,int paramIndex){
    float *campos = new float[3];
    campos[0] = camPos.x;    campos[1] = camPos.y;    campos[2] = camPos.z;
    smgr->setPropertyValue(sgNode->node->material, "eyePos", campos, DATA_FLOAT_VEC3, 3, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete campos;
}
void ShaderManager::setLightColor(SGNode *sgNode,int paramIndex){
    if(lightColor.size()) {
    float *lightCol = new float[3];
    lightCol[0] = lightColor[0].x;   lightCol[1] = lightColor[0].y;    lightCol[2] = lightColor[0].z;
    smgr->setPropertyValue(sgNode->node->material, "lightColor", lightCol, DATA_FLOAT_VEC3, 3, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete lightCol;
    }
}
void ShaderManager::setLightViewProjMatrix(SGNode *sgNode,int paramIndex){
    Mat4 lightViewProjMatrix = lighCamProjMatrix;
    lightViewProjMatrix *= lighCamViewMatrix;
    
    smgr->setPropertyValue(sgNode->node->material, "lightViewProjection", lightViewProjMatrix.pointer(), DATA_FLOAT_MAT4, 16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setVertexColorUniform(Material *material , Vector3 color,int paramIndex,int nodeIndex)
{
    float *vertColor = new float[3];
    vertColor[0] = color.x;
    vertColor[1] = color.y;
    vertColor[2] = color.z;
    smgr->setPropertyValue(material, "perVertexColor", vertColor, DATA_FLOAT_VEC3, 3, false, paramIndex,nodeIndex);
    delete vertColor;
}
void ShaderManager::setTexturesUniforms(SGNode *sgNode,u16 paramIndex){
    int textureValue = 0;
    if(deviceType == OPENGLES2){
        OGLTexture* tex = (OGLTexture*)sgNode->node->getTextureByIndex(1);
        textureValue = tex->OGLTextureName;
        smgr->setPropertyValue(sgNode->node->material,"texture1",&textureValue,DATA_TEXTURE_2D,1, true, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()),tex,0);
    }else if(deviceType == METAL){
        textureValue = 1;
        smgr->setPropertyValue(sgNode->node->material,"texture1",&textureValue,DATA_TEXTURE_2D,1, true, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()),NULL,1);
    }

    if(sgNode->node->getTextureCount() > 1){
        if(deviceType == OPENGLES2){
            OGLTexture* tex = (OGLTexture*)sgNode->node->getTextureByIndex(2);
            if(tex != NULL){
                textureValue = tex->OGLTextureName;
                smgr->setPropertyValue(sgNode->node->material,"depthTexture",&textureValue,DATA_TEXTURE_2D,1, true, paramIndex+1,smgr->getNodeIndexByID(sgNode->node->getID()),tex,1);
            }
        }else if(deviceType == METAL){
            textureValue = 2;
            smgr->setPropertyValue(sgNode->node->material,"depthTexture",&textureValue,DATA_TEXTURE_2D,1, true, SHADER_COMMON_texture2,smgr->getNodeIndexByID(sgNode->node->getID()),NULL,1);
        }
    }
}
void ShaderManager::setNodeTransparency(SGNode *sgNode,u16 paramIndex){
    float transparency = sgNode->props.isSelected ?  NODE_SELECTION_TRANSPARENCY : sgNode->props.transparency;
    transparency = sgNode->props.isVisible ? transparency : NODE_SELECTION_TRANSPARENCY;
    smgr->setPropertyValue(sgNode->node->material,"transparency",&transparency,DATA_FLOAT,1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setSceneDataUniforms(SGNode *node,u16 paramIndex){
    
}
void ShaderManager::setModelViewProjMatrix(SGNode *sgNode,u16 paramIndex){
        Mat4 projMat = smgr->getActiveCamera()->getProjectionMatrix();
        Mat4 viewMat = smgr->getActiveCamera()->getViewMatrix();
        Mat4 mvp = projMat * viewMat * sgNode->node->getModelMatrix();
        smgr->setPropertyValue(sgNode->node->material,"mvp",mvp.pointer(),DATA_FLOAT_MAT4,16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setViewProjMatrix(SGNode *sgNode,u16 paramIndex){
    Mat4 projMat = smgr->getActiveCamera()->getProjectionMatrix();
    Mat4 viewMat = smgr->getActiveCamera()->getViewMatrix();
    Mat4 mvp = projMat * viewMat;
    smgr->setPropertyValue(sgNode->node->material,"vp",mvp.pointer(),DATA_FLOAT_MAT4,16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setViewMatrix(SGNode *sgNode,u16 paramIndex){
    Mat4 viewMat = smgr->getActiveCamera()->getViewMatrix();
    Mat4 view = viewMat;
    smgr->setPropertyValue(sgNode->node->material,"view",view.pointer(),DATA_FLOAT_MAT4,16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::setModelMatrix(SGNode *sgNode,u16 paramIndex){
    Mat4 modelMat = sgNode->node->getModelMatrix();
    smgr->setPropertyValue(sgNode->node->material,"Model",modelMat.pointer(),DATA_FLOAT_MAT4,16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}
void ShaderManager::copyMat(float* pointer,Mat4& mat){
    for(int i = 0;i < 16;++i)
        *pointer++ = mat[i];
}
void ShaderManager::setJointTransform(SGNode *sgNode,int paramIndex,SceneManager *smgr){
    SkinMesh *sMesh = (SkinMesh*)(dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getMesh();
    float* JointArray = new float[sMesh->joints->size() * 16];
    int copyIncrement = 0;
    for(int i = 0; i < sMesh->joints->size(); ++i){
        Mat4 JointVertexPull;
        JointVertexPull.setbyproduct((*sMesh->joints)[i]->GlobalAnimatedMatrix,(*sMesh->joints)[i]->GlobalInversedMatrix);
        copyMat(JointArray + copyIncrement,JointVertexPull);
        copyIncrement += 16;
    }
    smgr->setPropertyValue(sgNode->node->material,"jointTransforms", JointArray, DATA_FLOAT_MAT4, sMesh->joints->size() * 16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete(JointArray);
}
Mat4 ShaderManager::ortho2d_oc(float left,
                                float right,
                                float bottom,
                                float top,
                                float near,
                                float far)
{
    Mat4 mat;
    float sLength = 1.0f / (right - left);
    float sHeight = 1.0f / (top   - bottom);
    float sDepth  = 1.0f / (far   - near);
    
    mat[0] = 2.0f * sLength;
    mat[4] = 0.0f;
    mat[8] = 0.0f;
    mat[12] = 0.0f;
    
    mat[1] = 0.0f;
    mat[5] = 2.0f * sHeight;
    mat[9] = 0.0f;
    mat[13] = 0.0f;
    
    mat[2] = 0.0f;
    mat[6] = 0.0f;
    mat[10] = sDepth;
    mat[14] = 0.0f;
    
    mat[3] = -sLength * (left + right);
    mat[7] = -sHeight * (top + bottom);
    mat[11] = -sDepth  * near;
    mat[15] =  1.0f;
    
    return mat;
}

