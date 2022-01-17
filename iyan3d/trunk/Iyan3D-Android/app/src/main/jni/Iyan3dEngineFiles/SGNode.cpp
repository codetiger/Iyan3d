#include "HeaderFiles/SGNode.h"
#include "HeaderFiles/ShaderManager.h"
#include "TextMesh3d.h"

#ifdef ANDROID
#include "../opengl.h"
#endif


SGNode::SGNode(NODE_TYPE type){
    this->type = type;
    //node = shared_ptr<Node>();
    
    isTempNode = false;
    optionalFilePath = "";
    props.transparency = 1.0;
    props.isVisible = true;
    props.isSelected = false;
    isMirrorEnabled = false;
    props.isLighting = true;
    props.brightness = 1.0;
    props.shininess = 0.0;
    props.fontSize = 20;
    props.perVertexColor = false;
    props.nodeSpecificFloat = 0.0;
    textureName = "";
}
SGNode::~SGNode(){
    clearSGJoints();
    node.reset();
}
shared_ptr<Node> SGNode::loadNode(int assetId, std::string texturePath,NODE_TYPE objectType,SceneManager *smgr, std::wstring objectName,int width,int height, Vector4 objSpecificColor , string &specificFilePath)
{
    shared_ptr<Node> node;
    switch (objectType){
        case NODE_CAMERA:{
            Mesh *mesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/camera.sgm",smgr->device);
            node = smgr->createNodeFromMesh(mesh,"setUniforms");
            node->setPosition(Vector3(RENDER_CAM_INIT_POS_X,RENDER_CAM_INIT_POS_Y,RENDER_CAM_INIT_POS_Z));
            props.isLighting = false;
            node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
            break;
        }
        case NODE_LIGHT:{
            node = initLightSceneNode(smgr);
            props.vertexColor = DEFAULT_LIGHT_COLOR;
            props.nodeSpecificFloat = DEFAULT_FADE_DISTANCE;
            break;
        }
        case NODE_OBJ:
        case NODE_SGM:{
            textureName = texturePath;
            oriTextureName = textureName;
            props.vertexColor = Vector3(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z);
            node = loadSGMandOBJ(assetId,objectType,smgr);
            break;
        }
        case NODE_TEXT:{
            textureName = texturePath;
            props.vertexColor = Vector3(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z);
            props.oriVertexColor = props.vertexColor;
            node = load3DText(smgr, objectName, 4, 4, 16, specificFilePath, objSpecificColor, height / 50.0f, 4);
            props.transparency = 1.0;
            props.nodeSpecificFloat = height;
            optionalFilePath = specificFilePath;
            props.fontSize = width;
            break;
        }
        case NODE_RIG:{
            textureName = texturePath;
            node = loadSGR(assetId,objectType,smgr);
            isMirrorEnabled = joints.size() == HUMAN_JOINTS_SIZE ? true : false;
            break;
        }
        case NODE_IMAGE:{
            float aspectRatio = (float)objSpecificColor.x/(float)objSpecificColor.y;
            props.vertexColor = Vector3(objSpecificColor.x,objSpecificColor.y,objSpecificColor.z);
            node = loadImage(ConversionHelper::getStringForWString(objectName) + ".png", smgr , aspectRatio);
            break;
        }
        case NODE_VIDEO:{
            float aspectRatio = (float)objSpecificColor.x/(float)objSpecificColor.y;
            props.vertexColor = Vector3(objSpecificColor.x,objSpecificColor.y,objSpecificColor.z);
            node = loadVideo(ConversionHelper::getStringForWString(objectName), smgr , aspectRatio);
            break;
        }
        case NODE_TEXT_SKIN:{
            // 'width' here is font size and 'height' is bevel value
            textureName = texturePath;
            props.vertexColor = Vector3(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z);
            node = loadSkin3DText(smgr, objectName, 4, 4, 16, specificFilePath, objSpecificColor, height / 50.0f, 4);
            props.transparency = 1.0;
            props.nodeSpecificFloat = height;
            optionalFilePath = specificFilePath;
            props.fontSize = width;
            break;
        }
        case NODE_ADDITIONAL_LIGHT:{
            this->assetId = assetId;
            node = addAdittionalLight(smgr, width, Vector3(objSpecificColor.x,objSpecificColor.y,objSpecificColor.z) , height);
            props.vertexColor = Vector3(objSpecificColor.x,objSpecificColor.y,objSpecificColor.z);
            props.nodeSpecificFloat = (height == 0) ? 50.0 : height;
            break;
        }
        default:
            Logger::log(ERROR, "SGNode::loadNode ", "Unknown node type to load");
            break;
    }
    if(node && node->getTextureCount() == 0) {
        Texture *nodeTex = smgr->loadTexture("Dummy Light",constants::BundlePath + "/dummy.png",TEXTURE_RGBA8,TEXTURE_BYTE);
        node->setTexture(nodeTex,1);
    }
    return node;
}

