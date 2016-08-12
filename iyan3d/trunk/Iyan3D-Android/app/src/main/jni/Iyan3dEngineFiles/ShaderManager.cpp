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
int ShaderManager::maxJoints = 1;
float ShaderManager::ambientLight = 0.0f;

ShaderManager::ShaderManager(SceneManager *smgr,DEVICE_TYPE deviceType, int maxUniforms, int maxJoints)
{
    isRenderingDepthPass = false;
    shadowDensity = 0.0;
    shadowTextureSize = 2048.0;
    camPos = Vector3(0.0);
    this->smgr = smgr;
    ShaderManager::BundlePath = constants::BundlePath;
    ShaderManager::deviceType = deviceType;
#ifndef UBUNTU
    loadAllShaders(smgr,deviceType, maxUniforms, maxJoints);
#endif
}

ShaderManager::~ShaderManager()
{
    sceneProps.clear();
}

void ShaderManager::addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE propType, string title, string groupName, string fileName, ICON_INDEX iconId)
{
    Property property;
    property.index = index;
    property.parentIndex = parentProp;
    property.value = value;
    property.iconId = iconId;
    
    if(title.length() > 2) {
        property.title = title;
        property.type = propType;
    }
    
    if(groupName.length() > 2)
        property.groupName = groupName;
    if(fileName.length() > 5 || propType == IMAGE_TYPE)
        property.fileName = fileName;
    
    if(parentProp != UNDEFINED && sceneProps.find(parentProp) != sceneProps.end()) {
        checkAndUpdatePropsMap(sceneProps[parentProp].subProps, property);
    } else if(sceneProps.find(index) == sceneProps.end()) {
        property.type = propType;
        sceneProps.insert(pair<PROP_INDEX, Property>( index, property));
    }
    else {
        sceneProps.find(index)->second.value = property.value;
        if(property.fileName.length() > 5 || propType == IMAGE_TYPE)
            sceneProps.find(index)->second.fileName = property.fileName;
    }
    
}

void ShaderManager::checkAndUpdatePropsMap(std::map < PROP_INDEX, Property > &propsMap, Property property)
{
    if(propsMap.find(property.index) == propsMap.end()) {
        propsMap.insert(pair<PROP_INDEX, Property>( property.index, property));
    } else {
        propsMap.find(property.index)->second.value = property.value;
        if(property.fileName.length() > 5 || property.type == IMAGE_TYPE)
            propsMap.find(property.index)->second.fileName = property.fileName;
    }
}

Property& ShaderManager::getProperty(PROP_INDEX pIndex)
{
    if(sceneProps.find(pIndex) == sceneProps.end()) {
        std::map<PROP_INDEX, Property>::iterator pIt;
        for(pIt = sceneProps.begin(); pIt != sceneProps.end(); pIt++) {
            if(pIt->second.subProps.find(pIndex) != pIt->second.subProps.end()) {
                return pIt->second.subProps[pIndex];
            }
        }
        
        addOrUpdateProperty(pIndex, Vector4(0), UNDEFINED);
        printf("\n No such scene property ..");
        return sceneProps[pIndex];

    } else {
        if(sceneProps.size() > 0 && sceneProps.find(pIndex) != sceneProps.end()) {
            return sceneProps.find(pIndex)->second;
        } else {
            addOrUpdateProperty(pIndex, Vector4(0), UNDEFINED);
            printf("\n No such scene property 2..");
            return sceneProps[pIndex];
        }
    }
}

