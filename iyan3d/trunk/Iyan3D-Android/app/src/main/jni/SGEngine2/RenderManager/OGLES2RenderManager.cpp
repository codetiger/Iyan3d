
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

#ifdef ANDROID
//#include "../../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-arm/usr/include/stdint.h"
#include "../../opengl.h"
#endif

OGLES2RenderManager::OGLES2RenderManager(float screenWidth,float screenHeight,float screenScale){
    c = 0;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->screenScale = screenScale;
    depthBuffer = colorBuffer = frameBuffer = 0;
}
OGLES2RenderManager::~OGLES2RenderManager(){
    glDeleteFramebuffers(1,&depthBuffer);
    glDeleteFramebuffers(1,&colorBuffer);
    glDeleteFramebuffers(1,&frameBuffer);
    
    if(this->camera)
        this->camera.reset();
}
void OGLES2RenderManager::Initialize(){
 
}
bool OGLES2RenderManager::PrepareNode(shared_ptr<Node> node, int meshBufferIndex, int nodeIndex){
    if(node->type <= NODE_TYPE_CAMERA)
        return false;
    
    if(node->type == NODE_TYPE_SKINNED && node->shouldUpdateMesh) {
        createVertexAndIndexBuffers(node , MESH_TYPE_LITE , false);
        node->shouldUpdateMesh = false;
    }
    
    shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    OGLMaterial *material = (OGLMaterial*)node->material;
    glUseProgram(material->shaderProgram);

        MESH_TYPE meshType = MESH_TYPE_LITE;
        
        if(node->type == NODE_TYPE_MORPH_SKINNED)
            meshType = MESH_TYPE_HEAVY;
        else if (node->type == NODE_TYPE_SKINNED)
            meshType = (node->skinType == CPU_SKIN) ? MESH_TYPE_LITE : MESH_TYPE_HEAVY;
        
        glBindBuffer(GL_ARRAY_BUFFER, OGLNode->vertexBufLocations[meshBufferIndex]);
        BindAttributes(node->material,meshType);    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OGLNode->IndexBufLocations[meshBufferIndex]);
    
    
    for(int i = 0;i < material->uniforms.size();i++){
        switch(material->uniforms[i].property){
            case NODE_PROPERTY_UV1:{
                int textureIndex = 0;
                material->uniforms[i].values = &textureIndex;
            }
            default:
                break;
        }
    }
    return (glGetError() == GL_NO_ERROR);
}
void OGLES2RenderManager::endDisplay(){
    
}
void OGLES2RenderManager::Render(shared_ptr<Node> node, bool isRTT, int nodeIndex, int meshBufferIndex){
    glDepthFunc(GL_LEQUAL);
    if(!node || node->type <= NODE_TYPE_CAMERA)
        return;
    Mesh *nodeMes;
    if(node->type == NODE_TYPE_MORPH)
        nodeMes = (dynamic_pointer_cast<MorphNode>(node))->getMeshByIndex(0);
    else if(node->type == NODE_TYPE_MORPH_SKINNED)
        nodeMes = (dynamic_pointer_cast<AnimatedMorphNode>(node))->getMeshByIndex(0);
    else if(node->type == NODE_TYPE_SKINNED) {
        if(node->skinType == GPU_SKIN)
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
        else
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
    }
    else
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();
    
    if(nodeMes == NULL)
        return;
    
    GLenum indicesDataType = GL_UNSIGNED_SHORT;
    if(node->instanceCount)
        drawElements(getOGLDrawMode(node->drawMode),(GLsizei)nodeMes->getIndicesCount(meshBufferIndex),indicesDataType, 0, node->instanceCount + 1);
    else if (node->type == NODE_TYPE_PARTICLES) {
        if(!isRTT)
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        drawElements(getOGLDrawMode(node->drawMode),(GLsizei)nodeMes->getIndicesCount(meshBufferIndex),indicesDataType, 0, dynamic_pointer_cast<ParticleManager>(node)->getParticlesCount());

        if(!isRTT)
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else
        drawElements(getOGLDrawMode(node->drawMode),(GLsizei)nodeMes->getIndicesCount(meshBufferIndex),indicesDataType, 0, 0);
    
    for(int i = 0; i < node->getBufferCount();i++){
        UnBindAttributes(node->material);
    }
}
void OGLES2RenderManager::drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *data, GLsizei instanceCount)
{
   #ifdef ANDROID
    glDrawElements(mode, count, type, data);
    #elif IOS

        if(instanceCount > 0)
             glDrawElementsInstancedEXT(mode, count, type, data, instanceCount);
        else
              glDrawElements(mode, count, type, data);
      #endif
}
void OGLES2RenderManager::BindAttributes(Material *material, MESH_TYPE meshType){
  int valueIndex = 0;
  OGLMaterial *mat = (OGLMaterial*)material;
  if (meshType == MESH_TYPE_LITE) {
      for(int i = 0; i < MAX_VERTEX_DATA;i++){
          GLenum type = Helper::getOGLES2DataType(attributesType[i]);
          int attributeIndex = mat->getMaterialAttribIndexByName(attributesName[i]);
          if(attributeIndex != NOT_EXISTS){
              glEnableVertexAttribArray((GLuint)mat->attributes[attributeIndex].location);
              glVertexAttribPointer((GLuint)mat->attributes[attributeIndex].location,attributesTotalValues[i],type,GL_FALSE,sizeof(vertexData),(GLvoid*)valueIndex);
          }
          valueIndex += sizeof(type) * attributesTotalValues[i];
      }
  } else {
      for(int i = 0; i < MAX_VERTEX_DATA_SKINNED;i++){
          GLenum type = Helper::getOGLES2DataType(attributesTypeSkinned[i]);
          int attributeIndex = mat->getMaterialAttribIndexByName(attributesNameSkinned[i]);
          if(attributeIndex != NOT_EXISTS){
              glEnableVertexAttribArray((GLuint)mat->attributes[attributeIndex].location);
              glVertexAttribPointer((GLuint)mat->attributes[attributeIndex].location,attributesTotalValuesSkinned[i],type,GL_FALSE,sizeof(vertexDataHeavy),(GLvoid*)valueIndex);
          }
          valueIndex += sizeof(type) * attributesTotalValuesSkinned[i];
      }
  }
}
void OGLES2RenderManager::UnBindAttributes(Material *material){
  OGLMaterial *mat = (OGLMaterial*)material;
  for(int i = 0; i < mat->attributes.size();i++)
      glDisableVertexAttribArray((GLuint)mat->attributes[i].location);
}
void OGLES2RenderManager::useMaterialToRender(Material *material)
{
  glUseProgram(((OGLMaterial*)material)->shaderProgram);
}
void OGLES2RenderManager::BindUniform(Material* mat,shared_ptr<Node> node,u16 uIndex, bool isFragmentData, int userValue){
  uniform uni = ((OGLMaterial*)mat)->uniforms[uIndex];
  switch (uni.type){
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
          glActiveTexture(GL_TEXTURE0 + userValue);
          u_int32_t *textureName = (u_int32_t*)uni.values;
          glBindTexture(GL_TEXTURE_2D,*textureName);
          glUniform1i(uni.location,userValue);
      }
          break;
      default:
          Logger::log(ERROR, "OGLES2RenderManager","Uniform: " + uni.name + " Type Not Matched");
          break;
  }
}
void OGLES2RenderManager::bindDynamicUniform(Material *material,string name,void* values,DATA_TYPE type,unsigned short count,u16 paramIndex,int nodeIndex,Texture *tex, bool isFragmentData){

}
void OGLES2RenderManager::setActiveCamera(shared_ptr<CameraNode> camera){
  this->camera = camera;
  this->camera->setActive(true);
}
shared_ptr<CameraNode> OGLES2RenderManager::getActiveCamera(){
  return camera;
}
void OGLES2RenderManager::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}
void OGLES2RenderManager::draw2DImage(Texture *texture,Vector2 originCoord,Vector2 endCoord,bool isBGImage,Material *material,bool isRTT)
{
  glDepthFunc(GL_ALWAYS);
  // to flip horizontally for opengl textures
  Vector2 bottomRight = Helper::screenToOpenglCoords(originCoord,(float)screenWidth * screenScale,(float)screenHeight * screenScale);
  Vector2 upperLeft = Helper::screenToOpenglCoords(endCoord,(float)screenWidth * screenScale,(float)screenHeight * screenScale);
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
  u_int32_t _vertexBuffer = createAndBindBuffer(GL_ARRAY_BUFFER, (GLuint)(vertices.size() * sizeof(vertexData)),&vertices[0], GL_STATIC_DRAW);
  u_int32_t _indexBuffer = createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (6 * sizeof(unsigned short)) ,&indices[0], GL_STATIC_DRAW);
  BindAttributes(material);
  drawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0,0);
  UnBindAttributes(material);
  deleteAndUnbindBuffer(GL_ARRAY_BUFFER, 1, &_vertexBuffer);
  deleteAndUnbindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1, &_indexBuffer);
  if(isBGImage)
      glClear(GL_DEPTH_BUFFER_BIT);
}
void OGLES2RenderManager::draw3DLine(Vector3 start,Vector3 end,Material *material)
{
  vertexData vertData[2];
  vertData[0].vertPosition = start;
  vertData[1].vertPosition = end;
  u16 indices[] = {0, 1};

  u_int32_t _vertexBuffer = createAndBindBuffer(GL_ARRAY_BUFFER, (GLuint)(2*sizeof(vertexData)),&vertData[0], GL_STATIC_DRAW);
  u_int32_t _indexBuffer = createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (2 * sizeof(unsigned short)) ,&indices[0], GL_STATIC_DRAW);
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

