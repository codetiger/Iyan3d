//
//  MetalWrapper.h
//  SGEngine2
//
//  Created by Vivek on 09/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef SGEngine2_MetalWrapper_h
#define SGEngine2_MetalWrapper_h
#include "../Core/Nodes/Node.h"
#include "../SceneManager/SceneManager.h"

void* initMetalRenderManager(void *renderView,float screenWidth,float screenHeight,float screenScale);
void* initMTLTexture();
Material* LoadMetalShaders(string vertexFuncName,string fragFuncName,bool isDepthPass = false);
#endif
