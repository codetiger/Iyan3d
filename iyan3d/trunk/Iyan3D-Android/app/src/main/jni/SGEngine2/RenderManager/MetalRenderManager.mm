//
//  OGLES2RenderManager.cpp
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifdef IOS

#include "../Core/Nodes/ParticleManager.h"

#import "MetalRenderManager.h"
#import "../Core/Nodes/MTLNodeData.h"
struct BufferState{
    id <MTLBuffer> buf;
    id <MTLTexture> texture;
    bool isOccupied;
};
std::map<int,vector<BufferState> > MTLBuffersMap;

MetalRenderManager::MetalRenderManager(void *renderView, float screenWidth, float screenHeight, float screenScale)
{
    _currentDrawable = drawable = nil;
    this->renderView = (__bridge UIView*)renderView;
    mtlRenderView = (RenderingView*)this->renderView;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->screenScale = screenScale;
    setupMetal();
    setUpDepthState(CompareFunctionLessEqual);
    isCmdBufferCommited = false; isEncodingEnded = true;
    renderingTexture = NULL;
}

MetalRenderManager::~MetalRenderManager()
{
    device = nil;
    _commandQueue = nil;
    
    if(this->camera)
        this->camera.reset();
}

void MetalRenderManager::setupMetal()
{
    device = MTLCreateSystemDefaultDevice();
    _commandQueue = [device newCommandQueue];
    _defaultLibrary = [device newDefaultLibrary];
    setupMetalLayer();
    MetalHandler::setDevice(device);
    MetalHandler::setMTLLayer(_metalLayer);
    MetalHandler::setMTLLibrary(_defaultLibrary);
    _inflight_semaphore = dispatch_semaphore_create(4);
}

void MetalRenderManager::setupMetalLayer()
{
    _metalLayer = (CAMetalLayer*)mtlRenderView.layer;
    _metalLayer.device = device;
    _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    _metalLayer.framebufferOnly = YES;
    CGSize drawableSize = renderView.bounds.size;
    drawableSize.width  *= screenScale;
    drawableSize.height *= screenScale;
    _metalLayer.drawableSize = drawableSize;
    Logger::log(INFO,"MetalRendermanager","SetupMetal");
    [_metalLayer nextDrawable];
    renderView.opaque = YES;
    renderView.backgroundColor = nil;
    renderView.contentScaleFactor = [UIScreen mainScreen].scale;
}

void MetalRenderManager::setUpDepthState(METAL_DEPTH_FUNCTION func,bool writeDepth,bool clearDepthBuffer)
{
    if(isEncodingEnded)
        return;
    
    MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStateDesc.depthCompareFunction = func;
    depthStateDesc.depthWriteEnabled = writeDepth;
    _depthState = [device newDepthStencilStateWithDescriptor:depthStateDesc];
    [RenderCMDBuffer setDepthStencilState:_depthState];
    
    if(clearDepthBuffer){
        endEncoding();
        _renderPassDescriptor.depthAttachment.clearDepth = 1.0f;
        _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
        if(ANTI_ALIASING_SAMPLE_COUNT > 1)
            _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
        else
            _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        if(_renderPassDescriptor && CMDBuffer) {
            RenderCMDBuffer = [CMDBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];
            [RenderCMDBuffer setFrontFacingWinding:MTLWindingClockwise];
            [RenderCMDBuffer setCullMode:MTLCullModeBack];
            RenderCMDBuffer.label = @"MyRenderEncoder";
            [RenderCMDBuffer pushDebugGroup:@"DrawCube"];
        }
        isEncodingEnded = false;
    }
}

void MetalRenderManager::setupRenderPassDescriptorForTexture(id <MTLTexture> texture, Vector4 color, bool isDepthPass)
{
    if (_renderPassDescriptor == nil || isDepthPass)
        _renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    int colorTextureType = MTLTextureType2D,sampleCount = 1;
    
    if(!isDepthPass) {
        if(ANTI_ALIASING_SAMPLE_COUNT > 1){
            colorTextureType = MTLTextureType2DMultisample;    sampleCount = 2;
            _renderPassDescriptor.colorAttachments[0].texture = getMSATexture(sampleCount, (int)texture.width, (int)texture.height);
            _renderPassDescriptor.colorAttachments[0].resolveTexture = texture;
            _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
        } else {
            _renderPassDescriptor.colorAttachments[0].texture = texture;
            _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        }
        _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        _renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(color.x, color.y, color.z, color.w);
    }

    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatDepth32Float width: texture.width height: texture.height mipmapped: NO];
    desc.textureType = colorTextureType;
    desc.sampleCount = sampleCount;
    _depthTex = [device newTextureWithDescriptor: desc];
    
    _renderPassDescriptor.depthAttachment.texture = _depthTex;
    _renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
    _renderPassDescriptor.depthAttachment.clearDepth = 1.0f;
    _renderPassDescriptor.depthAttachment.storeAction = (isDepthPass)? MTLStoreActionStore:MTLStoreActionDontCare;
}