bool OGLES2RenderManager::PrepareDisplay(int width,int height,bool clearColorBuf,bool clearDepthBuf,bool isDepthPass,Vector4 color){
//    int maxUniformVectors;
//    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxUniformVectors);
//    printf("Maximum uni size %d ",maxUniformVectors);
  //glGetIntegerv(GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformVectors);
  glViewport(0, 0, width,height);
  glEnable(GL_CULL_FACE); // as now default is back face culling
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(color.x ,color.y ,color.z ,color.w);
  GLbitfield mask = 0;
  if(clearColorBuf)
      mask |= GL_COLOR_BUFFER_BIT;
  if(clearDepthBuf)
      mask |= GL_DEPTH_BUFFER_BIT;
  glClear(mask);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  return true;
}
Texture* OGLES2RenderManager::createRenderTargetTexture(string textureName ,TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height){
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
void OGLES2RenderManager::setRenderTarget(Texture *renderTexture,bool clearBackBuffer,bool clearZBuffer,bool isDepthPass,Vector4 color)
{
  if(renderTexture){
      glBindFramebuffer(GL_FRAMEBUFFER,((OGLTexture*)renderTexture)->rttFrameBuffer);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, ((OGLTexture*)renderTexture)->OGLTextureName);
      PrepareDisplay(renderTexture->width,renderTexture->height,clearBackBuffer,clearZBuffer,false,color);
  }
  else
      resetToMainBuffers();
}
void OGLES2RenderManager::writeImageToFile(Texture *texture, char *filePath , IMAGE_FLIP flipType)
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

  delete buffer;
  delete imageData;
}
Vector4 OGLES2RenderManager::getPixelColor(Vector2 touchPosition, Texture *texture)
{
  float mid = texture->height / 2.0;
  float difFromMid = touchPosition.y - mid;
  glBindTexture(GL_TEXTURE_2D,((OGLTexture*)texture)->OGLTextureName);
  GLubyte pixelColor[4];
  glReadPixels((int)touchPosition.x,(int)(mid - difFromMid),1,1,GL_RGBA,GL_UNSIGNED_BYTE,&pixelColor[0]);
  return Vector4((int)pixelColor[0],(int)pixelColor[1],(int)pixelColor[2],(int)pixelColor[3]);
}

