//
//  SceneManager.cpp
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "SceneManager.h"

#import "TargetConditionals.h"

SceneManager::SceneManager(float width, float height, float screenScale, string bundlePath, void *renderView)
{
    displayWidth = width;
    displayHeight = height;
    this->screenScale = screenScale;
    this->bundlePath = bundlePath;
    renderTargetIndex = 0;

    renderMan = (RenderManager*)initMetalRenderManager(renderView, width, height, screenScale);
    if(!renderMan)
        return NULL;
    
    renderMan->supportsInstancing = true;
    mtlManger = new MaterialManager();
    
    renderMan->maxInstances = 4000;
    renderMan->emptyTexture = loadTexture("Env Texture", bundlePath + "/envmap.png", TEXTURE_RGBA8, TEXTURE_BYTE, true, 0);
}

SceneManager::~SceneManager()
{
    RemoveAllNodes();
    RemoveAllTextures();
    if(mtlManger)
        delete mtlManger;
    if(renderMan)
        delete renderMan;
}

void SceneManager::clearMaterials()
{
    mtlManger->RemoveAllMaterials();
}

Vector2 SceneManager::getViewPort()
{
    if(renderMan)
        return renderMan->getViewPort();
    return Vector2(0.0, 0.0);
}

void SceneManager::RemoveAllTextures()
{
    for(int i = 0; i < textures.size(); i++) {
        if(textures[i])
            delete textures[i];
    }
    textures.clear();
    
}

void SceneManager::setDisplayResolution(int width, int height)
{
    displayWidth = width;
    displayHeight = height;
}

void SceneManager::AddNode(shared_ptr<Node> node, MESH_TYPE meshType)
{
    if(node->type != NODE_TYPE_INSTANCED || (!renderMan->supportsVAO && node->type != NODE_TYPE_INSTANCED))
        renderMan->createVertexAndIndexBuffers(node,meshType);
    nodes.push_back(node);
}

void SceneManager::updateVertexAndIndexBuffers(shared_ptr<Node> node, MESH_TYPE meshType)
{
    if(!renderMan->supportsVAO)
        renderMan->createVertexAndIndexBuffers(node,meshType, true);
    
    if(node->type == NODE_TYPE_PARTICLES) {
        for( int i = 0; i < dynamic_pointer_cast<MeshNode>(node)->getMesh()->getMeshBufferCount(); i++) {
            if(renderMan->supportsVAO)
                ((OGLES2RenderManager*)renderMan)->updateVAO(node, true, false, i);
            else
                ((OGLES2RenderManager*)renderMan)->bindBufferAndAttributes(node, i, meshType);
        }
    }
}

void SceneManager::RemoveNode(shared_ptr<Node> node)
{
    int sameNodeIdcount = 0;
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->getID() == node->getID()) {
            sameNodeIdcount++;
        }
    }
    
    if(sameNodeIdcount > 1)
        Logger::log(ERROR,"SceneManager::RemoveNode","Node id repeats");
    
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->getID() == node->getID()) {
            nodes[i]->detachFromParent();
            nodes[i]->detachAllChildren();
            if(nodes[i])
                nodes[i].reset();
            
            nodes.erase(nodes.begin() + i);
            break;
        }
    }
}

void SceneManager::RemoveTexture(Texture *texture)
{
    for(int i = 0; i < textures.size(); i++) {
        if(textures[i] == texture) {
            if(textures[i] == texture)
                delete textures[i];
            textures.erase(textures.begin() + i);
            break;
        }
    }
}

void SceneManager::RemoveAllNodes()
{
    for(int i = 0; i < nodes.size(); i++) {
        if (nodes[i]) {
            nodes[i]->detachFromParent();
            nodes[i]->detachAllChildren();
            nodes[i].reset();
            nodes.erase(nodes.begin() + i);
        }
    }
    nodes.clear();
}

bool SceneManager::PrepareDisplay(int width, int height, bool clearColorBuf, bool clearDepthBuf, bool isDepthPass, Vector4 color)
{
    renderTargetIndex++;
    return renderMan->PrepareDisplay(width,height,clearColorBuf,clearDepthBuf,isDepthPass,color);
}