id <CAMetalDrawable> MetalRenderManager::currentDrawable()
{
    if(_currentDrawable == nil)
    {
        _currentDrawable = [_metalLayer nextDrawable];
        if (!_currentDrawable)
        {
            NSLog(@"CurrentDrawable is nil");
        }
    }
    return _currentDrawable;
}

void* initMetalRenderManager(void *renderView, float screenWidth, float screenHeight, float screenScale)
{
    #if !(TARGET_IPHONE_SIMULATOR)
    MetalRenderManager *metalRenderManager = new MetalRenderManager(renderView, screenWidth, screenHeight, screenScale);
    return metalRenderManager;
    #endif
}

void MetalRenderManager::Initialize()
{
    
}

void MetalRenderManager::draw3DLine(Vector3 start,Vector3 end,Material *material)
{
    if(_currentDrawable == nil)
        PrepareDisplay(0,0);
    int vertBufSize = sizeof(vertexData) * 2;
    int indexBufSize = 2 * sizeof(unsigned short);
    vertexData vertData[2];
    vertData[0].vertPosition = start;
    vertData[1].vertPosition = end;
    int bufIndex = getAvailableBfferWithSize(vertBufSize);
    uint8_t *bufferPointer = (uint8_t*)[MTLBuffersMap[vertBufSize][bufIndex].buf contents];
    memcpy(bufferPointer,&vertData[0],vertBufSize);
    [this->RenderCMDBuffer setVertexBuffer:MTLBuffersMap[vertBufSize][bufIndex].buf offset:0 atIndex:0];
    
    u16 indices[] = {0, 1};
    bufIndex = getAvailableBfferWithSize(indexBufSize);
    uint8_t *indexBufferPointer = (uint8_t *)[MTLBuffersMap[indexBufSize][bufIndex].buf contents];
    memcpy(indexBufferPointer,&indices[0],indexBufSize);
    drawPrimitives(MTLPrimitiveTypeLine,2,MTLIndexTypeUInt16,MTLBuffersMap[indexBufSize][bufIndex].buf,0);
}

void MetalRenderManager::draw3DLines(vector<Vector3> vPositions,Material *material)
{
    if(_currentDrawable == nil)
        PrepareDisplay(0,0);
    int vertBufSize = sizeof(vertexData) * (int)vPositions.size();
    int indexBufSize = (int)vPositions.size() * sizeof(unsigned short);
    vector<vertexData> vertDatas;
    vector<u16> indices;
    for(int i = 0; i < vPositions.size(); i++)
    {
        vertexData vData;
        vData.vertPosition = vPositions[i];
        vertDatas.push_back(vData);
        indices.push_back(i);
    }
    
    int bufIndex = getAvailableBfferWithSize(vertBufSize);
    uint8_t *bufferPointer = (uint8_t*)[MTLBuffersMap[vertBufSize][bufIndex].buf contents];
    memcpy(bufferPointer,&vertDatas[0],vertBufSize);
    [this->RenderCMDBuffer setVertexBuffer:MTLBuffersMap[vertBufSize][bufIndex].buf offset:0 atIndex:0];
    
    bufIndex = getAvailableBfferWithSize(indexBufSize);
    uint8_t *indexBufferPointer = (uint8_t *)[MTLBuffersMap[indexBufSize][bufIndex].buf contents];
    memcpy(indexBufferPointer,&indices[0],indexBufSize);
    drawPrimitives(MTLPrimitiveTypeLine, (int)vPositions.size(),MTLIndexTypeUInt16,MTLBuffersMap[indexBufSize][bufIndex].buf,0);
    
    vertDatas.clear();
    indices.clear();    
}

void MetalRenderManager::clearDepthBuffer()
{
    setUpDepthState(CompareFunctionLessEqual,true,false);
}

void MetalRenderManager::setTransparencyBlending(bool enable)
{
    
}

