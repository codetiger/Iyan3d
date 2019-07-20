//
//  SceneImporter.cpp
//  Iyan3D
//
//  Created by sabish on 28/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "SceneImporter.h"
#include <fstream>

SceneImporter::SceneImporter() {
    scene            = NULL;
    rigNode          = NULL;
    hasLoadedRigNode = false;
    bones            = new map<string, Joint*>();
}

SceneImporter::~SceneImporter() {
    if (bones != NULL) {
        delete bones;
        bones = NULL;
    }
    if (scene)
        delete scene;
}

string getFileExtention(const string& s) {
    char sep = '.';

    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return (s.substr(i + 1, s.length() - i));
    }

    return ("");
}

string getFileName(const string& s) {
    char   sep = '/';
    size_t i   = s.rfind(sep, s.length());
    if (i != string::npos) {
        return (s.substr(i + 1, s.length() - i));
    }

    sep = '\\';
    i   = s.rfind(sep, s.length());
    if (i != string::npos) {
        return (s.substr(i + 1, s.length() - i));
    }

    return s;
}

bool copyFile(string srcFile, string dstFile) {
    std::ifstream src(srcFile, std::ios::binary);
    if (src.is_open()) {
        std::ofstream dest(dstFile, std::ios::binary);
        dest << src.rdbuf();
        return src && dest;
    }
    return false;
}

Mat4 getDeltaMatrix(string ext, bool isRigged) {
    Quaternion r1 = Quaternion();
    Quaternion r2 = Quaternion();

    if (ext == "dae") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 0.0, 1.0));
        if (isRigged)
            r2.fromAngleAxis(M_PI_2, Vector3(1.0, 0.0, 0.0));
        else
            r2.fromAngleAxis(M_PI, Vector3(1.0, 0.0, 0.0));
    } else if (ext == "3ds") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 0.0, 1.0));
        r2.fromAngleAxis(M_PI_2, Vector3(-1.0, 0.0, 0.0));
    } else if (ext == "fbx" || ext == "obj") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 0.0, 1.0));
        r2.fromAngleAxis(M_PI, Vector3(1.0, 0.0, 0.0));
    } else if (ext == "sgm" || ext == "sgr") {
        r1.fromAngleAxis(M_PI, Vector3(0.0, 1.0, 0.0));
    }

    return (r1 * r2).getMatrix();
}

Mat4 AssimpToMat4(aiMatrix4x4 assimpMatrix) {
    Mat4 mtrix;

    mtrix[0] = assimpMatrix.a1;
    mtrix[1] = assimpMatrix.b1;
    mtrix[2] = assimpMatrix.c1;
    mtrix[3] = assimpMatrix.d1;

    mtrix[4] = assimpMatrix.a2;
    mtrix[5] = assimpMatrix.b2;
    mtrix[6] = assimpMatrix.c2;
    mtrix[7] = assimpMatrix.d2;

    mtrix[8]  = assimpMatrix.a3;
    mtrix[9]  = assimpMatrix.b3;
    mtrix[10] = assimpMatrix.c3;
    mtrix[11] = assimpMatrix.d3;

    mtrix[12] = assimpMatrix.a4;
    mtrix[13] = assimpMatrix.b4;
    mtrix[14] = assimpMatrix.c4;
    mtrix[15] = assimpMatrix.d4;

    return mtrix;
}

void SceneImporter::import3DText(SGEditorScene* sgScene, wstring text, string fontPath, int bezierSegments, float extrude, float bevelRadius, int bevelSegments, bool hasBones, bool isTempNode) {
    sgScene->freezeRendering = true;

    Assimp::Importer* importer = new Assimp::Importer();
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

    if (!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return;
    }

    this->isTempNode   = isTempNode;
    this->sgScene      = sgScene;
    this->hasMeshColor = false;
    this->mColor       = Vector3(0.5);
    this->ext          = "text";
    this->folderPath   = "";

    importNode(scene->mRootNode, aiMatrix4x4());

    if (rigNode)
        loadDetails2Node(rigNode, rigMesh, aiMatrix4x4());

    sgScene->selectMan->removeChildren(sgScene->getParentNode());
    sgScene->updater->setDataForFrame(sgScene->currentFrame);
    sgScene->selectMan->updateParentPosition();
    sgScene->updater->resetMaterialTypes(false);

    delete importer;
    scene = NULL;

    sgScene->freezeRendering = false;
}

