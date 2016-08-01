
#include "HeaderFiles/SceneHelper.h"

#include "HeaderFiles/SGNode.h"
#include "HeaderFiles/ShaderManager.h"
#include "TextMesh3d.h"

#ifdef ANDROID
#include "../opengl.h"
#endif


SGNode::SGNode(NODE_TYPE type)
{
    this->type = type;
    //node = shared_ptr<Node>();
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

    if(type == NODE_CAMERA) {
        //TODO
        addOrUpdateProperty(FOV, Vector4(72.0, 0, 0, 0), UNDEFINED, SLIDER_TYPE, "Field Of View");
        addOrUpdateProperty(CAM_RESOLUTION, Vector4(0), UNDEFINED, TYPE_NONE, "Resolution");
        addOrUpdateProperty(THOUSAND_EIGHTY, Vector4(0), UNDEFINED, LIST_TYPE, "1080p", "Resolution");
        addOrUpdateProperty(SEVEN_TWENTY, Vector4(0), UNDEFINED, LIST_TYPE, "720p", "Resolution");
        addOrUpdateProperty(FOUR_EIGHTY, Vector4(0), UNDEFINED, LIST_TYPE, "480p", "Resolution");
        addOrUpdateProperty(THREE_SIXTY, Vector4(0), UNDEFINED, LIST_TYPE, "360p", "Resolution");
        addOrUpdateProperty(TWO_FORTY, Vector4(0), UNDEFINED, LIST_TYPE, "240p", "Resolution");
        
        addOrUpdateProperty(MATERIAL_PROPS, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Material Properties", "PROPERTIES");
        addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED, TYPE_NONE, "Transparency");
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Visible");
        addOrUpdateProperty(SELECTED, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
        addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Lighting");
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(true, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "IsVertexColor");
        addOrUpdateProperty(REFLECTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Reflection");
        addOrUpdateProperty(REFRACTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Glassy");
        addOrUpdateProperty(VERTEX_COLOR, Vector4(1.0), MATERIAL_PROPS, TYPE_NONE, "Color");
    } else if(type == NODE_LIGHT || type == NODE_ADDITIONAL_LIGHT) {
        
        addOrUpdateProperty(SHADOW_DARKNESS, Vector4(0, 0, 0, 0), UNDEFINED, (type == NODE_LIGHT) ? SLIDER_TYPE : TYPE_NONE, "Shadow Darkness");
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(300.0, 0, 0, 0), UNDEFINED, (type == NODE_LIGHT) ? TYPE_NONE : SLIDER_TYPE, "Distance");
        int lightType = (type == NODE_LIGHT) ? DIRECTIONAL_LIGHT : POINT_LIGHT;
        
        addOrUpdateProperty(LIGHT_TYPE, Vector4(lightType, 0, 0, 0), UNDEFINED, TYPE_NONE, "Light Type");
        addOrUpdateProperty(LIGHT_DIRECTIONAL, Vector4(1, 0, 0, 0), UNDEFINED, LIST_TYPE, "Directional", "LIGHT TYPE");
        addOrUpdateProperty(LIGHT_POINT, Vector4(0, 0, 0, 0), UNDEFINED, LIST_TYPE, "Point", "LIGHT TYPE");
        
        addOrUpdateProperty(MATERIAL_PROPS, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Material Properties", "PROPERTIES");
        addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED, TYPE_NONE, "Transparency");
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Visible");
        addOrUpdateProperty(SELECTED, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
        addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Lighting");
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(true, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "IsVertexColor");
        addOrUpdateProperty(REFLECTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Reflection");
        addOrUpdateProperty(REFRACTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Glassy");
        addOrUpdateProperty(VERTEX_COLOR, Vector4(1.0), MATERIAL_PROPS, COLOR_TYPE, "Light Color");

    } else if(type == NODE_SGM || type == NODE_OBJ || type == NODE_TEXT || type == NODE_RIG || type == NODE_TEXT_SKIN) {
        
        addOrUpdateProperty(CLONE, Vector4(1, 0, 0, 0), UNDEFINED,  ICON_TYPE, "Clone", "GENERAL", "", CLONE_ICON);
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(300.0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Distance");
        
        if(type != NODE_RIG && type != NODE_TEXT_SKIN) {
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
        }
        
        addOrUpdateProperty(MATERIAL_PROPS, Vector4(1, 0, 0, 0), UNDEFINED, PARENT_TYPE, "Material Properties", "PROPERTIES");
        addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED, TYPE_NONE, "Transparency");
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Visible", "PROPERTIES");
        addOrUpdateProperty(SELECTED, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
        addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Lighting", "PROPERTIES");
        addOrUpdateProperty(FONT_SIZE, Vector4(20.0, 0, 0, 0), UNDEFINED, TYPE_NONE, "FontSize");
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "SpecificFloat");
        addOrUpdateProperty(VERTEX_COLOR, Vector4(1.0), MATERIAL_PROPS, TYPE_NONE, "Color");
        addOrUpdateProperty(ORIG_VERTEX_COLOR, Vector4(1.0), UNDEFINED, TYPE_NONE, "Color");
        addOrUpdateProperty(TEXT_COLOR, Vector4(1.0), UNDEFINED, TYPE_NONE, "Color");
        
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(false, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "IsVertexColor");
        addOrUpdateProperty(REFLECTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, SLIDER_TYPE, "Reflection", "SAMPLE");
        addOrUpdateProperty(REFRACTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, SLIDER_TYPE, "Glassy", "SAMPLE");
        addOrUpdateProperty(TEXTURE, Vector4(1), MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", "");
        addOrUpdateProperty(TEXTURE_SCALE, Vector4(1, 0, 0, true), MATERIAL_PROPS, SLIDER_TYPE, "Scale", "SKIN");
        addOrUpdateProperty(TEXTURE_SMOOTH, Vector4(1, 0, 0, 0), MATERIAL_PROPS, SWITCH_TYPE, "Smooth", "SKIN");
        addOrUpdateProperty(BUMP_MAP, Vector4(1), MATERIAL_PROPS, IMAGE_TYPE, "Bump Map", "SKIN", "");
        addOrUpdateProperty(BUMP_DEPTH, Vector4(1, 0, 0, 0), MATERIAL_PROPS, SLIDER_TYPE, "Depth", "SKIN");

    } else if(type == NODE_IMAGE || type == NODE_VIDEO || type == NODE_PARTICLES) {
        addOrUpdateProperty(CLONE, Vector4(1, 0, 0, 0), UNDEFINED,  ICON_TYPE, "Clone", "GENERAL", "", CLONE_ICON);
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Visible", "PROPERTIES");

        if(type != NODE_PARTICLES)
            addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Lighting", "PROPERTIES");
    }
}

SGNode::~SGNode(){
    instanceNodes.clear();
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
            addOrUpdateProperty(LIGHTING, Vector4(0, 0, 0, 0), UNDEFINED);
            node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
            break;
        }
        case NODE_LIGHT:{
            node = initLightSceneNode(smgr);
            addOrUpdateProperty(VERTEX_COLOR, DEFAULT_LIGHT_COLOR, MATERIAL_PROPS);
            addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(DEFAULT_FADE_DISTANCE, 0, 0, 0), UNDEFINED);
            break;
        }
        case NODE_OBJ:
        case NODE_SGM:{
            textureName = texturePath;
            oriTextureName = textureName;
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            addOrUpdateProperty(ORIG_VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), UNDEFINED);
            node = loadSGMandOBJ(assetId, objectType, smgr);
            break;
        }
        case NODE_TEXT:{
            textureName = texturePath;
            oriTextureName = textureName;
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            addOrUpdateProperty(ORIG_VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), UNDEFINED);
            addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED);

            node = load3DText(smgr, objectName, 4, 4, width, specificFilePath, objSpecificColor, height / 50.0f, 4);
            addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(height, 0, 0, 0), UNDEFINED);
            addOrUpdateProperty(FONT_SIZE, Vector4(width, 0, 0, 0), UNDEFINED);
            optionalFilePath = specificFilePath;
            break;
        }
        case NODE_RIG:{
            textureName = texturePath;
            oriTextureName = textureName;
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            addOrUpdateProperty(ORIG_VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), UNDEFINED);
            node = loadSGR(assetId,objectType,smgr);
            isMirrorEnabled = joints.size() == HUMAN_JOINTS_SIZE ? true : false;
            break;
        }
        case NODE_IMAGE:{
            float aspectRatio = (float)objSpecificColor.x/(float)objSpecificColor.y;
            textureName = ConversionHelper::getStringForWString(objectName);
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            node = loadImage(ConversionHelper::getStringForWString(objectName) + ".png", smgr , aspectRatio);
            break;
        }
        case NODE_VIDEO:{
            float aspectRatio = (float)objSpecificColor.x/(float)objSpecificColor.y;
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            textureName = ConversionHelper::getStringForWString(objectName);
            node = loadVideo(ConversionHelper::getStringForWString(objectName), smgr , aspectRatio);
            break;
        }
        case NODE_TEXT_SKIN:{
            // 'width' here is font size and 'height' is bevel value
            textureName = texturePath;
            oriTextureName = textureName;
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            addOrUpdateProperty(ORIG_VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), UNDEFINED);
            node = loadSkin3DText(smgr, objectName, 4, 4, width, specificFilePath, objSpecificColor, height / 50.0f, 4);
            addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0, 0, 0), UNDEFINED);
            addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(width, 0, 0, 0), UNDEFINED);
            optionalFilePath = specificFilePath;
            addOrUpdateProperty(FONT_SIZE, Vector4(width, 0, 0, 0), UNDEFINED);
            break;
        }
        case NODE_ADDITIONAL_LIGHT:{
            this->assetId = assetId;
            node = addAdittionalLight(smgr, width, Vector3(objSpecificColor.x,objSpecificColor.y,objSpecificColor.z) , height);
            addOrUpdateProperty(VERTEX_COLOR, Vector4(objSpecificColor.x, objSpecificColor.y, objSpecificColor.z, 0.0), MATERIAL_PROPS);
            addOrUpdateProperty(SPECIFIC_FLOAT, Vector4((height == 0) ? 50.0 : height, 0, 0, 0), UNDEFINED);
            break;
        }
        case NODE_PARTICLES:{
            textureName = to_string(assetId) + "-cm";

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
            Mesh *mesh = CSGRMeshFileLoader::createSGMMesh(meshPath ,smgr->device);
            node = smgr->createParticlesFromMesh(mesh, "setUniforms", MESH_TYPE_LITE, SHADER_PARTICLES);
            setParticlesData(node, pData);
            node->setMaterial(smgr->getMaterialByIndex(SHADER_PARTICLES));
            Texture *nodeTex = smgr->loadTexture("Particle Texture", texPath,TEXTURE_RGBA8,TEXTURE_BYTE, true);
            node->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
            addOrUpdateProperty(TRANSPARENCY, Vector4(0.7, 0, 0, 0), UNDEFINED);
            break;
        }
        default:
            Logger::log(ERROR, "SGNode::loadNode ", "Unknown node type to load");
            break;
    }
    
    if(node) {
        Texture *nodeEnvTex = smgr->loadTexture("Env Texture", constants::BundlePath + "/envmap.png", TEXTURE_RGBA8, TEXTURE_BYTE, true, 50);
        node->setTexture(nodeEnvTex, NODE_TEXTURE_TYPE_REFLECTIONMAP);

//        Texture *nodeNormalTex = smgr->loadTexture("Normal Texture", constants::BundlePath + "/norm1.png", TEXTURE_RGBA8, TEXTURE_BYTE, true);
//        node->setTexture(nodeNormalTex, NODE_TEXTURE_TYPE_NORMALMAP);
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

shared_ptr<Node> SGNode::loadSkin3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments)
{
    if(bevelRadius == 0 || bevelSegments == 0)
        bevelSegments = 0;
    
    string pathForFont;
    if(fontPath == "") {
        width = 3;
        fontPath = DEFAULT_FONT_FILE;
        Vector4 vertColor = getProperty(VERTEX_COLOR).value;
        fontColor = Vector4(vertColor.x, vertColor.y, vertColor.z,1.0);
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
    
    shared_ptr<AnimatedMeshNode> node = smgr->createAnimatedNodeFromMesh(mesh, "setUniforms", ShaderManager::maxJoints, TEXT_RIG, MESH_TYPE_HEAVY);
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
        node->getMeshCache()->recalculateNormals();
        node->getMeshCache()->fixOrientation();
    }
    node->setMaterial(smgr->getMaterialByIndex(SHADER_TEXT_SKIN));
    node->getMesh()->Commit();
    string textureFileName = "";
    textureFileName = FileHelper::getDocumentsDirectory() +"Resources/Textures/"+ textureName + ".png";
    if(!checkFileExists(textureFileName)){
    #ifndef ANDROID
        textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + textureName+".png";
        if(!checkFileExists(textureFileName)){
            textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Textures/" + textureName+".png";
            if(!checkFileExists(textureFileName))
                textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Rigs/" + textureName+".png";
        }
         #elif ANDROID
                            textureFileName = constants::BundlePath+"/"+textureName+".png";
                            if(!checkFileExists(textureFileName))
                                textureFileName = constants::DocumentsStoragePath+"/importedImages/"+textureName+".png";
                        #endif
    }

    printf("Texture File Path : %s " , textureFileName.c_str());
    
    if(textureName != "" && checkFileExists(textureFileName)) {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(0, 0, 0, 0), MATERIAL_PROPS);
        Texture *nodeTex = smgr->loadTexture(textureFileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
        node->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
    } else
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(1, 0, 0, 0), MATERIAL_PROPS);
    
    return node;
}