void ShaderManager::loadAllShaders(SceneManager *smgr,DEVICE_TYPE deviceType, int maxUniforms, int maxJoints)
{
    std::map< string, string > strsToReplace;
    
    if(deviceType == OPENGLES2) {
        std::map< string, string > temp;
        temp = getShaderStringsToReplace(maxUniforms-1);
        strsToReplace.insert(temp.begin(), temp.end());
        temp.clear();
        temp = getStringsForRiggedObjects(maxJoints);
        strsToReplace.insert(temp.begin(), temp.end());
    } else {
        ShaderManager::maxIntsances = 4000;
        ShaderManager::maxJoints = 161; //TODO
    }
    
    for(int i = 0; i < TOTAL_MATERIALS;i++) {
        if(deviceType == OPENGLES2)
            LoadShader(smgr, deviceType, OGLMaterialAndShaderNames[i][0], OGLMaterialAndShaderNames[i][1], OGLMaterialAndShaderNames[i][2], strsToReplace);
        else if(deviceType == METAL)
            LoadShader(smgr, deviceType, MTLMaterialAndShaderNames[i][0], MTLMaterialAndShaderNames[i][1], MTLMaterialAndShaderNames[i][2], strsToReplace);
    }
}

std::map<string, string> ShaderManager::getShaderStringsToReplace(int maxUniforms)
{
    std::map<string, string> strsToReplace;
    
    string extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    
    if(extensions.find("GL_EXT_draw_instanced") == std::string::npos) {
        strsToReplace.insert(std::pair<string, string>("gl_InstanceIDEXT", "int(optionalData1.w)"));
    }
    
    ShaderManager::maxIntsances = maxUniforms;
    strsToReplace.insert(std::pair<string, string>("uniSize", to_string(ShaderManager::maxIntsances+1)));
    
    return strsToReplace;
}

std::map<string, string> ShaderManager::getStringsForRiggedObjects(int maxJoints)
{
    std::map<string, string> strsToReplace;
        
    ShaderManager::maxJoints = maxJoints;
    strsToReplace.insert(std::pair<string, string>("jointsSize", to_string(ShaderManager::maxJoints+1)));
    
    return strsToReplace;
}

bool ShaderManager::LoadShader(SceneManager* smgr, DEVICE_TYPE deviceType,string materialName,string vShaderName,string fShaderName, std::map< string, string > shadersStr, bool isTest)
{
    bool isDepthPass = (fShaderName == "") ? true : false;
    
    if(deviceType == OPENGLES2)
        return smgr->LoadShaders(materialName, BundlePath + "/" + vShaderName, BundlePath + "/" + fShaderName, shadersStr, false, isTest);
    else if(deviceType == METAL)
        return smgr->LoadShaders(materialName, vShaderName, fShaderName, shadersStr, isDepthPass);
    
    return false;
}

