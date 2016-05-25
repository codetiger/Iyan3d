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

//std::map< string, float > transparencies;
//std::map< string, float >::iterator transIt;
//
//std::map< string, float > reflections;
//std::map< string, float >::iterator reflectIt;
//
//std::map< string, float > isVColored;
//std::map< string, float >::iterator isVColorIt;
//
//std::map< string, int > nodesLighting;
//std::map< string, int >::iterator lightingIt;
//
//float prevShadow = -1.0;
//float lightsCount = -1.0;
//Vector3 prevCamPos = Vector3(-9999.0);

bool ShaderManager::renderingPreview = false;
bool ShaderManager::isRenderingDepthPass = false;
bool ShaderManager::shadowsOff = false;
bool ShaderManager::lightChanged = true;
bool ShaderManager::isRendering = false;
bool ShaderManager::sceneLighting = true;
float ShaderManager::shadowDensity = 0.0, ShaderManager::shadowTextureSize = 2048.0;
Vector3 ShaderManager::camPos = Vector3(0.0);
vector<Vector3> ShaderManager::lightPosition;// = Vector3(0.0);
vector<Vector3> ShaderManager::lightColor;// = Vector3(1.0);
vector<float> ShaderManager::lightFadeDistances;
vector< int > ShaderManager::lightTypes;
Mat4 ShaderManager::lighCamProjMatrix;
Mat4 ShaderManager::lighCamViewMatrix;
string ShaderManager::BundlePath = " ";
DEVICE_TYPE ShaderManager::deviceType = OPENGLES2;
int ShaderManager::maxIntsances = 100;