void MetalRenderManager::draw2DImage(Texture *texture,Vector2 originCoord,Vector2 endCoord,bool isBGImage,Material *material,bool isRTT)
{
    setUpDepthState(CompareFunctionAlways,false,false);
    Vector2 upperLeft = Helper::screenToOpenglCoords(originCoord,(float)screenWidth * screenScale,(float)screenHeight * screenScale);
    Vector2 bottomRight = Helper::screenToOpenglCoords(endCoord,(float)screenWidth * screenScale,(float)screenHeight * screenScale);
    Vector3 uu = Vector3(upperLeft.x,upperLeft.y,0.0);
    Vector3 ub = Vector3(upperLeft.x,bottomRight.y,0.0);
    Vector3 bb = Vector3(bottomRight.x,bottomRight.y,0.0);
    Vector3 bu = Vector3(bottomRight.x,upperLeft.y,0.0);
    
    vector<Vector3> vertPosition;
    vertPosition.push_back((isRTT)?ub:uu);
    vertPosition.push_back((isRTT)?uu:ub);
    vertPosition.push_back((isRTT)?bu:bb);
    vertPosition.push_back((isRTT)?bb:bu);
    
    vector<Vector2> texCoord1;
    texCoord1.push_back(Vector2(0.0,1.0));
    texCoord1.push_back(Vector2(0.0,0.0));
    texCoord1.push_back(Vector2(1.0,0.0));
    texCoord1.push_back(Vector2(1.0,1.0));

    
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
    u16 indicesRTT[6] = {
        0,1,3,
        2,3,1
    };
    int vertBufSize = sizeof(vertexData) * (int)vertices.size();
    int indexBufSize = 6 * sizeof(unsigned short);
    int bufIndex = getAvailableBfferWithSize(vertBufSize);
    uint8_t *bufferPointer = (uint8_t *)[MTLBuffersMap[vertBufSize][bufIndex].buf contents];
    memcpy(bufferPointer,&vertices[0],vertBufSize);
    [this->RenderCMDBuffer setVertexBuffer:MTLBuffersMap[vertBufSize][bufIndex].buf offset:0 atIndex:0];

    
    bufIndex = getAvailableBfferWithSize(indexBufSize);
    uint8_t *indexBufferPointer = (uint8_t *)[MTLBuffersMap[indexBufSize][bufIndex].buf contents];
    if(isRTT)
        memcpy(indexBufferPointer,&indicesRTT[0],indexBufSize);
    else
        memcpy(indexBufferPointer,&indices[0],indexBufSize);
    drawPrimitives(MTLPrimitiveTypeTriangle,6,MTLIndexTypeUInt16,MTLBuffersMap[indexBufSize][bufIndex].buf,0);
    if(isBGImage)
        setUpDepthState(CompareFunctionLessEqual,true,false);
}

void MetalRenderManager::useMaterialToRender(Material *mat)
{
    if(_currentDrawable == nil)
        PrepareDisplay(0,0,true,true,false,Vector4(1.0,1.0,1.0,1.0));
    [RenderCMDBuffer setRenderPipelineState:((MTLMaterial*)mat)->PipelineState];
}

Vector2 MetalRenderManager::getViewPort()
{
    return Vector2(viewportWidth, viewportHeight);
}

void MetalRenderManager::changeViewport(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;
}

bool MetalRenderManager::PrepareNode(shared_ptr<Node> node, int meshBufferIndex, bool isRTT, int nodeIndex)
{
    if(node->type <= NODE_TYPE_CAMERA || node->type == NODE_TYPE_INSTANCED)
        return false;
    
    if(node->shouldUpdateMesh) {
        createVertexAndIndexBuffers(node , dynamic_pointer_cast<MeshNode>(node)->getMesh()->meshType, true);
        node->shouldUpdateMesh = false;
    }
    
    useMaterialToRender(node->material);
    BindAttribute(node,meshBufferIndex);
    //node.reset();
     return true;
}