bool SceneImporter::importNodesFromFile(SGEditorScene* sgScene, string name, string filePath, string fileLocation, bool hasMeshColor, Vector3 meshColor, bool isTempNode, string* error) {
    rigNode          = NULL;
    hasLoadedRigNode = false;

    sgScene->freezeRendering = true;
    string ext               = getFileExtention(filePath);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "sgm" || ext == "sgr" || ext == "obj" || ext == "fbx" || ext == "dae" || ext == "3ds") {
        Assimp::Importer* importer = new Assimp::Importer();
        importer->SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MAX_VERTICES_COUNT);
        importer->SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8);

        unsigned int pFlags = aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_OptimizeGraph;

        scene = importer->ReadFile(filePath, pFlags);

        if (!scene || scene->mNumMeshes == 0) {
            printf("Error in Loading: %s\n", importer->GetErrorString());
            (*error)                 = string(importer->GetErrorString());
            sgScene->freezeRendering = false;
            return false;
        } else {
            this->isTempNode   = isTempNode;
            this->sgScene      = sgScene;
            this->hasMeshColor = hasMeshColor;
            this->mColor       = meshColor;
            this->ext          = ext;
            this->folderPath   = fileLocation;
            this->nodeName     = name;

            importNode(scene->mRootNode, scene->mRootNode->mTransformation);

            if (rigNode) {
                loadDetails2Node(rigNode, rigMesh, aiMatrix4x4());
                printf("Bone Count: %lu\n", rigNode->joints.size());
            }

            sgScene->selectMan->removeChildren(sgScene->getParentNode());
            sgScene->updater->setDataForFrame(sgScene->currentFrame);
            sgScene->selectMan->updateParentPosition();
            sgScene->updater->resetMaterialTypes(false);
        }

        delete importer;
        scene = NULL;
    }

    sgScene->freezeRendering = false;
    return true;
}

void SceneImporter::importNodeFromMesh(SGEditorScene* sgScene, SGNode* sceneNode, Mesh* lMesh) {
    sgScene->freezeRendering = true;

    if (sceneNode->getType() == NODE_RIG || sceneNode->getType() == NODE_TEXT_SKIN) {
        SkinMesh* mesh = (SkinMesh*)lMesh;

        shared_ptr<AnimatedMeshNode> sgn = sgScene->getSceneManager()->createAnimatedNodeFromMesh(mesh, "setUniforms", ShaderManager::maxJoints, CHARACTER_RIG, MESH_TYPE_HEAVY);
        sceneNode->node                  = sgn;
        bool isSGJointsCreated           = (sceneNode->joints.size() > 0) ? true : false;
        int  jointsCount                 = dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->getJointCount();

        for (int i = 0; i < jointsCount; i++) {
            dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->getJointNode(i)->setID(i);
            if (!isSGJointsCreated) {
                SGJoint* joint   = new SGJoint();
                joint->jointNode = dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->getJointNode(i);
                sceneNode->joints.push_back(joint);
            }
        }

        sgn->setMaterial(sgScene->getSceneManager()->getMaterialByIndex((sceneNode->getType() == NODE_RIG) ? SHADER_SKIN : SHADER_TEXT_SKIN));
        sceneNode->setInitialKeyValues(OPEN_SAVED_FILE);
        sgScene->loader->setJointsScale(sceneNode);
        dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->updateMeshCache();

    } else {
        Mesh*                mesh = lMesh;
        shared_ptr<MeshNode> sgn  = sgScene->getSceneManager()->createNodeFromMesh(mesh, "setUniforms", MESH_TYPE_LITE, SHADER_MESH);
        sceneNode->node           = sgn;
        sceneNode->setInitialKeyValues(OPEN_SAVED_FILE);
    }

    sceneNode->actionId = ++sgScene->actionObjectsSize;

    sceneNode->node->setID(sgScene->assetIDCounter++);
    sgScene->selectMan->removeChildren(sgScene->getParentNode());
    sgScene->updater->setDataForFrame(sgScene->currentFrame);
    sgScene->selectMan->updateParentPosition();
    sgScene->updater->resetMaterialTypes(false);

    sgScene->freezeRendering = false;

    if (sceneNode->getType() == NODE_TEXT_SKIN) {
        sgScene->textJointsBasePos[(int)sgScene->nodes.size()] = sgScene->animMan->storeTextInitialPositions(sceneNode);
    }
}