void SceneManager::Render(bool isRTT)
{
    renderMan->setTransparencyBlending(false);
    vector<int> nodeIndex;
    for (int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->type <= NODE_TYPE_CAMERA || nodes[i]->getVisible() == false)
            continue;
        
        if(isTransparentCallBack(nodes[i]->getID(), nodes[i]->callbackFuncName)) {
            nodeIndex.push_back(i);
            continue;
        }
        RenderNode(isRTT, i);
    }
    
    if(nodeIndex.size()) {
        renderMan->setTransparencyBlending(true);
        
        for (int i = 0; i < nodeIndex.size(); i++) {
            int nodeId = nodeIndex[i];
            if((nodes[nodeId]->getID() >= 600000 && nodes[nodeId]->getID() < 600010) || (nodes[nodeId]->getID() >= 300000 && nodes[nodeId]->getID() <= 301000))
                continue;
            
            RenderNode(isRTT, nodeId);
        }
        nodeIndex.clear();
    }
}

void SceneManager::EndDisplay()
{
    renderTargetIndex = 0;
    renderMan->endDisplay();
}

void SceneManager::clearDepthBuffer()
{
    renderMan->setUpDepthState(CompareFunctionLessEqual, false, true); // ToDo change in depthstate for each render, may need
}

void SceneManager::RenderNodeAlone(shared_ptr<Node> node)
{
    int index = 0;
    
    for( int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->getID() == node->getID()) {
            index = i;
            break;
        }
    }
    
    nodes[index]->update();
    Mesh* meshToRender;
    if(nodes[index]->instancedNodes.size() > 0 && !renderMan->supportsInstancing)
        meshToRender = dynamic_pointer_cast<MeshNode>(nodes[index])->meshCache;
    else
        meshToRender = dynamic_pointer_cast<MeshNode>(nodes[index])->getMesh();
    
    for(int meshBufferIndex = 0; meshBufferIndex < meshToRender->getMeshBufferCount(); meshBufferIndex++) {
        
        if(!renderMan->PrepareNode(nodes[index], meshBufferIndex, false, index))
            return;
        
        int materialIndex = dynamic_pointer_cast<MeshNode>(nodes[index])->getMesh()->getMeshBufferMaterialIndices(meshBufferIndex);
        
        if(nodes[index]->instancedNodes.size() > 0) {
            for(nodes[index]->instancingRenderIt = 0; nodes[index]->instancingRenderIt < nodes[index]->instancedNodes.size(); nodes[index]->instancingRenderIt += renderMan->maxInstances) {
                ShaderCallBackForNode(nodes[index]->getID(), nodes[index]->material->name, materialIndex, nodes[index]->callbackFuncName);
                renderMan->Render(nodes[index], false, index, meshBufferIndex);
            }
        } else {
            ShaderCallBackForNode(nodes[index]->getID(), nodes[index]->material->name, materialIndex, nodes[index]->callbackFuncName);
            renderMan->Render(nodes[index], false, index,meshBufferIndex);
        }
        
    }
    
    nodes[index]->shouldUpdateMesh = false;
}

void SceneManager::RenderNode(bool isRTT, int index, bool clearDepthBuffer, METAL_DEPTH_FUNCTION func, bool changeDepthState)
{
    if(!nodes[index])
        return;
    
    renderMan->setUpDepthState(func, true, clearDepthBuffer); // ToDo change in depthstate for each render,  need optimisation
    
    nodes[index]->update();
    Mesh* meshToRender;
    if(nodes[index]->instancedNodes.size() > 0 && !renderMan->supportsInstancing)
        meshToRender = dynamic_pointer_cast<MeshNode>(nodes[index])->meshCache;
    else
        meshToRender = dynamic_pointer_cast<MeshNode>(nodes[index])->getMesh();
    
    if(nodes[index]->drawMode == DRAW_MODE_LINES && isRTT)
        return;
    
    for(int meshBufferIndex = 0; meshBufferIndex < meshToRender->getMeshBufferCount(); meshBufferIndex++) {
        if(!renderMan->PrepareNode(nodes[index], meshBufferIndex, isRTT, index))
            return;
        
        int materialIndex = dynamic_pointer_cast<MeshNode>(nodes[index])->getMesh()->getMeshBufferMaterialIndices(meshBufferIndex);
        if(nodes[index]->instancedNodes.size() > 0) {
            for(nodes[index]->instancingRenderIt = 0; nodes[index]->instancingRenderIt < nodes[index]->instancedNodes.size(); nodes[index]->instancingRenderIt += renderMan->maxInstances) {
                
                if(!renderMan->supportsVAO && nodes[index]->instancingRenderIt > 0)
                    renderMan->PrepareNode(nodes[index], meshBufferIndex, isRTT, index);
                
                ShaderCallBackForNode(nodes[index]->getID(), nodes[index]->material->name, materialIndex, nodes[index]->callbackFuncName);
                renderMan->Render(nodes[index],isRTT, index,meshBufferIndex);
            }
        } else {
            ShaderCallBackForNode(nodes[index]->getID(), nodes[index]->material->name, materialIndex, nodes[index]->callbackFuncName);
            renderMan->Render(nodes[index], isRTT, index, meshBufferIndex);
        }
        
    }
    
    nodes[index]->shouldUpdateMesh = false;
}

