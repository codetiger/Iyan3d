//
//  OGLES2RenderManager.cpp
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
#ifdef IOS
#import "MetalHandler.h"
#import "MTLNodeData.h"
#include "MeshNode.h"
#include "SceneManager.h"
#include "MTLMaterial.h"

static id <MTLDevice> device;
static CAMetalLayer* MTLLayer;
static id <MTLLibrary> MTLLib;


void MetalHandler::setDevice(id <MTLDevice> metalDevice){
    device = metalDevice;
}
id <MTLDevice> MetalHandler::getMTLDevice(){
    if(!device)
        Logger::log(ERROR,"MetalHandler" , "Metal Device is null,not from metal view");
    return device;
}
void MetalHandler::setMTLLayer(CAMetalLayer* mtlLayer){
    MTLLayer = mtlLayer;
}
void MetalHandler::setMTLLibrary(id <MTLLibrary> mtlLib){
    MTLLib = mtlLib;
}
Material* LoadMetalShaders(string vertexFuncName,string fragFuncName,bool isDepthPass){
    
    Material *newMat = new MTLMaterial();
    NSString *vertFuncName = [NSString stringWithUTF8String:vertexFuncName.c_str()];
    NSString *pixelFuncName = [NSString stringWithUTF8String:fragFuncName.c_str()];
    
    id <MTLFunction> vertexProgram = [MTLLib newFunctionWithName:vertFuncName];
    id <MTLFunction> fragmentProgram = [MTLLib newFunctionWithName:pixelFuncName];
    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = [NSString stringWithFormat:@"%s",vertexFuncName.c_str()];
    [pipelineStateDescriptor setSampleCount:(isDepthPass)?1:ANTI_ALIASING_SAMPLE_COUNT];
    [pipelineStateDescriptor setFragmentFunction:(isDepthPass)?nil:fragmentProgram];
    [pipelineStateDescriptor setVertexFunction:vertexProgram];
    if(!isDepthPass){
//        finalBlend = alpha_source * source_RGB + (1 - alpha_source) * dest_color
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineStateDescriptor.colorAttachments[0].blendingEnabled = YES;
        pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        if([vertFuncName isEqualToString:@"Particle_Vertex"])
            pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        else
            pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
        
        if([vertFuncName isEqualToString:@"Particle_Vertex"])
            pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
        else
            pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        
        pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorDestinationAlpha;
    }
    pipelineStateDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    NSError* error = NULL;
    //printf("\n PipelineStatenIdex %s %s ",vertexFuncName.c_str(),fragFuncName.c_str());
    ((MTLMaterial*)newMat)->PipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
    if (!((MTLMaterial*)newMat)->PipelineState){
        Logger::log(ERROR,"MetalHandler","Failed to create pipeline state " + vertexFuncName);
        delete newMat;
        return NULL;
    }
    return newMat;
}

#endif