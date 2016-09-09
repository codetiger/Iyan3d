//
//  SceneInitializeHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//


#include "HeaderFiles/SceneHelper.h"
#include "HeaderFiles/ShaderParamOrder.h"
#include "SceneImporter.h"

float SceneHelper::screenWidth = 0.0;
float SceneHelper::screenHeight = 0.0;
Mesh* SceneHelper::pointLightMesh = NULL;

shared_ptr<CameraNode> SceneHelper::initViewCamera(SceneManager *smgr, Vector3& cameraTarget, float& cameraRadius)
{
#ifndef UBUNTU
    CameraViewHelper::readData();
#endif

    shared_ptr<CameraNode> viewCamera = smgr->createCameraNode("NoUniformCallbackFunctions");
    smgr->setActiveCamera(viewCamera);

    viewCamera->setPosition(Vector3(VIEW_CAM_INIT_POS_X, VIEW_CAM_INIT_POS_Y, VIEW_CAM_INIT_POS_Z));
    viewCamera->updateAbsoluteTransformation();
    //TODO: viewCamera->setID(3);
    cameraTarget = Vector3(0.0);
    viewCamera->setTarget(Vector3(0.0,10.0,0.0));//cameraTarget);
    viewCamera->setFOVInRadians(65.0 * (PI / 180));
    float aspectRatio = screenWidth/screenHeight;
    viewCamera->setAspectRatio(aspectRatio);
    cameraRadius = 20.0;

    //testPlane.setPositionAndNormal(Vector3(0.0,0.0,100.0),Vector3(0.0,1.0,0.0));
    return viewCamera;

}

shared_ptr<CameraNode> SceneHelper::initRenderCamera(SceneManager *smgr, float fov)
{
    shared_ptr<CameraNode>renderCamera = smgr->createCameraNode("RenderingUniforms");
    renderCamera->setTargetAndRotationBindStatus(true);
    renderCamera->setFOVInRadians(fov * PI / 180.0f);
    return renderCamera;
}

void SceneHelper::initLightMesh(SceneManager *smgr)
{
    SceneImporter *importer = new SceneImporter();
    pointLightMesh = importer->loadMeshFromFile(constants::BundlePath + "/sphere.sgm");
    delete importer;
}

Vector3 SceneHelper::planeFacingDirection(int controlType)
{
    if(controlType == X_MOVE || controlType == X_ROTATE || controlType == X_SCALE)
        return Vector3(0,1,0);
    else if(controlType == Y_MOVE || controlType == Y_ROTATE || controlType == Y_SCALE)
        return Vector3(0,0,1);
    else
        return Vector3(0,1,0);
}

Vector3 SceneHelper::controlDirection(int controlType)
{
    if(controlType == X_MOVE || controlType == X_ROTATE || controlType == X_SCALE)
        return Vector3(1,0,0);
    else if(controlType == Y_MOVE || controlType == Y_ROTATE || controlType == Y_SCALE)
        return Vector3(0,1,0);
    else
        return Vector3(0,0,1);
}

SGNode* SceneHelper::createCircle(SceneManager *smgr)
{
    SGNode* rotationCircle = new SGNode(NODE_SGM);
    rotationCircle->materialProps.push_back(new MaterialProperty(NODE_SGM));

    rotationCircle->node = smgr->createCircleNode(100,1.0,"RotationCircle");
    rotationCircle->node->setID(CIRCLE_NODE_ID);
    rotationCircle->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    Vector4 vColor = Vector4(0.0, 1.0, 0.0, 1.0);
    rotationCircle->getProperty(VERTEX_COLOR).value = vColor;
    rotationCircle->getProperty(TRANSPARENCY).value.x = 1.0;
    return rotationCircle;
}

SGNode* SceneHelper::createLightCircles(SceneManager *smgr)
{
    SGNode* circles = new SGNode(NODE_SGM);
    circles->materialProps.push_back(new MaterialProperty(NODE_SGM));

    circles->node = smgr->createCircleNode(50, 1.0, "LightCircles", true);
    circles->node->setID(LIGHT_CIRCLES);
    circles->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    Vector4 vColor = Vector4(1.0, 1.0, 0.0, 1.0);
    circles->getProperty(VERTEX_COLOR).value = vColor;
    circles->getProperty(TRANSPARENCY).value.x = 1.0;

    circles->node->setVisible(false);
    return circles;
}

