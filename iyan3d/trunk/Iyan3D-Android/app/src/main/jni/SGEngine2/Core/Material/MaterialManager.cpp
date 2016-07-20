//
//  MaterialManager
//  SGEngine2
//
//  Created by Vivek on 07/02/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "MaterialManager.h"

#ifdef IOS
#import "TargetConditionals.h"
#include "../../RenderManager/MetalWrapper.h"
#elif ANDROID
#include "../../../opengl.h"
#endif

MaterialManager::MaterialManager(DEVICE_TYPE deviceType)
{
    this->deviceType = deviceType;
    materials = new vector<Material*>();
}

MaterialManager::~MaterialManager()
{
    RemoveAllMaterials();
    delete materials;
}

bool MaterialManager::CreateMaterial(string MaterialName,string vShaderName,string fShaderName, std::map< string, string > shadersStr, bool isDepthPass, bool isTest)
{
    Material *newMat = NULL;
    bool status;
#ifdef ANDROID
    LOGI("Material initialize");
    newMat = new OGLMaterial();
      status = ((OGLMaterial*)newMat)->LoadShaders(vShaderName,fShaderName,shadersStr);
      if(!status && isTest)
        return false;
    #elif IOS
    if(deviceType == METAL){
        #if !(TARGET_IPHONE_SIMULATOR)
        newMat = LoadMetalShaders(vShaderName,fShaderName,isDepthPass);
        #endif
    }
    else if(deviceType == OPENGLES2){
        newMat = new OGLMaterial();
        status = ((OGLMaterial*)newMat)->LoadShaders(vShaderName,fShaderName, shadersStr);
        if(!status && isTest)
            return false;
    }
    #endif
    if(!newMat){
        Logger::log(ERROR,"MaterialManager","Null Material");
        return -1;
    }

    newMat->name = MaterialName;
    materials->push_back(newMat);
    return true;
}

void MaterialManager::RemoveAllMaterials()
{
    for(int i = 0;i < materials->size();i++) {
        if((*materials)[i])
            delete (*materials)[i];
    }
    materials->clear();
}
