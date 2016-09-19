
#include "HeaderFiles/SceneHelper.h"

#include "HeaderFiles/SGNode.h"
#include "HeaderFiles/ShaderManager.h"
#include "MeshRW.h"
#include "SceneImporter.h"

#ifdef ANDROID
#include "../opengl.h"
#endif

SGNode::SGNode(NODE_TYPE type)
{
    this->type = type;
    smoothTexture = true;
    instanceNodes.clear();
    isTempNode = false;
    optionalFilePath = "";
    if(type != NODE_UNDEFINED)
        setPropertiesOfNode();
}

void SGNode::setPropertiesOfNode()
{
    
    addOrUpdateProperty(DELETE, Vector4(1, 0, 0, 0), UNDEFINED, ICON_TYPE, "Delete", "GENERAL", "", DELETE_ICON);
    addOrUpdateProperty(SELECTED, Vector4(false, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
    
    if(type == NODE_CAMERA) {
        //TODO
        addOrUpdateProperty(FOV, Vector4(72.0, 0, 5.0, true), UNDEFINED, SLIDER_TYPE, "Field Of View");
        addOrUpdateProperty(CAM_RESOLUTION, Vector4(0), UNDEFINED, TYPE_NONE, "Resolution");
        addOrUpdateProperty(THOUSAND_EIGHTY, Vector4(1.0, 0, 0, 0), UNDEFINED, LIST_TYPE, "1080p", "Resolution");
        addOrUpdateProperty(SEVEN_TWENTY, Vector4(0), UNDEFINED, LIST_TYPE, "720p", "Resolution");
        addOrUpdateProperty(FOUR_EIGHTY, Vector4(0), UNDEFINED, LIST_TYPE, "480p", "Resolution");
        addOrUpdateProperty(THREE_SIXTY, Vector4(0), UNDEFINED, LIST_TYPE, "360p", "Resolution");
        addOrUpdateProperty(TWO_FORTY, Vector4(0), UNDEFINED, LIST_TYPE, "240p", "Resolution");
        
    } else if(type == NODE_LIGHT || type == NODE_ADDITIONAL_LIGHT) {
        
        addOrUpdateProperty(SHADOW_DARKNESS, Vector4(0, 0, 0, 0), UNDEFINED, (type == NODE_LIGHT) ? SLIDER_TYPE : TYPE_NONE, "Shadow Darkness");
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(300.0, 0, 0, 0), UNDEFINED, (type == NODE_LIGHT) ? TYPE_NONE : SLIDER_TYPE, "Distance");
        int lightType = (type == NODE_LIGHT) ? DIRECTIONAL_LIGHT : POINT_LIGHT;
        
        addOrUpdateProperty(LIGHT_TYPE, Vector4(lightType, 0, 0, 0), UNDEFINED, TYPE_NONE, "Light Type");
        addOrUpdateProperty(LIGHT_DIRECTIONAL, Vector4(1, 0, 0, 0), UNDEFINED, LIST_TYPE, "Directional", "LIGHT TYPE");
        addOrUpdateProperty(LIGHT_POINT, Vector4(0, 0, 0, 0), UNDEFINED, LIST_TYPE, "Point", "LIGHT TYPE");
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(true, 0, 0, 0), UNDEFINED, TYPE_NONE, "IsVertexColor");
        addOrUpdateProperty(VERTEX_COLOR, Vector4(1.0), UNDEFINED, COLOR_TYPE, "Light Color");
        
    } else if(type == NODE_SGM || type == NODE_OBJ || type == NODE_TEXT || type == NODE_RIG || type == NODE_TEXT_SKIN) {
        
        addOrUpdateProperty(CLONE, Vector4(1, 0, 0, 0), UNDEFINED,  ICON_TYPE, "Clone", "GENERAL", "", CLONE_ICON);
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(300.0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Distance");
        addOrUpdateProperty(FONT_SIZE, Vector4(20.0, 0, 0, 0), UNDEFINED, TYPE_NONE, "FontSize");
        
        addOrUpdateProperty(HAS_PHYSICS, Vector4(0, 0, 0, 0), UNDEFINED, PARENT_TYPE, "Physics Properties", "PROPERTIES");
        addOrUpdateProperty(PHYSICS_NONE, Vector4(1), HAS_PHYSICS, LIST_TYPE, "None", "PHYSICS TYPE");
        addOrUpdateProperty(PHYSICS_STATIC, Vector4(0), HAS_PHYSICS, LIST_TYPE, "Static", "PHYSICS TYPE");
        addOrUpdateProperty(PHYSICS_LIGHT, Vector4(0), HAS_PHYSICS, LIST_TYPE, "Light", "PHYSICS TYPE");
        addOrUpdateProperty(PHYSICS_MEDIUM, Vector4(0), HAS_PHYSICS, LIST_TYPE, "Medium", "PHYSICS TYPE");
        addOrUpdateProperty(PHYSICS_HEAVY, Vector4(0), HAS_PHYSICS, LIST_TYPE, "Heavy", "PHYSICS TYPE");
        addOrUpdateProperty(PHYSICS_CLOTH, Vector4(0), HAS_PHYSICS, LIST_TYPE, "Cloth", "PHYSICS TYPE");
        addOrUpdateProperty(PHYSICS_JELLY, Vector4(0), HAS_PHYSICS, LIST_TYPE, "Jelly", "PHYSICS TYPE");
        
        addOrUpdateProperty(WEIGHT, Vector4(0.0, 0.0, 0.0, 0.0), HAS_PHYSICS, TYPE_NONE, "Mass");
        addOrUpdateProperty(FORCE_MAGNITUDE, Vector4(0, 0, 0, true), HAS_PHYSICS, SLIDER_TYPE, "Velocity", "APPLY FORCE");
        addOrUpdateProperty(FORCE_DIRECTION, Vector4(0, 0, 0, 0), HAS_PHYSICS, BUTTON_TYPE, "Direction", "APPLY FORCE");
        addOrUpdateProperty(IS_SOFT, Vector4(0, 0, 0, 0), HAS_PHYSICS, TYPE_NONE, "Soft");
        addOrUpdateProperty(PHYSICS_KIND, Vector4(PHYSICS_NONE), HAS_PHYSICS, TYPE_NONE, "Physics Type");
        
    } else if(type == NODE_IMAGE || type == NODE_VIDEO || type == NODE_PARTICLES) {
        
        addOrUpdateProperty(CLONE, Vector4(1, 0, 0, 0), UNDEFINED,  ICON_TYPE, "Clone", "GENERAL", "", CLONE_ICON);
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Visible", "PROPERTIES");
    }
    
}

SGNode::~SGNode()
{
    instanceNodes.clear();
    clearSGJoints();
    node.reset();
}

shared_ptr<Node> SGNode::loadNode(int assetId, std::string meshPath, std::string texturePath,NODE_TYPE objectType,SceneManager *smgr, std::wstring objectName,int width,int height, Vector4 objSpecificColor , string &specificFilePath)
{
    shared_ptr<Node> node;
    switch (objectType){
        case NODE_CAMERA:
        {
            SceneImporter *importer = new SceneImporter();
            Mesh *mesh = importer->loadMeshFromFile(constants::BundlePath + "/camera.sgm");
            delete importer;
            
            node = smgr->createNodeFromMesh(mesh,"setUniforms");
            node->setPosition(Vector3(RENDER_CAM_INIT_POS_X,RENDER_CAM_INIT_POS_Y,RENDER_CAM_INIT_POS_Z));
            getProperty(LIGHTING).value.x = false;
            getProperty(VERTEX_COLOR).value = Vector4(-1.0);
            node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
            break;
        }
        case NODE_LIGHT:
        {
            node = initLightSceneNode(smgr);
            getProperty(VERTEX_COLOR).value = DEFAULT_LIGHT_COLOR;
            getProperty(SPECIFIC_FLOAT).value.x = DEFAULT_FADE_DISTANCE;
            break;
        }
        case NODE_OBJ:
        case NODE_SGM:
        {
            getProperty(TEXTURE).fileName = texturePath;
            oriTextureName = texturePath;
            getProperty(VERTEX_COLOR).value = Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0);
            getProperty(ORIG_VERTEX_COLOR).value = Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0);
            break;
        }
        case NODE_IMAGE:
        {
            float aspectRatio = (float)objSpecificColor.x/(float)objSpecificColor.y;
            getProperty(TEXTURE).fileName = ConversionHelper::getStringForWString(objectName);
            getProperty(VERTEX_COLOR).value = Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0);
            node = loadImage(ConversionHelper::getStringForWString(objectName) + ".png", smgr , aspectRatio);
            break;
        }
        case NODE_VIDEO:
        {
            float aspectRatio = (float)objSpecificColor.x/(float)objSpecificColor.y;
            getProperty(VERTEX_COLOR).value = Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0);
            getProperty(TEXTURE).fileName = ConversionHelper::getStringForWString(objectName);
            node = loadVideo(ConversionHelper::getStringForWString(objectName), smgr , aspectRatio);
            break;
        }
        case NODE_ADDITIONAL_LIGHT:
        {
            this->assetId = assetId;
            node = addAdittionalLight(smgr, width, Vector3(objSpecificColor.x,objSpecificColor.y,objSpecificColor.z) , height);
            getProperty(VERTEX_COLOR).value = Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0);
            getProperty(SPECIFIC_FLOAT).value.x = (height == 0) ? 50.0 : height;
            break;
        }
        case NODE_PARTICLES:
        {
            getProperty(TEXTURE).fileName = to_string(assetId) + "-cm";
            
            string meshPath = "";
            string texPath = "";
            string jsonPath = "";
#ifdef IOS
            meshPath = constants::CachesStoragePath + "/" + to_string(assetId) + ".sgm";
            texPath = constants::CachesStoragePath + "/" + to_string(assetId) + "-cm.png";
#elif ANDROID
            meshPath = constants::DocumentsStoragePath + "/mesh/" + to_string(assetId) + ".sgm";
            texPath = constants::DocumentsStoragePath + "/mesh/" + to_string(assetId) + "-cm.png";
            jsonPath = constants::DocumentsStoragePath + "/mesh/" + to_string(assetId) + ".json";
#else
            meshPath = to_string(assetId) + ".sgm";
            texPath = to_string(assetId) + "-cm.png";
#endif
#ifdef ANDROID
            if (!checkFileExists(meshPath) || !checkFileExists(texPath) || !checkFileExists(jsonPath)) {
                return shared_ptr<Node>();
            }
#endif
            Json::Value pData = parseParticlesJson(assetId);
            
            SceneImporter *importer = new SceneImporter();
            Mesh *mesh = importer->loadMeshFromFile(meshPath);
            delete importer;
            
            node = smgr->createParticlesFromMesh(mesh, "setUniforms", MESH_TYPE_LITE, SHADER_PARTICLES);
            setParticlesData(node, pData);
            node->setMaterial(smgr->getMaterialByIndex(SHADER_PARTICLES));
            Texture *nodeTex = smgr->loadTexture("Particle Texture", texPath,TEXTURE_RGBA8,TEXTURE_BYTE, true);
            materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
            getProperty(TRANSPARENCY).value.x = 0.7 ;// Vector4(0.7, 0, 0, 0), UNDEFINED);
            break;
        }
        default:
            Logger::log(ERROR, "SGNode::loadNode ", "Unknown node type to load");
            break;
    }
    
    return node;
}

