//
//  MaterialManager.h
//  SGEngine2
//
//  Created by Vivek on 07/02/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MaterialManager__
#define __SGEngine2__MaterialManager__

#include "../../RenderManager/MetalWrapperForMaterial.h"
#include "../../Core/Textures/Texture.h"
#include "../../Core/Nodes/MeshNode.h"
#include "../../Core/Nodes/Node.h"
#include "../../Core/common/common.h"
#include "Material.h"

#ifndef UBUNTU
#include "../../Core/Textures/OGLTexture.h"
#endif

class MaterialManager {
public:
    DEVICE_TYPE deviceType;
    vector<Material*> *materials;
    
    MaterialManager(DEVICE_TYPE deviceType);
    ~MaterialManager();
    bool CreateMaterial(string MaterialNamel,string vShaderName,string fShaderName, std::map< string, string > shadersStr, bool isDepthPass = false, bool isTest = false);
    void RemoveAllMaterials();
    void clearMaterials();
};

#endif /* defined(__SGEngine2__MaterialManager__) */