shared_ptr<Node> SGNode::load3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments)
{
    if(bevelRadius == 0 || bevelSegments == 0)
        bevelSegments = 0;
    
    string pathForFont;
    if(fontPath == "") {
        width = 3;
        fontPath = DEFAULT_FONT_FILE;
        Vector4 vertColor = getProperty(VERTEX_COLOR).value;
        fontColor = Vector4(vertColor.x, vertColor.y, vertColor.z,1.0);
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
                Logger::log(INFO,"SGNode","Font Path : " + pathForFont);
                printf("File not exists at %s ",pathForFont.c_str());
                return shared_ptr<Node>();
            }
        }
    }
    Mesh *mesh = TextMesh3d::get3DTextMesh(text, bezierSegments, extrude, width, (char*)pathForFont.c_str(), fontColor, smgr->device, bevelRadius, bevelSegments);
    
    if(mesh == NULL)
        return shared_ptr<Node>();

    shared_ptr<MeshNode> node = smgr->createNodeFromMesh(mesh, "setUniforms",MESH_TYPE_LITE);
    node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    string textureFileName = "";
    textureFileName = FileHelper::getDocumentsDirectory() +"Resources/Textures/"+ textureName + ".png";
    if(!checkFileExists(textureFileName)){
    #ifndef ANDROID
        textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + textureName+".png";
        if(!checkFileExists(textureFileName)){
            textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Textures/" + textureName+".png";
            if(!checkFileExists(textureFileName))
                textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Rigs/" + textureName+".png";
        }
         #elif ANDROID
                    textureFileName = constants::BundlePath+"/"+textureName+".png";
                    if(!checkFileExists(textureFileName))
                        textureFileName = constants::DocumentsStoragePath+"/importedImages/"+textureName+".png";
                #endif
    }
    if(textureName != "" && checkFileExists(textureFileName)) {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(0, 0, 0, 0), MATERIAL_PROPS);
        Texture *nodeTex = smgr->loadTexture(textureFileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
        node->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
        addOrUpdateProperty(TEXTURE, getProperty(VERTEX_COLOR).value, MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", textureFileName);
    } else {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(1, 0, 0, 0), MATERIAL_PROPS);
        addOrUpdateProperty(TEXTURE, getProperty(VERTEX_COLOR).value, MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", "");
    }
    
    node->updateBoundingBox();
    return node;
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

    Logger::log(INFO,"SGNODE","Particle Path " + jsonPath);

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

shared_ptr<Node> SGNode::loadSGMandOBJ(int assetId, NODE_TYPE objectType, SceneManager *smgr)
{
    string StoragePath;
    OBJMeshFileLoader *objLoader;
    string fileExt = (objectType == NODE_SGM) ? ".sgm" : ".obj";
    
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
#ifdef IOS
        meshPath = FileHelper::getDocumentsDirectory() + to_string(assetId) + fileExt;
        if(!checkFileExists(meshPath)){
            meshPath = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + to_string(assetId) + fileExt;
        }
#else
            meshPath = constants::BundlePath+"/"+to_string(assetId)+fileExt;
#endif
        if(!checkFileExists(meshPath))
            return shared_ptr<Node>();
    }
    if(!checkFileExists(textureFileName)){
        #ifdef IOS
        textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + textureName+".png";
        if(!checkFileExists(textureFileName))
            textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Textures/" + textureName+".png";
        if(!checkFileExists(textureFileName))
            textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Rigs/" + textureName+".png";
        #elif ANDROID
            textureFileName = constants::BundlePath+"/"+textureName+".png";
            if(!checkFileExists(textureFileName))
                textureFileName = constants::DocumentsStoragePath+"/importedImages/"+textureName+".png";
        #endif
    }

    int objLoadStatus = 0;
    Mesh *mesh = (objectType == NODE_SGM) ? CSGRMeshFileLoader::createSGMMesh(meshPath,smgr->device) : objLoader->createMesh(meshPath,objLoadStatus,smgr->device);
    shared_ptr<MeshNode> node = smgr->createNodeFromMesh(mesh,"setUniforms");
    
    node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));

    if(textureName != "" && checkFileExists(textureFileName))
    {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(0, 0, 0, 0), MATERIAL_PROPS);
        Texture *nodeTex = smgr->loadTexture(textureFileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
        node->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
        addOrUpdateProperty(TEXTURE, getProperty(VERTEX_COLOR).value, MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", textureFileName);
    } else {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(1, 0, 0, 0), MATERIAL_PROPS);
        addOrUpdateProperty(TEXTURE, getProperty(VERTEX_COLOR).value, MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", "");
    }
    
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

shared_ptr<Node> SGNode::loadSGR(int assetId,NODE_TYPE objectType,SceneManager *smgr)
{
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
    
    if(!checkFileExists(textureFileName)){
        #ifndef ANDROID
            textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Sgm/" + textureName+".png";
            if(!checkFileExists(textureFileName)){
                textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Textures/" + textureName+".png";
                if(!checkFileExists(textureFileName))
                    textureFileName = FileHelper::getDocumentsDirectory()+ "/Resources/Rigs/" + textureName+".png";
                    if(!checkFileExists(textureFileName))
                        textureFileName = constants::CachesStoragePath + "/"+textureName+".png";
            }
         #elif ANDROID
                    textureFileName = constants::BundlePath+"/"+textureName+".png";
                    if(!checkFileExists(textureFileName))
                        textureFileName = constants::DocumentsStoragePath+"/importedImages/"+textureName+".png";
            #endif
    }
    
    AnimatedMesh *mesh = CSGRMeshFileLoader::LoadMesh(meshPath,smgr->device);
    setSkinningData((SkinMesh*)mesh);
    shared_ptr<AnimatedMeshNode> node = smgr->createAnimatedNodeFromMesh(mesh,"setUniforms", ShaderManager::maxJoints, CHARACTER_RIG, MESH_TYPE_HEAVY);
    bool isSGJointsCreated = (joints.size() > 0) ? true : false;
    int jointsCount = node->getJointCount();
        
    for(int i = 0;i < jointsCount;i++){
        node->getJointNode(i)->setID(i);
        if(!isSGJointsCreated){
            SGJoint *joint = new SGJoint();
            joint->jointNode = node->getJointNode(i);
            joints.push_back(joint);
        }
    }
    node->setMaterial(smgr->getMaterialByIndex(SHADER_SKIN),true);
    
    if(textureName != "" && checkFileExists(textureFileName)) {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(0, 0, 0, 0), MATERIAL_PROPS);
        Texture *nodeTex = smgr->loadTexture(textureFileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
        node->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
        addOrUpdateProperty(TEXTURE, getProperty(VERTEX_COLOR).value, MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", textureFileName);
    } else {
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(1, 0, 0, 0), MATERIAL_PROPS);
        addOrUpdateProperty(TEXTURE, getProperty(VERTEX_COLOR).value, MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", "");
    }
    
    return node;
}

void SGNode::setSkinningData(SkinMesh *mesh)
{
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
            }
//            else
//                Logger::log(INFO, "SGNODE:SetSkinningData()", "More joints affecting a vertex");
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

shared_ptr<Node> SGNode::loadImage(string textureName,SceneManager *smgr, float aspectRatio)
{
char* textureFileName = new char[256];

#ifdef ANDROID
string path = constants::DocumentsStoragePath+ "/importedImages/"+textureName;
textureFileName=(path).c_str();
#else
sprintf(textureFileName, "%s/%s", constants::CachesStoragePath.c_str(),textureName.c_str());
#endif
    Texture *nodeTex = smgr->loadTexture(textureFileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
    Logger::log(INFO, "SGNODE", "aspectratio" + to_string(aspectRatio));
    shared_ptr<PlaneMeshNode> planeNode = smgr->createPlaneNode("setUniforms" , aspectRatio);
    planeNode->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    planeNode->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
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
    //Texture *nodeTex = smgr->loadTextureFromVideo(videoFileName,TEXTURE_RGBA8,TEXTURE_BYTE);
    Logger::log(INFO, "SGNODE", "aspectratio" + to_string(aspectRatio));
    shared_ptr<PlaneMeshNode> planeNode = smgr->createPlaneNode("setUniforms" , aspectRatio);
    planeNode->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    planeNode->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
    //delete [] textureFileName; //TODO TEST
    return planeNode;
}

shared_ptr<Node> SGNode::initLightSceneNode(SceneManager *smgr)
{
    // CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/light.sgm",smgr->device);
    
    Mesh* lightMesh = new Mesh();
    lightMesh->copyDataFromMesh(SceneHelper::pointLightMesh);

    shared_ptr<Node> lightNode = smgr->createNodeFromMesh(lightMesh,"setUniforms");
    //Texture *nodeTex = smgr->loadTexture("Text light.png",constants::BundlePath + "/light.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    //lightNode->setTexture(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
    lightNode->setPosition(Vector3(-LIGHT_INIT_POS_X,LIGHT_INIT_POS_Y,LIGHT_INIT_POS_Z));
    lightNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    lightNode->setScale(Vector3(3.0));
    //lightNode->setScale(Vector3(4.0));
    addOrUpdateProperty(LIGHTING, Vector4(0, 0, 0, 0), UNDEFINED);
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
            addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(distance, 0, 0, 0), UNDEFINED);

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
            KeyHelper::addKey(positionKeys, positionKey);
            KeyHelper::addKey(rotationKeys, rotationKey);
            KeyHelper::addKey(scaleKeys, scaleKey);
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
        
        if(type == NODE_ADDITIONAL_LIGHT || type == NODE_LIGHT) {
            
            if(options[LIGHT_TYPE].value.x == -1) {
                
                if(rotationKeys.size() > 0 && type == NODE_ADDITIONAL_LIGHT) {
                    for(int i = 0; i < rotationKeys.size(); i++) {
                        SGScaleKey scaleKey;
                        scaleKey.scale = Vector3(rotationKeys[i].rotation.x, rotationKeys[i].rotation.y, rotationKeys[i].rotation.z);
                        scaleKey.id = rotationKeys[i].id;
                        KeyHelper::addKey(scaleKeys, scaleKey);
                    }
                    addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(rotationKeys[0].rotation.w, 0, 0, 0), UNDEFINED);
                }
                rotationKeys.clear();
                
                Vector3 direction = Vector3(0.0) - node->getAbsolutePosition();
                
                Quaternion delta = MathHelper::rotationBetweenVectors(direction, Vector3(0.0, 1.0, 0.0));
                Vector3 lineRot = Vector3(0.0);
                SGRotationKey rotationKey;
                rotationKey.rotation = MathHelper::RotateNodeInWorld(lineRot, delta);
                rotationKey.id = 0;
                KeyHelper::addKey(rotationKeys, rotationKey);
                addOrUpdateProperty(LIGHT_TYPE, Vector4(0, 0, 0, 0), UNDEFINED);
                
            }
            
            if(options[SPECIFIC_FLOAT].value.x == 0.0)
                addOrUpdateProperty(SPECIFIC_FLOAT, Vector4((type == NODE_LIGHT) ? 300.0 : 50.0, 0, 0, 0), UNDEFINED);
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
    //selectedNode.reset();
    //bone.reset();
    return q * getGlobalQuaternion(parent).makeInverse();
}

ActionKey SGNode::getKeyForFrame(int frameId)
{
    ActionKey key;
    int index = KeyHelper::getKeyIndex(positionKeys, frameId);
    if(index!=-1 && index < positionKeys.size() &&  positionKeys[index].id == frameId){
        key.position = positionKeys[index].position;
        key.isPositionKey = true;
    }
    
    index = KeyHelper::getKeyIndex(rotationKeys, frameId);
    if(index!=-1 && index < rotationKeys.size() && rotationKeys[index].id == frameId){
        key.rotation = rotationKeys[index].rotation;
        key.isRotationKey = true;
    }
    
    index = KeyHelper::getKeyIndex(scaleKeys, frameId);
    if(index!=-1 && index < scaleKeys.size() && scaleKeys[index].id == frameId){
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
    //selectedNode.reset();
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

        parent->setRotation(local, true);
        joints[parent->getID()]->setRotation(local, currentFrame);
        joints[parent->getID()]->setRotationOnNode(local, true);
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
                Quaternion local = delta * grandParent->getRotation();
                grandParent->setRotation(local, true);
                joints[grandParent->getID()]->setRotation(local,currentFrame);
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

void SGNode::readData(ifstream *filePointer, int &origIndex)
{
    joints.clear();
    assetId = FileHelper::readInt(filePointer);
    
    int sgbVersion = FileHelper::readInt(filePointer);
    int hasPhysics, physicsType, lightType, isSoft;
    float weight, forceMagnitude;
    Vector4 forceDir;
    float specificFloat;
    
    if(sgbVersion == SGB_VERSION_CURRENT) { // Empty Data for future use
        hasPhysics = FileHelper::readInt(filePointer);
        physicsType = FileHelper::readInt(filePointer);
        origIndex = FileHelper::readInt(filePointer);
        lightType = FileHelper::readInt(filePointer);
        int texSmooth = FileHelper::readInt(filePointer);
        smoothTexture = (texSmooth == 0 || texSmooth == 2) ? true : false;
        weight = FileHelper::readFloat(filePointer);
        forceMagnitude = FileHelper::readFloat(filePointer);
        forceDir.x = FileHelper::readFloat(filePointer);
        forceDir.y = FileHelper::readFloat(filePointer);
        forceDir.z = FileHelper::readFloat(filePointer);
        forceDir.w = 0.0;
        specificFloat = FileHelper::readFloat(filePointer);
        isSoft = (int)FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
    }
    if(sgbVersion >= SGB_VERSION_2)
        textureName = FileHelper::readString(filePointer,sgbVersion);
    else
        textureName = to_string(assetId)+"-cm";
    type = (NODE_TYPE)FileHelper::readInt(filePointer);
    setPropertiesOfNode();
    if(sgbVersion == SGB_VERSION_CURRENT) {
        addOrUpdateProperty(HAS_PHYSICS, Vector4(hasPhysics, 0, 0, 0), UNDEFINED);
        addOrUpdateProperty(PHYSICS_KIND, Vector4((physicsType < PHYSICS_CONST) ? physicsType + PHYSICS_CONST : physicsType), HAS_PHYSICS);
        std::map<PROP_INDEX, Property> physicsProps = options[HAS_PHYSICS].subProps;

        for(int pI = PHYSICS_NONE; pI < PHYSICS_NONE+7; pI++)
            addOrUpdateProperty((PROP_INDEX)pI, Vector4((physicsProps[PHYSICS_KIND].value.x == pI) ? 1 : 0), HAS_PHYSICS);
        
        if(type == NODE_LIGHT || type == NODE_ADDITIONAL_LIGHT)
            addOrUpdateProperty(LIGHT_TYPE, Vector4(lightType - 1, 0, 0, 0), UNDEFINED);
        addOrUpdateProperty(WEIGHT, Vector4(weight, 0, 0, 0), HAS_PHYSICS);
        addOrUpdateProperty(FORCE_MAGNITUDE, Vector4(forceMagnitude, 0, 0, true), HAS_PHYSICS);
        addOrUpdateProperty(FORCE_DIRECTION, forceDir, HAS_PHYSICS);
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(specificFloat, 0, 0, 0), UNDEFINED);
        addOrUpdateProperty(IS_SOFT, Vector4(isSoft), HAS_PHYSICS);
    }

    isRigged = FileHelper::readBool(filePointer);
    addOrUpdateProperty(LIGHTING, Vector4(FileHelper::readBool(filePointer), 0, 0, 0), UNDEFINED);
    addOrUpdateProperty(REFRACTION, Vector4(FileHelper::readFloat(filePointer), 0, 0, 0), MATERIAL_PROPS);
    addOrUpdateProperty(REFLECTION, Vector4(FileHelper::readFloat(filePointer), 0, 0, 0), MATERIAL_PROPS);
    
    if(sgbVersion > SGB_VERSION_1) {

        std::wstring nodeSpecificString = FileHelper::readWString(filePointer);
        
        if (nodeSpecificString.find(L"$_@") != std::wstring::npos) {
            
            vector<std::wstring> fontProperties;
            std::size_t separator = nodeSpecificString.find(L"$_@");
            for (int i = 0; i < 4; i++) {
                
                separator = nodeSpecificString.find(L"$_@");
                fontProperties.push_back(nodeSpecificString.substr(0,separator));
                if(separator+3 >= nodeSpecificString.length() ||  separator == std::wstring::npos)
                    break;
                nodeSpecificString = nodeSpecificString.substr(separator+3,nodeSpecificString.size()-1);
                
            }
            name = fontProperties[0];
            optionalFilePath = ConversionHelper::getStringForWString(fontProperties[1]);//string(fontProperties[1].begin(), fontProperties[1].end());
            int fontSize = stoi(ConversionHelper::getStringForWString(fontProperties[2]).c_str());
            addOrUpdateProperty(FONT_SIZE, Vector4(fontSize, 0, 0, 0), UNDEFINED);

            if(separator != std::wstring::npos) {
                float specificFloat = stof(ConversionHelper::getStringForWString(fontProperties[3]).c_str());
                addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(specificFloat, 0, 0, 0), UNDEFINED);
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
                if(separator+3 >= nodeSpecificString.length() ||  separator == std::string::npos)
                    break;
                nodeSpecificString = nodeSpecificString.substr(separator+3,nodeSpecificString.size()-1);
                
            }
            name = ConversionHelper::getWStringForString(fontProperties[0]) ;//wstring(fontProperties[0].begin(), fontProperties[0].end());
            optionalFilePath = fontProperties[1];
            addOrUpdateProperty(FONT_SIZE, Vector4(stoi(fontProperties[2].c_str()), 0, 0, 0), UNDEFINED);
            
            if(separator != std::string::npos) {
                addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(stof(fontProperties[3].c_str()), 0, 0, 0), UNDEFINED);
            }
            
        } else
            name = ConversionHelper::getWStringForString(nodeSpecificString);//wstring(nodeSpecificString.begin(), nodeSpecificString.end());
         }
    
    Vector4 textColor;
    textColor.x = FileHelper::readFloat(filePointer);
    textColor.y = FileHelper::readFloat(filePointer);
    textColor.z = FileHelper::readFloat(filePointer);
    textColor.w = 0.0;
    if(type == NODE_SGM || type == NODE_TEXT)
        addOrUpdateProperty(TEXT_COLOR,textColor, UNDEFINED, TYPE_NONE);

    if(type == NODE_IMAGE && options[TEXT_COLOR].value.x == 0.0) options[TEXT_COLOR].value.x = 2.0;
    if(type == NODE_IMAGE && options[TEXT_COLOR].value.y == 0.0) options[TEXT_COLOR].value.y = 1.0;
    
    addOrUpdateProperty(VERTEX_COLOR, options[TEXT_COLOR].value, MATERIAL_PROPS);
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

void SGNode::writeData(ofstream *filePointer, vector<SGNode*> &nodes)
{
    std::map<PROP_INDEX, Property> physicsProps = options[HAS_PHYSICS].subProps;

    FileHelper::writeInt(filePointer,assetId);
    FileHelper::writeInt(filePointer,SGB_VERSION_CURRENT); // New sgb version because of changing the format
    FileHelper::writeInt(filePointer, options[HAS_PHYSICS].value.x);
    FileHelper::writeInt(filePointer, (int)physicsProps[PHYSICS_KIND].value.x);
    
    int nodeIndex = 0;
    if(node->type == NODE_TYPE_INSTANCED) {
        int actionId = ((SGNode*)node->original->getUserPointer())->actionId;
        for (int i = 0; i < nodes.size(); i++) {
            if(actionId == nodes[i]->actionId)
                nodeIndex = i;
        }
    }
    
    FileHelper::writeInt(filePointer, nodeIndex); //Node Index of Original Node if Instanced
    FileHelper::writeInt(filePointer, options[LIGHT_TYPE].value.x + 1); // Light Type + 1
    FileHelper::writeInt(filePointer, (int)smoothTexture + 1);
    FileHelper::writeFloat(filePointer, physicsProps[WEIGHT].value.x);
    FileHelper::writeFloat(filePointer, physicsProps[FORCE_MAGNITUDE].value.x);
    FileHelper::writeFloat(filePointer, physicsProps[FORCE_DIRECTION].value.x);
    FileHelper::writeFloat(filePointer, physicsProps[FORCE_DIRECTION].value.y);
    FileHelper::writeFloat(filePointer, physicsProps[FORCE_DIRECTION].value.z);
    FileHelper::writeFloat(filePointer, options[SPECIFIC_FLOAT].value.x);
    FileHelper::writeFloat(filePointer, physicsProps[IS_SOFT].value.x);
    FileHelper::writeFloat(filePointer, 0.0);
    FileHelper::writeFloat(filePointer, 0.0);
    FileHelper::writeFloat(filePointer, 0.0);
    FileHelper::writeFloat(filePointer, 0.0);
    FileHelper::writeFloat(filePointer, 0.0);
    FileHelper::writeString(filePointer, textureName);
    FileHelper::writeInt(filePointer,(int)type);
    FileHelper::writeBool(filePointer,isRigged);
    FileHelper::writeBool(filePointer, options[LIGHTING].value.x);
    FileHelper::writeFloat(filePointer,getProperty(REFRACTION).value.x);
    FileHelper::writeFloat(filePointer,getProperty(REFLECTION).value.x);
    
    std::wstring nodeSpecificString;
    if(type == NODE_TEXT_SKIN || type == NODE_TEXT) {
        nodeSpecificString = name + L"$_@" + ConversionHelper::getWStringForString(optionalFilePath) + L"$_@" + to_wstring(options[FONT_SIZE].value.x) + L"$_@" + to_wstring(options[SPECIFIC_FLOAT].value.x) + L"$_@";
    } else
        nodeSpecificString = name;
    
    FileHelper::writeWString(filePointer,nodeSpecificString);
    FileHelper::writeFloat(filePointer, getProperty(VERTEX_COLOR).value.x);
    FileHelper::writeFloat(filePointer, getProperty(VERTEX_COLOR).value.y);
    FileHelper::writeFloat(filePointer, getProperty(VERTEX_COLOR).value.z);
    KeyHelper::writeData(filePointer, positionKeys, rotationKeys, scaleKeys, visibilityKeys);
    FileHelper::writeInt(filePointer, int(joints.size()));
    int i;
    for(i = 0; i < joints.size(); i++)
        joints[i]->writeData(filePointer);
}

void SGNode::setMeshProperties(float refraction, float reflection, bool isLighting, bool isVisible , bool isPhysicsObj, int physicsType, float fMagnitude, float currentFrame)
{
    addOrUpdateProperty(LIGHTING, Vector4(isLighting, 0, 0, 0), UNDEFINED);
    addOrUpdateProperty(REFRACTION, Vector4(refraction, 0, 0, 0), MATERIAL_PROPS);
    addOrUpdateProperty(REFLECTION, Vector4(reflection, 0, 0, 0), MATERIAL_PROPS);
    addOrUpdateProperty(VISIBILITY, Vector4(isVisible, 0, 0, 0), UNDEFINED);
    addOrUpdateProperty(HAS_PHYSICS, Vector4(isPhysicsObj, 0, 0, 0), UNDEFINED);
    addOrUpdateProperty(PHYSICS_KIND, Vector4(physicsType), HAS_PHYSICS);
    addOrUpdateProperty(FORCE_MAGNITUDE, Vector4(fMagnitude, 0, 0, true), HAS_PHYSICS);
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

Property& SGNode::getProperty(PROP_INDEX pIndex)
{
    if(options.find(pIndex) == options.end()) {
        std::map<PROP_INDEX, Property>::iterator pIt;
        for(pIt = options.begin(); pIt != options.end(); pIt++) {
            if(pIt->second.subProps.find(pIndex) != pIt->second.subProps.end())
                return pIt->second.subProps[pIndex];
        }
        
    } else {
        return options[pIndex];
    }
}

void SGNode::addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE propType, string title, string groupName, string fileName, ICON_INDEX iconId)
{
    Property property;
    property.index = index;
    property.parentIndex = parentProp;
    property.value = value;
    property.iconId = iconId;
    
    if(title.length() > 2) {
        property.title = title;
        property.type = propType;
    }
    
    if(groupName.length() > 2)
        property.groupName = groupName;
    if(fileName.length() > 5 || propType == IMAGE_TYPE)
        property.fileName = fileName;
    
    if(parentProp != UNDEFINED && options.find(parentProp) != options.end())
        checkAndUpdatePropsMap(options[parentProp].subProps, property);
    else if(options.find(index) == options.end()) {
        property.type = propType;
        options.insert(pair<PROP_INDEX, Property>( index, property));
    }
    else {
        options.find(index)->second.value = property.value;
        if(property.fileName.length() > 5 || propType == IMAGE_TYPE)
            options.find(index)->second.fileName = property.fileName;
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


