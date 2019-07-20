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

class MaterialManager {
public:
    vector<Material*>* materials;

    MaterialManager();
    ~MaterialManager();
    bool CreateMaterial(string MaterialNamel, string vShaderName, string fShaderName, std::map<string, string> shadersStr, bool isDepthPass = false, bool isTest = false);
    void RemoveAllMaterials();
};

#endif /* defined(__SGEngine2__MaterialManager__) */