void OGLES2RenderManager::setFrameBufferObjects(u_int32_t framebuff, u_int32_t colorbuff, u_int32_t depthduff)
{
  frameBuffer = framebuff;
  colorBuffer = colorbuff;
  depthBuffer = depthduff;
}
void OGLES2RenderManager::setUpDepthState(METAL_DEPTH_FUNCTION func,bool writeDepth,bool setToRenderBuffer)
{

}
void OGLES2RenderManager::createVertexAndIndexBuffers(shared_ptr<Node> node,MESH_TYPE meshType , bool updateBothBuffers){
  if(node->type <= NODE_TYPE_CAMERA)
      return;
  shared_ptr<OGLNodeData> OGLNode = dynamic_pointer_cast<OGLNodeData>(node->nodeData);
    
  u16 meshBufferCount = 1;
  if(node->type == NODE_TYPE_MORPH)
      meshBufferCount = (dynamic_pointer_cast<MorphNode>(node))->getMeshCount();
  else if (node->type == NODE_TYPE_MORPH_SKINNED)
      meshBufferCount = (dynamic_pointer_cast<AnimatedMorphNode>(node))->getMeshCount();
  else
      meshBufferCount = dynamic_pointer_cast<MeshNode>(node)->getMesh()->getMeshBufferCount();

    for(int i = 0; i < meshBufferCount;i++) {
        createVertexBuffer(node,i,meshType);
        if(updateBothBuffers)
            OGLNode->IndexBufLocations.push_back(bindIndexBuffer(node,i));
    }
}

