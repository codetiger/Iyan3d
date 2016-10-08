
//
//  OGLES2RenderManager.cpp
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include <stdint.h>
#include "../Core/Nodes/ParticleManager.h"
#include "OGLES2RenderManager.h"
#ifdef IOS
#include <OpenGLES/ES2/glext.h>
#endif
#ifdef ANDROID
//#include "../../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-arm/usr/include/stdint.h"
#include "../../opengl.h"
#include <EGL/egl.h>

PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;
PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT;

void OGLES2RenderManager::initialiseOtherVAOFunc ()
{
    glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress ( "glGenVertexArraysOES" );
    glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress ( "glBindVertexArrayOES" );
    glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress ( "glIsVertexArrayOES" );
    if(glGenVertexArraysOES == NULL){
        Logger::log(INFO, "OGLRenderManager", "glGenVertexArraysOES is null");
    }
}

#endif

OGLES2RenderManager::OGLES2RenderManager(float screenWidth,float screenHeight,float screenScale)
{
    c = 0;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->screenScale = screenScale;
    supportsVAO = false;
    depthBuffer = colorBuffer = frameBuffer = 0;
    shaderPrograms.clear();
    Initialize();

    #ifdef ANDROID
        glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)eglGetProcAddress("glDiscardFramebufferEXT");
    #endif

    resetTextureCache();
}

OGLES2RenderManager::~OGLES2RenderManager()
{
    glDeleteFramebuffers(1,&depthBuffer);
    glDeleteFramebuffers(1,&colorBuffer);
    glDeleteFramebuffers(1,&frameBuffer);
    
    if(this->camera)
        this->camera.reset();
}

void OGLES2RenderManager::Initialize()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
}

Vector2 OGLES2RenderManager::getViewPort()
{
    return Vector2(viewportWidth, viewportHeight);
}

void OGLES2RenderManager::changeViewport(int width, int height)
{
    if(viewportWidth != width || viewportHeight != height) {
        viewportHeight = height;
        viewportWidth = width;
        glViewport(0, 0, viewportWidth, viewportHeight);
    }
}

void OGLES2RenderManager::changeClearColor(Vector4 lClearColor)
{
    if(clearColor.x != lClearColor.x || clearColor.y != lClearColor.y || clearColor.z != lClearColor.z || clearColor.w != lClearColor.w) {
        clearColor = lClearColor;
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    }
}

bool OGLES2RenderManager::PrepareNode(shared_ptr<Node> node, int meshBufferIndex, bool isRTT, int nodeIndex)
{
    if(node->type <= NODE_TYPE_CAMERA || node->type == NODE_TYPE_INSTANCED)
        return false;
    
    if(node->instancedNodes.size() > 0 && !supportsInstancing && node->shouldUpdateMesh) {
        shared_ptr<OGLNodeData> nData = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
        while(node->nodeData.use_count() > 0)
            node->nodeData.reset();
        node->nodeData = make_shared<OGLNodeData>();
    }
    
    OGLMaterial *material = (OGLMaterial*)node->material;
    Mesh *mesh = (node->instancedNodes.size() > 0 && !supportsInstancing) ? dynamic_pointer_cast<MeshNode>(node)->meshCache : dynamic_pointer_cast<MeshNode>(node)->getMesh();
    
    bool bindAttrib = false;
    if(shaderPrograms.find(node) == shaderPrograms.end()) {
        if(meshBufferIndex == mesh->getMeshBufferCount()-1)
            shaderPrograms.insert(std::pair< shared_ptr<Node>, u_int32_t >(node, material->shaderProgram));
    } else if(shaderPrograms[node] != material->shaderProgram) {
        bindAttrib = true;
        if(meshBufferIndex == mesh->getMeshBufferCount()-1)
            shaderPrograms[node] = material->shaderProgram;
    }
    
    useMaterialToRender(material);
    if(!supportsVAO) {
        MESH_TYPE mType = mesh->meshType;
        if(node->shouldUpdateMesh) {
            createVertexAndIndexBuffers(node, mType, true);
        }
        bindBufferAndAttributes(node, meshBufferIndex, mType);
    } else if((node->shouldUpdateMesh && dynamic_pointer_cast<OGLNodeData>(node->nodeData)->VAOCreated) || bindAttrib) {
        updateVAO(node, true, bindAttrib, meshBufferIndex);
    } else
        createVAO(node, meshBufferIndex);
    
    return true;
}

