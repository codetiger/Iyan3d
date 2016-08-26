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
#include "SGEditorScene.h"

#include "CSGRMeshFileLoader.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

class SceneImporter {
public:
    
    SceneImporter();
    ~SceneImporter();
    
    void importNodesFromFile(SGEditorScene *sgScene, string name, string filePath, string fileLocation, bool hasMeshColor, Vector3 meshColor, bool isTempNode);
    void importNodeFromMesh(SGEditorScene *sgScene, SGNode* sgNode, Mesh* lMesh);
    void import3DText(SGEditorScene *sgScene, wstring text, string fontPath, int bezierSegments, float extrude, float bevelRadius, int bevelSegments, bool hasBones, bool isTempNode);
    
private:
    const aiScene* scene = NULL;

    void loadNodes(SGEditorScene *sgScene, string path, bool isTempNode, string ext, bool hasMeshColor = false, Vector3 mColor = Vector3(1.0));
    void loadBonesFromMesh(aiMesh *aiM, SkinMesh *m, map< string, Joint*> *bones);
    void loadBoneHierarcy(SkinMesh *m, map< string, Joint*> *bones);

    void getSkinMeshFrom(vector<vertexDataHeavy> &mbvd, vector<unsigned short> &mbi, aiMesh *aiM);
    void getMeshFrom(vector<vertexData> &mbvd, vector<unsigned short> &mbi, aiMesh *aiM);
};

#endif /* ObjectImporter_hpp */
