//
//  MaterialManager
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "MaterialManager.h"
#import "TargetConditionals.h"
#include "../../RenderManager/MetalWrapper.h"

MaterialManager::MaterialManager() {
    materials = new vector<Material*>();
}

MaterialManager::~MaterialManager() {
    RemoveAllMaterials();
    delete materials;
}

bool MaterialManager::CreateMaterial(string MaterialName, string vShaderName, string fShaderName, std::map<string, string> shadersStr, bool isDepthPass, bool isTest) {
    Material* newMat = LoadMetalShaders(vShaderName, fShaderName, isDepthPass);

    if (!newMat) {
        Logger::log(ERROR, "MaterialManager", "Null Material");
        return -1;
    }

    newMat->name = MaterialName;
    materials->push_back(newMat);
    return true;
}

void MaterialManager::RemoveAllMaterials() {
    for (int i = 0; i < materials->size(); i++) {
        if ((*materials)[i])
            delete (*materials)[i];
    }
    materials->clear();
}