void MetalRenderManager::Render(shared_ptr<Node> node, bool isRTT, int nodeIndex, int meshBufferIndex)
{
    if(node->type <= NODE_TYPE_CAMERA)
        return;
    shared_ptr<MTLNodeData> MTLNode = dynamic_pointer_cast<MTLNodeData>(node->nodeData);
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
    } else if (node->instancedNodes.size() > 0 && !supportsInstancing)
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->meshCache;
    else
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();

    
    MTLIndexType indexType = MTLIndexTypeUInt16;
    unsigned int indicesCount = nodeMes->getIndicesCount(meshBufferIndex);
    id<MTLBuffer> buf = [MTLNode->indexBuffers objectAtIndex:meshBufferIndex];
    if (node->type == NODE_TYPE_PARTICLES) {
        MTLDepthStencilDescriptor *depthStateDesc;
        if(!isRTT) {
            depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
            depthStateDesc.depthCompareFunction = CompareFunctionLessEqual;
            depthStateDesc.depthWriteEnabled = NO;
            _depthState = [device newDepthStencilStateWithDescriptor:depthStateDesc];
            [RenderCMDBuffer setDepthStencilState:_depthState];
        }
        
        drawPrimitives(getMTLDrawMode(DRAW_MODE_POINTS), indicesCount,indexType, buf, 0);
        
        if(!isRTT) {
            depthStateDesc.depthCompareFunction = CompareFunctionLessEqual;
            depthStateDesc.depthWriteEnabled = YES;
            _depthState = [device newDepthStencilStateWithDescriptor:depthStateDesc];
            [RenderCMDBuffer setDepthStencilState:_depthState];
        }

    } else {
        int instancingCount = (node->instancedNodes.size() == 0) ? 0 : (node->instancingRenderIt + maxInstances > (int)node->instancedNodes.size()) ? ((int)node->instancedNodes.size() - node->instancingRenderIt) :  maxInstances;
        drawPrimitives(getMTLDrawMode(node->drawMode), indicesCount,indexType, buf, (supportsInstancing) ? instancingCount+1 : 0);
    }
        
}

bool MetalRenderManager::PrepareDisplay(int width,int height,bool clearColorBuf,bool clearDepthBuf,bool isDepthPass,Vector4 color)
{
    if(!isCmdBufferCommited) // Temporary try for perfection
        endDisplay();
    isCmdBufferCommited = false;
    changeViewport(width, height);
    CMDBuffer = [_commandQueue commandBuffer];
    drawable = currentDrawable();
    if(drawable == nil)
        return false;
    dispatch_semaphore_wait(_inflight_semaphore, DISPATCH_TIME_FOREVER);
    
    renderingTexture = drawable.texture;
    if(clearColorBuf) {
        isEncodingEnded = false;
        setupRenderPassDescriptorForTexture(drawable.texture, color);
        if(_renderPassDescriptor && CMDBuffer) {
            RenderCMDBuffer = [CMDBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];
            [RenderCMDBuffer setCullMode:MTLCullModeBack];
            [RenderCMDBuffer setFrontFacingWinding:MTLWindingClockwise];
            RenderCMDBuffer.label = @"MyRenderEncoder";
            setUpDepthState(CompareFunctionLessEqual,true,false);
            [RenderCMDBuffer setDepthStencilState:_depthState];
            [RenderCMDBuffer pushDebugGroup:@"DrawCube"];
            mainRenderCMDBuffer = RenderCMDBuffer;
        }
    }
    return true;
}

void MetalRenderManager::endDisplay()
{
    if(isCmdBufferCommited || drawable == nil)
        return;
    isCmdBufferCommited= true;
    if(!isEncodingEnded)
        endEncoding();
    
    __block dispatch_semaphore_t block_sema = _inflight_semaphore;
    [CMDBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(block_sema);
    }];
    
    [CMDBuffer presentDrawable:drawable];
    [CMDBuffer commit];
    [CMDBuffer waitUntilCompleted];
    _currentDrawable = nil;
    freeDynamicBuffers();
}

void MetalRenderManager::drawPrimitives(MTLPrimitiveType primitiveType,unsigned int count,MTLIndexType type,id <MTLBuffer> indexBuf, GLsizei instanceCount)
{
    if(instanceCount > 1)
        [this->RenderCMDBuffer drawIndexedPrimitives:primitiveType indexCount:count indexType:type indexBuffer:indexBuf indexBufferOffset:0 instanceCount:instanceCount];
    else
        [this->RenderCMDBuffer drawIndexedPrimitives:primitiveType indexCount:count indexType:type indexBuffer:indexBuf indexBufferOffset:0];
}

void MetalRenderManager::BindAttribute(shared_ptr<Node> node,int meshIndx)
{
    shared_ptr<MTLNodeData> MTLNode = dynamic_pointer_cast<MTLNodeData>(node->nodeData);
    if(meshIndx < [MTLNode->VertexBuffers count])
        [this->RenderCMDBuffer setVertexBuffer:[MTLNode->VertexBuffers objectAtIndex:meshIndx] offset:0 atIndex:0];
}

