//
//  OBJMeshFileLoader.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 30/03/2015.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "OBJMeshFileLoader.h"
#include "../../Iyan3dEngineFiles/HeaderFiles/Constants.h"

#ifdef  ANDROID
//#include "../../../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-x86_64/usr/include/string.h"
//#include "../../../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-x86_64/usr/include/math.h"
#endif

OBJMeshFileLoader::OBJMeshFileLoader()
{
    scene = NULL;
    importer = new Assimp::Importer();
}

OBJMeshFileLoader::~OBJMeshFileLoader()
{
    delete importer;
}

Mesh* OBJMeshFileLoader::createMesh(string filepath, int& status, DEVICE_TYPE device)
{
    scene = importer->ReadFile(filepath, aiProcessPreset_TargetRealtime_Quality);
    
    if(!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return 0;
    }

    printf("Scene Loaded, Node count: %d\n", scene->mNumMaterials);
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMaterial *mat = scene->mMaterials[i];
//        printf("Materials: %s\n", mat->);
        
    }
    

    Mesh *m = new Mesh();
    unsigned int count = 0;
    
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *aiM = scene->mMeshes[i];
        
        for (int j = 0; j < aiM->mNumVertices; j++) {
            vertexData vd;
            vd.vertPosition = Vector3(aiM->mVertices[j].x, aiM->mVertices[j].y, aiM->mVertices[j].z);
            vd.vertNormal = Vector3(aiM->mNormals[j].x, aiM->mNormals[j].y, aiM->mNormals[j].z);

            if(aiM->mColors[0])
                vd.optionalData1 = Vector4(aiM->mColors[0][j].r, aiM->mColors[0][j].g, aiM->mColors[0][j].b, aiM->mColors[0][j].a);
            else
                vd.optionalData1 = Vector4(-1.0);
            
            if(aiM->mTextureCoords[0])
                vd.texCoord1 = Vector2(aiM->mTextureCoords[0][j].x, aiM->mTextureCoords[0][j].y);
            else
                vd.texCoord1 = Vector2(0.0, 0.0);
            
            m->addVertex(&vd);
        }
        
        for (int j = 0; j < aiM->mNumFaces; j++) {
            aiFace face = aiM->mFaces[j];
            
            for (int k = 0; k < face.mNumIndices; k++) {
                m->addToIndicesArray(face.mIndices[k] + count);
            }
        }
        count += aiM->mNumVertices;
    }
    
    m->Commit();
    return m;
}