void OGLES2RenderManager::createVertexBuffer(shared_ptr<Node> node,short meshBufferIndex,MESH_TYPE meshType){
  Mesh *nodeMes;
  if(node->type == NODE_TYPE_MORPH)
      nodeMes = (dynamic_pointer_cast<MorphNode>(node))->getMeshByIndex(meshBufferIndex);
  else if(node->type == NODE_TYPE_MORPH_SKINNED)
      nodeMes = (dynamic_pointer_cast<AnimatedMorphNode>(node))->getMeshByIndex(meshBufferIndex);
  else if(node->type == NODE_TYPE_SKINNED) {
      if(node->skinType == GPU_SKIN)
          nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
      else {
          nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
          meshType = MESH_TYPE_LITE;
      }
  }
  else
      nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();

  GLuint size = (GLuint)nodeMes->getVerticesCountInMeshBuffer(meshBufferIndex) * ((meshType == MESH_TYPE_LITE) ? sizeof(vertexData) : sizeof(vertexDataHeavy));
  unsigned int vertexBufLoc = 0;
    
    //TODO: implement functionality for CPU_SKIN type
    
    if(meshType == MESH_TYPE_LITE){
        vertexBufLoc = createAndBindBuffer(GL_ARRAY_BUFFER, size, &(nodeMes->getLiteVerticesArray(meshBufferIndex))[0], GL_STATIC_DRAW);
    }else{
        vertexBufLoc = createAndBindBuffer(GL_ARRAY_BUFFER, size, &(nodeMes->getHeavyVerticesArray(meshBufferIndex))[0], GL_STATIC_DRAW);
    }
    
        (dynamic_pointer_cast<OGLNodeData>(node->nodeData))->vertexBufLocations.push_back(vertexBufLoc);

}

u_int32_t OGLES2RenderManager::bindIndexBuffer(shared_ptr<Node> node, int meshBufferIndex){
  Mesh *nodeMes;
  if(node->type == NODE_TYPE_MORPH)
      nodeMes = (dynamic_pointer_cast<MorphNode>(node))->getMeshByIndex(0);
  else if(node->type == NODE_TYPE_MORPH_SKINNED)
      nodeMes = (dynamic_pointer_cast<AnimatedMorphNode>(node))->getMeshByIndex(0);
  else if(node->type == NODE_TYPE_SKINNED) {
      if(node->skinType == GPU_SKIN)
          nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
      else
          nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
  } else
      nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();

    size_t indexCount =  nodeMes->getIndicesCount(meshBufferIndex);
    GLsizeiptr size;
    
    size = sizeof(unsigned short) * indexCount;
    return createAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER , size , nodeMes->getIndicesArray(meshBufferIndex) , GL_STATIC_DRAW);
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
    //glBufferSubData(target, 0, size, data);
    //GLvoid *currentData = glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
    //memcpy(currentData, data, sizeof(currentData));
    //currentData = data;
    //glUnmapBufferOES(GL_ARRAY_BUFFER);
    return _bufferToBind;
}

void OGLES2RenderManager::deleteAndUnbindBuffer(GLenum target,GLsizei size,const GLuint *bufferToDelete)
{
  glDeleteBuffers(size, bufferToDelete);
  glBindBuffer(target, 0);
}
void OGLES2RenderManager::resetToMainBuffers(){
  glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER,colorBuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER,depthBuffer);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
}
GLenum OGLES2RenderManager::getOGLDrawMode(DRAW_MODE mode){
  switch(mode){
      case DRAW_MODE_POINTS:
          return GL_POINTS;
      case DRAW_MODE_LINES:
          return GL_LINES;
      case DRAW_MODE_LINE_LOOP:
          return GL_LINE_LOOP;
      case DRAW_MODE_LINE_STRIP:
          return GL_LINE_STRIP;
      case DRAW_MODE_TRIANGLES:
          return GL_TRIANGLES;
      case DRAW_MODE_TRIANGLE_STRIP:
          return GL_TRIANGLE_STRIP;
      case DRAW_MODE_TRIANGLE_FAN:
          return GL_TRIANGLE_FAN;
      default:
          return GL_TRIANGLES;
  }
}
