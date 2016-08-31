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
}

SceneImporter::~SceneImporter()
{
    if(scene)
        delete scene;
}

string getFileExtention(const string& s)
{
    
    char sep = '.';
    
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(i+1, s.length() - i));
    }
    
    return("");
}

string getFileName(const string& s)
{
    
    char sep = '/';
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(i+1, s.length() - i));
    }

    sep = '\\';
    i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(i+1, s.length() - i));
    }
    
    return s;
}

Mat4 AssimpToMat4(aiMatrix4x4 assimpMatrix)
{
    Mat4 mtrix;
    
    mtrix[0] = assimpMatrix.a1;
    mtrix[1] = assimpMatrix.b1;
    mtrix[2] = assimpMatrix.c1;
    mtrix[3] = assimpMatrix.d1;
    
    mtrix[4] = assimpMatrix.a2;
    mtrix[5] = assimpMatrix.b2;
    mtrix[6] = assimpMatrix.c2;
    mtrix[7] = assimpMatrix.d2;
    
    mtrix[8] = assimpMatrix.a3;
    mtrix[9] = assimpMatrix.b3;
    mtrix[10] = assimpMatrix.c3;
    mtrix[11] = assimpMatrix.d3;
    
    mtrix[12] = assimpMatrix.a4;
    mtrix[13] = assimpMatrix.b4;
    mtrix[14] = assimpMatrix.c4;
    mtrix[15] = assimpMatrix.d4;
    
    return mtrix;
}

void SceneImporter::import3DText(SGEditorScene *sgScene, wstring text, string fontPath, int bezierSegments, float extrude, float bevelRadius, int bevelSegments, bool hasBones, bool isTempNode)
{
    sgScene->freezeRendering = true;
    
    Assimp::Importer *importer = new Assimp::Importer();
    importer->SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MAX_VERTICES_COUNT);
    importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 65);
    
    importer->SetPropertyWString("TEXT3D_TEXT", text);
    importer->SetPropertyBool("TEXT3D_HASBONES", hasBones);
    importer->SetPropertyString("TEXT3D_FONTPATH", fontPath);
    importer->SetPropertyInteger("TEXT3D_BEZIERSTEPS", bezierSegments);
    importer->SetPropertyFloat("TEXT3D_EXTRUDE", extrude);
    importer->SetPropertyFloat("TEXT3D_BEVELRADIUS", bevelRadius);
    importer->SetPropertyInteger("TEXT3D_BEVELSEGMENTS", bevelSegments);
    
    importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    
    scene = importer->ReadFile(fontPath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes);
    
    if(!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return;
    }
    
    loadNodes2Scene(sgScene, fontPath, isTempNode, "text", true);
    delete importer;
    scene = NULL;
    
    sgScene->freezeRendering = false;
    
}

void SceneImporter::importNodesFromFile(SGEditorScene *sgScene, string name, string filePath, string fileLocation, bool hasMeshColor, Vector3 meshColor, bool isTempNode)
{
    sgScene->freezeRendering = true;
    string ext = getFileExtention(filePath);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if(ext == "sgm" || ext == "sgr" || ext == "obj" || ext == "fbx" || ext == "dae" || ext == "3ds") {
        Assimp::Importer *importer = new Assimp::Importer();
        importer->SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MAX_VERTICES_COUNT);
        importer->SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8);
        
        unsigned int pFlags = aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs;
        
        scene = importer->ReadFile(filePath, pFlags);
        
        if(!scene || scene->mNumMeshes == 0) {
            printf("Error in Loading: %s\n", importer->GetErrorString());
            return;
        } else {
            loadNodes2Scene(sgScene, fileLocation, isTempNode, ext, hasMeshColor, meshColor);
        }
        
        delete importer;
        scene = NULL;
    }
    
    sgScene->freezeRendering = false;
}