void SceneManager::setShaderState(int nodeIndex)
{
    if(nodes[nodeIndex]->type <= NODE_TYPE_CAMERA)
        return;
    
    MTLPipelineStateCallBack(nodeIndex);
}

void SceneManager::setActiveCamera(shared_ptr<Node> node)
{
    for(int i = 0;i < nodes.size();i++){
        if(nodes[i]->type <= NODE_TYPE_CAMERA)
            (dynamic_pointer_cast<CameraNode>(nodes[i]))->setActive(false);
    }
    renderMan->setActiveCamera(dynamic_pointer_cast<CameraNode>(node));
}

shared_ptr<CameraNode> SceneManager::getActiveCamera()
{
    return renderMan->getActiveCamera();
}

Texture* SceneManager::loadTexture(string textureName, string filePath, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE type, bool blurTexture, int blurRadius)
{
    Texture *newTex = (Texture*)initMTLTexture();
    
    if(newTex->loadTexture(textureName, filePath, format, type, blurTexture, blurRadius)) {
        renderMan->resetTextureCache();
        textures.push_back(newTex);
        return newTex;
    } else {
        if(newTex)
            delete newTex;
        return NULL;
    }
}

Texture* SceneManager::loadTextureFromVideo(string videoFileName, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE type)
{
    Texture *newTex = (Texture*)initMTLTexture();
    
    newTex->loadTextureFromVideo(videoFileName, format, type);
    renderMan->resetTextureCache();
    textures.push_back(newTex);
    
    return newTex;
}

shared_ptr<MeshNode> SceneManager::createNodeFromMesh(Mesh* mesh, string callbackFuncName, MESH_TYPE meshType, int matIndex)
{
    shared_ptr<MeshNode> node = make_shared<MeshNode>(); //shared_ptr<MeshNode>(new MeshNode());
    if(matIndex != NOT_EXISTS && mtlManger->materials->size())
        node->setMaterial(getMaterialByIndex(matIndex));
    node->mesh = mesh;
    node->callbackFuncName = callbackFuncName;
    node->mesh->meshType = meshType;
    AddNode(node,meshType);
    return node;
}

shared_ptr<AnimatedMeshNode> SceneManager::createAnimatedNodeFromMesh(SkinMesh* mesh, string callbackFuncName, int maxJoints, rig_type rigType, MESH_TYPE meshType)
{
    shared_ptr<AnimatedMeshNode> node = make_shared<AnimatedMeshNode>();
    node->setMesh(mesh, maxJoints, rigType);
    node->callbackFuncName = callbackFuncName;
    node->mesh->meshType = meshType;
    AddNode(node,meshType);
    return node;
}

shared_ptr<ParticleManager> SceneManager::createParticlesFromMesh(Mesh* mesh, string callBackFuncName, MESH_TYPE meshType, int matIndex)
{
    shared_ptr<ParticleManager> node = make_shared<ParticleManager>();
    if(matIndex != NOT_EXISTS && mtlManger->materials->size())
        node->setMaterial(getMaterialByIndex(matIndex));
    node->mesh = mesh;
    node->callbackFuncName = callBackFuncName;
    node->mesh->meshType = meshType;
    AddNode(node,meshType);
    return node;
}

shared_ptr<CameraNode> SceneManager::createCameraNode(string callBackFuncName)
{
    shared_ptr<CameraNode> cam = make_shared<CameraNode>();
    cam->callbackFuncName = callBackFuncName;
    AddNode(cam,MESH_TYPE_LITE);
    return cam;
}

shared_ptr<PlaneMeshNode> SceneManager::createPlaneNode(string callBackFuncName, float aspectRatio)
{
    shared_ptr<PlaneMeshNode> plane(new PlaneMeshNode(aspectRatio));// = make_shared<PlaneMeshNode>(aspectRatio);
    plane->callbackFuncName = callBackFuncName;
    AddNode(plane,MESH_TYPE_LITE);
    return plane;
}