void OGLES2RenderManager::createVAO(shared_ptr<Node> node, short meshBufferIndex)
{
    shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    MESH_TYPE mType = (node->instancedNodes.size() > 0 && !supportsInstancing) ? dynamic_pointer_cast<MeshNode>(node)->meshCache->meshType : dynamic_pointer_cast<MeshNode>(node)->getMesh()->meshType;
    Mesh* mesh = (node->instancedNodes.size() > 0 && !supportsInstancing) ? dynamic_pointer_cast<MeshNode>(node)->meshCache : dynamic_pointer_cast<MeshNode>(node)->getMesh();
    
    if(!OGLNode->VAOCreated) {
        if(meshBufferIndex == mesh->getMeshBufferCount()-1) {
            OGLNode->VAOCreated = true;
        }
        
        handleVAO(node, 1 , meshBufferIndex, mType);
        createVertexBuffer(node, meshBufferIndex, mType);
        
        BindAttributes(node->material, mType);
        
        if(OGLNode->IndexBufLocations.size() > meshBufferIndex) {
            size_t indexCount = mesh->getIndicesCount(meshBufferIndex);
            GLsizeiptr size = sizeof(unsigned short) * indexCount;
            u_int32_t indexBuf = updateBuffer(GL_ELEMENT_ARRAY_BUFFER, size,  mesh->getIndicesArray(meshBufferIndex), node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, OGLNode->IndexBufLocations[meshBufferIndex]);
            std::replace(OGLNode->IndexBufLocations.begin(), OGLNode->IndexBufLocations.end(), OGLNode->IndexBufLocations[meshBufferIndex], indexBuf);
        } else
            OGLNode->IndexBufLocations.push_back(bindIndexBuffer(node,meshBufferIndex));
        
        handleVAO(node, 3);
    }
}

void OGLES2RenderManager::updateVAO(shared_ptr<Node> node, bool updateIndices, bool bindAttrib, short meshBufferIndex)
{
    shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    Mesh *mesh = (node->instancedNodes.size() > 0 && !supportsInstancing) ? dynamic_pointer_cast<MeshNode>(node)->meshCache : dynamic_pointer_cast<MeshNode>(node)->getMesh();
    MESH_TYPE mType = mesh->meshType;
    
    handleVAO(node, 2 , meshBufferIndex, mType);
    if(bindAttrib) {
        bindBufferAndAttributes(node, meshBufferIndex, mType);
    } else {
        createVertexBuffer(node, meshBufferIndex, mType);
        
        BindAttributes(node->material, mType);
        
        if(updateIndices) {
            if(OGLNode->IndexBufLocations.size() > meshBufferIndex) {
                size_t indexCount =  mesh->getIndicesCount(meshBufferIndex);
                GLsizeiptr size = sizeof(unsigned short) * indexCount;
                u_int32_t indexBuf = updateBuffer(GL_ELEMENT_ARRAY_BUFFER, size,  mesh->getIndicesArray(meshBufferIndex), node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, OGLNode->IndexBufLocations[meshBufferIndex]);
                std::replace(OGLNode->IndexBufLocations.begin(), OGLNode->IndexBufLocations.end(), OGLNode->IndexBufLocations[meshBufferIndex], indexBuf);
            } else
                OGLNode->IndexBufLocations.push_back(bindIndexBuffer(node,meshBufferIndex));
        }
    }
    
    handleVAO(node, 3);
}

void OGLES2RenderManager::bindBufferAndAttributes(shared_ptr<Node> node, int meshBufferIndex, MESH_TYPE meshType)
{
    shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLNode->vertexBufLocations[meshBufferIndex]);
    BindAttributes(node->material,meshType);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OGLNode->IndexBufLocations[meshBufferIndex]);
}

void OGLES2RenderManager::endDisplay()
{
    
}