shared_ptr<Node> SGNode::addAdittionalLight(SceneManager* smgr, float distance , Vector3 lightColor, float fadeLevel)
{
    
    Mesh* lightMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm",smgr->device);
    shared_ptr<LightNode> lightNode = smgr->createLightNode(lightMesh,"setUniforms");
    
    lightNode->setPosition(Vector3((assetId - (LIGHT_STARTING_ID+4)) * 2.0 ,10.0,10.0));
    lightNode->setScale(Vector3(3.0));
    lightNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    lightNode->decayStartDistance = distance;
    lightNode->decayEndDistance = fadeLevel * 10;
    lightNode->lightColor = lightColor;
    
    return lightNode;
}

shared_ptr<Node> SGNode::loadSkin3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments) {
    if(bevelRadius == 0 || bevelSegments == 0)
        bevelSegments = 0;
    
    string pathForFont;
    if(fontPath == "") {
        width = 3;
        fontPath = DEFAULT_FONT_FILE;
        fontColor = Vector4(props.vertexColor.x, props.vertexColor.y, props.vertexColor.z,1.0);
    }
#ifdef IOS
    pathForFont = FileHelper::getCachesDirectory();
#elif ANDROID
    pathForFont = constants::DocumentsStoragePath + "/fonts/";
#endif
    
    pathForFont += fontPath;
    if(FILE *file = fopen(pathForFont.c_str(), "r"))
        fclose(file);
    else {
#ifdef IOS
        pathForFont = FileHelper::getDocumentsDirectory() + "Resources/Fonts/" + fontPath;
#elif ANDROID
        pathForFont = constants::DocumentsStoragePath + "/fonts/userFonts/"+fontPath;
#endif
        if(FILE *file = fopen(pathForFont.c_str(), "r"))
            fclose(file);
        else {
#ifdef IOS
            pathForFont = FileHelper::getDocumentsDirectory() + "Resources/Fonts/" + fontPath;
#elif ANDROID
            pathForFont = constants::DocumentsStoragePath + "/fonts/userFonts/"+fontPath;
#endif
            if(FILE *file = fopen(pathForFont.c_str(), "r"))
                fclose(file);
            else {
                printf("File not exists at %s ",pathForFont.c_str());
                return shared_ptr<Node>();
            }
        }
    }
    AnimatedMesh *mesh = TextMesh3d::get3DTextAnimatedMesh(text, bezierSegments, extrude, width, (char*)pathForFont.c_str(), fontColor, smgr->device, bevelRadius, bevelSegments);
    if(mesh == NULL)
        return shared_ptr<Node>();
    
    shared_ptr<AnimatedMeshNode> node = smgr->createAnimatedNodeFromMesh(mesh, "setUniforms",TEXT_RIG,MESH_TYPE_HEAVY);
    bool isSGJointsCreated = (joints.size() > 0) ? true : false;
    for(int i = 0;i < node->getJointCount(); i++){
        node->getJointNode(i)->setID(i);
        if(!isSGJointsCreated){
            SGJoint *joint = new SGJoint();
            joint->jointNode = node->getJointNode(i);
            joints.push_back(joint);
        }
    }
    
    if(node->skinType == CPU_SKIN) {
        node->getMeshCache()->recalculateNormalsT();
        node->getMeshCache()->pivotToOrigin();
    }
    node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_SHADOW_SKIN_L1));
    node->getMesh()->Commit();
    
    string textureFileName = FileHelper::getDocumentsDirectory() + textureName + ".png";
    
    if(textureName != "" && checkFileExists(textureFileName)) {
        props.perVertexColor = false;
        Texture *nodeTex = smgr->loadTexture(textureFileName,textureFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
        node->setTexture(nodeTex,1);
    } else
        props.perVertexColor = true;
    
    return node;
}