shared_ptr<SGCircleNode> SceneManager::createCircleNode(int totVertices, float radius, string callBackFuncName, bool allAxis)
{
    shared_ptr<SGCircleNode> node = shared_ptr<SGCircleNode>(new SGCircleNode(totVertices, radius, allAxis));
    node->callbackFuncName = callBackFuncName;
    AddNode(node,MESH_TYPE_LITE);
    return node;
}

shared_ptr<LightNode> SceneManager::createLightNode(Mesh *mesh, string callBackFuncName)
{
    shared_ptr<LightNode> light(new LightNode());
    light->callbackFuncName = callBackFuncName;
    light->mesh = mesh;
    AddNode(light,MESH_TYPE_LITE);
    return light;
}

shared_ptr<Node> SceneManager::createInstancedNode(shared_ptr<Node> original, string callBackFuncName)
{
    //shared_ptr< InstanceNode > iNode(new InstanceNode(original));
    shared_ptr<MeshNode> iNode = make_shared<MeshNode>();
    iNode->callbackFuncName = callBackFuncName;
    iNode->type = NODE_TYPE_INSTANCED;
    iNode->original = original;
    iNode->mesh = new Mesh();
    
    if(!renderMan->supportsInstancing && original->instancedNodes.size() < renderMan->maxInstances) {
        if(!dynamic_pointer_cast<MeshNode>(original)->meshCache)
            dynamic_pointer_cast<MeshNode>(original)->meshCache = new Mesh();
        
        dynamic_pointer_cast<MeshNode>(original)->meshCache->copyInstanceToMeshCache(dynamic_pointer_cast<MeshNode>(original)->mesh, (int)original->instancedNodes.size()+1);
        original->shouldUpdateMesh = true;
    }
    else if(!renderMan->supportsVAO){
        original->shouldUpdateMesh = true;
    }
    
    AddNode(iNode, MESH_TYPE_LITE);
    original->instancedNodes.push_back(iNode);
    return iNode;
}

void SceneManager::draw2DImage(Texture *texture, Vector2 originCoord, Vector2 endCoord, Material *material, bool isRTT)
{
    if(!material || !texture)
        return;
    
    renderMan->useMaterialToRender(material);
    int textureValue = 0;
    setPropertyValue(material, "texture1", &textureValue, DATA_TEXTURE_2D, 1, true, 0, NOT_EXISTS, NOT_EXISTS, texture);
    renderMan->draw2DImage(texture, originCoord, endCoord, material, isRTT);
    
}

void SceneManager::draw3DLine(Vector3 start, Vector3 end, Vector3 color, Material *material, int mvpUniParamIndex, int vertexColorUniParamIndex, int transparencyUniParamIndex)
{
    renderMan->useMaterialToRender(material);
    float vertColor[3] = {color.x, color.y, color.z};
    float transparency = 1.0;
    getActiveCamera()->update();
    Mat4 mat = getActiveCamera()->getProjectionMatrix() * getActiveCamera()->getViewMatrix();
    
    setPropertyValue(material, "mvp", mat.pointer(), DATA_FLOAT_MAT4, 16, false, mvpUniParamIndex);
    setPropertyValue(material, "perVertexColor", &vertColor[0], DATA_FLOAT_VEC3, 3, false, vertexColorUniParamIndex);
    setPropertyValue(material, "transparency", &transparency, DATA_FLOAT, 1, false, transparencyUniParamIndex);
    
    renderMan->draw3DLine(start, end, material);
}

void SceneManager::draw3DLines(vector<Vector3> vPositions, Vector3 color, Material *material, int mvpUniParamIndex, int vertexColorUniParamIndex, int transparencyUniParamIndex)
{
    renderMan->useMaterialToRender(material);
    float vertColor[3] = {color.x, color.y, color.z};
    float transparency = 1.0;
    getActiveCamera()->update();
    Mat4 mat = getActiveCamera()->getProjectionMatrix() * getActiveCamera()->getViewMatrix();
    setPropertyValue(material, "mvp", mat.pointer(), DATA_FLOAT_MAT4, 16, false, mvpUniParamIndex);
    setPropertyValue(material, "perVertexColor", &vertColor[0], DATA_FLOAT_VEC3, 3, false, vertexColorUniParamIndex);
    setPropertyValue(material, "transparency", &transparency, DATA_FLOAT, 1, false, transparencyUniParamIndex);
    renderMan->draw3DLines(vPositions, material);
}