void SceneImporter::importNodeFromMesh(SGEditorScene *sgScene, SGNode* sceneNode, Mesh* lMesh)
{
    sgScene->freezeRendering = true;
    
    if(sceneNode->getType() == NODE_RIG || sceneNode->getType() == NODE_TEXT_SKIN) {
        
        SkinMesh *mesh = (SkinMesh*)lMesh;
        //sceneNode->setSkinningData(mesh);
        
        shared_ptr<AnimatedMeshNode> sgn = sgScene->getSceneManager()->createAnimatedNodeFromMesh(mesh, "setUniforms", ShaderManager::maxJoints,  CHARACTER_RIG, MESH_TYPE_HEAVY);
        sceneNode->node = sgn;
        bool isSGJointsCreated = (sceneNode->joints.size() > 0) ? true : false;
        int jointsCount = dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->getJointCount();
        
        for(int i = 0;i < jointsCount;i++){
            dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->getJointNode(i)->setID(i);
            if(!isSGJointsCreated){
                SGJoint *joint = new SGJoint();
                joint->jointNode = dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->getJointNode(i);
                sceneNode->joints.push_back(joint);
            }
        }
        
        sgn->setMaterial(sgScene->getSceneManager()->getMaterialByIndex(SHADER_SKIN));
        sceneNode->setInitialKeyValues(OPEN_SAVED_FILE);
        sgScene->loader->setJointsScale(sceneNode);
        dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->updateMeshCache();
        
    } else { //TODO for all other types
        
        Mesh *mesh = lMesh;
        shared_ptr<MeshNode> sgn = sgScene->getSceneManager()->createNodeFromMesh(mesh, "setUniforms", MESH_TYPE_LITE, SHADER_MESH);
        sceneNode->node = sgn;
        sceneNode->setInitialKeyValues(OPEN_SAVED_FILE);
    }
    
    sceneNode->actionId = ++sgScene->actionObjectsSize;
    
    sceneNode->node->setID(sgScene->assetIDCounter++);
    sgScene->selectMan->removeChildren(sgScene->getParentNode());
    sgScene->updater->setDataForFrame(sgScene->currentFrame);
    sgScene->selectMan->updateParentPosition();
    sgScene->updater->resetMaterialTypes(false);
    
    sgScene->freezeRendering = false;
    
}

Mesh* SceneImporter::loadMeshFromFile(string filePath)
{
    Assimp::Importer *importer = new Assimp::Importer();
    scene = importer->ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes);
    
    if(!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return NULL;
    }
    
    Mesh* mesh = new Mesh();
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *aiM = scene->mMeshes[i];
        
        if(aiM && aiM->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
            vector< vertexData > mbvd;
            vector< unsigned short > mbi;
            getMeshFrom(mbvd, mbi, aiM);
            
            mesh->addMeshBuffer(mbvd, mbi, 0);
        }
    }
    
    delete importer;
    scene = NULL;
    return mesh;
}

SkinMesh* SceneImporter::loadSkinMeshFromFile(string filePath)
{
    Assimp::Importer *importer = new Assimp::Importer();
    importer->SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MAX_VERTICES_COUNT);
    scene = importer->ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes);
    
    if(!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return NULL;
    }
    
    map< string, Joint* > *bones = new map< string, Joint* >();
    
    SkinMesh* mesh = new SkinMesh();
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *aiM = scene->mMeshes[i];
        
        if(aiM && aiM->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
            vector< vertexDataHeavy > mbvd;
            vector< unsigned short > mbi;
            getSkinMeshFrom(mbvd, mbi, aiM);
            
            mesh->addMeshBuffer(mbvd, mbi, 0);
            
            if(aiM->HasBones())
                loadBonesFromMesh(aiM, mesh, bones);
        }
    }
    loadBoneHierarcy((SkinMesh*)mesh, bones);
    mesh->finalize();
    
    delete importer;
    scene = NULL;
    return mesh;
}