void OGLES2RenderManager::Render(shared_ptr<Node> node, bool isRTT, int nodeIndex, int meshBufferIndex)
{
    if(!node || node->type <= NODE_TYPE_CAMERA)
        return;

    Mesh *nodeMes;
    if(node->type == NODE_TYPE_SKINNED) {
        if(node->skinType == GPU_SKIN)
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
        else
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
    } else if (node->instancedNodes.size() > 0 && !supportsInstancing)
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->meshCache;
    else
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();
    
    if(nodeMes == NULL)
        return;
    
    if(supportsVAO)
        handleVAO(node, 2, meshBufferIndex);
    
    GLenum indicesDataType = GL_UNSIGNED_SHORT;
    if (node->type == NODE_TYPE_PARTICLES) {
        if(!isRTT) {
            blendFunction(GL_ONE);
            setDepthMask(false);
        }
        unsigned int indicesSize = nodeMes->getIndicesCount(0);
        shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
        if(nodeMes)
            drawElements(getOGLDrawMode(DRAW_MODE_POINTS), (GLsizei)indicesSize, indicesDataType, 0, 0);
        
        if(!isRTT) {
            setDepthMask(true);
            blendFunction(GL_ONE_MINUS_SRC_ALPHA);
        }
    } else {
        int instancingCount = (node->instancedNodes.size() == 0) ? 0 : (node->instancingRenderIt + maxInstances > (int)node->instancedNodes.size()) ? ((int)node->instancedNodes.size() - node->instancingRenderIt) : maxInstances;
        
        drawElements(getOGLDrawMode(node->drawMode), (GLsizei)nodeMes->getIndicesCount(meshBufferIndex), indicesDataType, 0, !supportsInstancing ? 0 : (GLsizei)instancingCount+1);
    }
    
    if(supportsVAO)
        handleVAO(node, 3);
    else {
        UnBindAttributes(node->material);
    }
}

void OGLES2RenderManager::drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *data, GLsizei instanceCount)
{
#ifdef IOS
    if(instanceCount > 1)
        glDrawElementsInstancedEXT(mode, count, type, data, instanceCount);
    else
#endif
        glDrawElements(mode, count, type, data);
}

void OGLES2RenderManager::BindAttributes(Material *material, MESH_TYPE meshType)
{
    int valueIndex = 0;
    OGLMaterial *mat = (OGLMaterial*)material;
    if (meshType == MESH_TYPE_LITE) {
        for(int i = 0; i < MAX_VERTEX_DATA;i++) {
            GLenum type = Helper::getOGLES2DataType(attributesType[i]);
            int attributeIndex = mat->getMaterialAttribIndexByName(attributesName[i]);
            if(attributeIndex != NOT_EXISTS) {
                glVertexAttribPointer((GLuint)mat->attributes[attributeIndex].location, attributesTotalValues[i], type, GL_FALSE, sizeof(vertexData), (GLvoid*)valueIndex);
                glEnableVertexAttribArray((GLuint)mat->attributes[attributeIndex].location);
            }
            valueIndex += sizeof(type) * attributesTotalValues[i];
        }
    } else {
        for(int i = 0; i < MAX_VERTEX_DATA_SKINNED;i++) {
            GLenum type = Helper::getOGLES2DataType(attributesTypeSkinned[i]);
            int attributeIndex = mat->getMaterialAttribIndexByName(attributesNameSkinned[i]);
            if(attributeIndex != NOT_EXISTS){
                glVertexAttribPointer((GLuint)mat->attributes[attributeIndex].location, attributesTotalValuesSkinned[i], type, GL_FALSE, sizeof(vertexDataHeavy), (GLvoid*)valueIndex);
                glEnableVertexAttribArray((GLuint)mat->attributes[attributeIndex].location);
            }
            valueIndex += sizeof(type) * attributesTotalValuesSkinned[i];
        }
    }
}

void OGLES2RenderManager::UnBindAttributes(Material *material)
{
    OGLMaterial *mat = (OGLMaterial*)material;
    for(int i = 0; i < mat->attributes.size();i++)
        glDisableVertexAttribArray((GLuint)mat->attributes[i].location);
}

void OGLES2RenderManager::useMaterialToRender(Material *material)
{
    if(currentMaterial != ((OGLMaterial*)material)->shaderProgram) {
        glUseProgram(((OGLMaterial*)material)->shaderProgram);
        currentMaterial = ((OGLMaterial*)material)->shaderProgram;
    }
}