void ShaderManager::setUniforms(SGNode *sgNode, string matName, int materialIndex)
{
    if(matName == "SHADER_MESH") {
        setModelViewProjMatrix(sgNode, SHADER_MESH_mvp);
        setModelMatrix(sgNode, SHADER_MESH_model);
        setLightViewProjMatrix(sgNode, SHADER_MESH_lightViewProjMatrix);
        setEyePos(sgNode, SHADER_MESH_eyePos);
        setShadowDakness(sgNode, SHADER_MESH_shadowDarkness);
        setHasMeshColor(sgNode, sgNode->getProperty(IS_VERTEX_COLOR).value.x, SHADER_MESH_hasMeshColor, false, materialIndex);
        Vector4 meshColor = (sgNode->getType() == NODE_CAMERA) ? Vector4(-1.0) : sgNode->getProperty(VERTEX_COLOR, materialIndex).value;
        setVertexColorUniform(sgNode, meshColor, SHADER_MESH_meshColor, smgr->getNodeIndexByID(sgNode->node->getID()), materialIndex);
        setReflectionValue(sgNode, SHADER_MESH_reflectionValue, materialIndex);
        setNodeTransparency(sgNode, SHADER_MESH_transparency, materialIndex);
        setNodeLighting(sgNode, SHADER_MESH_hasLighting, materialIndex);
        setTexturesUniforms(sgNode, SHADER_COMMON_colorMap, materialIndex);
        setUVScaleValue(sgNode, SHADER_MESH_uvScale, materialIndex);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos, SHADER_COMMON_lightColor, SHADER_COMMON_lightType, SHADER_COMMON_ambientLight, SHADER_COMMON_lightCount);
    } else if(matName == "SHADER_SKIN" || matName == "SHADER_TEXT_SKIN") {
        setJointTransform(sgNode, SHADER_COMMON_SKIN_jointData, smgr);
        setModelViewProjMatrix(sgNode, SHADER_COMMON_SKIN_mvp);
        setModelMatrix(sgNode, SHADER_COMMON_SKIN_model);
        setLightViewProjMatrix(sgNode, SHADER_COMMON_SKIN_lightViewProjMatrix);
        setEyePos(sgNode, SHADER_COMMON_SKIN_eyePos);
        setShadowDakness(sgNode, SHADER_COMMON_SKIN_shadowDarkness);
        setHasMeshColor(sgNode, sgNode->getProperty(IS_VERTEX_COLOR).value.x, SHADER_COMMON_SKIN_hasMeshColor, false, materialIndex);
        setVertexColorUniform(sgNode, sgNode->getProperty(VERTEX_COLOR).value, SHADER_COMMON_SKIN_meshColor, smgr->getNodeIndexByID(sgNode->node->getID()), materialIndex);
        setReflectionValue(sgNode, SHADER_COMMON_SKIN_reflectionValue, materialIndex);
        setNodeTransparency(sgNode, SHADER_COMMON_SKIN_transparency, materialIndex);
        setNodeLighting(sgNode, SHADER_COMMON_SKIN_hasLighting, materialIndex);
        setTexturesUniforms(sgNode, SHADER_COMMON_colorMap, materialIndex);
        setUVScaleValue(sgNode, SHADER_COMMON_SKIN_uvScale, materialIndex);
        setLightsProperties(sgNode, SHADER_COMMON_lightPos, SHADER_COMMON_lightColor, SHADER_COMMON_lightType, SHADER_COMMON_ambientLight, SHADER_COMMON_lightCount);
    } else if(matName == "SHADER_COLOR") {
        setModelViewProjMatrix(sgNode, SHADER_COLOR_mvp);
        Vector4 vertexColor = sgNode->getProperty(SELECTED).value.x ? Vector4(0.0, 1.0, 0.0, 0) : sgNode->getProperty(VERTEX_COLOR).value;
        setVertexColorUniform(sgNode, vertexColor, SHADER_COLOR_vertexColor, smgr->getNodeIndexByID(sgNode->node->getID()), materialIndex);
        setNodeTransparency(sgNode,SHADER_COLOR_transparency, materialIndex);
    } else if(matName == "SHADER_COLOR_SKIN") {
        setModelViewProjMatrix(sgNode,SHADER_COLOR_SKIN_mvp);
        Vector4 vertexColor = sgNode->getProperty(SELECTED).value.x ? Vector4(0.0, 1.0, 0.0, 0) : sgNode->getProperty(VERTEX_COLOR).value;
        setVertexColorUniform(sgNode, vertexColor, SHADER_COLOR_SKIN_vertexColor, smgr->getNodeIndexByID(sgNode->node->getID()), materialIndex);
        setNodeTransparency(sgNode,SHADER_COLOR_SKIN_transparency, materialIndex);
        setJointTransform(sgNode,SHADER_COLOR_SKIN_jointData,smgr);
    } else if(matName == "SHADER_2D_PLANE") {
        
    } else if(matName == "SHADER_DRAW_2D_IMAGE" || matName == "SHADER_DRAW_2D_IMAGE_DEPTH") {
        setTexturesUniforms(sgNode,SHADER_DRAW_2D_IMAGE_Texture1, materialIndex);
    } else if(matName == "SHADER_SHADOW_DEPTH_PASS_SKIN" || matName == "SHADER_SHADOW_DEPTH_PASS_TEXT") {
        setModelViewProjMatrix(sgNode,SHADER_DEPTH_PASS_SKIN_mvp, true);
        setJointTransform(sgNode,SHADER_DEPTH_PASS_SKIN_jointdata,smgr);
    } else if(matName == "SHADER_SHADOW_DEPTH_PASS") {
        setModelViewProjMatrix(sgNode,SHADER_DEPTH_PASS_mvp, true);
    } else if (matName.find("SHADER_PARTICLES") !=  string::npos) {
        setHasMeshColor(sgNode, sgNode->getProperty(IS_VERTEX_COLOR).value.x , SHADER_MESH_hasMeshColor, false, materialIndex);
        setMVPForParticles(sgNode,0);
        setTexturesUniforms(sgNode, SHADER_PARTICLE_texture1, materialIndex);
    }
}