void SceneImporter::loadNodes2Scene(SGEditorScene *sgScene, string folderPath, bool isTempNode, string ext, bool hasMeshColor, Vector3 mColor)
{
    Mesh* mesh;
    map< string, Joint* > *bones = new map< string, Joint* >();
    bool hasBones = false;
    
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *aiM = scene->mMeshes[i];
        if(!hasBones)
            hasBones = aiM->HasBones();
    }
    
    NODE_TYPE nType;
    if(ext == "text")
        nType = (hasBones) ? NODE_TEXT_SKIN : NODE_TEXT;
    else
        nType = (hasBones) ? NODE_RIG : NODE_SGM;
    
    SGNode *sceneNode = new SGNode(nType);
    sceneNode->isTempNode = isTempNode;
    
    if(hasBones)
        mesh = new SkinMesh();
    else
        mesh = new Mesh();
    
    for (int i = 0; i < scene->mNumMaterials; i++) {
        MaterialProperty *materialProps = new MaterialProperty(hasBones ? NODE_RIG : NODE_SGM);
        sceneNode->materialProps.push_back(materialProps);
        
        aiMaterial *material = scene->mMaterials[i];
        
        Property &p2 = sceneNode->getProperty(IS_VERTEX_COLOR, i);
        p2.value.x = 1.0;

        Property &p3 = sceneNode->getProperty(TEXTURE, i);
        p3.fileName = "";

        materialProps->setTextureForType(NULL, NODE_TEXTURE_TYPE_COLORMAP);

        Property &p1 = sceneNode->getProperty(VERTEX_COLOR, i);
        aiColor4D color;
        if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
            p1.value = Vector4(color.r, color.g, color.b, color.a);
        else if(hasMeshColor)
            p1.value = Vector4(mColor.x, mColor.y, mColor.z, 1.0);
        else
            p1.value = Vector4(0.5, 0.5, 0.5, 1.0);
        
        if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

            string texturePath = getFileName(string(path.data));

            Texture* texture = sgScene->getSceneManager()->loadTexture(texturePath, folderPath + texturePath, TEXTURE_RGBA8, TEXTURE_BYTE, true);
            if(texture) {
                Property &p1 = sceneNode->getProperty(TEXTURE, i);
                p1.fileName = texturePath;
                
                Property &p2 = sceneNode->getProperty(IS_VERTEX_COLOR, i);
                p2.value.x = 0.0;
                
                materialProps->setTextureForType(texture, NODE_TEXTURE_TYPE_COLORMAP);
            } else {
                printf("Loading Texture Failed: %s\n", texturePath.c_str());
            }
        }
        
        if(material->GetTextureCount(aiTextureType_NORMALS) > 0) {
            aiString path;
            material->GetTexture(aiTextureType_NORMALS, 0, &path);
            string texturePath = getFileName(string(path.data));
            
            Texture* texture = sgScene->getSceneManager()->loadTexture(texturePath, folderPath + texturePath, TEXTURE_RGBA8, TEXTURE_BYTE, true);
            if(texture) {
                Property &p1 = sceneNode->getProperty(TEXTURE, i);
                p1.fileName = texturePath;
                materialProps->setTextureForType(texture, NODE_TEXTURE_TYPE_NORMALMAP);
            }
        }
    }
    
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *aiM = scene->mMeshes[i];
        
        if(!aiM || aiM->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
            continue;
        
        unsigned short materialIndex = aiM->mMaterialIndex;
        
        if(hasBones) {
            
            vector< vertexDataHeavy > mbvd;
            vector< unsigned short > mbi;
            getSkinMeshFrom(mbvd, mbi, aiM);
            mesh->addMeshBuffer(mbvd, mbi, materialIndex);
            
            if(aiM->HasBones())
                loadBonesFromMesh(aiM, (SkinMesh*)mesh, bones);
            
        } else {
            
            vector< vertexData > mbvd;
            vector< unsigned short > mbi;
            getMeshFrom(mbvd, mbi, aiM);
            mesh->addMeshBuffer(mbvd, mbi, materialIndex);
        }

        Property &p1 = sceneNode->getProperty(VERTEX_COLOR, materialIndex);
        if(aiM->mColors[0])
            p1.value = Vector4(-1.0);
    }
    
    shared_ptr<Node> sgn;
    
    if(sceneNode->getType() == NODE_RIG || sceneNode->getType() == NODE_TEXT_SKIN) {
        loadBoneHierarcy((SkinMesh*)mesh, bones);
        ((SkinMesh*)mesh)->finalize();
        sceneNode->setSkinningData((SkinMesh*)mesh);
        
        sgn = sgScene->getSceneManager()->createAnimatedNodeFromMesh((SkinMesh*)mesh, "setUniforms", ShaderManager::maxJoints,  CHARACTER_RIG, MESH_TYPE_HEAVY);
        sceneNode->node = sgn;
        
        bool isSGJointsCreated = (sceneNode->joints.size() > 0) ? true : false;
        int jointsCount = dynamic_pointer_cast<AnimatedMeshNode>(sgn)->getJointCount();
        
        for(int i = 0;i < jointsCount;i++) {
            dynamic_pointer_cast<AnimatedMeshNode>(sgn)->getJointNode(i)->setID(i);
            if(!isSGJointsCreated) {
                SGJoint *joint = new SGJoint();
                joint->jointNode = dynamic_pointer_cast<AnimatedMeshNode>(sgn)->getJointNode(i);
                sceneNode->joints.push_back(joint);
            }
        }
        
        sgn->setScale(Vector3(1.0));
        sgn->updateAbsoluteTransformation();
        sgn->setMaterial(sgScene->getSceneManager()->getMaterialByIndex((sceneNode->getType() == NODE_RIG) ? SHADER_SKIN : SHADER_TEXT_SKIN));
        
    } else {
        sgn = sgScene->getSceneManager()->createNodeFromMesh(mesh, "setUniforms");
        sceneNode->node = sgn;
        
        sgn->setScale(Vector3(1.0));
        sgn->updateAbsoluteTransformation();
        sgn->setMaterial(sgScene->getSceneManager()->getMaterialByIndex(SHADER_MESH));
    }
    
    Quaternion r1 = Quaternion();
    Quaternion r2 = Quaternion();
    
    if(ext == "dae") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 0.0, 1.0));
        r2.fromAngleAxis(M_PI_2, Vector3(1.0, 0.0, 0.0));
    } else if(ext == "fbx") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 0.0, 1.0));
        if(hasBones)
            r2.fromAngleAxis(M_PI, Vector3(1.0, 0.0, 0.0));
        else
            r2.fromAngleAxis(M_PI_2, Vector3(1.0, 0.0, 0.0));
    } else if(ext == "obj") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 0.0, 1.0));
        r2.fromAngleAxis(M_PI, Vector3(1.0, 0.0, 0.0));
    } else if(ext == "sgm" || ext == "sgr") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 1.0, 0.0));
        //            r2.fromAngleAxis(M_PI, Vector3(1.0, 0.0, 0.0));
    }
    
    Quaternion r = r1 * r2;
    sceneNode->setRotation(r, 0);
    sgn->setRotation(r);
    
    sceneNode->setInitialKeyValues(IMPORT_ASSET_ACTION);
    if(sceneNode->getType() == NODE_RIG || sceneNode->getType() == NODE_TEXT_SKIN) {
        sgScene->loader->setJointsScale(sceneNode);
        dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->updateMeshCache();
    }
    
    sceneNode->name = ConversionHelper::getWStringForString(string(scene->mRootNode->mName.C_Str()));
    
    sceneNode->node = sgn;
    sceneNode->actionId = ++sgScene->actionObjectsSize;
    sceneNode->node->setID(sgScene->assetIDCounter++);
    
    sgScene->nodes.push_back(sceneNode);

    sgScene->selectMan->removeChildren(sgScene->getParentNode());
    sgScene->updater->setDataForFrame(sgScene->currentFrame);
    sgScene->selectMan->updateParentPosition();
    sgScene->updater->resetMaterialTypes(false);
        
    bones->clear();
    delete bones;
}

