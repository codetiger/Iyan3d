//
//  RenderManager.h
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__RenderManager__
#define __SGEngine2__RenderManager__

#include "../Core/common/common.h"
#include "../Core/Nodes/Node.h"
#include "../Core/Nodes/MeshNode.h"
#include "../Core/Meshes/CubeMeshNode.h"
#include "../Core/Nodes/LightNode.h"
#include "../Core/Nodes/CameraNode.h"


typedef enum {
    ATTRIB_VERTEX = 0,
    ATTRIB_NORMAL,
    ATTRIB_TANGENT,
    ATTRIB_TEXUVA,
    ATTRIB_TEXUVB,
    ATTRIB_BONEINDEXES,
    ATTRIB_BONEWEIGHTS,
    NUM_ATTRIBUTES
} vertex_attrib_t;

class RenderManager {

protected:
    shared_ptr<CameraNode> camera;
public:
    RenderManager(){
        
    }
    virtual ~RenderManager(){
        
    }
    
    bool supportsInstancing;
    bool supportsVAO;
    int maxInstances;
    int viewportWidth, viewportHeight;
    Texture *emptyTexture;
    
    virtual void Initialize() = 0;
    
    virtual Vector2 getViewPort() = 0;
    virtual void changeViewport(int width, int height) = 0;
    virtual bool PrepareNode(shared_ptr<Node> node,int meshBufferIndex, bool isRTT, int nodeIndex = 0) = 0;
    virtual void Render(shared_ptr<Node> node, bool isRTT, int nodeIndex = 0, int meshBufferIndex = 0) = 0;
    virtual void setActiveCamera(shared_ptr<CameraNode> camera) = 0;
    virtual shared_ptr<CameraNode> getActiveCamera() = 0;
    virtual void BindUniform(Material* mat,shared_ptr<Node> node,u16 uIndex,bool isFragmentData, int userValue = 0, bool blurTex = true) = 0;
    virtual void draw3DLine(Vector3 start,Vector3 end,Material *material) = 0;
    virtual void draw3DLines(vector<Vector3> vPositions,Material *material) = 0;
    virtual void clearDepthBuffer() = 0;
    virtual void setTransparencyBlending(bool enable) = 0;
    virtual void draw2DImage(Texture *texture,Vector2 originCoord,Vector2 endCoord,bool isBGImage,Material *material,bool isRTT = false) = 0;
    virtual bool PrepareDisplay(int width,int height,bool clearColorBuf = true,bool clearDepthBuf = true,bool isDepthPass = false,Vector4 color = Vector4(255,255,255,255)) = 0;
    virtual Texture* createRenderTargetTexture(string textureName ,TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height) = 0;
    virtual void endDisplay() = 0;
    virtual void useMaterialToRender(Material *mat) = 0;
    virtual void bindDynamicUniform(Material *material,string name,void* values,DATA_TYPE type,unsigned short count,u16 paramIndex,int nodeIndex,Texture *tex, bool isFragmentData, bool blurTex = true) = 0;
    virtual void setRenderTarget(Texture *renderTexture,bool clearBackBuffer,bool clearZBuffer,bool isDepthPass,Vector4 color) = 0;
    virtual void writeImageToFile(Texture *texture , char* filePath , IMAGE_FLIP flipType = FLIP_HORIZONTAL) = 0;
    virtual Vector4 getPixelColor(Vector2 touchPos,Texture* texture) = 0;
    virtual void setUpDepthState(METAL_DEPTH_FUNCTION func,bool writeDepth = true,bool setToRenderBuffer = false) = 0;
    virtual void createVertexAndIndexBuffers(shared_ptr<Node> node,MESH_TYPE meshType = MESH_TYPE_LITE , bool updateBothBuffers = true) = 0;
    virtual void createVertexBuffer(shared_ptr<Node> node,short meshBufferIndex = 0,MESH_TYPE meshType = MESH_TYPE_LITE) = 0;
};
#endif /* defined(__SGEngine2__RenderManager__) */