bool OGLES2RenderManager::bindTexture(int index, uint32_t texture)
{
    if(currentTextures[index] != texture || currentTextureIndex == -1) {
        if(currentTextureIndex != index) {
            glActiveTexture(GL_TEXTURE0 + index);
            currentTextureIndex = index;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        currentTextures[index] = texture;
        return true;
    }
    return false;
}

void OGLES2RenderManager::BindUniform(Material* mat, shared_ptr<Node> node, u16 uIndex, bool isFragmentData, int userValue, bool blurTex)
{
    OGLUniform uni = ((OGLMaterial*)mat)->uniforms[uIndex];

    if(uni.type != DATA_TEXTURE_2D && uni.type != DATA_TEXTURE_CUBE && !uni.isUpdated)
        return;
    
    switch (uni.type) {
        case DATA_FLOAT:
            glUniform1fv(uni.location,uni.count,(float*)uni.values);
            break;
        case DATA_FLOAT_VEC2:
            glUniform2fv(uni.location, uni.count/2,(float*)uni.values);
            break;
        case DATA_FLOAT_VEC3:
            glUniform3fv(uni.location, uni.count/3,(float*)uni.values);
            break;
        case DATA_FLOAT_VEC4:
            glUniform4fv(uni.location, uni.count/4,(float*)uni.values);
            break;
        case DATA_FLOAT_MAT2:
            glUniformMatrix2fv(uni.location, uni.count/4, false,(float*)uni.values);
            break;
        case DATA_FLOAT_MAT3:
            glUniformMatrix3fv(uni.location,uni.count/9, false,(float*)uni.values);
            break;
        case DATA_FLOAT_MAT4:{
            glUniformMatrix4fv(uni.location,uni.count/16, false,(float*)uni.values);
            break;
        }
        case DATA_INTEGER:{
            glUniform1iv(uni.location,uni.count,(int*)uni.values);
            break;
        }
        case DATA_TEXTURE_2D:
        case DATA_TEXTURE_CUBE:{
            u_int32_t *textureName = (u_int32_t*)uni.values;
            if(bindTexture(userValue, *textureName))
                glUniform1i(uni.location, userValue);
        }
            break;
        default:
            Logger::log(ERROR, "OGLES2RenderManager", "Uniform: " + uni.name + " Type Not Matched");
            break;
    }
//    GLenum err = GL_NO_ERROR;
//    while((err = glGetError()) != GL_NO_ERROR)
//        printf("GL Error on unfirm: %d\n", err);

    ((OGLMaterial*)mat)->uniforms[uIndex].isUpdated = false;
}

void OGLES2RenderManager::bindDynamicUniform(Material *material,string name,void* values,DATA_TYPE type,unsigned short count,u16 paramIndex,int nodeIndex,Texture *tex, bool isFragmentData, bool blurTex)
{
    
}

void OGLES2RenderManager::setActiveCamera(shared_ptr<CameraNode> camera)
{
    this->camera = camera;
    this->camera->setActive(true);
}

shared_ptr<CameraNode> OGLES2RenderManager::getActiveCamera()
{
    return camera;
}

void OGLES2RenderManager::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OGLES2RenderManager::setTransparencyBlending(bool enable)
{
    if(enable) {
        glEnable(GL_BLEND);
        blendFunction(GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void OGLES2RenderManager::blendFunction(GLenum func)
{
    if(currentBlendFunction != func) {
        glBlendFunc(GL_SRC_ALPHA, func);
        currentBlendFunction = func;
    }
}

void OGLES2RenderManager::draw2DImage(Texture *texture, Vector2 originCoord, Vector2 endCoord, Material *material, bool isRTT)
{
    // to flip horizontally for opengl textures
    setDepthFunction(GL_ALWAYS);
    Vector2 bottomRight = Helper::screenToOpenglCoords(originCoord, (float)screenWidth * screenScale, (float)screenHeight * screenScale);
    Vector2 upperLeft = Helper::screenToOpenglCoords(endCoord, (float)screenWidth * screenScale, (float)screenHeight * screenScale);
    vector<Vector3> vertPosition;
    vertPosition.push_back(Vector3(upperLeft.x,upperLeft.y,0.0));
    vertPosition.push_back(Vector3(upperLeft.x,bottomRight.y,0.0));
    vertPosition.push_back(Vector3(bottomRight.x,bottomRight.y,0.0));
    vertPosition.push_back(Vector3(bottomRight.x,upperLeft.y,0.0));
    
    vector<Vector2> texCoord1;
    texCoord1.push_back(Vector2(1.0,0.0));
    texCoord1.push_back(Vector2(1.0,1.0));
    texCoord1.push_back(Vector2(0.0,1.0));
    texCoord1.push_back(Vector2(0.0,0.0));
    
    vector<vertexData> vertices;
    for(int i = 0; i < vertPosition.size();i++){
        vertexData vData;
        vData.vertPosition = vertPosition[i];
        vData.texCoord1 = texCoord1[i];
        vertices.push_back(vData);
    }
    vertPosition.clear(); texCoord1.clear();
    u16 indices[6] = {
        0,3,1,
        2,1,3
    };
    u_int32_t _vertexBuffer = createAndBindBuffer(GL_ARRAY_BUFFER, (GLuint)(vertices.size() * sizeof(vertexData)), &vertices[0], GL_STATIC_DRAW);
    u_int32_t _indexBuffer = createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (6 * sizeof(unsigned short)), &indices[0], GL_STATIC_DRAW);
    BindAttributes(material);
    drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, 0);
    UnBindAttributes(material);
    deleteAndUnbindBuffer(GL_ARRAY_BUFFER, 1, &_vertexBuffer);
    deleteAndUnbindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1, &_indexBuffer);
}

void OGLES2RenderManager::draw3DLine(Vector3 start,Vector3 end,Material *material)
{
    vertexData vertData[2];
    vertData[0].vertPosition = start;
    vertData[1].vertPosition = end;
    u16 indices[] = {0, 1};
    
    u_int32_t _vertexBuffer = createAndBindBuffer(GL_ARRAY_BUFFER, (GLuint)(2 * sizeof(vertexData)), &vertData[0], GL_STATIC_DRAW);
    u_int32_t _indexBuffer = createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (2 * sizeof(unsigned short)), &indices[0], GL_STATIC_DRAW);
    BindAttributes(material);
    drawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, 0, 0);
    UnBindAttributes(material);
    deleteAndUnbindBuffer(GL_ARRAY_BUFFER, 1, &_vertexBuffer);
    deleteAndUnbindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1, &_indexBuffer);
}