void SceneManager::setPropertyValue(Material *material, string name, float* values, DATA_TYPE type, unsigned short count, bool isFragmentData, u16 paramIndex, int nodeIndex, int materialIndex, Texture *tex, int userValue)
{
    shared_ptr<Node> nod;
    if(nodeIndex != NOT_EXISTS) nod = nodes[nodeIndex];
    
    if(nodeIndex == NOT_EXISTS) {
        renderMan->bindDynamicUniform(material,name,values,type,count,paramIndex,nodeIndex,tex,isFragmentData);
    } else {
        short uIndex = material->setPropertyValue(name, values, type, count, paramIndex, nodeIndex, materialIndex, renderTargetIndex);
        renderMan->BindUniform(material, nod, uIndex, isFragmentData, userValue);
    }
}

void SceneManager::setPropertyValue(Material *material, string name, int* values, DATA_TYPE type, unsigned short count, bool isFragmentData, u16 paramIndex, int nodeIndex, int materialIndex, Texture *tex, int userValue, bool blurTex)
{
    shared_ptr<Node> nod;
    if(nodeIndex != NOT_EXISTS)
        nod = nodes[nodeIndex];
    
    if(nodeIndex == NOT_EXISTS) {
        renderMan->bindDynamicUniform(material, name, values, type, count, paramIndex, nodeIndex, tex, isFragmentData, blurTex);
    } else {
        short uIndex = material->setPropertyValue(name, values, type, count, paramIndex, nodeIndex, materialIndex, renderTargetIndex);
        renderMan->bindDynamicUniform(material, name, values, type, count, paramIndex, nodeIndex, tex, isFragmentData, blurTex);
    }
}

bool SceneManager::LoadShaders(string materialName, string vShaderName, string fShaderName, std::map< string, string > shadersStr, bool isDepthPass, bool isTest)
{
    return mtlManger->CreateMaterial(materialName, vShaderName, fShaderName, shadersStr, isDepthPass, isTest);
}

Material* SceneManager::getMaterialByIndex(int index)
{
    if(index < (int)(*mtlManger->materials).size())
        return (*mtlManger->materials)[index];
    else
        return 0;
}

Material* SceneManager::getMaterialByName(string name)
{
    for(int i = 0 ; i < mtlManger->materials->size();i++){
        if((*mtlManger->materials)[i]->name.compare(name) == 0)
            return (*mtlManger->materials)[i];
    }
    Logger::log(ERROR,"SceneManager::getMaterialByName", "No Material with the name " + name + " exists");
    return NULL;
}

int SceneManager::getNodeIndexByID(int id)
{
    for(int i = 0;i < nodes.size();i++){
        if(nodes[i]->getID() == id)
            return i;
    }
    Logger::log(ERROR, "SceneManager:getNodeIndexByID","Id not Exits");
    return NULL;
}

Texture* SceneManager::createRenderTargetTexture(string textureName, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height)
{
    textures.push_back(renderMan->createRenderTargetTexture(textureName,format,texelType,width,height));
    
    return textures[textures.size()-1];
}

void SceneManager::setRenderTarget(Texture *renderTexture, bool clearBackBuffer, bool clearZBuffer, bool isDepthPass, Vector4 color)
{
    if(renderTexture)
        renderTargetIndex++;
    renderMan->setRenderTarget(renderTexture,clearBackBuffer,clearZBuffer,isDepthPass,color);
}

Vector3 SceneManager::getPixelColor(Vector2 touchPosition, Texture* texture, Vector4 bgColor)
{
    Vector4 color = renderMan->getPixelColor(touchPosition,texture);
    return Vector3(color.x,color.y,color.z);
}

void SceneManager::writeImageToFile(Texture *texture, char* filePath, IMAGE_FLIP flipType)
{
    renderMan->writeImageToFile(texture, filePath , flipType);
}

void SceneManager::setFrameBufferObjects(uint32_t fb, uint32_t cb, uint32_t db)
{
    ((OGLES2RenderManager*)renderMan)->setFrameBufferObjects(fb,cb,db);
}

shared_ptr<EmptyNode> SceneManager::addEmptyNode()
{
    shared_ptr<EmptyNode> eNode = make_shared<EmptyNode>(); //shared_ptr<EmptyNode>(new EmptyNode());
    AddNode(eNode);
    return eNode;
}

void SceneManager::updateVertexBuffer(int nodeIndex)
{
    for(int i = 0; i < dynamic_pointer_cast<MeshNode>(nodes[nodeIndex])->getMesh()->getMeshBufferCount(); i++) {
        renderMan->createVertexBuffer(nodes[nodeIndex],i);
    }
}

bool SceneManager::setVAOSupport(bool status)
{
    renderMan->supportsVAO = status;
}