Mesh* SceneImporter::loadMeshFromFile(string filePath) {
    Assimp::Importer* importer = new Assimp::Importer();
    scene                      = importer->ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes);

    if (!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return NULL;
    }

    Mesh* mesh = new Mesh();
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* aiM = scene->mMeshes[i];

        if (aiM && aiM->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
            vector<vertexData>     mbvd;
            vector<unsigned short> mbi;
            getMeshFrom(mbvd, mbi, aiM);

            mesh->addMeshBuffer(mbvd, mbi, 0);
        }
    }

    delete importer;
    scene = NULL;
    return mesh;
}

SkinMesh* SceneImporter::loadSkinMeshFromFile(string filePath) {
    Assimp::Importer* importer = new Assimp::Importer();
    importer->SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MAX_VERTICES_COUNT);
    scene = importer->ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FindInstances | aiProcess_OptimizeMeshes);

    if (!scene) {
        printf("Error in Loading: %s\n", importer->GetErrorString());
        return NULL;
    }

    SkinMesh* mesh = new SkinMesh();
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* aiM = scene->mMeshes[i];

        if (aiM && aiM->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
            vector<vertexDataHeavy> mbvd;
            vector<unsigned short>  mbi;
            getSkinMeshFrom(mbvd, mbi, aiM);

            mesh->addMeshBuffer(mbvd, mbi, 0);

            if (aiM->HasBones())
                loadBonesFromMesh(aiM, mesh, bones);
        }
    }
    loadBoneHierarcy((SkinMesh*)mesh, bones);
    mesh->finalize();

    delete importer;
    scene = NULL;
    return mesh;
}

int SceneImporter::loadMaterial2Node(SGNode* sceneNode, int materialIndex, bool hasBones) {
    MaterialProperty* materialProps = new MaterialProperty(hasBones ? NODE_RIG : NODE_SGM);
    sceneNode->materialProps.push_back(materialProps);

    int nodeMaterialIndex = sceneNode->materialProps.size() - 1;

    aiMaterial* material = scene->mMaterials[materialIndex];

    Property& p2 = sceneNode->getProperty(IS_VERTEX_COLOR, nodeMaterialIndex);
    p2.value.x   = 1.0;

    Property& p3 = sceneNode->getProperty(TEXTURE, nodeMaterialIndex);
    p3.fileName  = "";

    materialProps->setTextureForType(NULL, NODE_TEXTURE_TYPE_COLORMAP);

    Property& p1 = sceneNode->getProperty(VERTEX_COLOR, nodeMaterialIndex);
    aiColor4D color;
    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
        p1.value = Vector4(color.r, color.g, color.b, color.a);
    else if (hasMeshColor)
        p1.value = Vector4(mColor.x, mColor.y, mColor.z, 1.0);
    else
        p1.value = Vector4(0.5, 0.5, 0.5, 1.0);

    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

        string texturePath = getFileName(string(path.data));
        if (folderPath != FileHelper::getTexturesDirectory())
            copyFile(folderPath + texturePath, FileHelper::getTexturesDirectory() + texturePath);

        Texture* texture = sgScene->getSceneManager()->loadTexture(texturePath, FileHelper::getTexturesDirectory() + texturePath, TEXTURE_RGBA8, TEXTURE_BYTE, true);
        if (texture) {
            Property& p1 = sceneNode->getProperty(TEXTURE, nodeMaterialIndex);
            p1.fileName  = texturePath;

            Property& p2 = sceneNode->getProperty(IS_VERTEX_COLOR, nodeMaterialIndex);
            p2.value.x   = 0.0;

            materialProps->setTextureForType(texture, NODE_TEXTURE_TYPE_COLORMAP);
        } else {
            printf("Loading Texture Failed: %s\n", texturePath.c_str());
        }
    }

    if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
        aiString path;
        material->GetTexture(aiTextureType_NORMALS, 0, &path);
        string texturePath = getFileName(string(path.data));
        copyFile(folderPath + texturePath, FileHelper::getTexturesDirectory() + texturePath);

        Texture* texture = sgScene->getSceneManager()->loadTexture(texturePath, FileHelper::getTexturesDirectory() + texturePath, TEXTURE_RGBA8, TEXTURE_BYTE, true);
        if (texture) {
            Property& p1 = sceneNode->getProperty(TEXTURE, nodeMaterialIndex);
            p1.fileName  = texturePath;
            materialProps->setTextureForType(texture, NODE_TEXTURE_TYPE_NORMALMAP);
        }
    }

    return nodeMaterialIndex;
}