shared_ptr<Node> SGNode::addAdittionalLight(SceneManager* smgr, float distance , Vector3 lightColor, float fadeLevel)
{
    Mesh* lightMesh = new Mesh();
    lightMesh->copyDataFromMesh(SceneHelper::pointLightMesh);
    shared_ptr<LightNode> lightNode = smgr->createLightNode(lightMesh,"setUniforms");
    lightNode->setPosition(Vector3((assetId - (LIGHT_STARTING_ID+4)) * 2.0 ,10.0,10.0));
    lightNode->setScale(Vector3(3.0));
    lightNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    lightNode->decayStartDistance = distance;
    lightNode->decayEndDistance = fadeLevel * 10;
    lightNode->lightColor = lightColor;
    
    return lightNode;
}

Json::Value SGNode::parseParticlesJson(int assetId)
{
    Json::Value particlesData;
    string jsonFileName = to_string(assetId) + ".json";
    string jsonPath = jsonFileName;
    
#ifdef IOS
    jsonPath = constants::CachesStoragePath + "/" + jsonFileName;
#elif ANDROID
    jsonPath = constants::DocumentsStoragePath + "/mesh/"+jsonFileName;
#endif
    
    ifstream jsonFile(jsonPath);
    Json::Reader reader;
    if(!reader.parse(jsonFile, particlesData, false)){
        Logger::log(ERROR, "Unable to parse jointsData.json", "AutoRigHelper");
        return particlesData;
    }
    jsonFile.close();
    return particlesData;
}

void SGNode::setParticlesData(shared_ptr<Node> particleNode, Json::Value pData)
{
    shared_ptr<ParticleManager> pNode = dynamic_pointer_cast<ParticleManager>(particleNode);
    Vector4 sColor = Vector4(pData["sColorX"].asDouble(), pData["sColorY"].asDouble(), pData["sColorZ"].asDouble(), 1.0);
    Vector4 mColor = Vector4(pData["mColorX"].asDouble(), pData["mColorY"].asDouble(), pData["mColorZ"].asDouble(), 1.0);
    Vector4 eColor = Vector4(pData["eColorX"].asDouble(), pData["eColorY"].asDouble(), pData["eColorZ"].asDouble(), 1.0);
    
    pNode->setDataFromJson(pData["Count"].asInt(), sColor, mColor, eColor, pData["Gravity"].asDouble(), pData["startVelocitySpreadAngle"].asDouble(), pData["startVelocityMagnitude"].asDouble(), pData["startVelocityMagnitudeRand"].asDouble(), pData["emissionSpeed"].asInt(), pData["maxLife"].asInt(), pData["maxLifeRandPercent"].asInt(), pData["startScale"].asDouble(), pData["deltaScale"].asDouble());
    
    printf(" Counnt %d gravity %d sAngle %f sMag %f magRand %f emSpeed %d maxLife %d perce %d scale %f delta %f ",pData["Count"].asInt(), pData["Gravity"].asBool(), pData["startVelocitySpreadAngle"].asDouble(), pData["startVelocityMagnitude"].asDouble(), pData["startVelocityMagnitudeRand"].asDouble(), pData["emissionSpeed"].asInt(), pData["maxLife"].asInt(), pData["maxLifeRandPercent"].asInt(), pData["startScale"].asDouble(), pData["deltaScale"].asDouble());
    
}