void MetalRenderManager::BindUniform(DATA_TYPE type, id<MTLBuffer> buf, void* values, int count, int parameterIndex, id<MTLTexture> tex, string matName, int nodeIndex, bool isFragmentData, bool blurTex)
{
    switch (type){
        case DATA_FLOAT:
        case DATA_FLOAT_VEC2:
        case DATA_FLOAT_VEC3:
        case DATA_FLOAT_VEC4:
        case DATA_FLOAT_MAT2:
        case DATA_FLOAT_MAT3:
        case DATA_FLOAT_MAT4:{
            uint8_t *bufferPointer = (uint8_t *)[buf contents];
            memcpy(bufferPointer,values,count * sizeof(float));
            if(isFragmentData)
                [this->RenderCMDBuffer setFragmentBuffer:buf offset:0 atIndex:parameterIndex];
            else
                [this->RenderCMDBuffer setVertexBuffer:buf offset:0 atIndex:parameterIndex];
            break;
        }
        case DATA_INTEGER:{
            uint8_t *bufferPointer = (uint8_t *)[buf contents];
            memcpy(bufferPointer,values,count * sizeof(int));
            if(isFragmentData)
                [this->RenderCMDBuffer setFragmentBuffer:buf offset:0 atIndex:parameterIndex];
            else
                [this->RenderCMDBuffer setVertexBuffer:buf offset:0 atIndex:parameterIndex];
            break;
        }
        case DATA_TEXTURE_CUBE:
        case DATA_TEXTURE_2D:{
            [this->RenderCMDBuffer setFragmentTexture:tex atIndex:parameterIndex];
            break;
        }
        default:
            Logger::log(ERROR, "OGLES2RenderManager", "Uniform: " + matName + " Type Not Matched");
            break;
    }
}

void MetalRenderManager::BindUniform(Material* mat, shared_ptr<Node> node, u16 uIndex, bool isFragmentData, int userValue, bool blurTex)
{
    MTLMaterial *MTLMat = ((MTLMaterial*)mat);
    id<MTLTexture> tex = NULL;
    if(MTLMat->uniforms[uIndex].type == DATA_TEXTURE_2D || MTLMat->uniforms[uIndex].type == DATA_TEXTURE_CUBE){
        int textureIndex = ((int*)MTLMat->uniforms[uIndex].values)[0];
        if((MTLTexture*)node->getTextureByIndex(textureIndex) != NULL)
            tex = ((MTLTexture*)node->getTextureByIndex(textureIndex))->texture;
        else
            tex = ((MTLTexture*)emptyTexture)->texture;
    }
    
    BindUniform(MTLMat->uniforms[uIndex].type, MTLMat->uniforms[uIndex].buf, MTLMat->uniforms[uIndex].values, MTLMat->uniforms[uIndex].count, MTLMat->uniforms[uIndex].parameterIndex, tex, MTLMat->uniforms[uIndex].name, MTLMat->uniforms[uIndex].nodeIndex, isFragmentData, blurTex);
}

void MetalRenderManager::bindDynamicUniform(Material *material, string name, void* values, DATA_TYPE type, ushort count, u16 paramIndex, int nodeIndex, Texture *tex, bool isFragmentData, bool blurTex)
{
    int dataTypeSize = (type == DATA_INTEGER) ? sizeof(int) : sizeof(float);
    int bufIndex = getAvailableBfferWithSize(count * dataTypeSize);
    id<MTLTexture> texture = (tex) ? ((MTLTexture*)tex)->texture : NULL;
    BindUniform(type, MTLBuffersMap[count * dataTypeSize][bufIndex].buf, values, count, paramIndex, texture, name, NOT_EXISTS, isFragmentData, blurTex);
}

int MetalRenderManager::getAvailableBfferWithSize(int bufSize)
{
    bool bufAvailable = false;
    int bufIndex = NOT_EXISTS;
    if(MTLBuffersMap.find(bufSize)->first == bufSize){
        for(int i = 0; i < MTLBuffersMap[bufSize].size();i++){
            if(!MTLBuffersMap[bufSize][i].isOccupied){
                MTLBuffersMap[bufSize][i].isOccupied = bufAvailable = true;
                bufIndex = i;
                break;
            }
        }
    }
    if(bufIndex == NOT_EXISTS){
        BufferState bufState;
        bufState.buf = [device newBufferWithLength:bufSize options:0];
        MTLBuffersMap[bufSize].push_back(bufState);
        bufIndex = (int)MTLBuffersMap[bufSize].size() - 1;
        MTLBuffersMap[bufSize][bufIndex].isOccupied = true;
    }
    return bufIndex;
}