void ShaderManager::setVertexColorUniform(SGNode *sgNode, Vector4 color, int paramIndex, int nodeIndex, int materialIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *vertColor = new float[((endIndex - startIndex)+1) * 3];
    Vector4 vertexColor = sgNode->getProperty(SELECTED, materialIndex).value.x ? Vector4(0.0, 1.0, 0.0, 0) : color;
    vertColor[0] = vertexColor.x;
    vertColor[1] = vertexColor.y;
    vertColor[2] = vertexColor.z;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        Vector4 vColor = v[j].second->getProperty(SELECTED, materialIndex).value.x ? Vector4(0.0, 1.0, 0.0, 0) : v[j].second->getProperty(VERTEX_COLOR, materialIndex).value;
        vertColor[((i+1) * 3)] = vColor.x;
        vertColor[((i+1) * 3) + 1] = vColor.y;
        vertColor[((i+1) * 3) + 2] = vColor.z;
        i++;
    }
    
    Material * material = sgNode->node->material;
    smgr->setPropertyValue(material, "meshColor", vertColor, DATA_FLOAT_VEC3, ((endIndex - startIndex)+1) * 3, false, paramIndex, nodeIndex);
    delete [] vertColor;
}

void ShaderManager::setMeshProps(SGNode *sgNode, bool isVColored, int paramIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *props = new float[((endIndex - startIndex)+1) * 4];
    
    props[0] = (sgNode->getProperty(SELECTED).value.x || !sgNode->getProperty(VISIBILITY).value.x) ?  NODE_SELECTION_TRANSPARENCY : sgNode->getProperty(TRANSPARENCY).value.x;
    props[1] = ((sgNode->getType() != NODE_PARTICLES) && (sgNode->getProperty(IS_VERTEX_COLOR).value.x || sgNode->getProperty(SELECTED).value.x || isVColored)) ? 1.0 : 0.0;
    props[2] = sgNode->getProperty(SELECTED).value.x ?  0.0 : sgNode->getProperty(REFLECTION).value.x;
    props[3] = (!ShaderManager::sceneLighting || sgNode->getType() == NODE_LIGHT || sgNode->getProperty(SELECTED).value.x) ? 0 : (int)sgNode->getProperty(LIGHTING).value.x;
    
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        props[(i+1) * 4] = (v[j].second->getProperty(SELECTED).value.x || !v[j].second->getProperty(VISIBILITY).value.x) ?  NODE_SELECTION_TRANSPARENCY : v[j].second->getProperty(TRANSPARENCY).value.x;
        props[((i+1) * 4) + 1] = ((v[j].second->getType() != NODE_PARTICLES) && (v[j].second->getProperty(IS_VERTEX_COLOR).value.x || v[j].second->getProperty(SELECTED).value.x)) ? 1.0 : 0.0;
        props[((i+1) * 4) + 2] = v[j].second->getProperty(SELECTED).value.x ?  0.0 : v[j].second->getProperty(REFLECTION).value.x;
        props[((i+1) * 4) + 3] = (!ShaderManager::sceneLighting || v[j].second->getType() == NODE_LIGHT || v[j].second->getProperty(SELECTED).value.x) ? 0 : (int)v[j].second->getProperty(LIGHTING).value.x;
        i++;
    }
    
    Material * material = sgNode->node->material;
    smgr->setPropertyValue(material, "props", props, DATA_FLOAT_VEC4, ((endIndex - startIndex)+1) * 4, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    delete [] props;
}