ShaderManager::~ShaderManager(){
//    transparencies.clear();
//    reflections.clear();
//    isVColored.clear();
//    nodesLighting.clear();
//    prevShadow = -1.0;
//    lightsCount = -1.0;
//    prevCamPos = Vector3(-9999.0);
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
    ShaderManager::maxIntsances = maxInstanceCount * ((deviceType == METAL) ? 40 : 1);
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
        
    } else if(matName.find("SHADER_VERTEX_COLOR_SHADOW_SKIN") !=  string::npos || matName.find("SHADER_VERTEX_COLOR_SKIN_TOON") !=  string::npos) {
        setVertexColorUniform(sgNode, sgNode->props.vertexColor, SHADER_COMMON_SKIN_VertexColor, smgr->getNodeIndexByID(sgNode->node->getID()));
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, false);
        
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, true);
        setJointTransform(sgNode, SHADER_PERVERTEXCOLOR_jointData, smgr);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode, SHADER_PERVERTEXCOLOR_mvp);
        setModelMatrix(sgNode, SHADER_PERVERTEXCOLOR_world);
        setNodeTransparency(sgNode,SHADER_PERVERTEXCOLOR_transparency);
        setNodeLighting(sgNode,SHADER_PERVERTEXCOLOR_isLighting);
        setShadowDakness(sgNode,SHADER_PERVERTEXCOLOR_shadowDarkness);
        setReflectionValue(sgNode,SHADER_PERVERTEXCOLOR_reflection);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setEyePos(sgNode,SHADER_PERVERTEXCOLOR_eyePos);
        setJointTransform(sgNode,SHADER_COMMON_SKIN_jointData,smgr);
        setLightViewProjMatrix(sgNode,SHADER_PERVERTEXCOLOR_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_PERVERTEXCOLOR_viewMatrix);
    } else if(matName.find("SHADER_VERTEX_COLOR") !=  string::npos) {
        setVertexColorUniform(sgNode, Vector3(-1.0,-1.0,-1.0), SHADER_COMMON_SKIN_VertexColor, smgr->getNodeIndexByID(sgNode->node->getID()));
        bool isVcolored = (sgNode->getType() == NODE_CAMERA || (sgNode->node->getID() >= CONTROLS_START_ID && sgNode->node->getID() < CONTROLS_START_ID+10)) ? true : sgNode->props.perVertexColor;
        setIsVertexColored(sgNode, isVcolored , SHADER_COMMON_isVertexColored, false);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode, SHADER_PERVERTEXCOLOR_mvp);
        setModelMatrix(sgNode, SHADER_PERVERTEXCOLOR_world);
        setNodeTransparency(sgNode,SHADER_PERVERTEXCOLOR_transparency);
        setNodeLighting(sgNode,SHADER_PERVERTEXCOLOR_isLighting);
        setShadowDakness(sgNode,SHADER_PERVERTEXCOLOR_shadowDarkness);
        setReflectionValue(sgNode,SHADER_PERVERTEXCOLOR_reflection);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setEyePos(sgNode,SHADER_PERVERTEXCOLOR_eyePos);
        setLightViewProjMatrix(sgNode,SHADER_PERVERTEXCOLOR_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_PERVERTEXCOLOR_viewMatrix);
    }else if(matName == "SHADER_COLOR"){
        setModelViewProjMatrix(sgNode, SHADER_COLOR_mvp);
        Vector3 vertexColor = sgNode->props.isSelected ? Vector3(0.0,1.0,0.0) : sgNode->props.vertexColor;
        setVertexColorUniform(sgNode, vertexColor, SHADER_COLOR_vertexColor, smgr->getNodeIndexByID(sgNode->node->getID()));
        setNodeTransparency(sgNode,SHADER_COLOR_transparency);
    }else if(matName == "SHADER_COLOR_SKIN"){
        setModelViewProjMatrix(sgNode,SHADER_COLOR_SKIN_mvp);
        Vector3 vertexColor = sgNode->props.isSelected ? Vector3(0.0,1.0,0.0) : sgNode->props.vertexColor;
        setVertexColorUniform(sgNode, vertexColor, SHADER_COLOR_SKIN_vertexColor, smgr->getNodeIndexByID(sgNode->node->getID()));
        setNodeTransparency(sgNode,SHADER_COLOR_SKIN_transparency);
        setJointTransform(sgNode,SHADER_COLOR_SKIN_jointData,smgr);
    }else if(matName.find("SHADER_COMMON_SKIN") !=  string::npos || matName.find("SHADER_TOON_SKIN") !=  string::npos){
        setVertexColorUniform(sgNode, sgNode->props.vertexColor, SHADER_COMMON_SKIN_VertexColor, smgr->getNodeIndexByID(sgNode->node->getID()));
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, false);
        
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, true);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode,SHADER_COMMON_SKIN_mvp);
        setModelMatrix(sgNode, SHADER_COMMON_SKIN_world);
        setNodeTransparency(sgNode,SHADER_COMMON_SKIN_transparency);
        setNodeLighting(sgNode,SHADER_COMMON_SKIN_isLighting);
        setShadowDakness(sgNode,SHADER_COMMON_SKIN_shadowDarkness);
        setReflectionValue(sgNode,SHADER_COMMON_SKIN_reflection);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setEyePos(sgNode,SHADER_COMMON_SKIN_eyePos);
        setJointTransform(sgNode,SHADER_COMMON_SKIN_jointData,smgr);
        setLightViewProjMatrix(sgNode,SHADER_COMMON_SKIN_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_COMMON_SKIN_ViewMatrix);
    }else if(matName.find("SHADER_COMMON") !=  string::npos || matName.find("SHADER_TOON") !=  string::npos){
        setVertexColorUniform(sgNode, sgNode->props.vertexColor, SHADER_COMMON_SKIN_VertexColor, smgr->getNodeIndexByID(sgNode->node->getID()));
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, false);

        //setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, true);
        setTexturesUniforms(sgNode,SHADER_COMMON_texture1);
        setModelViewProjMatrix(sgNode,SHADER_COMMON_mvp);
        setModelMatrix(sgNode, SHADER_COMMON_world);
        setNodeTransparency(sgNode,SHADER_COMMON_transparency);
        setNodeLighting(sgNode,SHADER_COMMON_isLighting);
        setShadowDakness(sgNode,SHADER_COMMON_shadowDarkness);
        setReflectionValue(sgNode,SHADER_COMMON_reflection);
        setEyePos(sgNode,SHADER_COMMON_eyePos);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos,SHADER_COMMON_lightColor);
        setLightViewProjMatrix(sgNode,SHADER_COMMON_lightViewProjMatrix);
        setViewMatrix(sgNode,SHADER_COMMON_viewMatrix);
    }else if(matName == "SHADER_2D_PLANE"){
        
    }else if(matName == "SHADER_DRAW_2D_IMAGE" || matName == "SHADER_DRAW_2D_IMAGE_DEPTH"){
        setTexturesUniforms(sgNode,SHADER_DRAW_2D_IMAGE_Texture1);
    }else if(matName == "SHADER_SHADOW_DEPTH_PASS_SKIN" || matName == "SHADER_SHADOW_DEPTH_PASS_TEXT"){
        setModelViewProjMatrix(sgNode,SHADER_DEPTH_PASS_SKIN_mvp, true);
        setJointTransform(sgNode,SHADER_DEPTH_PASS_SKIN_jointdata,smgr);
    }else if(matName == "SHADER_SHADOW_DEPTH_PASS"){
        setModelViewProjMatrix(sgNode,SHADER_DEPTH_PASS_mvp, true);
    }else if (matName.find("SHADER_PARTICLES") !=  string::npos) {
        setIsVertexColored(sgNode, sgNode->props.perVertexColor , SHADER_COMMON_isVertexColored, false);
        setMVPForParticles(sgNode,0);
        setTexturesUniforms(sgNode, SHADER_PARTICLE_texture1);
    }
}