void MetalRenderManager::freeDynamicBuffers()
{
    for(std::map<int,vector<BufferState> >::iterator it = MTLBuffersMap.begin();it != MTLBuffersMap.end();it++){
        for(int i = 0; i < it->second.size();i++)
            it->second[i].isOccupied = false;
    }
}

void MetalRenderManager::setActiveCamera(shared_ptr<CameraNode> camera)
{
    this->camera = camera;
    this->camera->setActive(true);
}

shared_ptr<CameraNode> MetalRenderManager::getActiveCamera()
{
    return camera;
}

Texture* MetalRenderManager::createRenderTargetTexture(string textureName ,TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height)
{
    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:getTexturePixelFormat(format) width:width height:height mipmapped:NO];
    id<MTLTexture> texture = [MetalHandler::getMTLDevice() newTextureWithDescriptor:texDesc];
    MTLTexture *MtlTex = new MTLTexture();
    MtlTex->texture = texture;
    MtlTex->textureName = textureName;
    MtlTex->width = width; MtlTex->height = height;
    return MtlTex;
}

int MetalRenderManager::getTexturePixelFormat(TEXTURE_DATA_FORMAT format)
{
    if(format == TEXTURE_RGBA8)
        return MTLPixelFormatBGRA8Unorm;
    else if(format == TEXTURE_DEPTH32)
        return MTLPixelFormatDepth32Float;
    return MTLPixelFormatBGRA8Unorm;
}

id<MTLTexture> MetalRenderManager::getMSATexture(int sampleCount,int width,int height)
{
    MTLTextureDescriptor *msaaTexDesc = [MTLTextureDescriptor
                                         texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                         width:width height:height mipmapped:NO];
    msaaTexDesc.textureType = MTLTextureType2DMultisample;
    msaaTexDesc.sampleCount = sampleCount;
    return [device newTextureWithDescriptor:msaaTexDesc];
}

void MetalRenderManager::setRenderTarget(Texture *renderTexture,bool clearBackBuffer,bool clearZBuffer,bool isDepthPass,Vector4 color)
{
    if(isCmdBufferCommited)
        return;
    
    if(renderTexture){
        endEncoding();
        changeViewport(renderTexture->width, renderTexture->height);
        _renderPassDescriptor = nil;
        _renderPassDescriptor = [MTLRenderPassDescriptor new];
        int sampleCount = 1,colorTextureType = MTLTextureType2D;
        if(!isDepthPass) {
            if(ANTI_ALIASING_SAMPLE_COUNT > 1){
                sampleCount = ANTI_ALIASING_SAMPLE_COUNT;colorTextureType = MTLTextureType2DMultisample;
                _renderPassDescriptor.colorAttachments[0].texture = getMSATexture(sampleCount,(int)((MTLTexture*)renderTexture)->texture.width,(int)((MTLTexture*)renderTexture)->texture.height);
                _renderPassDescriptor.colorAttachments[0].resolveTexture = ((MTLTexture*)renderTexture)->texture;
                _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
            }else{
                _renderPassDescriptor.colorAttachments[0].texture = ((MTLTexture*)renderTexture)->texture;
                _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            }
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            _renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(color.x, color.y, color.z, color.w);
            
            MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatDepth32Float width:((MTLTexture*)renderTexture)->texture.width height:((MTLTexture*)renderTexture)->texture.height mipmapped: NO];
            desc.sampleCount = sampleCount;
            desc.textureType = colorTextureType;
            _depthTex = [device newTextureWithDescriptor: desc];
        }
        
        _renderPassDescriptor.depthAttachment.texture = (isDepthPass)? ((MTLTexture*)renderTexture)->texture : _depthTex;
        _renderPassDescriptor.depthAttachment.loadAction = (isDepthPass)?MTLLoadActionClear:MTLLoadActionDontCare;
        _renderPassDescriptor.depthAttachment.clearDepth = 1.0f;
        _renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
        
        if(_renderPassDescriptor && CMDBuffer) {
            rttRenderCMDBuffer = [CMDBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];
            [rttRenderCMDBuffer setCullMode:MTLCullModeBack];
            [rttRenderCMDBuffer setFrontFacingWinding:MTLWindingClockwise];
            MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
            depthStateDesc.depthCompareFunction = CompareFunctionLessEqual;
            depthStateDesc.depthWriteEnabled = YES;
            _depthState = [device newDepthStencilStateWithDescriptor:depthStateDesc];
            [rttRenderCMDBuffer setDepthStencilState:_depthState];
            [rttRenderCMDBuffer pushDebugGroup:@"DrawCube"];
            rttRenderCMDBuffer.label = @"RttRenderEncoder";
            RenderCMDBuffer = rttRenderCMDBuffer;
        }
        isEncodingEnded = false;
    }else{
        endEncoding();
    }
}