void OGLES2RenderManager::draw3DLines(vector<Vector3> vPositions,Material *material)
{
    vector< vertexData > vertData;
    vector< u16 > indices;
    
    for(int i = 0; i < (int)vPositions.size(); i++) {
        vertexData v;
        v.vertPosition = vPositions[i];
        vertData.push_back(v);
        indices.push_back(i);
    }
    
    u_int32_t _vertexBuffer = createAndBindBuffer(GL_ARRAY_BUFFER, (GLuint)((int)vPositions.size() *sizeof(vertexData)),&vertData[0], GL_STATIC_DRAW);
    u_int32_t _indexBuffer = createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((int)vPositions.size() * sizeof(unsigned short)) ,&indices[0], GL_STATIC_DRAW);
    BindAttributes(material);
    drawElements(GL_LINES, (int)vPositions.size(), GL_UNSIGNED_SHORT, 0, 0);
    UnBindAttributes(material);
    deleteAndUnbindBuffer(GL_ARRAY_BUFFER, 1, &_vertexBuffer);
    deleteAndUnbindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1, &_indexBuffer);
    
    vertData.clear();
    indices.clear();
}

bool OGLES2RenderManager::PrepareDisplay(int width, int height, bool clearColorBuf, bool clearDepthBuf, bool isDepthPass, Vector4 color)
{
    changeViewport(width, height);
    changeClearColor(color);
    
    GLbitfield mask = 0;
    if(clearColorBuf)
        mask |= GL_COLOR_BUFFER_BIT;
    if(clearDepthBuf)
        mask |= GL_DEPTH_BUFFER_BIT;
    
    const GLenum discards[]  = {GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0};
    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 2, discards);
    setDepthMask(true);
    glClear(mask);
    
    GLenum func = (!isDepthPass) ? GL_LEQUAL : GL_LESS;
    setDepthFunction(func);
    return true;
}

