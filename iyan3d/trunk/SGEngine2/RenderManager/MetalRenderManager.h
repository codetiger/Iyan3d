//
//  OGLES2RenderManager.h
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MetalRenderManager__
#define __SGEngine2__MetalRenderManager__

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>

#import "MetalWrapper.h"
#include "RenderManager.h"
#include "../Utilities/Logger.h"
#import "MetalHandler.h"
#import "../Core/Textures/MTLTexture.h"
#import "../Core/Material/MTLMaterial.h"
#import "RenderingView.h"

class MetalRenderManager:public RenderManager{
    
private:
    CAMetalLayer *_metalLayer;
    id <CAMetalDrawable> _currentDrawable;
    MTLRenderPassDescriptor *_renderPassDescriptor;
    MTLRenderPassDescriptor *_rttRenderPassDescriptor;
    
    // controller
    id <MTLBuffer> _dynamicConstantBuffer;
    
    // renderer
    id <MTLDevice> _device;
    id <MTLCommandQueue> _commandQueue;
    id <MTLLibrary> _defaultLibrary;
    id <MTLBuffer> vertexBuffer;
    id <MTLTexture> _depthTex;
    id <MTLTexture> _renderTargetDepthTex;

    id <MTLDepthStencilState> _depthState;

    id <MTLDepthStencilState> _generalDepthWriteEnableState;
    id <MTLDepthStencilState> _generalDepthWriteDisableState;

    
    float _rotation;
    id <MTLCommandBuffer> CMDBuffer;
    id <MTLRenderCommandEncoder> RenderCMDBuffer;
    id <MTLRenderCommandEncoder> rttRenderCMDBuffer;
        id <MTLRenderCommandEncoder> mainRenderCMDBuffer;
    id <CAMetalDrawable> drawable;
        
    dispatch_semaphore_t        _inflight_semaphore;
    MTKView *renderView;
    bool isCmdBufferCommited,isEncodingEnded;
    float screenWidth,screenHeight, screenScale;

    bool setupMetal();
    void setUpDepthState(METAL_DEPTH_FUNCTION func,bool writeDepth = true,bool setToRenderBuffer = false);
    void setupRenderPassDescriptorForTexture(id <MTLTexture> texture, Vector4 color = Vector4(0.0,0.0,0.0,1.0), bool isDepthPass = false);
    void freeDynamicBuffers();
    void endEncoding();
    void setupMetalLayer();
    int getTexturePixelFormat(TEXTURE_DATA_FORMAT format);
    int getAvailableBfferWithSize(int bufSize);
    id <CAMetalDrawable> currentDrawable();
    int getMTLDrawMode(DRAW_MODE mode);
    id<MTLTexture> getMSATexture(int sampleCount,int width,int height);
    RenderingView *mtlRenderView;

public:
    MetalHandler *mtlHandler;
    id <MTLTexture> renderingTexture;
    MetalRenderManager(void *renderView, float screenWidth, float screenHeight, float screenScale);
    ~MetalRenderManager();
   
    void Initialize();
    
    Vector2 getViewPort();
    void changeViewport(int width, int height);
    bool PrepareNode(shared_ptr<Node> node,int meshBufferIndex, bool isRTT, int nodeIndex = 0);
    void Render(shared_ptr<Node> node, bool isRTT, int nodeIndex = 0, int meshBufferIndex = 0);
    void drawPrimitives(MTLPrimitiveType primitiveType,unsigned int count,MTLIndexType type,id <MTLBuffer> indexBuf, GLsizei instanceCount);
    void BindUniform(Material* mat,shared_ptr<Node> node,u16 uIndex,bool isFragment , int userValue = 0, bool blurTex = true);
    void BindUniform(DATA_TYPE type,id<MTLBuffer> buf,void* value,int count,int parameterIndex,id<MTLTexture> tex,string matName,int nodeIndex, bool isFragmentData, bool blurTex = true);
    void BindAttribute(shared_ptr<Node> node,int nodeIndx);
    void (*NodePropertyCallBack)(shared_ptr<Node> node);
    void setActiveCamera(shared_ptr<CameraNode> camera);
    shared_ptr<CameraNode> getActiveCamera();

    void draw3DLine(Vector3 start,Vector3 end,Material *material);
    void draw3DLines(vector<Vector3> vPositions,Material *material);
    void draw2DImage(Texture *texture, Vector2 originCoord, Vector2 endCoord, Material *material, bool isRTT = false);

    void clearDepthBuffer();
    void setTransparencyBlending(bool enable);
    bool PrepareDisplay(int width,int height,bool clearColorBuf = true,bool clearDepthBuf = true,bool isDepthPass = false,Vector4 color = Vector4(255,255,255,255));
    void endDisplay();
    Texture* createRenderTargetTexture(string textureName ,TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height);
    void setFrameBufferObjects(u_int32_t framebuff , u_int32_t colorbuff , u_int32_t depthduff);
    void useMaterialToRender(Material *mat);
    void bindDynamicUniform(Material *material,string name,void* values,DATA_TYPE type,ushort count,u16 paramIndex,int nodeIndex,Texture *tex, bool isFragmentData, bool blurTex = true);
    void setRenderTarget(Texture *renderTexture,bool clearBackBuffer,bool clearZBuffer,bool isDepthPass = false,Vector4 color = Vector4(1.0,1.0,1.0,1.0));
    Vector4 getPixelColor(Vector2 touchPosition,Texture* texture);
    void createVertexAndIndexBuffers(shared_ptr<Node> node,MESH_TYPE meshType = MESH_TYPE_LITE , bool updateBothBuffers = true);
    void createVertexBuffer(shared_ptr<Node> node,short meshBufferIndex = 0,MESH_TYPE meshType = MESH_TYPE_LITE);
    Material* LoadMetalShaders(string vertexFuncName,string fragFuncName,bool isDepthPass = false);
    void writeImageToFile(Texture *texture , char* filePath,IMAGE_FLIP flipType = NO_FLIP);

    void resetTextureCache();
};

#endif /* defined(__SGEngine2__MetalRenderManager__) */