SGNode* SceneHelper::createLightDirLine(SceneManager *smgr)
{
    float ld = 0.75;
    vector< Vector3 > vPositions;
    vPositions.push_back(Vector3(0.0, 0.0, 0.0)); vPositions.push_back(Vector3(0.0, 10.0, 0.0));
    vPositions.push_back(Vector3( ld, 0.0,  ld)); vPositions.push_back(Vector3( ld, 10.0,  ld));
    vPositions.push_back(Vector3(-ld, 0.0, -ld)); vPositions.push_back(Vector3(-ld, 10.0, -ld));
    vPositions.push_back(Vector3( ld, 0.0, -ld)); vPositions.push_back(Vector3( ld, 10.0, -ld));
    vPositions.push_back(Vector3(-ld, 0.0,  ld)); vPositions.push_back(Vector3(-ld, 10.0,  ld));
    vPositions.push_back(Vector3(-ld, 0.0,  ld)); vPositions.push_back(Vector3(-ld, 10.0,  ld));
    
    SGNode* gridLines = new SGNode(NODE_SGM);
    gridLines->materialProps.push_back(new MaterialProperty(NODE_SGM));

    Mesh* mesh = new Mesh();
    
    vector< vertexData > mbvd;
    vector< unsigned short > mbi;
    for(int i = 0; i < vPositions.size(); i++) {
        vertexData v;
        v.vertPosition = vPositions[i];
        mbvd.push_back(v);
        mbi.push_back(i);
    }
    mesh->addMeshBuffer(mbvd, mbi, 0);
    
    shared_ptr<Node> node = smgr->createNodeFromMesh(mesh, "LightLine");
    node->setID(LIGHT_DIRECTION_ID);
    node->type = NODE_TYPE_LINES;
    node->drawMode = DRAW_MODE_LINES;
    gridLines->node = node;
    gridLines->getProperty(LIGHTING).value.x = false;
    gridLines->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    Vector4 vColor = Vector4(1.0, 1.0, 0.0, 1.0);
    gridLines->getProperty(VERTEX_COLOR).value = vColor;
    gridLines->getProperty(TRANSPARENCY).value.x = 1.0;
    gridLines->node->setVisible(false);
    return gridLines;
}

SGNode* SceneHelper::createRedLines(SceneManager *smgr)
{
    int gridSize = 100;
    vector< Vector3 > vPositions;
    vPositions.push_back(Vector3(-gridSize, 0, 0));
    vPositions.push_back(Vector3(gridSize, 0, 0));
    return createLines(smgr, vPositions, Vector3(1.0,0.2,0.2), "RedLines", RED_LINES_ID);
}

SGNode* SceneHelper::createGreenLines(SceneManager *smgr)
{
    int gridSize = 100;
    vector< Vector3 > vPositions;
    vPositions.push_back(Vector3(0, 0, -gridSize));
    vPositions.push_back(Vector3(0, 0, gridSize));
    return createLines(smgr, vPositions, Vector3(0.2,1.0,0.2), "GreenLines", GREEN_LINES_ID);

}

SGNode* SceneHelper::createBlueLines(SceneManager *smgr)
{
    int gridSize = 100;

    vector<Vector3> vPositionsGrid;
    for (int i = -gridSize; i <= gridSize; i+= 5) {
        vPositionsGrid.push_back(Vector3(i, 0, -gridSize));
        vPositionsGrid.push_back(Vector3(i, 0,  gridSize));
        vPositionsGrid.push_back(Vector3(-gridSize, 0, i));
        vPositionsGrid.push_back(Vector3( gridSize, 0, i));
    }
//    vPositionsGrid.push_back(Vector3(gridSize, 0 , i));
    return createLines(smgr, vPositionsGrid, Vector3(0.6, 0.6, 1.0), "BlueLines", BLUE_LINES_ID);
}

SGNode* SceneHelper::createLines(SceneManager *smgr, vector<Vector3> positions, Vector3 color, string callbackName, int nodeId)
{
    SGNode* gridLines = new SGNode(NODE_SGM);
    gridLines->materialProps.push_back(new MaterialProperty(NODE_SGM));

    Mesh * mesh = new Mesh();
    
    vector< vertexData > mbvd;
    vector< unsigned short > mbi;
    for(int i = 0; i < positions.size(); i++) {
        vertexData v;
        v.vertPosition = positions[i];
        mbvd.push_back(v);
        mbi.push_back(i);
    }
    
    mesh->addMeshBuffer(mbvd, mbi, 0);
    
    shared_ptr<Node> node = smgr->createNodeFromMesh(mesh, callbackName);
    node->setID(nodeId);
    node->type = NODE_TYPE_LINES;
    node->drawMode = DRAW_MODE_LINES;
    gridLines->node = node;
    gridLines->getProperty(LIGHTING).value.x = false;
    gridLines->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    Vector4 vColor = Vector4(color, 1.0);
    gridLines->getProperty(VERTEX_COLOR).value = vColor;
    gridLines->getProperty(TRANSPARENCY).value.x = 1.0;
    
    return gridLines;
}