void SceneImporter::getSkinMeshFrom(vector<vertexDataHeavy> &mbvd, vector<unsigned short> &mbi, aiMesh *aiM)
{
    for (int j = 0; j < aiM->mNumVertices; j++) {
        vertexDataHeavy vd;
        vd.vertPosition = Vector3(aiM->mVertices[j].x, aiM->mVertices[j].y, aiM->mVertices[j].z);
        
        if(aiM->mNormals)
            vd.vertNormal = Vector3(aiM->mNormals[j].x, aiM->mNormals[j].y, aiM->mNormals[j].z);
        else
            vd.vertNormal = Vector3(0.0);
        
        if(aiM->mTangents)
            vd.vertTangent = Vector3(aiM->mTangents[j].x, aiM->mTangents[j].y, aiM->mTangents[j].z);
        else
            vd.vertTangent = Vector3(0.0);
        
        if(aiM->mBitangents)
            vd.vertBitangent = Vector3(aiM->mBitangents[j].x, aiM->mBitangents[j].y, aiM->mBitangents[j].z);
        else
            vd.vertBitangent = Vector3(0.0);
        
        if(aiM->mColors[0])
            vd.vertColor = Vector4(aiM->mColors[0][j].r, aiM->mColors[0][j].g, aiM->mColors[0][j].b, 0.0);
        else
            vd.vertColor = Vector4(0.0);
        
        if(aiM->mTextureCoords[0])
            vd.texCoord1 = Vector2(aiM->mTextureCoords[0][j].x, aiM->mTextureCoords[0][j].y);
        else
            vd.texCoord1 = Vector2(0.0, 0.0);

        vd.optionalData1 = vd.optionalData2 = vd.optionalData3 = vd.optionalData4 = Vector4(0.0);

        mbvd.push_back(vd);
    }
    
    for (int j = 0; j < aiM->mNumFaces; j++) {
        aiFace face = aiM->mFaces[j];
        
        for (int k = 0; k < face.mNumIndices; k++) {
            mbi.push_back(face.mIndices[k]);
        }
    }
}

