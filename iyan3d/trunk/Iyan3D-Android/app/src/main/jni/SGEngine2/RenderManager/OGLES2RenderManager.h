//
//  OGLES2RenderManager.h
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__OGLES2RenderManager__
#define __SGEngine2__OGLES2RenderManager__

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#include "RenderManager.h"

#include "../Utilities/Logger.h"
#include "../Core/Nodes/OGLNodeData.h"
#include "../Core/Textures/OGLTexture.h"
#include "../Core/Material/OGLMaterial.h"
#include "../Core/Nodes/AnimatedMeshNode.h"
#include "../Utilities/Helper.h"

class OGLES2RenderManager : public RenderManager {
private:
    uint32_t currentMaterial;
    uint32_t currentTextures[32];
    int currentTextureIndex;
    bool currentDepthMask;
    GLenum currentDepthFunction, currentBlendFunction;
    
    void deleteAndUnbindBuffer(GLenum target,GLsizei size,const GLuint *bufferToDelete);
    void resetToMainBuffers();
    u_int32_t bindIndexBuffer(shared_ptr<Node> node, int meshBufferIndex);
    u_int32_t createAndBindBuffer(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage);
    u_int32_t updateBuffer(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage , u_int32_t _bufferToBind);
    GLenum getOGLDrawMode(DRAW_MODE mode);
public:
   
    int c;
    float screenWidth,screenHeight,screenScale;
    GLuint frameBuffer;
    GLuint colorBuffer;
    GLuint depthBuffer;
    std::map< shared_ptr<Node>, u_int32_t > shaderPrograms;
    
    OGLES2RenderManager(float screenWidth, float screenHeight, float screenScale);
    ~OGLES2RenderManager();
    #ifdef ANDROID
    void initialiseOtherVAOFunc ();
    #endif
    Vector4 clearColor;
    GLenum depthState;
    GLenum dFactor;
    bool isDepthTestEnabled;

    Vector2 getViewPort();
    void changeViewport(int width, int height);
    void changeClearColor(Vector4 lClearColor);
    
    void Initialize();
    bool PrepareNode(shared_ptr<Node> node, int meshBufferIndex, bool isRTT, int nodeIndex = 0);
    void bindBufferAndAttributes(shared_ptr<Node> node, int meshBufferIndex, MESH_TYPE meshType);
    void endDisplay();
    void Render(shared_ptr<Node> node, bool isRTT, int nodeIndex = 0, int meshBufferIndex = 0);
    void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *data, GLsizei instanceCount);
    void BindUniform(Material* mat, shared_ptr<Node> node, u16 uIndex, bool isFragment, int userValue = 0, bool blurTex = true);
    void (*NodePropertyCallBack)(shared_ptr<Node> node);
    void setActiveCamera(shared_ptr<CameraNode> camera);
    void BindAttributes(Material *material, MESH_TYPE meshType = MESH_TYPE_LITE);
    void UnBindAttributes(Material *material);
    void useMaterialToRender(Material *material);
    bool bindTexture(int index, uint32_t texture);
    
    void draw3DLine(Vector3 start, Vector3 end, Material *material);
    void draw3DLines(vector<Vector3> vPositions, Material *material);
    void draw2DImage(Texture *texture, Vector2 originCoord, Vector2 endCoord, Material *material, bool isRTT = false);

    void clearDepthBuffer();
    void setTransparencyBlending(bool enable);
    shared_ptr<CameraNode> getActiveCamera();
    bool PrepareDisplay(int width, int height, bool clearColorBuf = true, bool clearDepthBuf = true, bool isDepthPass = false, Vector4 color = Vector4(255, 255, 255, 255));
    Texture* createRenderTargetTexture(string textureName, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height);
    void setFrameBufferObjects(u_int32_t framebuff , u_int32_t colorbuff , u_int32_t depthduff);
    void setRenderTarget(Texture* renderTexture, bool clearBackBuffer = true, bool clearZBuffer = true, bool isDepthPass = false, Vector4 color = Vector4(255, 255, 255, 255));
    void writeImageToFile(Texture *texture , char* filePath, IMAGE_FLIP flipType = NO_FLIP);
    void bindDynamicUniform(Material *material, string name, void* values, DATA_TYPE type, unsigned short count, u16 paramIndex, int nodeIndex, Texture *tex, bool isFragmentData, bool blurTex = true);
    Vector4 getPixelColor(Vector2 touchPos,Texture* texture);
    
    void setUpDepthState(METAL_DEPTH_FUNCTION func, bool writeDepth = true, bool clearDepthBuffer = false);

    void createVertexAndIndexBuffers(shared_ptr<Node> node, MESH_TYPE meshType = MESH_TYPE_LITE, bool updateBothBuffers = true);
    void handleVAO(shared_ptr<Node> node, int type, short meshBufferIndex = 0, MESH_TYPE meshType = MESH_TYPE_LITE);
    void createVertexBuffer(shared_ptr<Node> node, short meshBufferIndex = 0, MESH_TYPE meshType = MESH_TYPE_LITE);
    
    void createVAO(shared_ptr<Node> node, short meshBufferIndex = 0);
    void updateVAO(shared_ptr<Node> node, bool updateIndices, bool updateAttr, short meshBufferIndex = 0);

    void setDepthMask(bool enable);
    void setDepthFunction(GLenum func);
    void blendFunction(GLenum func);
    
    void resetTextureCache();
};

#endif /* defined(__SGEngine2__OGLES2RenderManager__) */
