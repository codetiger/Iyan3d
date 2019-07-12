//
//  SceneManager.h
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__SceneManager__
#define __SGEngine2__SceneManager__

#include "../RenderManager/RenderManager.h"
#include "../RenderManager/OGLES2RenderManager.h"
#include "../RenderManager/MetalWrapper.h"
#include "../Core/Textures/Texture.h"
#include "../Core/Textures/OGLTexture.h"
#include "../Core/Nodes/AnimatedMeshNode.h"
#include "../Core/Nodes/ParticleManager.h"
#include "../Core/Material/Material.h"
#include "../Core/Material/MaterialManager.h"
#include "../Core/Meshes/PlaneMeshNode.h"
#include "../Core/Nodes/CameraNode.h"
#include "../Core/Nodes/EmptyNode.h"
#include "../Core/Meshes/SGCircleNode.h"

class SceneManager {
private:
    int draw2DMatIndex;
    MaterialManager* mtlManger;
    void setShaderState(int nodeIndex);
    int renderTargetIndex;
    
public:
    void AddNode(shared_ptr<Node> node,MESH_TYPE meshType = MESH_TYPE_LITE);

    RenderManager *renderMan;
    string bundlePath;
    vector< shared_ptr<Node> > nodes;
    vector<Texture*> textures;
    float displayWidth,displayHeight,screenScale;

    SceneManager(float width,float height,float screenScale,string bundlePath,void *renderView = NULL);
    ~SceneManager();
    void initRenderManager();
    
    void clearMaterials();
    Vector2 getViewPort();
    void RemoveNode(shared_ptr<Node> node);
    void RemoveAllTextures();
    void RemoveTexture(Texture *texture);
    void RemoveAllNodes();
    void Render(bool isRTT);
    void RenderNode(bool isRTT, int index,bool clearDepthBuffer = false,METAL_DEPTH_FUNCTION func = CompareFunctionLessEqual,bool changeDepthState = false);
    void RenderNodeAlone(shared_ptr<Node> node);
    void setDisplayResolution(int width,int height);
    void setActiveCamera(shared_ptr<Node> node);
    shared_ptr<CameraNode> getActiveCamera();
    Texture* loadTexture(string textureName,string filePath,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE type, bool blurTexture, int blurRadius = 0);
    Texture* loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE type);
    shared_ptr<MeshNode> createNodeFromMesh(Mesh* mesh, string callBackFuncName, MESH_TYPE meshType = MESH_TYPE_LITE, int matIndex = -1);
    shared_ptr<AnimatedMeshNode> createAnimatedNodeFromMesh(SkinMesh* mesh, string callBackFuncName, int maxJoints, rig_type rigType = CHARACTER_RIG ,MESH_TYPE meshType = MESH_TYPE_LITE);
    shared_ptr<ParticleManager> createParticlesFromMesh(Mesh* mesh,string callBackFuncName,MESH_TYPE meshType = MESH_TYPE_LITE,int matIndex = -1);
    shared_ptr<CameraNode> createCameraNode(string callBackFuncName);
    shared_ptr<PlaneMeshNode> createPlaneNode(string callBackFuncName , float aspectRatio);
    shared_ptr<SGCircleNode> createCircleNode(int totVertices, float radius,string callBackFuncName, bool allAxis = false);
    shared_ptr<LightNode> createLightNode(Mesh *mesh, string callBackFuncName);
    shared_ptr<Node> createInstancedNode(shared_ptr<Node> original, string callBackFuncName);
    
    void (*ShaderCallBackForNode)(int nodeId, std::string mateialName, int materialIndex, std::string callbackFuncName);
    bool (*isTransparentCallBack)(int nodeId, string callbackFuncName);
    void setMaterialProperty(Material *mat,string name,float* values,DATA_TYPE type,unsigned short count,u16 paramIndex = 0);
    void (*MTLPipelineStateCallBack)(int);
    void (*MTLEndEncoding)();
    bool LoadShaders(string materialName,string vShaderName,string fShaderName, std::map< string, string > shadersStr, bool isDepthPass = false, bool isTest = false);
    Material* getMaterialByIndex(int index);
    Material* getMaterialByName(string name);
    int getNodeIndexByID(int id);

    void setPropertyValue(Material *material, string name, float* values, DATA_TYPE type, unsigned short count, bool isFragmentData, u16 paramIndex = 0, int nodeIndex = -1, int materialIndex = -1, Texture *tex = NULL, int userValue = 0);
    void setPropertyValue(Material *material, string name, int* values, DATA_TYPE type, unsigned short count, bool isFragmentData, u16 paramIndex = 0, int nodeIndex = -1, int materialIndex = -1, Texture *tex = NULL, int userValue = 0, bool blurTex = true);
    
    bool RemoveMaterialByIndex(u16 index);
    bool RemoveMaterial(Material *mat);

    void draw3DLine(Vector3 start , Vector3 end , Vector3 color , Material *material,int mvpUniParamIndex,int vertexColorUniParamIndex,int transparencyUniParamIndex);
    void draw3DLines(vector<Vector3> vPositions, Vector3 color, Material *material, int mvpUniParamIndex,int vertexColorUniParamIndex,int transparencyUniParamIndex);
    void draw2DImage(Texture *texture, Vector2 originCoord, Vector2 endCoord, Material *material, bool isRTT = false);
    
    Texture* createRenderTargetTexture(string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType,int width,int height);
    void setRenderTarget(Texture* renderTexture,bool clearBackBuffer = true,bool clearZBuffer = true,bool isDepthPass=false,Vector4 color = Vector4(255,255,255,255));
    Vector3 getPixelColor(Vector2 touchPosition,Texture* texture,Vector4 bgColor = Vector4(255.0,255.0,255.0,255.0));
    void setFrameBufferObjects(uint32_t fb,uint32_t rb,uint32_t db);
    bool PrepareDisplay(int width,int height,bool clearColorBuf = true,bool clearDepthBuf = true,bool isDepthPass = false,Vector4 color = Vector4(255,255,255,255));
    void EndDisplay();
    shared_ptr<EmptyNode> addEmptyNode();
    void updateVertexBuffer(int nodeIndex);
    void clearDepthBuffer();
    void writeImageToFile(Texture *texture , char* filePath, IMAGE_FLIP flipType = NO_FLIP);
    
    void updateVertexAndIndexBuffers(shared_ptr<Node> node,MESH_TYPE meshType = MESH_TYPE_LITE);
    bool setVAOSupport(bool status);
};

#endif /* defined(__SGEngine2__SceneManager__) */