void SceneImporter::loadBoneHierarcy(SkinMesh *m, map< string, Joint*> *bones)
{
    typedef map< string, Joint* >::iterator it_type;
    
    map< string, Joint*> *newBones = new map< string, Joint*>();
    
    do {
        if(newBones->size() > 0) {
            bones->insert(newBones->begin(), newBones->end());
            newBones->clear();
        }
        
        for(it_type iterator = bones->begin(); iterator != bones->end(); iterator++) {
            if(!iterator->second->Parent) {
                aiNode *n = scene->mRootNode->FindNode(iterator->first.c_str());
                
                if(n && n->mParent && n->mParent != scene->mRootNode) {
                    if(bones->find(string(n->mParent->mName.C_Str())) == bones->end() &&
                       newBones->find(string(n->mParent->mName.C_Str())) == newBones->end()) {
                        Joint* sgBone = m->addJoint(NULL);
                        sgBone->name = string(n->mParent->mName.C_Str());
                        
                        iterator->second->Parent = sgBone;
                        sgBone->childJoints->push_back(iterator->second);
                        
                        newBones->insert(pair<string, Joint*>(string(n->mParent->mName.C_Str()), sgBone));
                        
                        sgBone->LocalAnimatedMatrix = AssimpToMat4(n->mParent->mTransformation);
                        sgBone->originalJointMatrix = sgBone->LocalAnimatedMatrix;
                    } else {
                        it_type parentIterator = bones->find(string(n->mParent->mName.C_Str()));
                        if(parentIterator == bones->end())
                            parentIterator = newBones->find(string(n->mParent->mName.C_Str()));
                        
                        iterator->second->Parent = parentIterator->second;
                        parentIterator->second->childJoints->push_back(iterator->second);
                    }
                }
            }
        }
    } while(newBones->size() > 0);
    
    newBones->clear();
    delete newBones;
}

void SceneImporter::loadBonesFromMesh(aiMesh *aiM, SkinMesh *m, map< string, Joint*> *bones)
{
    for (int i = 0; i < aiM->mNumBones; i++) {
        aiBone* bone = aiM->mBones[i];
        if(bones->find(string(bone->mName.C_Str())) == bones->end()) {
            Joint* sgBone = m->addJoint(NULL);
            sgBone->name = string(bone->mName.C_Str());
            
            bones->insert(pair<string, Joint*>(string(bone->mName.C_Str()), sgBone));
            
            for (int j = 0; j < bone->mNumWeights; j++) {
                shared_ptr<PaintedVertex> pvInfo = make_shared<PaintedVertex>();
                pvInfo->vertexId = bone->mWeights[j].mVertexId;
                pvInfo->weight = bone->mWeights[j].mWeight;
                pvInfo->meshBufferIndex = m->getMeshBufferCount() - 1;
                sgBone->PaintedVertices->push_back(pvInfo);
            }
            
            aiNode *n = scene->mRootNode->FindNode(bone->mName);
            sgBone->LocalAnimatedMatrix = AssimpToMat4(n->mTransformation);
            sgBone->originalJointMatrix = sgBone->LocalAnimatedMatrix;
        }
    }
}

void SceneImporter::getMeshFrom(vector<vertexData> &mbvd, vector<unsigned short> &mbi, aiMesh *aiM)
{
    for (int j = 0; j < aiM->mNumVertices; j++) {
        vertexData vd;
        vd.vertPosition = Vector3(aiM->mVertices[j].x, aiM->mVertices[j].y, aiM->mVertices[j].z);
        
        if(aiM->mNormals)
            vd.vertNormal = Vector3(aiM->mNormals[j].x, aiM->mNormals[j].y, aiM->mNormals[j].z);
        else
            vd.vertNormal = Vector3(0.0);

        if(aiM->mTangents)
            vd.vertTangent = Vector3(aiM->mTangents[j].x, aiM->mTangents[j].y, aiM->mTangents[j].z);
        else
            vd.vertTangent = Vector3(0.0);

        if(aiM->mBitangents)
            vd.vertBitangent = Vector3(aiM->mBitangents[j].x, aiM->mBitangents[j].y, aiM->mBitangents[j].z);
        else
            vd.vertBitangent = Vector3(0.0);

        if(aiM->mColors[0])
            vd.vertColor = Vector4(aiM->mColors[0][j].r, aiM->mColors[0][j].g, aiM->mColors[0][j].b, 0.0);
        else
            vd.vertColor = Vector4(0.0);
        
        if(aiM->mTextureCoords[0])
            vd.texCoord1 = Vector2(aiM->mTextureCoords[0][j].x, aiM->mTextureCoords[0][j].y);
        else
            vd.texCoord1 = Vector2(0.0, 0.0);
        
        mbvd.push_back(vd);
    }
    
    for (int j = 0; j < aiM->mNumFaces; j++) {
        aiFace face = aiM->mFaces[j];
        
        for (int k = 0; k < face.mNumIndices; k++) {
            mbi.push_back(face.mIndices[k]);
        }
    }
}