vector<SGNode*> SceneHelper::initControls(SceneManager *smgr)
{
    vector<SGNode*> sceneControls;
    for(int i = 0; i < TOTAL_CONTROLS; i++) {
        SGNode *sgNode = new SGNode(NODE_SGM);
        sgNode->materialProps.push_back(new MaterialProperty(NODE_SGM));
        sgNode->getProperty(IS_VERTEX_COLOR).value = Vector4(true, 0, 0, 0);
        sgNode->getProperty(VERTEX_COLOR).value = Vector4(Vector3(-1.0), 0.0);
        
        SceneImporter *importer = new SceneImporter();
        Mesh *ctrlMesh = importer->loadMeshFromFile(constants::BundlePath + "/controls" + to_string(i+1) + ".sgm");
        delete importer;

        if(ctrlMesh == NULL) {
            Logger::log(ERROR,"SGScene", "SGRSpheres Mesh Not Loaded");
            return sceneControls;
        }
        
        shared_ptr<MeshNode> ctrlNode = smgr->createNodeFromMesh(ctrlMesh,"setCtrlUniforms");
        if(!ctrlNode) {
            Logger::log(ERROR,"SGScene", "Unable to create sgrSpheres");
            return sceneControls;
        }
        
        sgNode->node = ctrlNode;
        sgNode->getProperty(LIGHTING).value.x = false;
        sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
        
        Texture *nodeTex = smgr->loadTexture("Dummy Texture", constants::BundlePath + "/dummy.png", TEXTURE_RGBA8, TEXTURE_BYTE, true);
        sgNode->materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
        sgNode->materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_SHADOWMAP);

        sceneControls.push_back(sgNode);
        sgNode->node->setID(CONTROLS_START_ID + (int)sceneControls.size() - 1);

        if(i == Y_ROTATE)
            sgNode->node->setRotation(Quaternion(Vector3(180.0, 180.0, 0.0) * DEGTORAD));
    }
    
    return sceneControls;
}

SGNode* SceneHelper::initIndicatorNode(SceneManager *smgr)
{
    SGNode *sgNode = new SGNode(NODE_SGM);
    sgNode->materialProps.push_back(new MaterialProperty(NODE_SGM));

    SceneImporter *importer = new SceneImporter();
    Mesh *indMesh = importer->loadMeshFromFile(constants::BundlePath + "/indicator.sgm");
    delete importer;

    if(indMesh == NULL){
        Logger::log(ERROR,"SGScene", "Indicator Mesh Not Loaded");
    }
    shared_ptr<MeshNode> indNode = smgr->createNodeFromMesh(indMesh,"setCtrlUniforms");
    if(!indNode){
        Logger::log(ERROR,"SGScene", "Unable to create Indicator node");
    }
    sgNode->node = indNode;
    
    sgNode->getProperty(LIGHTING).value.x = false;// Vector4(false, 0, 0, 0), UNDEFINED);
    Vector4 vColor = Vector4(1.0, 1.0, 0.3, 1.0);
    sgNode->getProperty(VERTEX_COLOR).value = vColor;
    
    sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    sgNode->node->setID(FORCE_INDICATOR_ID);
    
    Texture *nodeTex = smgr->loadTexture("Dummy Texture",constants::BundlePath + "/dummy.png",TEXTURE_RGBA8,TEXTURE_BYTE, true);
    sgNode->materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
    sgNode->materialProps[0]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_SHADOWMAP);
    sgNode->node->setScale(Vector3(0.5,2.0,0.5));
    sgNode->node->setVisible(false);

    return sgNode;
}

void SceneHelper::limitPixelCoordsWithinTextureRange(float texWidth,float texHeight,float &xCoord,float &yCoord)
{
    xCoord = (xCoord >= texWidth)?(texWidth-1.0):xCoord;
    yCoord = (yCoord >= texHeight)?(texHeight-1.0):yCoord;
    
    xCoord = (xCoord < 0.0)? 0.0 : xCoord;
    yCoord = (yCoord < 0.0) ? 0.0 : yCoord;
}

