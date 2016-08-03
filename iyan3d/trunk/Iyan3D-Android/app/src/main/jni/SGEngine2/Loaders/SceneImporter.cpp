//
//  SceneImporter.cpp
//  Iyan3D
//
//  Created by sabish on 28/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "SceneImporter.h"

SceneImporter::SceneImporter()
{
    scene = NULL;
    importer = new Assimp::Importer();
}

SceneImporter::~SceneImporter()
{
    delete importer;
}

void SceneImporter::importNodesFromFile(string filepath, SGEditorScene *sgScene)
{
    scene = importer->ReadFile(filepath, aiProcessPreset_TargetRealtime_Fast);
    if(!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return;
    }
    
    loadNodes(scene->mRootNode, sgScene);
    return;
}

void SceneImporter::loadNodes(aiNode *n, SGEditorScene *sgScene)
{
    if (n->mNumMeshes) {
        for (int i = 0; i < n->mNumMeshes; i++) {
            aiMesh *aiM = scene->mMeshes[n->mMeshes[i]];
            if(aiM->HasBones()) {
                shared_ptr<AnimatedMeshNode> sgn = make_shared<AnimatedMeshNode>();

                Mat4 mat = Mat4();
                for (int j = 0; j < 16; j++)
                    mat.setElement(j, *n->mTransformation[j]);
                
                sgn->setPosition(mat.getTranslation());
                sgn->setRotation(mat.getRotation());
                sgn->setScale(mat.getScale());
                sgn->updateAbsoluteTransformation();
                
                aiMaterial *material = scene->mMaterials[aiM->mMaterialIndex];
                if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                    aiString path;
                    material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
                    printf("Texture: %s\n", path.C_Str());
                }
                
                aiColor4D color;
                if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                }
            } else {
                shared_ptr<MeshNode> sgn = make_shared<MeshNode>();
                sgn->mesh = getMeshFrom(aiM);

                sgn->setScale(Vector3(1.0));
                sgn->updateAbsoluteTransformation();
            }
        }
    } else {
        for (int i = 0; i < n->mNumChildren; i++) {
            loadNodes(n->mChildren[i], sgScene);
        }
    }
}

SkinMesh* SceneImporter::getSkinMeshFrom(aiMesh *aiM, aiNode* aiN)
{
    SkinMesh* mesh = new SkinMesh();
    
    for (int i = 0; i < aiM->mNumVertices; i++) {
        vertexDataHeavy vd;
        vd.vertPosition = Vector3(aiM->mVertices[i].x, aiM->mVertices[i].y, aiM->mVertices[i].z);
        vd.vertNormal = Vector3(aiM->mNormals[i].x, aiM->mNormals[i].y, aiM->mNormals[i].z);
        
        if(aiM->mColors[0])
            vd.optionalData1 = Vector4(aiM->mColors[0][i].r, aiM->mColors[0][i].g, aiM->mColors[0][i].b, aiM->mColors[0][i].a);
        else
            vd.optionalData1 = Vector4(-1.0);
        
        if(aiM->mTextureCoords[0])
            vd.texCoord1 = Vector2(aiM->mTextureCoords[0][i].x, aiM->mTextureCoords[0][i].y);
        else
            vd.texCoord1 = Vector2(0.0, 0.0);
        
        mesh->addHeavyVertex(&vd);
    }
    
    for (int i = 0; i < aiM->mNumFaces; i++) {
        aiFace face = aiM->mFaces[i];
        
        for (int j = 0; j < face.mNumIndices; j++) {
            mesh->addToIndicesArray(face.mIndices[j]);
        }
    }
    
    loadJoints(aiM, mesh, aiN, NULL);
    printf("JointCount: %d\n", (int)mesh->joints->size());
    mesh->finalize();
    mesh->Commit();
    return mesh;
}

void SceneImporter::loadJoints(aiMesh *aiM, SkinMesh *m, aiNode* aiN, Joint* parent)
{
    for (int i = 0; i < aiM->mNumBones; i++) {
        if(aiM->mBones[i]->mName == aiN->mName) {
            Joint* sgBone = m->addJoint(parent);
            
            aiBone* b = aiM->mBones[i];
            for (int j = 0; j < b->mNumWeights; j++) {
                shared_ptr<PaintedVertex> pvInfo = make_shared<PaintedVertex>();
                pvInfo->vertexId = b->mWeights[j].mVertexId;
                pvInfo->weight = b->mWeights[j].mWeight;
                sgBone->PaintedVertices->push_back(pvInfo);
            }
            
//            for (int j = 0; j < 16; j++)
//                sgBone->LocalAnimatedMatrix.setElement(j, *b->mOffsetMatrix[j]);

            sgBone->LocalAnimatedMatrix = Mat4();

            for (int j = 0; j < aiN->mNumChildren; j++) {
                loadJoints(aiM, m, aiN->mChildren[j], sgBone);
            }
            return;
        }
    }

    for (int j = 0; j < aiN->mNumChildren; j++) {
        loadJoints(aiM, m, aiN->mChildren[j], parent);
    }
}

Mesh* SceneImporter::getMeshFrom(aiMesh *aiM)
{
    Mesh* mesh = new Mesh();
    
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
        
        mesh->addVertex(&vd);
    }
    
    for (int j = 0; j < aiM->mNumFaces; j++) {
        aiFace face = aiM->mFaces[j];
        
        for (int k = 0; k < face.mNumIndices; k++) {
            mesh->addToIndicesArray(face.mIndices[k]);
        }
    }
    
    mesh->Commit();
    return mesh;
}