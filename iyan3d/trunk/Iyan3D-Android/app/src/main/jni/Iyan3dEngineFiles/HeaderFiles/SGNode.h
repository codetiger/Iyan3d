#ifndef Iyan3D_SGNode_h
#define Iyan3D_SGNode_h

#include "MaterialProperty.h"

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
    int isLighting,isSelected,fontSize, specificInt;
    float transparency,nodeSpecificFloat;
    float reflection, refraction;
    bool isVisible,perVertexColor, faceNormals;
    Vector3 vertexColor,oriVertexColor;
    Vector3 textColor;
    std::string prevMatName;
    
    bool isPhysicsEnabled;
    double weight;
    double forceMagnitude;
    Vector3 forceDirection;
    bool isSoft;
};

class SGNode {
private:
    NODE_TYPE type;
    
public:
    
    std::map< PROP_INDEX, Property > options;
    std::vector< MaterialProperty* > materialProps;
    bool isRigged;
    bool isMirrorEnabled;
    bool isTempNode;
    bool smoothTexture;
    int assetId,actionId;
    shared_ptr<Node> node;
    std::wstring name;
    std::string oriTextureName;
    string optionalFilePath;
    //properties props;
    Quaternion nodeInitialRotation;
    
    std::map< int, SGNode* > instanceNodes;
    vector<SGJoint*> joints;
    vector<SGPositionKey> positionKeys;
    vector<SGRotationKey> rotationKeys;
    vector<SGScaleKey> scaleKeys;
    vector<SGVisibilityKey> visibilityKeys;
    vector<Quaternion> jointsInitialRotations;
    
    SGNode(NODE_TYPE type);
    void setPropertiesOfNode();
    ~SGNode();
    bool checkFileExists(std::string fileName);
    void setSkinningData(SkinMesh *mesh);
    shared_ptr<Node> loadNode(int assetId, std::string meshPath, std::string texturePath, NODE_TYPE objectType, SceneManager *smgr, std::wstring imagePath, int width, int height, Vector4 textColor, string &filePath);
    shared_ptr<Node> load3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments);
    shared_ptr<Node> loadSkin3DText(SceneManager *smgr, std::wstring text, int bezierSegments, float extrude, float width, string fontPath, Vector4 fontColor, float bevelRadius, int bevelSegments);
    shared_ptr<Node> addAdittionalLight(SceneManager *smgr, float distance , Vector3 lightColor, float attenuation = 1.0);
    Json::Value parseParticlesJson(int assetId);
    void setParticlesData(shared_ptr<Node> node, Json::Value pData);
    shared_ptr<Node> loadSGMandOBJ(std::string meshPath, NODE_TYPE objectType, SceneManager *smgr);
    shared_ptr<Node> loadSGR(std::string meshPath, NODE_TYPE objectType, SceneManager *smgr);
    shared_ptr<Node> loadImage(string imageName,SceneManager *smgr , float aspectRatio = 1.0);
    shared_ptr<Node> loadVideo(string videoFileName,SceneManager *smgr, float aspectRatio = 1.0);
    shared_ptr<Node> initLightSceneNode(SceneManager *smgr);
    
    void writeData(ofstream* filePointer, vector<SGNode*> &nodes);
    void leagcyWrite(ofstream* filePointer, vector<SGNode*> &nodes);
    Mesh* readData(ifstream* filePointer, int &origIndex);
    void legacyReadData(ifstream* filePointer, int sgbVersion, int &origIndex);
    ActionKey getKeyForFrame(int frameId);
    void setKeyForFrame(int frameId, ActionKey& key);
    void removeAnimationInCurrentFrame(int currentFrame);

    void setPosition(Vector3 position, int frameId);
    void setRotation(Quaternion rotation, int frameId);
    void setScale(Vector3 scale, int frameId);
    void setVisibility(bool isVisible, int frameId);

    void setPositionOnNode(Vector3 position, bool updateBB = false);
    void setRotationOnNode(Quaternion rotation, bool updateBB = false);
    void setScaleOnNode(Vector3 scale, bool updateBB = false);
    void setVisibilityOnNode(bool visibility);
    void setInitialKeyValues(int actionType);
    void CCD(shared_ptr<JointNode> bone, Vector3 target,int parentHeirarchy,int currentFrame);
    void MoveBone(shared_ptr<JointNode> bone,Vector3 target,int currentFrame);
    void setMeshProperties(float refraction, float reflection, bool isLighting, bool isVisible , bool isPhysicsObj, int physicsType, float fMagnitude, float currentFrame);
    void clearSGJoints();
    void createSGJoints();
    void faceUserCamera(shared_ptr<CameraNode> viewCamera,int currentFrame);
    
    Vector3 getNodePosition();
    Vector3 getNodeScale();
    Vector3 getJointPosition(int jointId, bool isAbsolutePosition);
    
    Quaternion GetLocalQuaternion(int boneIndex, Quaternion q);
    Quaternion getGlobalQuaternion(shared_ptr<JointNode> bone);
    
    NODE_TYPE getType();
    void setType(NODE_TYPE type);
    
    
    bool IsPropertyExists(PROP_INDEX pIndex);
    std::map< PROP_INDEX, Property > getAllProperties(int meshBufferIndex = NOT_SELECTED);
    Property& getProperty(PROP_INDEX pIndex, int meshBufferIndex = NOT_SELECTED);
    PROP_INDEX checkPropertyInSubProps(std::map< PROP_INDEX, Property > propsMap, PROP_INDEX pIndex);
    void addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE type = TYPE_NONE, string title = "", string groupName = " ", string fileName = "", ICON_INDEX iconId = NO_ICON);
    void checkAndUpdatePropsMap(std::map < PROP_INDEX, Property > &propsMap, Property property);
    
    void addAINodeToSave(aiScene &scene, vector< aiNode* > &nodes, vector< aiMesh* > &meshes, vector< aiMaterial* > &materials, vector< aiTexture* > &textures, vector< aiLight* > &lights);
    void LoadNodeFromAINode(aiNode *n);
};
#endif