void SceneImporter::loadAnimationKeys(SGJoint* joint) {
    int maxFrames = 0;
    for (int i = 0; i < scene->mNumAnimations; i++) {
        for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++) {
            string name(scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());
            if (joint->jointNode->name == name) {
                aiNodeAnim* channel = scene->mAnimations[i]->mChannels[j];
                for (int k = 0; k < channel->mNumPositionKeys; k++) {
                    int     frame = (int)(24.0 * channel->mPositionKeys[k].mTime);
                    Vector3 p     = Vector3(channel->mPositionKeys[k].mValue.x, channel->mPositionKeys[k].mValue.y, channel->mPositionKeys[k].mValue.z);
                    joint->setPosition(p, frame);
                    if (frame > maxFrames)
                        maxFrames = frame;
                }
                for (int k = 0; k < channel->mNumScalingKeys; k++) {
                    int     frame = (int)(24.0 * channel->mScalingKeys[k].mTime);
                    Vector3 s     = Vector3(channel->mScalingKeys[k].mValue.x, channel->mScalingKeys[k].mValue.y, channel->mScalingKeys[k].mValue.z);
                    joint->setScale(s, frame);
                    if (frame > maxFrames)
                        maxFrames = frame;
                }
                for (int k = 0; k < channel->mNumRotationKeys; k++) {
                    int        frame = (int)(24.0 * channel->mRotationKeys[k].mTime);
                    Quaternion r     = Quaternion(channel->mRotationKeys[k].mValue.x, channel->mRotationKeys[k].mValue.y, channel->mRotationKeys[k].mValue.z, channel->mRotationKeys[k].mValue.w);
                    joint->setRotation(r, frame);
                    if (frame > maxFrames)
                        maxFrames = frame;
                }
            }
        }
    }

    //    if(maxFrames > sgScene->totalFrames)
    //        sgScene->totalFrames = maxFrames;
}

void SceneImporter::loadAnimationKeys(SGNode* node) {
    Mat4 m         = getDeltaMatrix(ext, node->getType() == NODE_RIG);
    int  maxFrames = 0;
    for (int i = 0; i < scene->mNumAnimations; i++) {
        for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++) {
            string name(scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());
            string nodeName = ConversionHelper::getStringForWString(node->name);

            if (nodeName == name) {
                aiNodeAnim* channel = scene->mAnimations[i]->mChannels[j];
                for (int k = 0; k < channel->mNumPositionKeys; k++) {
                    int     frame = (int)(24.0 * channel->mPositionKeys[k].mTime);
                    Vector3 p     = Vector3(channel->mPositionKeys[k].mValue.x, channel->mPositionKeys[k].mValue.y, channel->mPositionKeys[k].mValue.z);
                    Vector4 p4    = m * Vector4(p, 0.0);
                    p             = Vector3(p4.x, p4.y, p4.z);
                    node->setPosition(p, frame);
                    if (frame > maxFrames)
                        maxFrames = frame;
                }
                for (int k = 0; k < channel->mNumScalingKeys; k++) {
                    int     frame = (int)(24.0 * channel->mScalingKeys[k].mTime);
                    Vector3 s     = Vector3(channel->mScalingKeys[k].mValue.x, channel->mScalingKeys[k].mValue.y, channel->mScalingKeys[k].mValue.z);
                    node->setScale(s, frame);
                    if (frame > maxFrames)
                        maxFrames = frame;
                }
                for (int k = 0; k < channel->mNumRotationKeys; k++) {
                    int        frame = (int)(24.0 * channel->mRotationKeys[k].mTime);
                    Quaternion r     = Quaternion(channel->mRotationKeys[k].mValue.x, channel->mRotationKeys[k].mValue.y, channel->mRotationKeys[k].mValue.z, channel->mRotationKeys[k].mValue.w);
                    r                = m * r.getMatrix();
                    node->setRotation(r, frame);
                    if (frame > maxFrames)
                        maxFrames = frame;
                }
            }
        }
    }

    //    if(maxFrames > sgScene->totalFrames)
    //        sgScene->totalFrames = maxFrames;
}

