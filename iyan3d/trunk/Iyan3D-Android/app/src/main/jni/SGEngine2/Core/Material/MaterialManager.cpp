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
MaterialManager::MaterialManager(DEVICE_TYPE deviceType){
    this->deviceType = deviceType;
    materials = new vector<Material*>();
}
MaterialManager::~MaterialManager(){
    RemoveAllMaterials();
}

short MaterialManager::CreateMaterial(string MaterialName,string vShaderName,string fShaderName,bool isDepthPass){
    Material *newMat = NULL;

#ifdef ANDROID
    LOGI("Material initialize");
    newMat = new OGLMaterial();
      ((OGLMaterial*)newMat)->LoadShaders(vShaderName,fShaderName);
    #elif IOS
    if(deviceType == METAL){
        #if !(TARGET_IPHONE_SIMULATOR)
        newMat = LoadMetalShaders(vShaderName,fShaderName,isDepthPass);
        #endif
    }
    else if(deviceType == OPENGLES2){
        newMat = new OGLMaterial();
        ((OGLMaterial*)newMat)->LoadShaders(vShaderName,fShaderName);
    }
    #endif
    if(!newMat){
        Logger::log(ERROR,"MaterialManager","Null Material");
        return -1;
    }

    newMat->name = MaterialName;
    materials->push_back(newMat);
    return (materials->size() - 1);
}
void MaterialManager::RemoveAllMaterials(){
    for(int i = 0;i < materials->size();i++) {
        if((*materials)[i])
        delete (*materials)[i];
    }
    materials->clear();
    delete materials;
}
