//
//  OBJMeshFileLoader.h
//  SGEngine2
//
//  Created by Vivek shivam on 30/03/2015.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __C_OBJ_MESH_FILE_LOADER_H_INCLUDED__
#define __C_OBJ_MESH_FILE_LOADER_H_INCLUDED__

#include "../Core/common/common.h"
#include "../Core/Meshes/SkinMesh.h"

#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

class OBJMeshFileLoader {
public:

    OBJMeshFileLoader();
    ~OBJMeshFileLoader();

    Mesh* createMesh(string fileName, int& status, DEVICE_TYPE device = OPENGLES2);

private:
    const aiScene* scene = NULL;
    Assimp::Importer *importer = NULL;
};

#endif