bool SGNode::checkFileExists(std::string fileName)
{
    if(FILE *file = fopen(fileName.c_str(), "r"))
    {
        fclose(file);
        return true;
    }else {
        return false;
    }
}

void SGNode::setSkinningData(SkinMesh *mesh, bool applyInverseBindPose)
{
    for(int j = 0; j < mesh->joints->size(); j++) {
        Joint *meshJoint = (*mesh->joints)[j];
        for(int v = 0; v < meshJoint->PaintedVertices->size(); v++) {
            
            int vertexId = (*meshJoint->PaintedVertices)[v]->vertexId;
            float weight = (*meshJoint->PaintedVertices)[v]->weight;
            int meshBufferIndex = (*meshJoint->PaintedVertices)[v]->meshBufferIndex;
            
            vertexDataHeavy *vts = mesh->getHeavyVerticesForMeshBuffer(meshBufferIndex, vertexId);
            
            Vector4 *optionalData1 = &(vts->optionalData1);
            Vector4 *optionalData2 = &(vts->optionalData2);
            Vector4 *optionalData3 = &(vts->optionalData3);
            Vector4 *optionalData4 = &(vts->optionalData4);
            
            if((*optionalData1).x == 0.0f) {
                (*optionalData1).x = j + 1;
                (*optionalData2).x = weight;
            } else if((*optionalData1).y == 0.0f) {
                (*optionalData1).y = j + 1;
                (*optionalData2).y = weight;
            } else if((*optionalData1).z == 0.0f) {
                (*optionalData1).z = j + 1;
                (*optionalData2).z = weight;
            } else if((*optionalData1).w == 0.0f) {
                (*optionalData1).w = j + 1;
                (*optionalData2).w = weight;
            } else if((*optionalData3).x == 0.0f) {
                (*optionalData3).x = j + 1;
                (*optionalData4).x = weight;
            } else if((*optionalData3).y == 0.0f) {
                (*optionalData3).y = j + 1;
                (*optionalData4).y = weight;
            } else if((*optionalData3).z == 0.0f) {
                (*optionalData3).z = j + 1;
                (*optionalData4).z = weight;
            } else if((*optionalData3).w == 0.0f) {
                (*optionalData3).w = j + 1;
                (*optionalData4).w = weight;
            }
        }
    }
    
    if(applyInverseBindPose) {
        Mat4 *boneMatrix = new Mat4[mesh->joints->size()];
        Mat4 *boneMatrixIT = new Mat4[mesh->joints->size()];
        
        for(int h = 0; h < mesh->joints->size(); h++) {
            Joint *joint = (*mesh->joints)[h];
            
            boneMatrix[h] = joint->GlobalAnimatedMatrix * joint->inverseBindPoseMatrix;
            boneMatrixIT[h] = boneMatrix[h];
            boneMatrixIT[h].invert();
            boneMatrixIT[h].transpose();
        }
        
        for (int mbi = 0; mbi < mesh->getMeshBufferCount(); mbi++) {
            for (int v = 0; v < mesh->getVerticesCountInMeshBuffer(mbi); v++) {
                vertexDataHeavy *vts = mesh->getHeavyVerticesForMeshBuffer(mbi, v);
                
                Vector4 vp = Vector4(0.0);
                Vector4 vn = Vector4(0.0);
                
                if(vts->optionalData1.x > 0) {
                    vp += boneMatrix[(int)vts->optionalData1.x - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData2.x;
                    vn += boneMatrixIT[(int)vts->optionalData1.x - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData2.x;
                }
                if(vts->optionalData1.y > 0) {
                    vp += boneMatrix[(int)vts->optionalData1.y - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData2.y;
                    vn += boneMatrixIT[(int)vts->optionalData1.y - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData2.y;
                }
                if(vts->optionalData1.z > 0) {
                    vp += boneMatrix[(int)vts->optionalData1.z - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData2.z;
                    vn += boneMatrixIT[(int)vts->optionalData1.z - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData2.z;
                }
                if(vts->optionalData1.w > 0) {
                    vp += boneMatrix[(int)vts->optionalData1.w - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData2.w;
                    vn += boneMatrixIT[(int)vts->optionalData1.w - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData2.w;
                }
                if(vts->optionalData3.x > 0) {
                    vp += boneMatrix[(int)vts->optionalData3.x - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData4.x;
                    vn += boneMatrixIT[(int)vts->optionalData3.x - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData4.x;
                }
                if(vts->optionalData3.y > 0) {
                    vp += boneMatrix[(int)vts->optionalData3.y - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData4.y;
                    vn += boneMatrixIT[(int)vts->optionalData3.y - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData4.y;
                }
                if(vts->optionalData3.z > 0) {
                    vp += boneMatrix[(int)vts->optionalData3.z - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData4.z;
                    vn += boneMatrixIT[(int)vts->optionalData3.z - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData4.z;
                }
                if(vts->optionalData3.w > 0) {
                    vp += boneMatrix[(int)vts->optionalData3.w - 1] * Vector4(vts->vertPosition, 1.0) * vts->optionalData4.w;
                    vn += boneMatrixIT[(int)vts->optionalData3.w - 1] * Vector4(vts->vertNormal, 0.0) * vts->optionalData4.w;
                }
                vn.normalize();
                
                if(vp.w > 0) {
                    vts->vertPosition = Vector3(vp.x, vp.y, vp.z) / vp.w;
                    vts->vertNormal = Vector3(vn.x, vn.y, vn.z);
                }
            }
        }
        
        delete[] boneMatrixIT;
        delete[] boneMatrix;
    }
}

shared_ptr<Node> SGNode::loadImage(string textureName,SceneManager *smgr, float aspectRatio)
{
    char* textureFileName = new char[256];
    
#ifdef ANDROID
    string path = constants::DocumentsStoragePath+ "/importedImages/"+textureName;
    textureFileName=(path).c_str();
#else
    sprintf(textureFileName, "%s/%s", constants::CachesStoragePath.c_str(),textureName.c_str());
#endif
    Texture *nodeTex = smgr->loadTexture(textureName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
    shared_ptr<PlaneMeshNode> planeNode = smgr->createPlaneNode("setUniforms" , aspectRatio);
    planeNode->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
    //delete [] textureFileName; //TODO TEST
    return planeNode;
}

shared_ptr<Node> SGNode::loadVideo(string videoFileName,SceneManager *smgr, float aspectRatio)
{Texture *nodeTex;
#ifdef  ANDROID
    string dummyPath = constants::BundlePath + "/whiteborder.png";
    nodeTex = smgr->loadTexture("Dummy Vid Tex", dummyPath, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
#else
    nodeTex = smgr->loadTextureFromVideo(videoFileName, TEXTURE_RGBA8, TEXTURE_BYTE);
#endif
    shared_ptr<PlaneMeshNode> planeNode = smgr->createPlaneNode("setUniforms" , aspectRatio);
    planeNode->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
    return planeNode;
}

shared_ptr<Node> SGNode::initLightSceneNode(SceneManager *smgr)
{
    shared_ptr<Node> lightNode = smgr->createNodeFromMesh(SceneHelper::pointLightMesh, "setUniforms");
    
    lightNode->setPosition(Vector3(-LIGHT_INIT_POS_X,LIGHT_INIT_POS_Y,LIGHT_INIT_POS_Z));
    lightNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    lightNode->setScale(Vector3(3.0));
    getProperty(LIGHTING).value.x = false;
    
    return lightNode;
}

void SGNode::setPosition(Vector3 position, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(positionKeys, frameId);
    
    if(keyIndex == -1 || positionKeys[keyIndex].id != frameId){
        SGPositionKey positionKey;
        positionKey.id = frameId;
        positionKey.position = position;
        KeyHelper::addKey(positionKeys, positionKey);
    }
    else {
        if(keyIndex < positionKeys.size())
            positionKeys[keyIndex].position = position;
    }
}

void SGNode::setRotation(Quaternion rotation, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(rotationKeys,frameId);
    if(keyIndex == -1 || rotationKeys[keyIndex].id != frameId){
        SGRotationKey rotationKey;
        rotationKey.id = frameId;
        rotationKey.rotation = rotation;
        KeyHelper::addKey(rotationKeys, rotationKey);
    }
    else {
        if(keyIndex < rotationKeys.size())
            rotationKeys[keyIndex].rotation = rotation;
    }
}

void SGNode::setScale(Vector3 scale, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(scaleKeys,frameId);
    if(keyIndex == -1 || scaleKeys[keyIndex].id != frameId){
        SGScaleKey scaleKey;
        scaleKey.id = frameId;
        scaleKey.scale = scale;
        KeyHelper::addKey(scaleKeys,scaleKey);
    }
    else {
        if(keyIndex < scaleKeys.size())
            scaleKeys[keyIndex].scale = scale;
    }
}

void SGNode::setVisibility(bool isVisible, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(visibilityKeys,frameId);
    if(keyIndex == -1 || visibilityKeys[keyIndex].id != frameId){
        SGVisibilityKey visibilityKey;
        visibilityKey.id = frameId;
        visibilityKey.visibility = isVisible;
        KeyHelper::addKey(visibilityKeys, visibilityKey);
    }
    else {
        if(keyIndex < visibilityKeys.size())
            visibilityKeys[keyIndex].visibility = isVisible;
    }
}

void SGNode::setPositionOnNode(Vector3 position, bool updateBB)
{
    if(type == NODE_LIGHT || type == NODE_ADDITIONAL_LIGHT)
        ShaderManager::lightChanged = true;
    
    node->setPosition(position, updateBB);
    node->updateAbsoluteTransformation();
    
    if(type == NODE_RIG || type == NODE_TEXT_SKIN) {
        shared_ptr<JointNode> rootJointNode = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(0);
        if(rootJointNode)
            rootJointNode->updateAbsoluteTransformationOfChildren();
    }
}

void SGNode::setRotationOnNode(Quaternion rotation, bool updateBB)
{
    node->setRotation(rotation, updateBB);
    node->updateAbsoluteTransformation();
    
    if(type == NODE_RIG || type == NODE_TEXT_SKIN)
        (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(0)->updateAbsoluteTransformationOfChildren();
}

void SGNode::setScaleOnNode(Vector3 scale, bool updateBB)
{
    node->setScale(scale, updateBB);
    node->updateAbsoluteTransformation();
    
    if(type == NODE_RIG || type == NODE_TEXT_SKIN)
        (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(0)->updateAbsoluteTransformationOfChildren();
}

void SGNode::setVisibilityOnNode(bool visibility)
{
    // TODO
}

void SGNode::setInitialKeyValues(int actionType)
{
    if(actionType == IMPORT_ASSET_ACTION || actionType == UNDO_ACTION || actionType == REDO_ACTION){
        SGPositionKey positionKey;
        positionKey.position = Vector3(0.0f);
        
        SGRotationKey rotationKey;
        rotationKey.rotation = node->getRotation();
        nodeInitialRotation = node->getRotation();
        
        SGScaleKey scaleKey;
        scaleKey.scale = Vector3(1.0);
        
        SGVisibilityKey visibilityKey;
        
        if(type == NODE_CAMERA) {
            positionKey.position = Vector3(RENDER_CAM_INIT_POS_X,RENDER_CAM_INIT_POS_Y,RENDER_CAM_INIT_POS_Z);
            rotationKey.rotation = Vector3(0.0f);
            scaleKey.scale = Vector3(1.0);
            visibilityKey.visibility = true;
            positionKey.id = rotationKey.id = scaleKey.id = visibilityKey.id = 0;
            KeyHelper::addKey(positionKeys, positionKey);
            KeyHelper::addKey(rotationKeys, rotationKey);
            KeyHelper::addKey(scaleKeys, scaleKey);
            KeyHelper::addKey(visibilityKeys, visibilityKey);
            
        } else if (type == NODE_LIGHT || type == NODE_ADDITIONAL_LIGHT) {
            positionKey.position = node->getAbsolutePosition();
            float distance = (type == NODE_LIGHT) ? 300.0 : 50.0;
            float red = ((assetId - LIGHT_STARTING_ID) % 4 == 1) ? 1.0 : 0.0;
            float green = ((assetId - LIGHT_STARTING_ID) % 4 == 2) ? 1.0 : 0.0;
            float blue = ((assetId - LIGHT_STARTING_ID) % 4 == 3) ? 1.0 : 0.0;
            if((assetId - LIGHT_STARTING_ID) % 4 == 0){
                red = green = blue = 1.0;
            }
            getProperty(SPECIFIC_FLOAT).value.x = distance; // Vector4(distance, 0, 0, 0), UNDEFINED);
            
            rotationKey.rotation = Quaternion(0.8, 0.09, 0.3, -0.5);
            
            
            scaleKey.scale = Vector3(red, green, blue);
            visibilityKey.visibility = true;
            positionKey.id = rotationKey.id = scaleKey.id = visibilityKey.id = 0;
            
            KeyHelper::addKey(positionKeys, positionKey);
            KeyHelper::addKey(rotationKeys, rotationKey);
            KeyHelper::addKey(scaleKeys, scaleKey);
            KeyHelper::addKey(visibilityKeys, visibilityKey);
        } else {
            visibilityKey.visibility = true;
            positionKey.id = rotationKey.id = scaleKey.id = visibilityKey.id = 0;
            if(positionKeys.size() == 0)
                KeyHelper::addKey(positionKeys, positionKey);
            if(rotationKeys.size() == 0)
                KeyHelper::addKey(rotationKeys, rotationKey);
            if(scaleKeys.size() == 0)
                KeyHelper::addKey(scaleKeys, scaleKey);
            if(visibilityKeys.size() == 0)
                KeyHelper::addKey(visibilityKeys, visibilityKey);
            
            int jointCount = (type == NODE_RIG || type == NODE_TEXT_SKIN) ? (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointCount():0;
            
            for (int i = 0; i < jointCount; i++) {
                SGJoint *joint = joints[i];
                
                if(joint) {
                    shared_ptr<JointNode> jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(i);
                    SGPositionKey jointPositionKey;
                    if (type == NODE_TEXT_SKIN)
                        jointPositionKey.position = jointNode->getPosition();
                    else
                        jointPositionKey.position = Vector3(0.0f);
                    SGRotationKey jointRotationKey;
                    jointRotationKey.rotation = jointNode->getRotation();
                    SGScaleKey jointScaleKey;
                    jointScaleKey.scale = Vector3(1.0);
                    SGVisibilityKey jointVisibilityKey;
                    jointVisibilityKey.visibility = true;
                    jointsInitialRotations.push_back(jointRotationKey.rotation);
                    jointPositionKey.id = jointRotationKey.id = jointScaleKey.id = jointVisibilityKey.id = 0;
                    if(joint->positionKeys.size() == 0)
                        KeyHelper::addKey(joint->positionKeys, jointPositionKey);
                    if(joint->rotationKeys.size() == 0)
                        KeyHelper::addKey(joint->rotationKeys, jointRotationKey);
                    if(joint->scaleKeys.size() == 0)
                        KeyHelper::addKey(joint->scaleKeys, jointScaleKey);
                    if(joint->visibilityKeys.size() == 0)
                        KeyHelper::addKey(joint->visibilityKeys, jointVisibilityKey);
                    joint->jointNode = jointNode;
                }
            }
        }
    }else if(actionType == OPEN_SAVED_FILE){
        int jointCount = (type == NODE_RIG || type == NODE_TEXT_SKIN) ? (int)(dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointCount():0;
        
        if(type == NODE_ADDITIONAL_LIGHT || type == NODE_LIGHT) {
            
            if(getProperty(LIGHT_TYPE).value.x == -1) {
                
                if(rotationKeys.size() > 0 && type == NODE_ADDITIONAL_LIGHT) {
                    for(int i = 0; i < rotationKeys.size(); i++) {
                        SGScaleKey scaleKey;
                        scaleKey.scale = Vector3(rotationKeys[i].rotation.x, rotationKeys[i].rotation.y, rotationKeys[i].rotation.z);
                        scaleKey.id = rotationKeys[i].id;
                        KeyHelper::addKey(scaleKeys, scaleKey);
                    }
                    getProperty(SPECIFIC_FLOAT).value.x = rotationKeys[0].rotation.w;
                }
                rotationKeys.clear();
                
                Vector3 direction = Vector3(0.0) - node->getAbsolutePosition();
                
                Quaternion delta = MathHelper::rotationBetweenVectors(direction, Vector3(0.0, 1.0, 0.0));
                Vector3 lineRot = Vector3(0.0);
                SGRotationKey rotationKey;
                rotationKey.rotation = MathHelper::RotateNodeInWorld(lineRot, delta);
                rotationKey.id = 0;
                KeyHelper::addKey(rotationKeys, rotationKey);
                getProperty(LIGHT_TYPE).value.x = false;
                
            }
            
            if(getProperty(SPECIFIC_FLOAT).value.x == 0.0)
                getProperty(SPECIFIC_FLOAT).value.x = (type == NODE_LIGHT) ? 300.0 : 50.0;
        }
        
        for (int i = 0; i < jointCount; i++) {
            shared_ptr<JointNode> jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(i);
            SGRotationKey jointRotationKey;
            jointRotationKey.rotation = jointNode->getRotation();
            jointsInitialRotations.push_back(jointRotationKey.rotation);
            if(i < joints.size()) {
                if(joints[i]->positionKeys.size() <= 0) {
                    SGPositionKey jointPositionKey;
                    jointPositionKey.position = jointNode->getPosition();
                    joints[i]->setPosition(jointPositionKey.position, 0);
                    KeyHelper::addKey(joints[i]->positionKeys, jointPositionKey);
                }
                if(joints[i]->scaleKeys.size() <= 0) {
                    SGScaleKey jointScaleKey;
                    jointScaleKey.scale = Vector3(1.0);
                    joints[i]->setScale(jointScaleKey.scale,0);
                    KeyHelper::addKey(joints[i]->scaleKeys, jointScaleKey);
                }
                joints[i]->jointNode = jointNode;
            }
        }
    }
}

void SGNode::setKeyForFrame(int frameId, ActionKey& key)
{
    //erase all keys in this frame if exist
    int index;
    if(positionKeys.size()) {
        index = KeyHelper::getKeyIndex(positionKeys, frameId);
        if(index!=-1 && index < positionKeys.size() && positionKeys[index].id == frameId){
            positionKeys.erase(positionKeys.begin()+index);
        }
    }
    if(rotationKeys.size()) {
        index = KeyHelper::getKeyIndex(rotationKeys, frameId);
        if(index!=-1 &&  index < rotationKeys.size() && rotationKeys[index].id == frameId){
            rotationKeys.erase(rotationKeys.begin()+index);
        }
    }
    if(scaleKeys.size()) {
        index = KeyHelper::getKeyIndex(scaleKeys, frameId);
        if(index!=-1 && index < scaleKeys.size() && scaleKeys[index].id == frameId){
            scaleKeys.erase(scaleKeys.begin()+index);
        }
    }
    
    if(key.isPositionKey) setPosition(key.position, frameId);
    
    if(key.isRotationKey) setRotation(key.rotation, frameId);
    
    if(key.isScaleKey)    setScale(key.scale, frameId);
}

NODE_TYPE SGNode::getType()
{
    return type;
}

void SGNode::setType(NODE_TYPE type)
{
    this->type = type;
}

Quaternion SGNode::GetLocalQuaternion(int boneIndex, Quaternion q)
{
    shared_ptr<AnimatedMeshNode> selectedNode = dynamic_pointer_cast<AnimatedMeshNode>(node);
    shared_ptr<JointNode> bone = selectedNode->getJointNode(boneIndex);
    shared_ptr<JointNode> parent = dynamic_pointer_cast<JointNode>(bone->Parent);
    return q * getGlobalQuaternion(parent).makeInverse();
}

ActionKey SGNode::getKeyForFrame(int frameId)
{
    ActionKey key;
    int index = KeyHelper::getKeyIndex(positionKeys, frameId);
    if(index != -1 && index < positionKeys.size() && positionKeys[index].id == frameId){
        key.position = positionKeys[index].position;
        key.isPositionKey = true;
    }
    
    index = KeyHelper::getKeyIndex(rotationKeys, frameId);
    if(index != -1 && index < rotationKeys.size() && rotationKeys[index].id == frameId){
        key.rotation = rotationKeys[index].rotation;
        key.isRotationKey = true;
    }
    
    index = KeyHelper::getKeyIndex(scaleKeys, frameId);
    if(index != -1 && index < scaleKeys.size() && scaleKeys[index].id == frameId){
        key.scale = scaleKeys[index].scale;
        key.isScaleKey = true;
    }
    
    return key;
}

Vector3 SGNode::getNodePosition()
{
    return node->getPosition();
}

Vector3 SGNode::getNodeScale()
{
    return node->getScale();
}

Vector3 SGNode::getJointPosition(int jointId, bool isAbsolutePosition)
{
    shared_ptr<AnimatedMeshNode> selectedNode = dynamic_pointer_cast<AnimatedMeshNode>(node);
    shared_ptr<JointNode> joint = selectedNode->getJointNode(jointId);
    return ((isAbsolutePosition)? joint->getAbsolutePosition():joint->getPosition());
}

Quaternion SGNode::getGlobalQuaternion(shared_ptr<JointNode> bone)
{
    
    shared_ptr<JointNode> parent = dynamic_pointer_cast<JointNode>(bone->Parent);
    if(parent)
        return bone->getRotation() * getGlobalQuaternion(parent);
    else
        return bone->getRotation();
}

void SGNode::removeAnimationInCurrentFrame(int currentFrame)
{
    for(int i = 0; i < joints.size(); i++)
        joints[i]->removeAnimationInCurrentFrame(currentFrame);
    
    int keyIndex = KeyHelper::getKeyIndex(positionKeys, currentFrame);
    if(keyIndex != -1 && positionKeys[keyIndex].id == currentFrame)
        positionKeys.erase(positionKeys.begin()+keyIndex);
    
    keyIndex = KeyHelper::getKeyIndex(rotationKeys, currentFrame);
    if(keyIndex != -1 && rotationKeys[keyIndex].id == currentFrame)
        rotationKeys.erase(rotationKeys.begin()+keyIndex);
    
    keyIndex = KeyHelper::getKeyIndex(scaleKeys, currentFrame);
    if(keyIndex != -1 && scaleKeys[keyIndex].id == currentFrame)
        scaleKeys.erase(scaleKeys.begin()+keyIndex);
}

void SGNode::CCD(shared_ptr<JointNode> bone, Vector3 target, int parentHeirarchy, int currentFrame)
{
    
    shared_ptr<JointNode> parent = dynamic_pointer_cast<JointNode>(bone->getParent());
    if(parentHeirarchy == 1)
        parent = dynamic_pointer_cast<JointNode>(parent->getParent());
    
    bone->updateAbsoluteTransformation();
    parent->updateAbsoluteTransformation();
    
    Vector3 currentDir = bone->getAbsolutePosition() - parent->getAbsolutePosition();
    currentDir.normalize();
    Vector3 targetDir = target - parent->getAbsolutePosition();
    targetDir.normalize();
    
    double dot = currentDir.dotProduct(targetDir);
    
    if(dot < 1.0) {
        Vector3 cross = currentDir.crossProduct(targetDir);
        cross.normalize();
        
        double turnAngle = acos(dot);
        Quaternion delta;
        delta.fromAngleAxis(turnAngle, cross);
        delta.normalize();
        
        Quaternion global = MathHelper::getGlobalQuaternion(parent);
        global = global * delta;
        Quaternion local = global * MathHelper::getGlobalQuaternion(dynamic_pointer_cast<JointNode>(parent->getParent())).makeInverse();
        
        parent->setRotation(local, true);
        joints[parent->getID()]->setRotation(local, currentFrame);
        joints[parent->getID()]->setRotationOnNode(local, true);
        parent->updateAbsoluteTransformation();
        parent->updateAbsoluteTransformationOfChildren();
    }
}

void SGNode::MoveBone(shared_ptr<JointNode> bone,Vector3 target,int currentFrame)
{
    if(bone) {
        shared_ptr<JointNode> parent =  dynamic_pointer_cast<JointNode>(bone->getParent());
        shared_ptr<JointNode> grandParent = dynamic_pointer_cast<JointNode>(parent->getParent());
        bone->updateAbsoluteTransformation();
        parent->updateAbsoluteTransformation();
        grandParent->updateAbsoluteTransformation();
        
        double parentChildLength = bone->getAbsolutePosition().getDistanceFrom(parent->getAbsolutePosition());
        double parentTargetLength = target.getDistanceFrom(parent->getAbsolutePosition());
        double grandParentLength = parent->getAbsolutePosition().getDistanceFrom(grandParent->getAbsolutePosition());
        double grandTargetLength = target.getDistanceFrom(grandParent->getAbsolutePosition());
        // target outside the  Grandparent circle
        if((grandParentLength + parentTargetLength) > (grandParentLength + parentChildLength)){
            CCD(parent, target,0,currentFrame);
        }
        
        parentTargetLength = target.getDistanceFrom(parent->getAbsolutePosition());
        grandTargetLength = target.getDistanceFrom(grandParent->getAbsolutePosition());
        
        // target outside the parent circle
        if((parentTargetLength) > (parentChildLength)) {
            CCD(bone, target,1,currentFrame);
            CCD(bone, target,0,currentFrame);
        }
        else {
            float angleX = -0.1;
            float totAng = 0.1;
            while(1){
                Quaternion delta = Quaternion(Vector3(angleX, 0.0, 0.0) * DEGTORAD);
                Quaternion local = delta * grandParent->getRotation();
                grandParent->setRotation(local, true);
                joints[grandParent->getID()]->setRotation(local, currentFrame);
                joints[grandParent->getID()]->setRotationOnNode(local, true);
                grandParent->updateAbsoluteTransformation();
                grandParent->updateAbsoluteTransformationOfChildren();
                parentTargetLength = target.getDistanceFrom(parent->getAbsolutePosition());
                
                if(parentChildLength <= parentTargetLength) {
                    break;
                }
                totAng += 0.1;
                if(totAng >= 360.0) {
                    parentChildLength -= 0.10;
                }
            }
            CCD(bone,target,0,currentFrame);
        }
    }
}

int toString(const char a[])
{
    int sign = 1;
    if (a[0] == '-') {  // Handle negative integers
        sign = -1;
    }
    
    int offset = 0;
    if (sign == -1)  // Set starting position to convert
        offset = 1;
    else
        offset = 0;
    
    int n = 0;
    for (int c = offset; a[c] != '\0'; c++)
        n = n * 10 + a[c] - '0';
    
    n = n * sign;
    
    return n;
}

Mesh* SGNode::readData(ifstream *filePointer, int &origIndex)
{
    assetId = FileHelper::readInt(filePointer);
    int sgbVersion = FileHelper::readInt(filePointer);
//    if(sgbVersion < SGB_VERSION_CURRENT)
//        legacyReadData(filePointer, sgbVersion, origIndex);
    
    type = (NODE_TYPE)FileHelper::readInt(filePointer);
    setPropertiesOfNode();
    (IsPropertyExists(HAS_PHYSICS)) ? getProperty(HAS_PHYSICS).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(PHYSICS_KIND)) ? getProperty(PHYSICS_KIND).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(WEIGHT)) ? getProperty(WEIGHT).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(FORCE_MAGNITUDE)) ? getProperty(FORCE_MAGNITUDE).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(IS_SOFT)) ? getProperty(IS_SOFT).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(LIGHT_TYPE)) ? getProperty(LIGHT_TYPE).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(SPECIFIC_FLOAT)) ? getProperty(SPECIFIC_FLOAT).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    (IsPropertyExists(FONT_SIZE)) ? getProperty(FONT_SIZE).value.x = FileHelper::readFloat(filePointer) : FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(FORCE_DIRECTION))
        (getProperty(FORCE_DIRECTION).value = Vector4(FileHelper::readVector3(filePointer), 0.0));
    else
        FileHelper::readVector3(filePointer);
    
    if(IsPropertyExists(VERTEX_COLOR)) {
        (getProperty(VERTEX_COLOR).value = Vector4(FileHelper::readVector3(filePointer), 0.0));
        Vector4 value = getProperty(VERTEX_COLOR).value;
    } else
        FileHelper::readVector3(filePointer);
    
    FileHelper::readVector3(filePointer);
    FileHelper::readVector3(filePointer);
    FileHelper::readVector3(filePointer);
    FileHelper::readVector3(filePointer);
    
    int materialPropsCount = FileHelper::readInt(filePointer);
    
    for(int i = 0; i < materialPropsCount; i++) {
        MaterialProperty* mP = new MaterialProperty(type);
        mP->readProperties(filePointer);
        materialProps.push_back(mP);
    }
    
    Mesh * mesh = NULL;
    if(type != NODE_CAMERA && type != NODE_LIGHT) {
        MESH_TYPE meshType = (MESH_TYPE)FileHelper::readInt(filePointer);
        if(meshType == MESH_TYPE_LITE) {
            mesh = MeshRW::readMeshData(filePointer);
        } else {
            mesh = MeshRW::readSkinMeshData(filePointer);
        }
    }
    
    origIndex = FileHelper::readInt(filePointer);
    isRigged = FileHelper::readBool(filePointer);
    name = FileHelper::readWString(filePointer);
    int keysCount = FileHelper::readInt(filePointer);
    KeyHelper::readData(filePointer,keysCount,positionKeys, rotationKeys, scaleKeys, visibilityKeys);
    joints.clear();
    int jointsCount = FileHelper::readInt(filePointer);
    for(int i = 0; i < jointsCount; i++)
    {
        SGJoint *joint = new SGJoint();
        joint->readData(filePointer);
        joints.push_back(joint);
    }
        
    return mesh;
}

void SGNode::writeData(ofstream *filePointer, vector<SGNode*> &nodes)
{
    
    FileHelper::writeInt(filePointer, (type == NODE_ADDITIONAL_LIGHT) ? assetId : -1); // Asset Id in Previous version
    FileHelper::writeInt(filePointer,SGB_VERSION_CURRENT); // New sgb version because of changing the format
    FileHelper::writeInt(filePointer,(int)type);
    FileHelper::writeFloat(filePointer, IsPropertyExists(HAS_PHYSICS) ? getProperty(HAS_PHYSICS).value.x : -1.0);
    FileHelper::writeFloat(filePointer, IsPropertyExists(PHYSICS_KIND) ? getProperty(PHYSICS_KIND).value.x : -1.0);
    FileHelper::writeFloat(filePointer, IsPropertyExists(WEIGHT) ? getProperty(WEIGHT).value.x : -1.0);
    FileHelper::writeFloat(filePointer, IsPropertyExists(FORCE_MAGNITUDE) ? getProperty(FORCE_MAGNITUDE).value.x : -1.0);
    FileHelper::writeFloat(filePointer, IsPropertyExists(IS_SOFT) ? getProperty(IS_SOFT).value.x : -1.0);
    FileHelper::writeFloat(filePointer, IsPropertyExists(LIGHT_TYPE) ? getProperty(LIGHT_TYPE).value.x : -1.0);
    FileHelper::writeFloat(filePointer, (IsPropertyExists(SPECIFIC_FLOAT)) ? getProperty(SPECIFIC_FLOAT).value.x : -1.0);
    FileHelper::writeFloat(filePointer, (IsPropertyExists(FONT_SIZE)) ? getProperty(FONT_SIZE).value.x : -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    
    Vector4 forceDir = (IsPropertyExists(FORCE_DIRECTION)) ? getProperty(FORCE_DIRECTION).value : Vector4(-1.0);
    FileHelper::writeVector3(filePointer, Vector3(forceDir.x, forceDir.y, forceDir.z));
    Vector4 lightColor = (IsPropertyExists(VERTEX_COLOR)) ? getProperty(VERTEX_COLOR).value : Vector4(1.0);
    FileHelper::writeVector3(filePointer, Vector3(lightColor.x, lightColor.y, lightColor.z));
    FileHelper::writeVector3(filePointer, Vector3(-1.0));
    FileHelper::writeVector3(filePointer, Vector3(-1.0));
    FileHelper::writeVector3(filePointer, Vector3(-1.0));
    FileHelper::writeVector3(filePointer, Vector3(-1.0));
    
    FileHelper::writeInt(filePointer, (int)materialProps.size());
    for( int i = 0; i < materialProps.size(); i++) {
        materialProps[i]->writeProperties(filePointer);
    }
    
    if(type != NODE_CAMERA && type != NODE_LIGHT) {
        int meshType = (int)dynamic_pointer_cast<MeshNode>(node)->getMesh()->meshType;
        FileHelper::writeInt(filePointer, meshType);

        if(meshType == MESH_TYPE_LITE)
            MeshRW::writeMeshData(filePointer, dynamic_pointer_cast<MeshNode>(node)->getMesh());
        else
            MeshRW::writeSkinMeshData(filePointer, dynamic_pointer_cast<AnimatedMeshNode>(node)->getMesh(), dynamic_pointer_cast<AnimatedMeshNode>(node));
    }
    
    int nodeIndex = 0;
    if(node->type == NODE_TYPE_INSTANCED) {
        int actionId = ((SGNode*)node->original->getUserPointer())->actionId;
        for (int i = 0; i < nodes.size(); i++) {
            if(actionId == nodes[i]->actionId)
                nodeIndex = i;
        }
    }
    
    FileHelper::writeInt(filePointer, nodeIndex); //Node Index of Original Node if Instanced
    
    FileHelper::writeBool(filePointer,isRigged);
    
    FileHelper::writeWString(filePointer,name);
    KeyHelper::writeData(filePointer, positionKeys, rotationKeys, scaleKeys, visibilityKeys);
    FileHelper::writeInt(filePointer, int(joints.size()));
    int i;
    for(i = 0; i < joints.size(); i++)
        joints[i]->writeData(filePointer);
}

void SGNode::setMeshProperties(int matIndex, float refraction, float reflection, bool isLighting, bool isVisible , bool isPhysicsObj, int physicsType, float fMagnitude, float currentFrame)
{
    getProperty(LIGHTING).value.x = isLighting;
    getProperty(REFRACTION, matIndex).value.x = refraction;
    getProperty(REFLECTION, matIndex).value.x = reflection;
    getProperty(VISIBILITY).value.x = isVisible;
    getProperty(HAS_PHYSICS).value.x = isPhysicsObj;
    getProperty(PHYSICS_KIND).value.x = physicsType;
    getProperty(FORCE_MAGNITUDE).value.x = fMagnitude;
    setVisibility(isVisible, currentFrame);
}

void SGNode::clearSGJoints()
{
    if(type != NODE_RIG && type != NODE_TEXT_SKIN)
        return;
    for(int i = 0;i < joints.size();i++){
        if(joints[i])
            delete joints[i];
    }
    joints.clear();
}

void SGNode::createSGJoints()
{
    if(type != NODE_RIG)
        return;
    for(int i = 0;i < (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointCount();i++){
        SGJoint *joint = new SGJoint();
        joint->jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(i);
        joints.push_back(joint);
    }
}

void SGNode::faceUserCamera(shared_ptr<CameraNode> viewCamera, int currentFrame)
{
    if(type != NODE_LIGHT)
        return;
    Vector3 lightPos = node->getAbsolutePosition();
    Vector3 camPos = viewCamera->getPosition();
    Vector3 rotDir = (camPos - lightPos).normalize();
    Quaternion rotQ = MathHelper::rotationBetweenVectors(rotDir, Vector3(0.0, 0.0, 1.0));
    setRotation(rotQ, currentFrame);
    node->setRotation(rotQ, true);
}

bool SGNode::IsPropertyExists(PROP_INDEX pIndex)
{
    if(options.find(pIndex) != options.end())
        return true;
    else {
        std::map<PROP_INDEX, Property>::iterator pIt;
        for(pIt = options.begin(); pIt != options.end(); pIt++) {
            if(pIt->second.subProps.find(pIndex) != pIt->second.subProps.end())
                return true;
        }
        
        return false;
    }
}

std::map< PROP_INDEX, Property > SGNode::getAllProperties(int meshBufferIndex)
{
    std::map< PROP_INDEX, Property > allProps;
    
    if(meshBufferIndex == NOT_SELECTED || materialProps.size() == 1) {
        allProps.insert(options.begin(), options.end());
        std::map< PROP_INDEX, Property > matProps = materialProps[0]->getProps();
        allProps.insert(matProps.begin(), matProps.end());
    }
    
    if(materialProps.size() > 1 && meshBufferIndex != NOT_SELECTED) {
        std::map< PROP_INDEX, Property > matProps = materialProps[meshBufferIndex]->getProps();
        allProps.insert(matProps.begin(), matProps.end());
    }
    return allProps;
}

Property& SGNode::getProperty(PROP_INDEX pIndex, int meshBufferIndex)
{
    
    if(options.find(pIndex) == options.end() || meshBufferIndex != NOT_SELECTED) {
        std::map<PROP_INDEX, Property>::iterator pIt;
        
        int matIndex = 0;
        if(meshBufferIndex != NOT_SELECTED)
            matIndex = meshBufferIndex;
        
        if(materialProps.size() > 0) {
            std::map<PROP_INDEX, Property>& matProps = materialProps[matIndex]->getProps();
            if(matProps.find(pIndex) != matProps.end()) {
                return matProps[pIndex];
            } else {
                PROP_INDEX ind = checkPropertyInSubProps(matProps, pIndex);
                if(ind != UNDEFINED) {
                    if(matProps.size() > 0 && matProps.find(ind) != matProps.end()) {
                        return matProps.find(ind)->second.subProps.find(pIndex)->second;
                    }
                }
            }
        } else {
            //printf("\n Material Props 0 prop Index %d ", pIndex);
        }
        
        PROP_INDEX pI = checkPropertyInSubProps(options, pIndex);
        if(pI != UNDEFINED) {
            return options[pI].subProps[pIndex];
        }
        else {
            Property emptyProp;
            emptyProp.index = pIndex;
            emptyProp.value = Vector4(0.0);
            emptyProp.parentIndex = UNDEFINED;
            emptyProp.type = TYPE_NONE;
            emptyProp.isEnabled = false;
            options.insert(std::pair<PROP_INDEX, Property>(pIndex, emptyProp));
            return options[pIndex];
        }
        
    } else {
        if(options.size() > 0 && options.find(pIndex) != options.end()) {
            return options.find(pIndex)->second;
        } else {
            addOrUpdateProperty(pIndex, Vector4(0.0), UNDEFINED, TYPE_NONE);
            return options[pIndex];
        }
    }
}

PROP_INDEX SGNode::checkPropertyInSubProps(std::map< PROP_INDEX, Property >& propsMap, PROP_INDEX pIndex)
{
    std::map<PROP_INDEX, Property>::iterator pIt;
    for(pIt = propsMap.begin(); pIt != propsMap.end(); pIt++) {
        if(pIt->second.subProps.find(pIndex) != pIt->second.subProps.end())
            return pIt->first;
    }
    return UNDEFINED;
}

void SGNode::addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE propType, string title, string groupName, string fileName, ICON_INDEX iconId)
{
    Property property;
    property.index = index;
    property.parentIndex = (type == NODE_LIGHT || type == NODE_ADDITIONAL_LIGHT) ? UNDEFINED : parentProp;
    property.value = value;
    property.iconId = iconId;
    property.isEnabled = true;
    
    if(title.length() > 2) {
        property.title = title;
        property.type = propType;
    }
    
    if(groupName.length() > 2)
        property.groupName = groupName;
    if(fileName.length() > 5 || propType == IMAGE_TYPE)
        property.fileName = fileName;
    
    if(property.parentIndex != UNDEFINED && options.find(property.parentIndex) != options.end()) {
        checkAndUpdatePropsMap(options[parentProp].subProps, property);
    } else if(options.find(index) == options.end()) {
        property.type = propType;
        options.insert(pair<PROP_INDEX, Property>( index, property));
    }
    else {
        if(options.find(index)->second.isEnabled == false) {
            options[index] = property;
        } else {
            options.find(index)->second.value = property.value;
            if(property.fileName.length() > 5 || propType == IMAGE_TYPE)
                options.find(index)->second.fileName = property.fileName;
        }
    }
    
}

void SGNode::checkAndUpdatePropsMap(std::map < PROP_INDEX, Property > &propsMap, Property property)
{
    if(propsMap.find(property.index) == propsMap.end()) {
        propsMap.insert(pair<PROP_INDEX, Property>( property.index, property));
    } else {
        propsMap.find(property.index)->second.value = property.value;
        if(property.fileName.length() > 5 || property.type == IMAGE_TYPE)
            propsMap.find(property.index)->second.fileName = property.fileName;
    }
}