Texture* OGLES2RenderManager::createRenderTargetTexture(string textureName ,TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height)
{
    Texture *newTex = new OGLTexture();
    newTex->createRenderTargetTexture(textureName, format, texelType, width, height);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status == GL_FRAMEBUFFER_COMPLETE)
    {
    }else{
        Logger::log(ERROR,"OGLES2RenderManager::createRenderTargetTexture", "FrameBufferIncomplete");
    }
    return newTex;
}

void OGLES2RenderManager::setRenderTarget(Texture *renderTexture, bool clearBackBuffer, bool clearZBuffer, bool isDepthPass, Vector4 color)
{
    if(renderTexture){
        glBindFramebuffer(GL_FRAMEBUFFER,((OGLTexture*)renderTexture)->rttFrameBuffer);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ((OGLTexture*)renderTexture)->OGLTextureName);
        currentTextures[0] = -1;
        currentTextureIndex = -1;
        PrepareDisplay(renderTexture->width, renderTexture->height, clearBackBuffer, clearZBuffer, isDepthPass, color);
    }
    else {
        currentTextures[0] = -1;
        currentTextureIndex = -1;
        resetToMainBuffers();
    }
}

void OGLES2RenderManager::writeImageToFile(Texture *texture, char *filePath, IMAGE_FLIP flipType)
{
    int bytesPerPix = 4;
    size_t imageSize = bytesPerPix * size_t(texture->width) * size_t(texture->height);
    uint8_t * buffer = new uint8_t[imageSize];
    glReadPixels(0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    uint8_t *imageData = new uint8_t[imageSize];
    
    if(flipType == FLIP_VERTICAL) {
        for (int j = 0; j * 2 < texture->height; ++j) {
            int x = j * texture->width * 4;
            int y = (texture->height - 1 - j) * texture->width * 4;
            for (int i = texture->width * 4; i > 0; --i) {
                uint8_t tmp = buffer[x];
                buffer[x] = buffer[y];
                buffer[y] = tmp;
                ++x;
                ++y;
            }
        }
    }
    else if(flipType == FLIP_HORIZONTAL){
        int bytesperRow = (texture->width * bytesPerPix);
        int pixelsPerRow = (texture->width);
        for(int r = 0; r < texture->height; r++)
        {
            for(int c = 0; c < (pixelsPerRow)/2; c++)
            {
                int firstByte = (r * bytesperRow);
                int lastByte = (r * bytesperRow) + (bytesperRow - 1);
                for(int b = 0; b < bytesPerPix;b++){
                    int curByte = (c * bytesPerPix) + b;
                    int reversByte = (lastByte - (c * bytesPerPix));
                    imageData[firstByte + curByte] = buffer[reversByte - (bytesPerPix - (b + 1))];
                    imageData[reversByte - (bytesPerPix - (b + 1))] = buffer[firstByte + curByte];
                }
            }
        }
    }
    
#ifndef IOS
    PNGFileManager::write_png_file(filePath, buffer, texture->width, texture->height);
#else
    writePNGImage(buffer,texture->width,texture->height,filePath);
#endif
    
    delete[] buffer;
    delete[] imageData;
}

Vector4 OGLES2RenderManager::getPixelColor(Vector2 touchPosition, Texture *texture)
{
    float mid = texture->height / 2.0;
    float difFromMid = touchPosition.y - mid;
    glBindTexture(GL_TEXTURE_2D, ((OGLTexture*)texture)->OGLTextureName);
    currentTextureIndex = -1;
    GLubyte pixelColor[4];
    glReadPixels((int)touchPosition.x, (int)(mid - difFromMid), 1, 1, GL_RGBA,GL_UNSIGNED_BYTE, &pixelColor[0]);
    return Vector4((float)pixelColor[0], (float)pixelColor[1], (float)pixelColor[2], (float)pixelColor[3]);
}

void OGLES2RenderManager::setFrameBufferObjects(u_int32_t framebuff, u_int32_t colorbuff, u_int32_t depthduff)
{
    frameBuffer = framebuff;
    colorBuffer = colorbuff;
    depthBuffer = depthduff;
}

void OGLES2RenderManager::setUpDepthState(METAL_DEPTH_FUNCTION func, bool writeDepth, bool clearDepthBuffer)
{
    if(clearDepthBuffer)
        glClear(GL_DEPTH_BUFFER_BIT);
}

void OGLES2RenderManager::createVertexAndIndexBuffers(shared_ptr<Node> node,MESH_TYPE meshType , bool updateBothBuffers)
{
    if(node->type <= NODE_TYPE_CAMERA || node->type == NODE_TYPE_INSTANCED)
        return;
    
    shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    
    u16 meshBufferCount = 1;
    if (node->instancedNodes.size() > 0 && !supportsInstancing)
        meshBufferCount = (dynamic_pointer_cast<MeshNode>(node))->meshCache->getMeshBufferCount();
    else
        meshBufferCount = (dynamic_pointer_cast<MeshNode>(node))->getMesh()->getMeshBufferCount();
    
    for(int i = 0; i < meshBufferCount;i++) {
        createVertexBuffer(node,i,meshType);
        if(updateBothBuffers) {
            Mesh* mesh = (node->instancedNodes.size() > 0 && !supportsInstancing) ? (dynamic_pointer_cast<MeshNode>(node))->meshCache : (dynamic_pointer_cast<MeshNode>(node))->getMesh();
            
            if(OGLNode->IndexBufLocations.size() > i) {
                size_t indexCount =  mesh->getIndicesCount(i);
                GLsizeiptr size = sizeof(unsigned short) * indexCount;
                u_int32_t indexBuf = updateBuffer(GL_ELEMENT_ARRAY_BUFFER, size,  mesh->getIndicesArray(i), node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, OGLNode->IndexBufLocations[i]);
                std::replace(OGLNode->IndexBufLocations.begin(), OGLNode->IndexBufLocations.end(), OGLNode->IndexBufLocations[i], indexBuf);
            } else
                OGLNode->IndexBufLocations.push_back(bindIndexBuffer(node,i));
        }
    }
}

void OGLES2RenderManager::handleVAO(shared_ptr<Node> node,int type, short meshBufferIndex,MESH_TYPE meshType)
{
    shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    if(type == 1) {
        uint32_t arrayToBind;
        glGenVertexArraysOES(1, &arrayToBind);
        glBindVertexArrayOES(arrayToBind);
        if(OGLNode->vertexArrayLocations.size() > meshBufferIndex) {
            std::replace(OGLNode->vertexArrayLocations.begin(), OGLNode->vertexArrayLocations.end(), OGLNode->vertexArrayLocations[meshBufferIndex], arrayToBind);
        } else
            OGLNode->vertexArrayLocations.push_back(arrayToBind);
    } else if (type == 2) {
        if(OGLNode->vertexArrayLocations.size() <= meshBufferIndex)
            handleVAO(node, 1, meshBufferIndex, meshType);
        else
            glBindVertexArrayOES(OGLNode->vertexArrayLocations[meshBufferIndex]);
    } else
        glBindVertexArrayOES(0);
}

void OGLES2RenderManager::createVertexBuffer(shared_ptr<Node> node, short meshBufferIndex, MESH_TYPE meshType)
{
    Mesh *nodeMes;
    if(node->type == NODE_TYPE_SKINNED) {
        if(node->skinType == GPU_SKIN)
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
        else {
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
            meshType = MESH_TYPE_LITE;
        }
    } else if(node->instancedNodes.size() > 0 && !supportsInstancing) {
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->meshCache;
    } else
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();
    
    GLuint size = (GLuint)nodeMes->getVerticesCountInMeshBuffer(meshBufferIndex) * ((meshType == MESH_TYPE_LITE) ? sizeof(vertexData) : sizeof(vertexDataHeavy));
    unsigned int vertexBufLoc = 0;
    
    shared_ptr<OGLNodeData> nData = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    if(meshType == MESH_TYPE_LITE){
        if(nData->vertexBufLocations.size() > meshBufferIndex) {
            vertexBufLoc = updateBuffer(GL_ARRAY_BUFFER, size, &(nodeMes->getLiteVerticesArray(meshBufferIndex))[0], node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, nData->vertexBufLocations[meshBufferIndex]);
            std::replace(nData->vertexBufLocations.begin(), nData->vertexBufLocations.end(), nData->vertexBufLocations[meshBufferIndex], vertexBufLoc);
        } else {
            vertexBufLoc = createAndBindBuffer(GL_ARRAY_BUFFER, size, &(nodeMes->getLiteVerticesArray(meshBufferIndex))[0], node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
            nData->vertexBufLocations.push_back(vertexBufLoc);
        }
    }else{
        if(nData->vertexBufLocations.size() > meshBufferIndex) {
            vertexBufLoc = updateBuffer(GL_ARRAY_BUFFER, size, &(nodeMes->getHeavyVerticesArray(meshBufferIndex))[0], node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, nData->vertexBufLocations[meshBufferIndex]);
            std::replace(nData->vertexBufLocations.begin(), nData->vertexBufLocations.end(), nData->vertexBufLocations[meshBufferIndex], vertexBufLoc);
        } else {
            vertexBufLoc = createAndBindBuffer(GL_ARRAY_BUFFER, size, &(nodeMes->getHeavyVerticesArray(meshBufferIndex))[0], node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
            nData->vertexBufLocations.push_back(vertexBufLoc);
        }
    }
}

u_int32_t OGLES2RenderManager::bindIndexBuffer(shared_ptr<Node> node, int meshBufferIndex)
{
    Mesh *nodeMes;
    if(node->type == NODE_TYPE_SKINNED) {
        if(node->skinType == GPU_SKIN)
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
        else
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
    } else if(node->instancedNodes.size() > 0 && !supportsInstancing)
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->meshCache;
    else
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();
    
    size_t indexCount =  nodeMes->getIndicesCount(meshBufferIndex);
    GLsizeiptr size;
    shared_ptr<OGLNodeData> nData = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    size = sizeof(unsigned short) * indexCount;
    if(nData->IndexBufLocations.size() > meshBufferIndex)
        return updateBuffer(GL_ELEMENT_ARRAY_BUFFER, size, nodeMes->getIndicesArray(meshBufferIndex), node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, nData->IndexBufLocations[meshBufferIndex]);
    else
        return createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER , size , nodeMes->getIndicesArray(meshBufferIndex), node->memtype == NODE_GPUMEM_TYPE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

u_int32_t OGLES2RenderManager::createAndBindBuffer(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage)
{
    u_int32_t _bufferToBind;
    glGenBuffers(1, &_bufferToBind);
    glBindBuffer(target, _bufferToBind);
    glBufferData(target,size, data, usage);
    return _bufferToBind;
}

u_int32_t OGLES2RenderManager::updateBuffer(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage , u_int32_t _bufferToBind)
{
    
    glBindBuffer(target, _bufferToBind);
    glBufferData(target,size, data, usage);
    return _bufferToBind;
}

void OGLES2RenderManager::deleteAndUnbindBuffer(GLenum target,GLsizei size,const GLuint *bufferToDelete)
{
    glDeleteBuffers(size, bufferToDelete);
    glBindBuffer(target, 0);
}

void OGLES2RenderManager::resetToMainBuffers()
{
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
}

GLenum OGLES2RenderManager::getOGLDrawMode(DRAW_MODE mode)
{
    switch(mode){
        case DRAW_MODE_POINTS:
            return GL_POINTS;
        case DRAW_MODE_LINES:
            return GL_LINES;
        case DRAW_MODE_LINE_STRIP:
            return GL_LINE_STRIP;
        case DRAW_MODE_TRIANGLES:
            return GL_TRIANGLES;
        case DRAW_MODE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        default:
            return GL_TRIANGLES;
    }
}

void OGLES2RenderManager::setDepthMask(bool enable)
{
    if(currentDepthMask != enable) {
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
        currentDepthMask = enable;
    }
}

void OGLES2RenderManager::setDepthFunction(GLenum func)
{
    if(currentDepthFunction != func) {
        glDepthFunc(func);
        currentDepthFunction = func;
    }
}

void OGLES2RenderManager::resetTextureCache() {
    for (int i = 0; i < 32; i++) {
        currentTextures[i] = -1;
    }
    currentTextureIndex = -1;
}