void ShaderManager::setIsVertexColored(SGNode *sgNode,bool status, int paramIndex, bool isFragmentData)
{
    
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();

    float *isVertexColored = new float[((endIndex - startIndex)+1)];
    isVertexColored[0] = ((sgNode->getType() != NODE_PARTICLES) && (sgNode->props.perVertexColor || sgNode->props.isSelected || status)) ? 1.0 : 0.0;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        isVertexColored[i+1] = (v[j].second->props.perVertexColor || v[j].second->props.isSelected || status) ? 1.0 : 0.0;
        i++;
    }

    smgr->setPropertyValue(sgNode->node->material, "isVertexColored", isVertexColored, DATA_FLOAT, (endIndex - startIndex)+1,isFragmentData,paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete [] isVertexColored;
}

void ShaderManager::setNumberOfLights(SGNode *sgNode, int paramIndex)
{
    float lights = (float)ShaderManager::lightPosition.size();
        smgr->setPropertyValue(sgNode->node->material, "numberOfLights", &lights, DATA_FLOAT,1,true,paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setLightsProperties(SGNode *sgNode, int paramIndex1 , int paramIndex2)
{
        int lightsCount = (int)ShaderManager::lightPosition.size();
        float * lightPositions = new float[lightsCount * 3];
        float *lightColors = new float[lightsCount * 3];
        float * fadeEndDistance = new float[lightsCount];
        float * lightType = new float[lightsCount];
    
        int copyIncrement = 0;
        for(int i = 0; i < lightsCount; i++) {
            MathHelper::copyVector3ToPointer(lightPositions + copyIncrement, lightPosition[i]);
            MathHelper::copyVector3ToPointer(lightColors + copyIncrement, lightColor[i]);
            *(fadeEndDistance + i) = lightFadeDistances[i];
            *(lightType + i) = (float)lightTypes[i];
            copyIncrement += 3;
        }
        setLightsPosition(sgNode, lightPositions, paramIndex1);
        setLightsColors(sgNode, lightColors, paramIndex2);
        
        smgr->setPropertyValue(sgNode->node->material, "fadeEndDistance", fadeEndDistance, DATA_FLOAT, lightsCount, true ,SHADER_COMMON_lightFadeDistance,smgr->getNodeIndexByID(sgNode->node->getID()));
    
        smgr->setPropertyValue(sgNode->node->material, "lightTypes", lightType, DATA_FLOAT, lightsCount, true ,SHADER_COMMON_lightType,smgr->getNodeIndexByID(sgNode->node->getID()));

        lightChanged = false;
        delete [] fadeEndDistance;
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

void ShaderManager::setNodeLighting(SGNode *sgNode,int paramIndex)
{
    
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();

    float *lighting = new float[((endIndex - startIndex)+1)];
    lighting[0] = (sgNode->getType() == NODE_LIGHT || sgNode->props.isSelected) ? 0 : (int)sgNode->props.isLighting;
    lighting[0] = (ShaderManager::sceneLighting) ? lighting[0] : 0;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        lighting[i+1] = (v[j].second->getType() == NODE_LIGHT || v[j].second->props.isSelected) ? 0 : (float)v[j].second->props.isLighting;
        lighting[i+1] = (ShaderManager::sceneLighting) ? lighting[i+1] : 0;
        i++;
    }
    
    smgr->setPropertyValue(sgNode->node->material, "isLighting", lighting, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] lighting;

}
void ShaderManager::setShadowDakness(SGNode *sgNode,int paramIndex) {
    float finalShadow = (shadowsOff || sgNode->props.isSelected) ? 0.0 : shadowDensity;
        smgr->setPropertyValue(sgNode->node->material, "shadowDarkness", &finalShadow, DATA_FLOAT, 1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setReflectionValue(SGNode *sgNode,int paramIndex) {
    
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *value = new float[((endIndex - startIndex)+1)];
    value[0] = sgNode->props.isSelected ?  0.0 : sgNode->props.reflection;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        value[i+1] = v[j].second->props.isSelected ?  0.0 : v[j].second->props.reflection;
        i++;
    }

    smgr->setPropertyValue(sgNode->node->material, "reflection", value, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] value;

}

void ShaderManager::setEyePos(SGNode *sgNode,int paramIndex) {
        float *campos = new float[3];
        campos[0] = camPos.x;    campos[1] = camPos.y;    campos[2] = camPos.z;
        smgr->setPropertyValue(sgNode->node->material, "eyePos", campos, DATA_FLOAT_VEC3, 3, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
        delete[] campos;
}

void ShaderManager::setLightViewProjMatrix(SGNode *sgNode,int paramIndex){
        Mat4 lightViewProjMatrix = lighCamProjMatrix;
        lightViewProjMatrix *= lighCamViewMatrix;
    Mat4 lvp = lightViewProjMatrix;// * sgNode->node->getModelMatrix();
        
        smgr->setPropertyValue(sgNode->node->material, "lvp", lvp.pointer(), DATA_FLOAT_MAT4, 16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setVertexColorUniform(SGNode *sgNode , Vector3 color,int paramIndex,int nodeIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *vertColor = new float[((endIndex - startIndex)+1) * 3];
    
    Vector3 vertexColor = sgNode->props.isSelected ? Vector3(0.0,1.0,0.0) : color;
    vertColor[0] = vertexColor.x;
    vertColor[1] = vertexColor.y;
    vertColor[2] = vertexColor.z;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        Vector3 vColor = v[j].second->props.isSelected ? Vector3(0.0,1.0,0.0) : v[j].second->props.vertexColor;
        vertColor[((i+1) * 3)] = vColor.x;
        vertColor[((i+1) * 3) + 1] = vColor.y;
        vertColor[((i+1) * 3) + 2] = vColor.z;
        i++;
    }
    
    Material * material = sgNode->node->material;
    smgr->setPropertyValue(material, "perVertexColor", vertColor, DATA_FLOAT_VEC3, ((endIndex - startIndex)+1) * 3, false, paramIndex,nodeIndex);
    delete [] vertColor;
}

void ShaderManager::setTexturesUniforms(SGNode *sgNode,u16 paramIndex)
{
        int textureValue = 0;
        if(deviceType == OPENGLES2){
            OGLTexture* tex = (OGLTexture*)sgNode->node->getTextureByIndex(1);
            textureValue = tex->OGLTextureName;
            smgr->setPropertyValue(sgNode->node->material,"texture1",&textureValue,DATA_TEXTURE_2D,1, true, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()),tex,0);
        }else if(deviceType == METAL){
            textureValue =  1;
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

void ShaderManager::setNodeTransparency(SGNode *sgNode,u16 paramIndex) {

    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();

    float *transparency = new float[((endIndex - startIndex)+1)];
    transparency[0] = (sgNode->props.isSelected || !sgNode->props.isVisible) ?  NODE_SELECTION_TRANSPARENCY : sgNode->props.transparency;

    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        transparency[i+1] = (v[j].second->props.isSelected || !v[j].second->props.isVisible) ?  NODE_SELECTION_TRANSPARENCY : v[j].second->props.transparency;
        i++;
    }
    
    Material * material = sgNode->node->material;
    smgr->setPropertyValue(material, "transparency", transparency, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] transparency;

}
void ShaderManager::setSceneDataUniforms(SGNode *node,u16 paramIndex){
    
}

void ShaderManager::setModelViewProjMatrix(SGNode *sgNode,u16 paramIndex, bool isDepthPass) {
    
    Mat4 projMat = (isDepthPass) ? lighCamProjMatrix : smgr->getActiveCamera()->getProjectionMatrix();
    Mat4 viewMat = (isDepthPass) ? lighCamViewMatrix : smgr->getActiveCamera()->getViewMatrix();
    
    Mat4 mvp;
    std::string uniformName = "mvp";
    if((sgNode->node->getID() != CIRCLE_NODE_ID && sgNode->node->getID() != LIGHT_DIRECTION_ID && sgNode->node->drawMode == DRAW_MODE_LINES) || sgNode->node->material->name.find("SHADER_COMMON_L") != string::npos || sgNode->node->material->name.find("SHADER_VERTEX_COLOR_L") != string::npos) {
        uniformName = (sgNode->node->drawMode == DRAW_MODE_LINES) ? "mvp" : "vp";
        mvp = projMat * viewMat;
    } else
        mvp = projMat * viewMat * sgNode->node->getModelMatrix();
    
    smgr->setPropertyValue(sgNode->node->material, uniformName, mvp.pointer(), DATA_FLOAT_MAT4, 16, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setMVPForParticles(SGNode *sgNode, u16 paramIndex)
{
    Mat4 projMat = smgr->getActiveCamera()->getProjectionMatrix();
    Mat4 viewMat = smgr->getActiveCamera()->getViewMatrix();
    shared_ptr<ParticleManager> pNode = dynamic_pointer_cast<ParticleManager>(sgNode->node);
    //pNode->sortParticles(smgr->getActiveCamera()->getPosition());
    
    bool meshCacheCreated = false;
    if(!renderingPreview) {
        meshCacheCreated = pNode->updateParticles((sgNode->props.isSelected || isRendering), smgr->getActiveCamera()->getPosition());
    }
    if(!meshCacheCreated) {
        pNode->shouldUpdateMesh = true;
        smgr->updateVertexAndIndexBuffers(sgNode->node, MESH_TYPE_LITE);
    }
    
    Mat4 model = sgNode->node->getModelMatrix();
    Mat4 vp = projMat * viewMat;

    Vector3 camPos = smgr->getActiveCamera()->getAbsolutePosition();
    
    float* particleProps = new float[4];
    Vector4 props = pNode->getParticleProps();
    particleProps[0] = props.x;
    particleProps[1] = camPos.x;
    particleProps[2] = camPos.y;
    particleProps[3] = camPos.z;
    
    Vector2 viewport = smgr->getViewPort();
    float viewRatio = viewport.x/2048.0;
    
    float fovRatio = (smgr->getActiveCamera()->getFOVInRadians() * 180.0f/PI)/35.0;
    
    float* sColor = new float[4];
    Vector3 vColor = sgNode->props.vertexColor;
    sColor[0] = (sgNode->props.perVertexColor) ? sgNode->props.vertexColor.x : pNode->startColor.x;
    sColor[1] = (sgNode->props.perVertexColor) ? sgNode->props.vertexColor.y : pNode->startColor.y;
    sColor[2] = (sgNode->props.perVertexColor) ? sgNode->props.vertexColor.z : pNode->startColor.z;
    sColor[3] = viewRatio;

    float* mColor = new float[4];
    mColor[0] = pNode->midColor.x;
    mColor[1] = pNode->midColor.y;
    mColor[2] = pNode->midColor.z;
    mColor[3] = fovRatio;

    float* eColor = new float[4];
    eColor[0] = pNode->endColor.x;
    eColor[1] = pNode->endColor.y;
    eColor[2] = pNode->endColor.z;
    eColor[3] = pNode->endColor.w;
    
    smgr->setPropertyValue(sgNode->node->material,"props", particleProps, DATA_FLOAT_VEC4,  4, false, SHADER_PARTICLE_props,smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material,"startColor", sColor, DATA_FLOAT_VEC4,  4, false, SHADER_PARTICLE_sColor,smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material,"midColor", mColor, DATA_FLOAT_VEC4,  4, false, SHADER_PARTICLE_mColor,smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material,"endColor", eColor, DATA_FLOAT_VEC4,  4, false, SHADER_PARTICLE_eColor,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    smgr->setPropertyValue(sgNode->node->material,"vp", vp.pointer(), DATA_FLOAT_MAT4,  16, false, SHADER_PARTICLE_vp,smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material,"model", model.pointer(), DATA_FLOAT_MAT4,  16, false, SHADER_PARTICLE_world,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete [] particleProps;
    delete [] sColor;
    delete [] mColor;
    delete [] eColor;
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
void ShaderManager::setModelMatrix(SGNode *sgNode,u16 paramIndex) {
    
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();

    float* modelArray = new float[((endIndex - startIndex)+1) * 16];
    int copyIncrement = 0;
    Mat4 m = sgNode->node->getModelMatrix();
    copyMat(modelArray + copyIncrement, m);
    copyIncrement += 16;

    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        Mat4 mod = v[j].second->node->getModelMatrix();
        copyMat(modelArray + copyIncrement, mod);
        copyIncrement += 16;
    }

    smgr->setPropertyValue(sgNode->node->material,"model", modelArray, DATA_FLOAT_MAT4, ((endIndex - startIndex)+1) * 16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] modelArray;
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
    delete [] JointArray;
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