void SceneImporter::loadDetails2Node(SGNode* sceneNode, Mesh* mesh, aiMatrix4x4 transform) {
    shared_ptr<Node> sgn;

    if (sceneNode->getType() == NODE_RIG || sceneNode->getType() == NODE_TEXT_SKIN) {
        if (bones->size()) {
            loadBoneHierarcy((SkinMesh*)mesh, bones);
            bones->clear();
        }

        ((SkinMesh*)mesh)->finalize();
        sceneNode->setSkinningData((SkinMesh*)mesh, ext != "sgr" && ext != "text");

        sgn             = sgScene->getSceneManager()->createAnimatedNodeFromMesh((SkinMesh*)mesh, "setUniforms", ShaderManager::maxJoints, CHARACTER_RIG, MESH_TYPE_HEAVY);
        sceneNode->node = sgn;

        bool isSGJointsCreated = (sceneNode->joints.size() > 0) ? true : false;
        int  jointsCount       = dynamic_pointer_cast<AnimatedMeshNode>(sgn)->getJointCount();

        for (int i = 0; i < jointsCount; i++) {
            dynamic_pointer_cast<AnimatedMeshNode>(sgn)->getJointNode(i)->setID(i);
            if (!isSGJointsCreated) {
                SGJoint* joint   = new SGJoint();
                joint->jointNode = dynamic_pointer_cast<AnimatedMeshNode>(sgn)->getJointNode(i);
                loadAnimationKeys(joint);
                sceneNode->joints.push_back(joint);
            }
        }

        sgn->updateAbsoluteTransformation();
        sgn->setMaterial(sgScene->getSceneManager()->getMaterialByIndex((sceneNode->getType() == NODE_RIG) ? SHADER_SKIN : SHADER_TEXT_SKIN));

    } else {
        sgn             = sgScene->getSceneManager()->createNodeFromMesh(mesh, "setUniforms");
        sceneNode->node = sgn;

        sgn->updateAbsoluteTransformation();
        sgn->setMaterial(sgScene->getSceneManager()->getMaterialByIndex(SHADER_MESH));
    }

    sceneNode->setInitialKeyValues(IMPORT_ASSET_ACTION);

    Mat4 m       = AssimpToMat4(transform);
    m            = getDeltaMatrix(ext, sceneNode->getType() == NODE_RIG) * m;
    Quaternion r = m.getRotation();

    sceneNode->setPosition(m.getTranslation(), 0);
    sceneNode->setRotation(r, 0);
    sceneNode->nodeInitialRotation = r;
    sceneNode->setScale(m.getScale(), 0);
    sgn->setPosition(m.getTranslation());
    sgn->setRotation(r);
    sgn->setScale(m.getScale());
    sgn->updateAbsoluteTransformation();

    loadAnimationKeys(sceneNode);

    if (sceneNode->getType() == NODE_RIG || sceneNode->getType() == NODE_TEXT_SKIN) {
        sgScene->loader->setJointsScale(sceneNode);
        dynamic_pointer_cast<AnimatedMeshNode>(sceneNode->node)->updateMeshCache();
    }

    sceneNode->node     = sgn;
    sceneNode->actionId = ++sgScene->actionObjectsSize;
    sceneNode->node->setID(sgScene->assetIDCounter++);

    if (sceneNode->getType() == NODE_TEXT_SKIN) {
        sgScene->textJointsBasePos[(int)sgScene->nodes.size()] = sgScene->animMan->storeTextInitialPositions(sceneNode);
    }

    for (int i = 0; i < sceneNode->materialProps.size(); i++)
        sceneNode->materialProps[i]->setTextureForType(sgScene->shaderMGR->shadowTexture, NODE_TEXTURE_TYPE_SHADOWMAP);

    sgScene->nodes.push_back(sceneNode);
}