void MetalRenderManager::endEncoding()
{
    if(!isEncodingEnded){
        [RenderCMDBuffer popDebugGroup];
        [RenderCMDBuffer endEncoding];
        isEncodingEnded = true;
    }
}

Vector4 MetalRenderManager::getPixelColor(Vector2 touchPosition,Texture* renderTexture)
{
    unsigned char color[4];
    MTLRegion region = MTLRegionMake2D((int)touchPosition.x,(int)touchPosition.y,1,1);
    [((MTLTexture*)renderTexture)->texture getBytes:&color[0] bytesPerRow:renderTexture->width * 4 fromRegion:region mipmapLevel:0];
    return Vector4(color[2],color[1],color[0],color[3]);
}

void MetalRenderManager::createVertexAndIndexBuffers(shared_ptr<Node> node,MESH_TYPE meshType , bool updateBothBuffers)
{
    if(node->type <= NODE_TYPE_CAMERA)
        return;
    shared_ptr<MTLNodeData> MTLNode = dynamic_pointer_cast<MTLNodeData>(node->nodeData);
    
    if(MTLNode->VertexBuffers != nil && [MTLNode->VertexBuffers count] > 0) {
        [MTLNode->VertexBuffers removeAllObjects];
    } else if(MTLNode->VertexBuffers == nil)
        MTLNode->VertexBuffers = [[NSMutableArray alloc] init];
    
    if(MTLNode->indexBuffers == nil)
        MTLNode->indexBuffers = [[NSMutableArray alloc] init];
    else if(updateBothBuffers)
        [MTLNode->indexBuffers removeAllObjects];
    
    Mesh *nodeMes;
    if(node->type == NODE_TYPE_MORPH)
        nodeMes = (dynamic_pointer_cast<MorphNode>(node))->getMeshByIndex(0);
    else if(node->type == NODE_TYPE_MORPH_SKINNED)
        nodeMes = (dynamic_pointer_cast<AnimatedMorphNode>(node))->getMeshByIndex(0);
    else if (node->type == NODE_TYPE_SKINNED) {
        if(node->skinType == GPU_SKIN)
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMesh();
        else {
            nodeMes = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getMeshCache();
            meshType = MESH_TYPE_LITE;
        }
    } else if (node->instancedNodes.size() > 0 && !supportsInstancing)
        nodeMes = dynamic_pointer_cast<MeshNode>(node)->meshCache;
    else
        nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();

    for (int i = 0; i < nodeMes->getMeshBufferCount(); i++) {
        createVertexBuffer(node,i,meshType);
        
        if(updateBothBuffers) {
            unsigned int length = nodeMes->getIndicesCount(i) * sizeof(unsigned short);
            [MTLNode->indexBuffers addObject:[device newBufferWithBytes:nodeMes->getIndicesArray(i) length:length options:MTLResourceOptionCPUCacheModeDefault]];
        }
    }
}