shared_ptr<Node> SGNode::load3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments) {
    if(bevelRadius == 0 || bevelSegments == 0)
        bevelSegments = 0;
    
    string pathForFont;
    if(fontPath == "") {
        width = 3;
        fontPath = DEFAULT_FONT_FILE;
        fontColor = Vector4(props.vertexColor.x, props.vertexColor.y, props.vertexColor.z,1.0);
    }
#ifdef IOS
    pathForFont = FileHelper::getCachesDirectory();
#elif ANDROID
    pathForFont = constants::DocumentsStoragePath + "/fonts/";
#endif
    
    pathForFont += fontPath;
    if(FILE *file = fopen(pathForFont.c_str(), "r"))
        fclose(file);
    else {
#ifdef IOS
        pathForFont = FileHelper::getDocumentsDirectory() + "Resources/Fonts/" + fontPath;
#elif ANDROID
        pathForFont = constants::DocumentsStoragePath + "/fonts/userFonts/"+fontPath;
#endif
        if(FILE *file = fopen(pathForFont.c_str(), "r"))
            fclose(file);
        else {
#ifdef IOS
            pathForFont = FileHelper::getDocumentsDirectory() + "Resources/Fonts/" + fontPath;
#elif ANDROID
            pathForFont = constants::DocumentsStoragePath + "/fonts/userFonts/"+fontPath;
#endif
            if(FILE *file = fopen(pathForFont.c_str(), "r"))
                fclose(file);
            else {
                printf("File not exists at %s ",pathForFont.c_str());
                return shared_ptr<Node>();
            }
        }
    }
    
    Mesh *mesh = TextMesh3d::get3DTextMesh(text, bezierSegments, extrude, width, (char*)pathForFont.c_str(), fontColor, smgr->device, bevelRadius, bevelSegments);
    
    if(mesh == NULL)
        return shared_ptr<Node>();

    shared_ptr<MeshNode> node = smgr->createNodeFromMesh(mesh, "setUniforms",MESH_TYPE_LITE);
    node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
    
    string textureFileName = FileHelper::getDocumentsDirectory() + textureName + ".png";
    
    if(textureName != "" && checkFileExists(textureFileName)) {
        props.perVertexColor = false;
        Texture *nodeTex = smgr->loadTexture(textureFileName,textureFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
        node->setTexture(nodeTex,1);
    } else
        props.perVertexColor = true;

    return node;
}


shared_ptr<Node> SGNode::loadSGMandOBJ(int assetId,NODE_TYPE objectType,SceneManager *smgr)
{
    string StoragePath;
    OBJMeshFileLoader *objLoader;
    string fileExt = (objectType == NODE_SGM) ? ".sgm":".obj";
    if(objectType == NODE_OBJ)
        objLoader = new OBJMeshFileLoader();
#ifdef IOS
    if (fileExt == ".sgm")
        StoragePath = constants::CachesStoragePath + "/";
    else if(fileExt == ".obj") {
        StoragePath = FileHelper::getDocumentsDirectory() + "/Resources/Objs/";
    }
#endif
#ifdef ANDROID
    StoragePath = constants::DocumentsStoragePath + "/mesh/";
#endif

    string meshPath = StoragePath + to_string(assetId) + fileExt;
    string textureFileName = StoragePath + textureName+".png";
    
    if(!checkFileExists(meshPath)) {
        meshPath = FileHelper::getDocumentsDirectory() + to_string(assetId) + fileExt;
        if(!checkFileExists(meshPath)){
            meshPath = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + to_string(assetId) + fileExt;
        }
        if(!checkFileExists(meshPath))
            return shared_ptr<Node>();
    }
    if(!checkFileExists(textureFileName)){
        textureFileName = FileHelper::getDocumentsDirectory() + textureName+".png";
        if(!checkFileExists(textureFileName)){
            textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + textureName+".png";
            if(!checkFileExists(textureFileName))
                textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Textures/" + textureName+".png";
        }
    }

    int objLoadStatus = 0;
    Mesh *mesh = (objectType == NODE_SGM) ? CSGRMeshFileLoader::createSGMMesh(meshPath,smgr->device) : objLoader->createMesh(meshPath,objLoadStatus,smgr->device);
    
    shared_ptr<MeshNode> node = smgr->createNodeFromMesh(mesh,"setUniforms");
    
    node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));

    if(textureName != "" && checkFileExists(textureFileName))
    {
        props.perVertexColor = false;
        Texture *nodeTex = smgr->loadTexture(textureFileName,textureFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
        node->setTexture(nodeTex,1);
    } else
        props.perVertexColor = true;
    
    if(objectType == NODE_OBJ && objLoader != NULL)
        delete objLoader;
    return node;
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

shared_ptr<Node> SGNode::loadSGR(int assetId,NODE_TYPE objectType,SceneManager *smgr){
    string StoragePath;
    
#ifdef IOS
    if (assetId >= 40000 && assetId < 50000)
        StoragePath = FileHelper::getDocumentsDirectory() + "/Resources/Rigs/";
    else
        StoragePath = constants::CachesStoragePath + "/";
#endif
    
#ifdef ANDROID
    StoragePath = constants::DocumentsStoragePath + "/mesh/";
#endif
    
    string meshPath = StoragePath + to_string(assetId) + ".sgr";
    string textureFileName = StoragePath + textureName+".png";
    if (!checkFileExists(meshPath)) {
        return shared_ptr<Node>();
    }
    AnimatedMesh *mesh = CSGRMeshFileLoader::LoadMesh(meshPath,smgr->device);
    setSkinningData((SkinMesh*)mesh);
    shared_ptr<AnimatedMeshNode> node = smgr->createAnimatedNodeFromMesh(mesh,"setUniforms",CHARACTER_RIG,MESH_TYPE_HEAVY);
    bool isSGJointsCreated = (joints.size() > 0) ? true : false;
    for(int i = 0;i < node->getJointCount();i++){
        node->getJointNode(i)->setID(i);
        if(!isSGJointsCreated){
            SGJoint *joint = new SGJoint();
            joint->jointNode = node->getJointNode(i);
            joints.push_back(joint);
        }
    }
    node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_SKIN_L1),true);
    
    if(textureName != "" && checkFileExists(textureFileName)) {
        props.perVertexColor = false;
        Texture *nodeTex = smgr->loadTexture(textureFileName,textureFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
        node->setTexture(nodeTex,1);
    } else
        props.perVertexColor = true;
    
    return node;
}
void SGNode::setSkinningData(SkinMesh *mesh){
    for(int j = 0; j < mesh->joints->size();j++){
        Joint *meshJoint = (*mesh->joints)[j];
        for(int v = 0;v < meshJoint->PaintedVertices->size();v++){
            int vertexId = (*meshJoint->PaintedVertices)[v]->vertexId;
            float weight = (*meshJoint->PaintedVertices)[v]->weight;
            Vector4 *optionalData1 = &(mesh->getHeavyVertexByIndex(vertexId)->optionalData1);
            Vector4 *optionalData2 = &(mesh->getHeavyVertexByIndex(vertexId)->optionalData2);
            Vector4 *optionalData3 = &(mesh->getHeavyVertexByIndex(vertexId)->optionalData3);
            Vector4 *optionalData4 = &(mesh->getHeavyVertexByIndex(vertexId)->optionalData4);
            if((*optionalData1).x == 0.0f){
                (*optionalData1).x = j + 1;
                (*optionalData2).x = weight;
            }else if((*optionalData1).y == 0.0f){
                (*optionalData1).y = j + 1;
                (*optionalData2).y = weight;
            }else if((*optionalData1).z == 0.0f){
                (*optionalData1).z = j + 1;
                (*optionalData2).z = weight;
            }else if((*optionalData1).w == 0.0f){
                (*optionalData1).w = j + 1;
                (*optionalData2).w = weight;
            }else if((*optionalData3).x == 0.0f){
                (*optionalData3).x = j + 1;
                (*optionalData4).x = weight;
            }else if((*optionalData3).y == 0.0f){
                (*optionalData3).y = j + 1;
                (*optionalData4).y = weight;
            }else if((*optionalData3).z == 0.0f){
                (*optionalData3).z = j + 1;
                (*optionalData4).z = weight;
            }else if((*optionalData3).w == 0.0f){
                (*optionalData3).w = j + 1;
                (*optionalData4).w = weight;
            }else
                Logger::log(INFO, "SGNODE:SetSkinningData()", "More joints affecting a vertex");
        }
    }
    // equalize the weights sum to 1.0
    for(int j = 0; j < mesh->getVerticesCount();j++){
        Vector4 od2 = (mesh->getHeavyVertexByIndex(j)->optionalData2);
        Vector4 od4 = (mesh->getHeavyVertexByIndex(j)->optionalData4);
        float sum = od2.x + od2.y + od2.z + od2.w + od4.x + od4.y + od4.z + od4.w;
        if(sum != 1.0){
            double dif = 1.0 - sum;
            mesh->getHeavyVertexByIndex(j)->optionalData2 = Vector4(od2.x + ((od2.x/sum) * dif),od2.y + ((od2.y/sum) * dif),od2.z + ((od2.z/sum) * dif),od2.w + ((od2.w/sum) * dif));
            mesh->getHeavyVertexByIndex(j)->optionalData4 = Vector4(od4.x + ((od4.x/sum) * dif),od4.y + ((od4.y/sum) * dif),od4.z + ((od4.z/sum) * dif),od4.w + ((od4.w/sum) * dif));
        }
        Vector4 od21 = (mesh->getHeavyVertexByIndex(j)->optionalData2);
        Vector4 od41 = (mesh->getHeavyVertexByIndex(j)->optionalData4);
        sum = od21.x + od21.y + od21.z + od21.w + od41.x + od41.y + od41.z + od41.w;
    }
}
shared_ptr<Node> SGNode::loadImage(string textureName,SceneManager *smgr, float aspectRatio){
    char* textureFileName = new char[256];
    
#ifdef ANDROID
    string path = constants::DocumentsStoragePath+ "/importedImages/"+textureName;
    textureFileName=(path).c_str();
#else
    sprintf(textureFileName, "%s/%s", constants::CachesStoragePath.c_str(),textureName.c_str());
#endif
    Texture *nodeTex = smgr->loadTexture(textureFileName,textureFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
    Logger::log(INFO, "SGNODE", "aspectratio" + to_string(aspectRatio));
    shared_ptr<PlaneMeshNode> planeNode = smgr->createPlaneNode("setUniforms" , aspectRatio);
    planeNode->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
    planeNode->setTexture(nodeTex,1);
    //delete [] textureFileName; //TODO TEST
    return planeNode;
}
shared_ptr<Node> SGNode::loadVideo(string videoFileName,SceneManager *smgr, float aspectRatio)
{
    Texture *nodeTex = smgr->loadTextureFromVideo(videoFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
    Logger::log(INFO, "SGNODE", "aspectratio" + to_string(aspectRatio));
    shared_ptr<PlaneMeshNode> planeNode = smgr->createPlaneNode("setUniforms" , aspectRatio);
    planeNode->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
    planeNode->setTexture(nodeTex,1);
    //delete [] textureFileName; //TODO TEST
    return planeNode;
}
shared_ptr<Node> SGNode::initLightSceneNode(SceneManager *smgr)
{
    Mesh* lightMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/light.sgm",smgr->device);
    shared_ptr<Node> lightNode = smgr->createNodeFromMesh(lightMesh,"setUniforms");
    Texture *nodeTex = smgr->loadTexture("Text light.png",constants::BundlePath + "/light.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    lightNode->setTexture(nodeTex,1);
    lightNode->setPosition(Vector3(-LIGHT_INIT_POS_X,LIGHT_INIT_POS_Y,LIGHT_INIT_POS_Z));
    lightNode->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
    lightNode->setScale(Vector3(4.0));
    props.isLighting = false;
    return lightNode;
}

void SGNode::setPosition(Vector3 position, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(positionKeys,frameId);
    
    if(keyIndex == -1 || positionKeys[keyIndex].id != frameId){
        SGPositionKey positionKey;
        positionKey.id = frameId;
        positionKey.position = position;
        KeyHelper::addKey(positionKeys, positionKey);
    }
    else
        positionKeys[keyIndex].position = position;
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
    else
        rotationKeys[keyIndex].rotation = rotation;
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
    else
        scaleKeys[keyIndex].scale = scale;
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
    else
        visibilityKeys[keyIndex].visibility = isVisible;
}
void SGNode::setPositionOnNode(Vector3 position)
{
    node->setPosition(position);
    node->updateAbsoluteTransformation();
    
    if(type == NODE_RIG || type == NODE_TEXT_SKIN) {
        shared_ptr<JointNode> rootJointNode = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(0);
        if(rootJointNode)
            rootJointNode->updateAbsoluteTransformationOfChildren();
    }
}
void SGNode::setRotationOnNode(Quaternion rotation)
{
    node->setRotationInDegrees(MathHelper::getEulerRotation(rotation));
    node->updateAbsoluteTransformation();
    
    if(type == NODE_RIG || type == NODE_TEXT_SKIN)
        (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(0)->updateAbsoluteTransformationOfChildren();
}
void SGNode::setScaleOnNode(Vector3 scale)
{
    node->setScale(scale);
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
        rotationKey.rotation = Quaternion(node->getRotationInRadians());
        nodeInitialRotation = Quaternion(node->getRotationInRadians());
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
            rotationKey.rotation = Quaternion(red,green,blue,distance);
            scaleKey.scale = Vector3(1.0);
            visibilityKey.visibility = true;
            positionKey.id = rotationKey.id = scaleKey.id = visibilityKey.id = 0;
            KeyHelper::addKey(positionKeys, positionKey);
            KeyHelper::addKey(rotationKeys, rotationKey);
            KeyHelper::addKey(scaleKeys, scaleKey);
            KeyHelper::addKey(visibilityKeys, visibilityKey);
            
        } else {
            visibilityKey.visibility = true;
            positionKey.id = rotationKey.id = scaleKey.id = visibilityKey.id = 0;
            KeyHelper::addKey(positionKeys, positionKey);
            KeyHelper::addKey(rotationKeys, rotationKey);
            KeyHelper::addKey(scaleKeys, scaleKey);
            KeyHelper::addKey(visibilityKeys, visibilityKey);
            
            int jointCount = (type == NODE_RIG || type == NODE_TEXT_SKIN) ? (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointCount():0;
            
            for (int i = 0; i < jointCount; i++) {
                SGJoint *joint = joints[i];
                
                if(joint) {
                    shared_ptr<JointNode> jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(
                                                                                              node))->getJointNode(i);
                    SGPositionKey jointPositionKey;
                    if (type == NODE_TEXT_SKIN)
                        jointPositionKey.position = jointNode->getPosition();
                    else
                        jointPositionKey.position = Vector3(0.0f);
                    SGRotationKey jointRotationKey;
                    jointRotationKey.rotation = Quaternion(jointNode->getRotationInRadians());
                    SGScaleKey jointScaleKey;
                    jointScaleKey.scale = Vector3(1.0);
                    SGVisibilityKey jointVisibilityKey;
                    jointVisibilityKey.visibility = true;
                    jointsInitialRotations.push_back(jointRotationKey.rotation);
                    jointPositionKey.id = jointRotationKey.id = jointScaleKey.id = jointVisibilityKey.id = 0;
                    KeyHelper::addKey(joint->positionKeys, jointPositionKey);
                    KeyHelper::addKey(joint->rotationKeys, jointRotationKey);
                    KeyHelper::addKey(joint->scaleKeys, jointScaleKey);
                    KeyHelper::addKey(joint->visibilityKeys, jointVisibilityKey);
                    joint->jointNode = jointNode;
                }
            }
        }
    }else if(actionType == OPEN_SAVED_FILE){
        int jointCount = (type == NODE_RIG || type == NODE_TEXT_SKIN) ? (int)(dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointCount():0;
        if(type == NODE_ADDITIONAL_LIGHT) {
            if(rotationKeys.size() > 0) {
                rotationKeys[0].rotation.x = (rotationKeys[0].rotation.x < 0.0 || rotationKeys[0].rotation.x > 1.0) ? 1.0 : rotationKeys[0].rotation.x;
                rotationKeys[0].rotation.y = (rotationKeys[0].rotation.y < 0.0 || rotationKeys[0].rotation.y > 1.0) ? 1.0 : rotationKeys[0].rotation.y;
                rotationKeys[0].rotation.z = (rotationKeys[0].rotation.z < 0.0 || rotationKeys[0].rotation.z > 1.0) ? 1.0 : rotationKeys[0].rotation.z;
                rotationKeys[0].rotation.w = (rotationKeys[0].rotation.w < 50.0 || rotationKeys[0].rotation.w > 300.0) ? 50.0 : rotationKeys[0].rotation.w;
            } else {
                SGRotationKey lrotationKey;
                lrotationKey.rotation = Quaternion(1.0,1.0,1.0,10.0);
                KeyHelper::addKey(rotationKeys, lrotationKey);
            }
        } else if (type == NODE_LIGHT) {
            if(scaleKeys.size() > 0) {
                scaleKeys[0].scale.x = (scaleKeys[0].scale.x < 0.0 || scaleKeys[0].scale.x > 1.0) ? 1.0 : scaleKeys[0].scale.x;
                scaleKeys[0].scale.y = (scaleKeys[0].scale.y < 0.0 || scaleKeys[0].scale.y > 1.0) ? 1.0 : scaleKeys[0].scale.y;
                scaleKeys[0].scale.z = (scaleKeys[0].scale.z < 0.0 || scaleKeys[0].scale.z > 1.0) ? 1.0 : scaleKeys[0].scale.z;
            } else {
                SGScaleKey lscaleKey;
                lscaleKey.scale = Vector3(1.0);
                KeyHelper::addKey(scaleKeys, lscaleKey);
            }
        }
        
        for (int i = 0; i < jointCount; i++) {
            shared_ptr<JointNode> jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(node))->getJointNode(i);
            SGRotationKey jointRotationKey;
            jointRotationKey.rotation = Quaternion(jointNode->getRotationInRadians());
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
void SGNode::setKeyForFrame(int frameId, ActionKey& key){
    //erase all keys in this frame if exist
    int index;
    if(positionKeys.size()) {
        index = KeyHelper::getKeyIndex(positionKeys, frameId);
        if(index!=-1 && positionKeys[index].id == frameId){
            positionKeys.erase(positionKeys.begin()+index);
        }
    }
    if(rotationKeys.size()) {
        index = KeyHelper::getKeyIndex(rotationKeys, frameId);
        if(index!=-1 && rotationKeys[index].id == frameId){
            rotationKeys.erase(rotationKeys.begin()+index);
        }
    }
    if(scaleKeys.size()) {
        index = KeyHelper::getKeyIndex(scaleKeys, frameId);
        if(index!=-1 && scaleKeys[index].id == frameId){
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
    //selectedNode.reset();
    //bone.reset();
    return q * getGlobalQuaternion(parent).makeInverse();
}
ActionKey SGNode::getKeyForFrame(int frameId){
    ActionKey key;
    int index = KeyHelper::getKeyIndex(positionKeys, frameId);
    if(index!=-1 &&  positionKeys[index].id == frameId){
        key.position = positionKeys[index].position;
        key.isPositionKey = true;
    }
    
    index = KeyHelper::getKeyIndex(rotationKeys, frameId);
    if(index!=-1 && rotationKeys[index].id == frameId){
        key.rotation = rotationKeys[index].rotation;
        key.isRotationKey = true;
    }
    
    index = KeyHelper::getKeyIndex(scaleKeys, frameId);
    if(index!=-1 && scaleKeys[index].id == frameId){
        key.scale = scaleKeys[index].scale;
        key.isScaleKey = true;
    }
    return key;
}
Vector3 SGNode::getNodePosition()
{
    return node->getPosition();
}
Vector3 SGNode::getNodeRotation()
{
    return node->getRotationInDegrees();
}
Vector3 SGNode::getNodeScale()
{
    return node->getScale();
}
Vector3 SGNode::getJointPosition(int jointId, bool isAbsolutePosition)
{
    shared_ptr<AnimatedMeshNode> selectedNode = dynamic_pointer_cast<AnimatedMeshNode>(node);
    shared_ptr<JointNode> joint = selectedNode->getJointNode(jointId);
    //selectedNode.reset();
    return ((isAbsolutePosition)? joint->getAbsolutePosition():joint->getPosition());
}
Quaternion SGNode::getGlobalQuaternion(shared_ptr<JointNode> bone)
{
    
    shared_ptr<JointNode> parent = dynamic_pointer_cast<JointNode>(bone->Parent);
    if(parent)
        return Quaternion(bone->getRotationInRadians()) * getGlobalQuaternion(parent);
    else
        return Quaternion(bone->getRotationInRadians());
}
void SGNode::removeAnimationInCurrentFrame(int currentFrame)
{
    for(int i = 0; i < joints.size(); i++)
    {
        joints[i]->removeAnimationInCurrentFrame(currentFrame);
    }
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
void SGNode::CCD(shared_ptr<JointNode> bone, Vector3 target,int parentHeirarchy,int currentFrame)
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
    
    float dot = currentDir.dotProduct(targetDir);
    
    if(dot < 0.9999) {
        Vector3 cross = currentDir.crossProduct(targetDir);
        cross.normalize();
        
        float turnAngle = acos(dot);
        Quaternion delta;
        delta.fromAngleAxis(turnAngle, cross);
        delta.normalize();
        
        Quaternion global = MathHelper::getGlobalQuaternion(parent);
        global = global * delta;
        Quaternion local = global * MathHelper::getGlobalQuaternion(dynamic_pointer_cast<JointNode>(parent->getParent())).makeInverse();
        ;
        parent->setRotationInDegrees(MathHelper::getEulerRotation(local));
        joints[parent->getID()]->setRotation(local,currentFrame);
        joints[parent->getID()]->setRotationOnNode(local);
        parent->updateAbsoluteTransformation();
        parent->updateAbsoluteTransformationOfChildren();
    }
    //parent.reset();
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
                Quaternion delta = Quaternion(Vector3(angleX,0.0,0.0) * DEGTORAD);
                Quaternion local = delta * Quaternion(grandParent->getRotationInRadians());
                grandParent->setRotationInDegrees(MathHelper::getEulerRotation(local));
                joints[grandParent->getID()]->setRotation(local,currentFrame);
                joints[grandParent->getID()]->setRotationOnNode(local);
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
        //bone.reset();
        //parent.reset();
        //grandParent.reset();
    }
}
int toString(const char a[]) {
    int c, sign, offset, n;
    
    if (a[0] == '-') {  // Handle negative integers
        sign = -1;
    }
    
    if (sign == -1) {  // Set starting position to convert
        offset = 1;
    }
    else {
        offset = 0;
    }
    
    n = 0;
    
    for (c = offset; a[c] != '\0'; c++) {
        n = n * 10 + a[c] - '0';
    }
    
    if (sign == -1) {
        n = -n;
    }
    
    return n;
}


void SGNode::readData(ifstream *filePointer)
{
    joints.clear();
    assetId = FileHelper::readInt(filePointer);
    int sgbVersion = FileHelper::readInt(filePointer);
    if(sgbVersion == SGB_VERSION)
        textureName = FileHelper::readString(filePointer,sgbVersion);
    type = (NODE_TYPE)FileHelper::readInt(filePointer);
    isRigged = FileHelper::readBool(filePointer);
    props.isLighting = FileHelper::readBool(filePointer);
    props.brightness = FileHelper::readFloat(filePointer);
    props.shininess = FileHelper::readFloat(filePointer);
    
    if(sgbVersion > SGB_VERSION_1) {
        
        std::wstring nodeSpecificString = FileHelper::readWString(filePointer);
        
        if (nodeSpecificString.find(L"$_@") != std::wstring::npos) {
            
            vector<std::wstring> fontProperties;
            std::size_t separator = nodeSpecificString.find(L"$_@");
            for (int i = 0; i < 4; i++) {
                
                separator = nodeSpecificString.find(L"$_@");
                fontProperties.push_back(nodeSpecificString.substr(0,separator));
                if(separator >= nodeSpecificString.size() ||  separator == std::wstring::npos)
                    break;
                nodeSpecificString = nodeSpecificString.substr(separator+3,nodeSpecificString.size()-1);
                
            }
            name = fontProperties[0];
            optionalFilePath = ConversionHelper::getStringForWString(fontProperties[1]);//string(fontProperties[1].begin(), fontProperties[1].end());
            props.fontSize = stoi(ConversionHelper::getStringForWString(fontProperties[2]).c_str());
            
            if(separator != std::wstring::npos) {
                props.nodeSpecificFloat = stof(ConversionHelper::getStringForWString(fontProperties[3]).c_str());
            }
            
        } else
            name = nodeSpecificString;
    } else {
        
        string nodeSpecificString = FileHelper::readString(filePointer,sgbVersion);
        
        if (nodeSpecificString.find("$_@") != std::string::npos){
            vector<string> fontProperties;
            std::size_t separator = nodeSpecificString.find("$_@");
            for (int i = 0; i < 4; i++) {
                
                separator = nodeSpecificString.find("$_@");
                fontProperties.push_back(nodeSpecificString.substr(0,separator));
                if(separator >= nodeSpecificString.size() ||  separator == std::string::npos)
                    break;
                nodeSpecificString = nodeSpecificString.substr(separator+3,nodeSpecificString.size()-1);
                
            }
            name = ConversionHelper::getWStringForString(fontProperties[0]) ;//wstring(fontProperties[0].begin(), fontProperties[0].end());
            optionalFilePath = fontProperties[1];
            props.fontSize = stoi(fontProperties[2].c_str());
            
            if(separator != std::string::npos) {
                props.nodeSpecificFloat = stof(fontProperties[3].c_str());
            }
            
        } else
            name = ConversionHelper::getWStringForString(nodeSpecificString);//wstring(nodeSpecificString.begin(), nodeSpecificString.end());
        
        printf(" Name %s ", ConversionHelper::getStringForWString(name).c_str());
    }
    
    props.textColor.x = FileHelper::readFloat(filePointer);
    props.textColor.y = FileHelper::readFloat(filePointer);
    props.textColor.z = FileHelper::readFloat(filePointer);
    
    if(type == NODE_IMAGE && props.textColor.x == 0.0) props.textColor.x = 2.0;
    if(type == NODE_IMAGE && props.textColor.y == 0.0) props.textColor.y = 1.0;
    
    props.vertexColor = props.textColor;
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
}

void SGNode::writeData(ofstream *filePointer)
{
    FileHelper::writeInt(filePointer,assetId);
    FileHelper::writeInt(filePointer,SGB_VERSION); // New sgb version because of changing the format
    FileHelper::writeString(filePointer, textureName);
    FileHelper::writeInt(filePointer,(int)type);
    FileHelper::writeBool(filePointer,isRigged);
    FileHelper::writeBool(filePointer,props.isLighting);
    FileHelper::writeFloat(filePointer,props.brightness);
    FileHelper::writeFloat(filePointer,props.shininess);
    
    std::wstring nodeSpecificString;
    if(type == NODE_TEXT_SKIN) {
        nodeSpecificString = name + L"$_@" + ConversionHelper::getWStringForString(optionalFilePath) + L"$_@" + to_wstring(props.fontSize) + L"$_@" + to_wstring(props.nodeSpecificFloat) + L"$_@";
    } else
        nodeSpecificString = name;
    
    FileHelper::writeWString(filePointer,nodeSpecificString);
    FileHelper::writeFloat(filePointer,props.vertexColor.x);
    FileHelper::writeFloat(filePointer,props.vertexColor.y);
    FileHelper::writeFloat(filePointer,props.vertexColor.z);
    KeyHelper::writeData(filePointer, positionKeys, rotationKeys, scaleKeys, visibilityKeys);
    FileHelper::writeInt(filePointer, int(joints.size()));
    int i;
    for(i = 0; i < joints.size(); i++)
        joints[i]->writeData(filePointer);
}
void SGNode::setShaderProperties(float brightness, float specular, bool isLighting, bool isVisible , float currentFrame)
{
    props.isLighting = isLighting;
    props.brightness = brightness;
    props.shininess = specular;
    props.isVisible = isVisible;
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
void SGNode::faceUserCamera(shared_ptr<CameraNode> viewCamera,int currentFrame){
    Vector3 lightPos = node->getAbsolutePosition();
    Vector3 camPos = viewCamera->getPosition();
    Vector3 rotDir = (camPos - lightPos).normalize();
    Quaternion rotQ = MathHelper::rotationBetweenVectors(rotDir, Vector3(0.0,0.0,1.0));
    setRotation(rotQ, currentFrame);
    Vector3 rotEuler;rotQ.toEuler(rotEuler);
    node->setRotationInDegrees(rotEuler * RADTODEG);
    //viewCamera.reset();
}