void SceneImporter::importNode(aiNode* node, aiMatrix4x4 parentTransform) {
    aiMatrix4x4 transform = parentTransform * node->mTransformation;

    if (node->mNumMeshes > 0) {
        bool hasBones = false;

        for (int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* aiM = scene->mMeshes[node->mMeshes[i]];
            if (!hasBones)
                hasBones = aiM->HasBones();
        }

        SGNode* sceneNode = NULL;
        Mesh*   mesh;

        if (hasBones && rigNode != NULL) {
            sceneNode = rigNode;
            mesh      = rigMesh;
        } else {
            NODE_TYPE nType;
            if (ext == "text")
                nType = (hasBones) ? NODE_TEXT_SKIN : NODE_TEXT;
            else
                nType = (hasBones) ? NODE_RIG : NODE_SGM;

            sceneNode             = new SGNode(nType);
            sceneNode->isTempNode = isTempNode;

            if (hasBones)
                mesh = new SkinMesh();
            else
                mesh = new Mesh();

            if (hasBones) {
                rigNode = sceneNode;
                rigMesh = mesh;
            }

            if (this->nodeName == "") {
                string name     = node->mName.C_Str();
                sceneNode->name = ConversionHelper::getWStringForString(name);
            } else {
                string name     = node->mName.C_Str();
                sceneNode->name = ConversionHelper::getWStringForString(this->nodeName);
            }
        }

        for (int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* aiM = scene->mMeshes[node->mMeshes[i]];
            if (!aiM || aiM->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
                continue;

            unsigned short materialIndex     = aiM->mMaterialIndex;
            int            nodeMaterialIndex = loadMaterial2Node(sceneNode, materialIndex, hasBones);

            if (hasBones) {
                vector<vertexDataHeavy> mbvd;
                vector<unsigned short>  mbi;
                getSkinMeshFrom(mbvd, mbi, aiM);
                mesh->addMeshBuffer(mbvd, mbi, nodeMaterialIndex);

                if (aiM->HasBones())
                    loadBonesFromMesh(aiM, (SkinMesh*)mesh, bones);
            } else {
                vector<vertexData>     mbvd;
                vector<unsigned short> mbi;
                getMeshFrom(mbvd, mbi, aiM);
                mesh->addMeshBuffer(mbvd, mbi, nodeMaterialIndex);
            }

            Property& px = sceneNode->getProperty(VERTEX_COLOR, nodeMaterialIndex);
            if (aiM->mColors[0])
                px.value = Vector4(-1.0);
        }

        if (!hasBones)
            loadDetails2Node(sceneNode, mesh, transform);
    }

    for (int i = 0; i < node->mNumChildren; i++)
        importNode(node->mChildren[i], transform);
}

void SceneImporter::getSkinMeshFrom(vector<vertexDataHeavy>& mbvd, vector<unsigned short>& mbi, aiMesh* aiM) {
    for (int j = 0; j < aiM->mNumVertices; j++) {
        vertexDataHeavy vd;
        vd.vertPosition = Vector3(aiM->mVertices[j].x, aiM->mVertices[j].y, aiM->mVertices[j].z);

        if (aiM->mNormals)
            vd.vertNormal = Vector3(aiM->mNormals[j].x, aiM->mNormals[j].y, aiM->mNormals[j].z);
        else
            vd.vertNormal = Vector3(0.0);

        if (aiM->mTangents)
            vd.vertTangent = Vector3(aiM->mTangents[j].x, aiM->mTangents[j].y, aiM->mTangents[j].z);
        else
            vd.vertTangent = Vector3(0.0);

        if (aiM->mBitangents)
            vd.vertBitangent = Vector3(aiM->mBitangents[j].x, aiM->mBitangents[j].y, aiM->mBitangents[j].z);
        else
            vd.vertBitangent = Vector3(0.0);

        if (aiM->mColors[0])
            vd.vertColor = Vector4(aiM->mColors[0][j].r, aiM->mColors[0][j].g, aiM->mColors[0][j].b, 0.0);
        else
            vd.vertColor = Vector4(0.0);

        if (aiM->mTextureCoords[0])
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

void SceneImporter::loadBoneHierarcy(SkinMesh* m, map<string, Joint*>* bones) {
    typedef map<string, Joint*>::iterator it_type;

    map<string, Joint*>* newBones = new map<string, Joint*>();

    do {
        if (newBones->size() > 0) {
            bones->insert(newBones->begin(), newBones->end());
            newBones->clear();
        }

        for (it_type iterator = bones->begin(); iterator != bones->end(); iterator++) {
            if (!iterator->second->Parent) {
                aiNode* n = scene->mRootNode->FindNode(iterator->first.c_str());

                if (n && n->mParent && n->mParent != scene->mRootNode) {
                    if (bones->find(string(n->mParent->mName.C_Str())) == bones->end() &&
                        newBones->find(string(n->mParent->mName.C_Str())) == newBones->end()) {
                        Joint* sgBone = m->addJoint(NULL);
                        sgBone->name  = string(n->mParent->mName.C_Str());

                        iterator->second->Parent = sgBone;
                        sgBone->childJoints->push_back(iterator->second);

                        newBones->insert(pair<string, Joint*>(string(n->mParent->mName.C_Str()), sgBone));

                        sgBone->LocalAnimatedMatrix   = AssimpToMat4(n->mParent->mTransformation);
                        sgBone->originalJointMatrix   = sgBone->LocalAnimatedMatrix;
                        sgBone->inverseBindPoseMatrix = Mat4();
                    } else {
                        it_type parentIterator = bones->find(string(n->mParent->mName.C_Str()));
                        if (parentIterator == bones->end())
                            parentIterator = newBones->find(string(n->mParent->mName.C_Str()));

                        iterator->second->Parent = parentIterator->second;
                        parentIterator->second->childJoints->push_back(iterator->second);
                    }
                }
            }
        }
    } while (newBones->size() > 0);

    newBones->clear();
    delete newBones;
}

void SceneImporter::loadBonesFromMesh(aiMesh* aiM, SkinMesh* m, map<string, Joint*>* bones) {
    for (int i = 0; i < aiM->mNumBones; i++) {
        aiBone* bone  = aiM->mBones[i];
        string  bName = string(bone->mName.C_Str());
        if (bones->find(bName) == bones->end()) {
            Joint* sgBone = m->addJoint(NULL);
            sgBone->name  = string(bone->mName.C_Str());

            bones->insert(pair<string, Joint*>(string(bone->mName.C_Str()), sgBone));

            for (int j = 0; j < bone->mNumWeights; j++) {
                shared_ptr<PaintedVertex> pvInfo = make_shared<PaintedVertex>();
                pvInfo->vertexId                 = bone->mWeights[j].mVertexId;
                pvInfo->weight                   = bone->mWeights[j].mWeight;
                pvInfo->meshBufferIndex          = m->getMeshBufferCount() - 1;
                sgBone->PaintedVertices->push_back(pvInfo);
            }

            aiNode* n                     = scene->mRootNode->FindNode(bone->mName);
            sgBone->LocalAnimatedMatrix   = AssimpToMat4(n->mTransformation);
            sgBone->originalJointMatrix   = sgBone->LocalAnimatedMatrix;
            sgBone->inverseBindPoseMatrix = AssimpToMat4(bone->mOffsetMatrix);
        } else {
            Joint* sgBone = (*bones)[bName];

            for (int j = 0; j < bone->mNumWeights; j++) {
                shared_ptr<PaintedVertex> pvInfo = make_shared<PaintedVertex>();
                pvInfo->vertexId                 = bone->mWeights[j].mVertexId;
                pvInfo->weight                   = bone->mWeights[j].mWeight;
                pvInfo->meshBufferIndex          = m->getMeshBufferCount() - 1;
                sgBone->PaintedVertices->push_back(pvInfo);
            }
        }
    }
}

void SceneImporter::getMeshFrom(vector<vertexData>& mbvd, vector<unsigned short>& mbi, aiMesh* aiM) {
    for (int j = 0; j < aiM->mNumVertices; j++) {
        vertexData vd;
        vd.vertPosition = Vector3(aiM->mVertices[j].x, aiM->mVertices[j].y, aiM->mVertices[j].z);

        if (aiM->mNormals)
            vd.vertNormal = Vector3(aiM->mNormals[j].x, aiM->mNormals[j].y, aiM->mNormals[j].z);
        else
            vd.vertNormal = Vector3(0.0);

        if (aiM->mTangents)
            vd.vertTangent = Vector3(aiM->mTangents[j].x, aiM->mTangents[j].y, aiM->mTangents[j].z);
        else
            vd.vertTangent = Vector3(0.0);

        if (aiM->mBitangents)
            vd.vertBitangent = Vector3(aiM->mBitangents[j].x, aiM->mBitangents[j].y, aiM->mBitangents[j].z);
        else
            vd.vertBitangent = Vector3(0.0);

        if (aiM->mColors[0])
            vd.vertColor = Vector4(aiM->mColors[0][j].r, aiM->mColors[0][j].g, aiM->mColors[0][j].b, 0.0);
        else
            vd.vertColor = Vector4(0.0);

        if (aiM->mTextureCoords[0])
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