void MetalRenderManager::createVertexBuffer(shared_ptr<Node> node,short meshBufferIndex, MESH_TYPE meshType)
{
    if(node->type <= NODE_TYPE_CAMERA)
        return;
    shared_ptr<MTLNodeData> MTLNode = dynamic_pointer_cast<MTLNodeData>(node->nodeData);
    
    if(MTLNode->VertexBuffers != nil && [MTLNode->VertexBuffers count] > 0) {
        
    } else if(MTLNode->VertexBuffers == nil)
        MTLNode->VertexBuffers = [[NSMutableArray alloc] init];
    
    u16 meshCount = 1;
    if(node->type == NODE_TYPE_MORPH)
        meshCount = (dynamic_pointer_cast<MorphNode>(node))->getMeshCount();
    else if (node->type == NODE_TYPE_MORPH_SKINNED)
        meshCount = (dynamic_pointer_cast<AnimatedMorphNode>(node))->getMeshCount();
    
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
        } else if (node->instancedNodes.size() > 0 && !supportsInstancing)
            nodeMes = (dynamic_pointer_cast<MeshNode>(node))->meshCache;
        else
            nodeMes = (dynamic_pointer_cast<MeshNode>(node))->getMesh();
    
        id<MTLBuffer> buf;
    
    if([MTLNode->VertexBuffers count] >= meshBufferIndex+1){
        buf = [MTLNode->VertexBuffers objectAtIndex:meshBufferIndex];
        uint8_t *bufferPointer = (uint8_t *)[buf contents];
        
        if(meshType == MESH_TYPE_LITE)
            memcpy(bufferPointer,&(nodeMes->getLiteVerticesArray(meshBufferIndex)[0]),sizeof(vertexData) * nodeMes->getVerticesCountInMeshBuffer(meshBufferIndex));
        else
            memcpy(bufferPointer,&(nodeMes->getHeavyVerticesArray(meshBufferIndex)[0]),sizeof(vertexDataHeavy) * nodeMes->getVerticesCountInMeshBuffer(meshBufferIndex));
        
    }else{
        
        if(meshType == MESH_TYPE_LITE)
            buf = [device newBufferWithBytes:&(nodeMes->getLiteVerticesArray(meshBufferIndex)[0]) length:sizeof(vertexData) * nodeMes->getVerticesCountInMeshBuffer(meshBufferIndex) options:MTLResourceOptionCPUCacheModeDefault];
        else
            buf = [device newBufferWithBytes:&(nodeMes->getHeavyVerticesArray(meshBufferIndex)[0]) length:sizeof(vertexDataHeavy) * nodeMes->getVerticesCountInMeshBuffer(meshBufferIndex) options:MTLResourceOptionCPUCacheModeDefault];
        if(buf)
            [MTLNode->VertexBuffers addObject:buf];
    }
}

void MetalRenderManager::writeImageToFile(Texture *texture , char* filePath,IMAGE_FLIP flipType)
{
    int bytesPerPix = 4;
    size_t imageSize = bytesPerPix * size_t(texture->width) * size_t(texture->height);
    uint8_t * buffer = new uint8_t[imageSize];
    
    MTLRegion region = MTLRegionMake2D(0,0,texture->width,texture->height);
    [((MTLTexture*)texture)->texture getBytes:&buffer[0] bytesPerRow:texture->width * bytesPerPix fromRegion:region mipmapLevel:0];
    
    int bytesPerRow = texture->width * bytesPerPix;
    
    for(int r = 0;r < texture->height;r++){
        for(int c = 0;c < (texture->width);c++){
            int pixelPos = (r * bytesPerRow) + (c * bytesPerPix);
            uint8_t R = buffer[pixelPos + 2];
            uint8_t G = buffer[pixelPos + 1];
            uint8_t B = buffer[pixelPos];
            uint8_t A = buffer[pixelPos + 3];
            buffer[pixelPos + 2] = B;
            buffer[pixelPos + 1] = G;
            buffer[pixelPos] = R;
            buffer[pixelPos + 3] = A;
        }
    }
#ifndef IOS
    PNGFileManager::write_png_file(filePath, buffer, texture->width, texture->height);
#else
    writePNGImage(buffer,texture->width,texture->height,filePath);
#endif
    
    delete buffer;
}

int MetalRenderManager::getMTLDrawMode(DRAW_MODE mode)
{
    switch(mode){
        case DRAW_MODE_POINTS:
            return MTLPrimitiveTypePoint;
        case DRAW_MODE_LINES:
            return MTLPrimitiveTypeLine;
        case DRAW_MODE_LINE_LOOP:
            return MTLPrimitiveTypeLine;
        case DRAW_MODE_LINE_STRIP:
            return MTLPrimitiveTypeLineStrip;
        case DRAW_MODE_TRIANGLES:
            return MTLPrimitiveTypeTriangle;
        case DRAW_MODE_TRIANGLE_STRIP:
            return MTLPrimitiveTypeTriangleStrip;
        case DRAW_MODE_TRIANGLE_FAN:
            return MTLPrimitiveTypeTriangle;
        default:
            return MTLPrimitiveTypeTriangle;
    }
}

#endif