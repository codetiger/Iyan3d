#ifndef Iyan3D_SGNode_h
#define Iyan3D_SGNode_h

#include "Constants.h"

#include <vector>
#include "json_json.h"
#include "../../SGEngine2/Utilities/Logger.h"
#include <fstream>
#include "KeyHelper.h"
#include "SGJoint.h"
#include "SGPositionKey.h"
#include "SGRotationKey.h"
#include "SGScaleKey.h"
#include "SGVisibilityKey.h"
#include "ActionKey.h"
#include "../../SGEngine2/Loaders/OBJMeshFileLoader.h"

struct properties{
    int isLighting,isSelected,fontSize;
    float brightness,shininess,transparency,isColorMaterial,nodeSpecificFloat;
    float reflection, refraction;
    bool isVisible,perVertexColor, faceNormals;
    Vector3 vertexColor,oriVertexColor;
    Vector3 textColor;
    std::string prevMatName;
};

class SGNode{
private:
    NODE_TYPE type;
    

public:
    bool isRigged;
    bool isMirrorEnabled;
    bool isTempNode;
    int assetId,actionId;
    shared_ptr<Node> node;
    std::wstring name;
    std::string textureName,oriTextureName;
    string optionalFilePath;
    properties props;
    Quaternion nodeInitialRotation;
    vector<SGJoint*> joints;
    vector<SGPositionKey> positionKeys;
    vector<SGRotationKey> rotationKeys;
    vector<SGScaleKey> scaleKeys;
    vector<SGVisibilityKey> visibilityKeys;
    vector<Quaternion> jointsInitialRotations;
    
    SGNode(NODE_TYPE type);
    ~SGNode();
    bool checkFileExists(std::string fileName);
    void setSkinningData(SkinMesh *mesh);
    shared_ptr<Node> loadNode(int assetId,std::string texturePath,NODE_TYPE objectType,SceneManager *smgr, std::wstring imagePath,int width,int height,Vector4 textColor, string &filePath);
    shared_ptr<Node> load3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments);
    shared_ptr<Node> loadSkin3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments);
    shared_ptr<Node> addAdittionalLight(SceneManager *smgr, float distance , Vector3 lightColor, float attenuation = 1.0);
    Json::Value parseParticlesJson(int assetId);
    void setParticlesData(shared_ptr<Node> node, Json::Value pData);
    shared_ptr<Node> loadSGMandOBJ(int assetId,NODE_TYPE objectType,SceneManager *smgr);
    shared_ptr<Node> loadSGR(int assetId,NODE_TYPE objectType,SceneManager *smgr);
    shared_ptr<Node> loadImage(string imageName,SceneManager *smgr , float aspectRatio = 1.0);
    shared_ptr<Node> loadVideo(string videoFileName,SceneManager *smgr, float aspectRatio = 1.0);
    shared_ptr<Node> initLightSceneNode(SceneManager *smgr);
    
    void writeData(ofstream* filePointer);
    void readData(ifstream* filePointer);
    ActionKey getKeyForFrame(int frameId);
    void setKeyForFrame(int frameId, ActionKey& key);
    void removeAnimationInCurrentFrame(int currentFrame);

    void setPosition(Vector3 position, int frameId);
    void setRotation(Quaternion rotation, int frameId);
    void setScale(Vector3 scale, int frameId);
    void setVisibility(bool isVisible, int frameId);

    void setPositionOnNode(Vector3 position);
    void setRotationOnNode(Quaternion rotation);
    void setScaleOnNode(Vector3 scale);
    void setVisibilityOnNode(bool visibility);
    void setInitialKeyValues(int actionType);
    void CCD(shared_ptr<JointNode> bone, Vector3 target,int parentHeirarchy,int currentFrame);
    void MoveBone(shared_ptr<JointNode> bone,Vector3 target,int currentFrame);
    void setShaderProperties(float brightness, float specular, bool isLighting, bool isVisible , float currentFrame);
    void clearSGJoints();
    void createSGJoints();
    void faceUserCamera(shared_ptr<CameraNode> viewCamera,int currentFrame);
    
    Vector3 getNodePosition();
    Vector3 getNodeRotation();
    Vector3 getNodeScale();
    Vector3 getJointPosition(int jointId, bool isAbsolutePosition);
    
    Quaternion GetLocalQuaternion(int boneIndex, Quaternion q);
    Quaternion getGlobalQuaternion(shared_ptr<JointNode> bone);
    
    NODE_TYPE getType();
    void setType(NODE_TYPE type);
};
#endif