void ShaderManager::setHasMeshColor(SGNode *sgNode, bool status, int paramIndex, bool isFragmentData, int materialIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *hasMeshColor = new float[(endIndex - startIndex) + 1];
    hasMeshColor[0] = ((sgNode->getType() != NODE_PARTICLES) && (sgNode->getProperty(IS_VERTEX_COLOR, materialIndex).value.x || sgNode->getProperty(SELECTED, materialIndex).value.x || status)) ? 1.0 : 0.0;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        hasMeshColor[i+1] = (v[j].second->getProperty(IS_VERTEX_COLOR, materialIndex).value.x || v[j].second->getProperty(SELECTED, materialIndex).value.x || status) ? 1.0 : 0.0;
        i++;
    }
    
    smgr->setPropertyValue(sgNode->node->material, "hasMeshColor", hasMeshColor, DATA_FLOAT, (endIndex - startIndex)+1, isFragmentData, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    delete [] hasMeshColor;
}

void ShaderManager::setNumberOfLights(SGNode *sgNode, int paramIndex)
{
    float lights = (float)ShaderManager::lightPosition.size();
    smgr->setPropertyValue(sgNode->node->material, "numberOfLights", &lights, DATA_FLOAT,1,true,paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setLightsProperties(SGNode *sgNode, int param1, int param2, int param3, int param4, int param5)
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
    setLightsPosition(sgNode, lightPositions, param1);
    setLightsColors(sgNode, lightColors, param2);
    
    ambientLight = getProperty(AMBIENT_LIGHT).value.x;
    
    smgr->setPropertyValue(sgNode->node->material, "fadeEndDistance", fadeEndDistance, DATA_FLOAT, lightsCount, true ,SHADER_COMMON_lightFadeDistance,smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material, "lightTypes", lightType, DATA_FLOAT, lightsCount, true ,param3,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    smgr->setPropertyValue(sgNode->node->material, "ambientLight", &ambientLight, DATA_FLOAT, 1, true, param4, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    float lCount = lightsCount;
    smgr->setPropertyValue(sgNode->node->material, "lightCount", &lCount, DATA_FLOAT, 1, true, param5, smgr->getNodeIndexByID(sgNode->node->getID()));
    lightChanged = false;
    delete [] fadeEndDistance;
    delete [] lightType;
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

void ShaderManager::setNodeLighting(SGNode *sgNode, int paramIndex, int materialIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *lighting = new float[((endIndex - startIndex)+1)];
    lighting[0] = (sgNode->getType() == NODE_LIGHT || sgNode->getProperty(SELECTED, materialIndex).value.x) ? 0 : (int)sgNode->getProperty(LIGHTING, materialIndex).value.x;
    lighting[0] = (ShaderManager::sceneLighting) ? lighting[0] : 0;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        lighting[i+1] = (v[j].second->getType() == NODE_LIGHT || v[j].second->getProperty(SELECTED, materialIndex).value.x) ? 0 : (float)v[j].second->getProperty(LIGHTING, materialIndex).value.x;
        lighting[i+1] = (ShaderManager::sceneLighting) ? lighting[i+1] : 0;
        i++;
    }
    
    smgr->setPropertyValue(sgNode->node->material, "hasLighting", lighting, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] lighting;
}

void ShaderManager::setShadowDakness(SGNode *sgNode, int paramIndex)
{
    float finalShadow = (shadowsOff || sgNode->getProperty(SELECTED).value.x) ? 0.0 : shadowDensity;
    smgr->setPropertyValue(sgNode->node->material, "shadowDarkness", &finalShadow, DATA_FLOAT, 1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setReflectionValue(SGNode *sgNode, int paramIndex, int materialIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *value = new float[((endIndex - startIndex)+1)];
    value[0] = sgNode->getProperty(SELECTED, materialIndex).value.x ?  0.0 : sgNode->getProperty(REFLECTION, materialIndex).value.x;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        value[i+1] = v[j].second->getProperty(SELECTED, materialIndex).value.x ?  0.0 : v[j].second->getProperty(REFLECTION, materialIndex).value.x;
        i++;
    }
    
    smgr->setPropertyValue(sgNode->node->material, "reflectionValue", value, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] value;
    
}

void ShaderManager::setEyePos(SGNode *sgNode,int paramIndex)
{
    float *campos = new float[3];
    campos[0] = camPos.x;    campos[1] = camPos.y;    campos[2] = camPos.z;
    smgr->setPropertyValue(sgNode->node->material, "eyePos", campos, DATA_FLOAT_VEC3, 3, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
    delete[] campos;
}

void ShaderManager::setLightViewProjMatrix(SGNode *sgNode,int paramIndex)
{
    Mat4 lightViewProjMatrix = lighCamProjMatrix;
    lightViewProjMatrix *= lighCamViewMatrix;
    Mat4 lvp = lightViewProjMatrix;// * sgNode->node->getModelMatrix();
    
    smgr->setPropertyValue(sgNode->node->material, "lvp", lvp.pointer(), DATA_FLOAT_MAT4, 16, false, paramIndex,smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setTexturesUniforms(SGNode *sgNode, u16 paramIndex, int materialIndex)
{
    string textureNames[] = {"colorMap", "normalMap", "shadowMap", "reflectionMap"};
    
    for (int i = NODE_TEXTURE_TYPE_COLORMAP; i <= NODE_TEXTURE_TYPE_REFLECTIONMAP; i++) {
        setTextureForNode(sgNode, sgNode->materialProps[materialIndex]->getTextureOfType((node_texture_type)i), textureNames[i], paramIndex, i);
    }
    
    float hasReflectionMap = (environmentTex) ? 1.0 : 0.0;
    smgr->setPropertyValue(sgNode->node->material, "hasReflectionMap", &hasReflectionMap, DATA_FLOAT, 1, true, SHADER_COMMON_hasReflectionMap, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    float hasNormalMap = sgNode->node->hasNormalMap;
    smgr->setPropertyValue(sgNode->node->material, "hasNormalMap", &hasNormalMap, DATA_FLOAT, 1, true, SHADER_COMMON_hasNormalMap, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    setSamplerType(sgNode, SHADER_COMMON_samplerType);
}

void ShaderManager::setTextureForNode(SGNode* sgNode, Texture* texture, string textureName, int paramIndex, int userValue)
{
    int textureValue = 0;
    
    if(deviceType == OPENGLES2) {
        OGLTexture* tex = (OGLTexture*)texture;
        if(texture != NULL) {
            textureValue = tex->OGLTextureName;
            smgr->setPropertyValue(sgNode->node->material, textureName, &textureValue, DATA_TEXTURE_2D, 1, true, paramIndex + userValue, smgr->getNodeIndexByID(sgNode->node->getID()), tex, userValue);
        }
    } else if(deviceType == METAL) {
        textureValue =  userValue;
        Texture* tex = sgNode->node->getTextureByIndex(userValue);
        if(tex != NULL) {
            smgr->setPropertyValue(sgNode->node->material, textureName, &textureValue, DATA_TEXTURE_2D, 1, true, paramIndex + userValue, smgr->getNodeIndexByID(sgNode->node->getID()), NULL, 1, (userValue == NODE_TEXTURE_TYPE_COLORMAP) ? sgNode->smoothTexture : false);
        } else {
            smgr->setPropertyValue(sgNode->node->material, textureName, &textureValue, DATA_TEXTURE_2D, 1, true, paramIndex + userValue, smgr->getNodeIndexByID(sgNode->node->getID()), NULL, 1, (userValue == NODE_TEXTURE_TYPE_COLORMAP) ? sgNode->smoothTexture : false);
        }
    }

}

void ShaderManager::setSamplerType(SGNode *sgNode, u16 paramIndex)
{
    float samplerType = (sgNode->smoothTexture) ? 0.0 : 1.0;
    smgr->setPropertyValue(sgNode->node->material, "samplerType", &samplerType, DATA_FLOAT, 1, true , SHADER_COMMON_samplerType, smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setNodeTransparency(SGNode *sgNode, u16 paramIndex, int materialIndex)
{
    
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *transparency = new float[((endIndex - startIndex)+1)];
    transparency[0] = (sgNode->getProperty(SELECTED, materialIndex).value.x || !sgNode->getProperty(VISIBILITY, materialIndex).value.x) ?  NODE_SELECTION_TRANSPARENCY : sgNode->getProperty(TRANSPARENCY, materialIndex).value.x;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        transparency[i+1] = (v[j].second->getProperty(SELECTED, materialIndex).value.x || !v[j].second->getProperty(VISIBILITY, materialIndex).value.x) ?  NODE_SELECTION_TRANSPARENCY : v[j].second->getProperty(TRANSPARENCY, materialIndex).value.x;
        i++;
    }
    
    Material * material = sgNode->node->material;
    smgr->setPropertyValue(material, "transparencyValue", transparency, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] transparency;
    
}

void ShaderManager::setUVScaleValue(SGNode *sgNode, u16 paramIndex, int materialIndex)
{
    int startIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt : 0;
    int endIndex = (sgNode->instanceNodes.size() > 0) ? sgNode->node->instancingRenderIt + ShaderManager::maxIntsances : 0;
    if(endIndex > sgNode->instanceNodes.size())
        endIndex = (int)sgNode->instanceNodes.size();
    
    float *uvScale = new float[((endIndex - startIndex)+1)];
    uvScale[0] = sgNode->node->uvScale;
    
    int i = 0;
    vector < std::pair<int, SGNode*> > v(sgNode->instanceNodes.begin(), sgNode->instanceNodes.end());
    for(int j = startIndex; j < endIndex; j++) {
        uvScale[i+1] = sgNode->node->uvScale;// v[j].second->getProperty(TRANSPARENCY).value.x;
        i++;
    }
    
    Material * material = sgNode->node->material;
    smgr->setPropertyValue(material, "uvScaleValue", uvScale, DATA_FLOAT, (endIndex - startIndex)+1, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] uvScale;
}

void ShaderManager::setSceneDataUniforms(SGNode *node, u16 paramIndex)
{
    
}

void ShaderManager::setModelViewProjMatrix(SGNode *sgNode, u16 paramIndex, bool isDepthPass)
{
    
    Mat4 projMat = (isDepthPass) ? lighCamProjMatrix : smgr->getActiveCamera()->getProjectionMatrix();
    Mat4 viewMat = (isDepthPass) ? lighCamViewMatrix : smgr->getActiveCamera()->getViewMatrix();
    
    Mat4 mvp;
    std::string uniformName = "mvp";

    if(sgNode->node->getID() == GREEN_LINES_ID || sgNode->node->getID() == BLUE_LINES_ID || sgNode->node->getID() == RED_LINES_ID || sgNode->node->material->name == "SHADER_MESH") {
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
    
    bool meshCacheCreated = false;
    if(!renderingPreview) {
        meshCacheCreated = pNode->updateParticles((sgNode->getProperty(SELECTED).value.x || isRendering), smgr->getActiveCamera()->getPosition());
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
    
    float fovRatio = (smgr->getActiveCamera()->getFOVInRadians() * 180.0f / PI) / 35.0;
    
    float* sColor = new float[4];
    Vector4 vColor = sgNode->getProperty(VERTEX_COLOR).value.x;
    sColor[0] = (sgNode->getProperty(IS_VERTEX_COLOR).value.x) ? vColor.x : pNode->startColor.x;
    sColor[1] = (sgNode->getProperty(IS_VERTEX_COLOR).value.x) ? vColor.y : pNode->startColor.y;
    sColor[2] = (sgNode->getProperty(IS_VERTEX_COLOR).value.x) ? vColor.z : pNode->startColor.z;
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
    
    smgr->setPropertyValue(sgNode->node->material, "props", particleProps, DATA_FLOAT_VEC4, 4, false, SHADER_PARTICLE_props, smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material, "startColor", sColor, DATA_FLOAT_VEC4, 4, false, SHADER_PARTICLE_sColor, smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material, "midColor", mColor, DATA_FLOAT_VEC4, 4, false, SHADER_PARTICLE_mColor, smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material, "endColor", eColor, DATA_FLOAT_VEC4, 4, false, SHADER_PARTICLE_eColor, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    smgr->setPropertyValue(sgNode->node->material, "vp", vp.pointer(), DATA_FLOAT_MAT4, 16, false, SHADER_PARTICLE_vp, smgr->getNodeIndexByID(sgNode->node->getID()));
    smgr->setPropertyValue(sgNode->node->material, "model", model.pointer(), DATA_FLOAT_MAT4, 16, false, SHADER_PARTICLE_world, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] particleProps;
    delete [] sColor;
    delete [] mColor;
    delete [] eColor;
}

void ShaderManager::setViewProjMatrix(SGNode *sgNode, u16 paramIndex)
{
    Mat4 projMat = smgr->getActiveCamera()->getProjectionMatrix();
    Mat4 viewMat = smgr->getActiveCamera()->getViewMatrix();
    Mat4 mvp = projMat * viewMat;
    smgr->setPropertyValue(sgNode->node->material, "vp", mvp.pointer(), DATA_FLOAT_MAT4, 16, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setViewMatrix(SGNode *sgNode,u16 paramIndex)
{
    Mat4 viewMat = smgr->getActiveCamera()->getViewMatrix();
    Mat4 view = viewMat;
    smgr->setPropertyValue(sgNode->node->material, "view", view.pointer(), DATA_FLOAT_MAT4, 16, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
}

void ShaderManager::setModelMatrix(SGNode *sgNode, u16 paramIndex)
{
    
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
    
    smgr->setPropertyValue(sgNode->node->material, "model", modelArray, DATA_FLOAT_MAT4, ((endIndex - startIndex)+1) * 16, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    
    delete [] modelArray;
}

void ShaderManager::copyMat(float* pointer,Mat4& mat)
{
    for(int i = 0;i < 16;++i)
        *pointer++ = mat[i];
}

void ShaderManager::setJointTransform(SGNode *sgNode, int paramIndex, SceneManager *smgr)
{
    SkinMesh *sMesh = (SkinMesh*)(dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getMesh();
    float* JointArray = new float[sMesh->joints->size() * 16];
    int copyIncrement = 0;
    
    for(int i = 0; i < sMesh->joints->size(); ++i){
        Mat4 JointVertexPull;
        JointVertexPull.setbyproduct((*sMesh->joints)[i]->GlobalAnimatedMatrix, (*sMesh->joints)[i]->GlobalInversedMatrix);
        copyMat(JointArray + copyIncrement, JointVertexPull);
        copyIncrement += 16;
    }
    smgr->setPropertyValue(sgNode->node->material, "jointTransforms", JointArray, DATA_FLOAT_MAT4, sMesh->joints->size() * 16, false, paramIndex, smgr->getNodeIndexByID(sgNode->node->getID()));
    delete [] JointArray;
}

Mat4 ShaderManager::ortho2d_oc(float left, float right, float bottom, float top, float near, float far)
{
    Mat4 mat;
    float sLength = 1.0f / (right - left);
    float sHeight = 1.0f / (top   - bottom);
    float sDepth  = 1.0f / (far   - near);
    
    mat.setElement(0, 2.0f * sLength);
    mat.setElement(4, 0.0f);
    mat.setElement(8, 0.0f);
    mat.setElement(12, 0.0f);
    
    mat.setElement(1, 0.0f);
    mat.setElement(5, 2.0f * sHeight);
    mat.setElement(9, 0.0f);
    mat.setElement(13, 0.0f);
    
    mat.setElement(2, 0.0f);
    mat.setElement(6, 0.0f);
    mat.setElement(10, sDepth);
    mat.setElement(14, 0.0f);
    
    mat.setElement(3, -sLength * (left + right));
    mat.setElement(7, -sHeight * (top + bottom));
    mat.setElement(11, -sDepth  * near);
    mat.setElement(15, 1.0f);
    
    return mat;
}

