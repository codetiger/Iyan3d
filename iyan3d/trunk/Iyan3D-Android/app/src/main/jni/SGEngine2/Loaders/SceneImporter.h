//
//  ObjectImporter.hpp
//  Iyan3D
//
//  Created by sabish on 28/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef ObjectImporter_hpp
#define ObjectImporter_hpp

#include <stdio.h>

#include "../Core/common/common.h"
#include "../Core/Nodes/MeshNode.h"
#include "../Core/Nodes/AnimatedMeshNode.h"
#include "../Core/Meshes/SkinMesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

class SceneImporter {
public:
    
    SceneImporter();
    ~SceneImporter();
    
    vector< shared_ptr<Node> > importNodesFromFile(string fileName);
    
private:
    const aiScene* scene = NULL;
    Assimp::Importer *importer = NULL;

    void loadJoints(aiMesh *aiM, SkinMesh *m, aiNode* aiN, Joint* parent);
    void loadNodes(vector< shared_ptr<Node> > *nodes, aiNode *n);
    SkinMesh* getSkinMeshFrom(aiMesh *aiM, aiNode*aiN);
    Mesh *getMeshFrom(aiMesh *aiM);
};

#endif /* ObjectImporter_hpp */
