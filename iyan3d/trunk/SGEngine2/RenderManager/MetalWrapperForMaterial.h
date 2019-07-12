//
//  MetalWrapper.h
//  SGEngine2
//
//  Created by Vivek on 09/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
#ifndef SGEngine2_MetalWrapperForMaterial_h
#define SGEngine2_MetalWrapperForMaterial_h
#include "Material.h"
Material* LoadMetalShaders(std::string vertexFuncName,std::string fragFuncName,bool isDepthPass);
#endif
