
#include "HeaderFiles/AutoRigJointsDataHelper.h"

void AutoRigJointsDataHelper::getTPoseJointsData(vector<TPoseJoint> & tPoseJoints)
{
    tPoseJoints.clear();
    
    Json::Value jointsData;
    Logger::log(INFO, "Autorig joints data", "Tjoints.clear");

    //std::string documentsDir = FileHelper::getDocumentsDirectory();
    ifstream jsonFile( constants::BundlePath + "/jointsData.json");
    Logger::log(INFO, "Autorig joints data", "Bundlepath:"+constants::BundlePath);
    Json::Reader reader;
    if(!reader.parse(jsonFile, jointsData, false)){
        Logger::log(ERROR, "Unable to parse jointsData.json", "AutoRigHelper");
        return;
    }
    Json::Value joints = jointsData["Joints"];
    Vector3 position, rotation;
    TPoseJoint joint;
    for(int i = 0 ; i < joints.size(); i++) {
        Json::Value jointJson = joints[i];
        joint.id = jointJson.get("id", 0).asInt();
        joint.parentId = jointJson.get("parentId", 0).asInt();
        joint.position.x = jointJson.get("XPos" , 0.0).asDouble();
        joint.position.y  = jointJson.get("YPos" , 0.0).asDouble();
        joint.position.z= jointJson.get("ZPos" , 0.0).asDouble();
        joint.rotation.x = jointJson.get("XRot", 0.0).asDouble();
        joint.rotation.y = jointJson.get("YRot", 0.0).asDouble();
        joint.rotation.z = jointJson.get("ZRot", 0.0).asDouble();
        joint.sphereRadius = jointJson.get("sphereRadius", 0.0).asDouble();
        joint.envelopeRadius = jointJson.get("envelopeRadius", 0.0).asDouble();
        tPoseJoints.push_back(joint);
    }
    
    if(tPoseJoints.size()!=54){
        Logger::log(ERROR,"Number of joints is not 54. It's "+to_string(tPoseJoints.size()),"AutoRigHelper");
        tPoseJoints.clear();
    }
    jsonFile.close();
    return;
}

void AutoRigJointsDataHelper::getTPoseJointsDataFromMesh(vector<TPoseJoint> &tPoseJoints, SkinMesh *sMesh)
{
    tPoseJoints.clear();

    TPoseJoint tJoint;
    
    Mat4 globalTransfrom;

    for(int i = 0 ; i < sMesh->joints->size(); i++) {
        Joint * joint = (*sMesh->joints)[i];
        tJoint.id = joint->Index;
        tJoint.parentId = (joint->Parent) ? joint->Parent->Index : -1;
        Vector3 localPos = joint->LocalAnimatedMatrix.getTranslation();
        Vector3 localRot = joint->LocalAnimatedMatrix.getRotationInDegree();
        tJoint.position = Vector3(localPos.x, localPos.y, localPos.z);
        tJoint.rotation = Vector3(localRot.x, localRot.y, localRot.z);
        tJoint.sphereRadius = (i ==0) ? 0.0 : (joint->sphereRadius > 1.0) ? 1.0 :joint->sphereRadius;
        tJoint.envelopeRadius = joint->envelopeRadius;
        tPoseJoints.push_back(tJoint);
    }
    
    return;
}

void AutoRigJointsDataHelper::getTPoseJointsDataForOwnRig(vector<TPoseJoint> & tPoseJoints,int boneCount)
{
    tPoseJoints.clear();
    
    Json::Value jointsData;
    ifstream jsonFile( constants::BundlePath + "/jointsData.json");
    Json::Reader reader;
    if(!reader.parse(jsonFile, jointsData, false)){
        Logger::log(ERROR, "Unable to parse jointsData.json", "AutoRigHelper");
        return;
    }
    Json::Value joints = jointsData["Joints"];
    Vector3 position, rotation;
    TPoseJoint joint;
    float posY = 0;
    for(int i = 0 ; i < 3; i++) {
        Json::Value jointJson = joints[i];
        joint.id = jointJson.get("id", 0).asInt();
        joint.parentId = jointJson.get("parentId", 0).asInt();
        joint.position.x = 0.0;
        joint.position.y = posY;
        joint.position.z = 0.0;
        joint.rotation.x = 0.0;//jointJson.get("XRot", 0.0).asDouble();
        joint.rotation.y = 0.0;//jointJson.get("YRot", 0.0).asDouble();
        joint.rotation.z = 0.0;//jointJson.get("ZRot", 0.0).asDouble();
        joint.sphereRadius = 1.0;//jointJson.get("sphereRadius", 0.0).asDouble();
        joint.envelopeRadius = 1.0;//jointJson.get("envelopeRadius", 0.0).asDouble();
        posY += 1.0;
        tPoseJoints.push_back(joint);
    }
    jsonFile.close();
    return;
}
void AutoRigJointsDataHelper::addNewTPoseJointData(vector<TPoseJoint> & tPoseJoints,int fromJointId)
{
    Vector3 position, rotation;
    TPoseJoint joint;
    joint.id = (int)tPoseJoints.size();
    joint.parentId = fromJointId;
    joint.position.x =  0.0;
    joint.position.y = tPoseJoints[fromJointId].position.y + 1.0;
    joint.position.z =  0.0;
    joint.rotation.x = 0.0;//jointJson.get("XRot", 0.0).asDouble();
    joint.rotation.y = 0.0;//jointJson.get("YRot", 0.0).asDouble();
    joint.rotation.z = 0.0;//jointJson.get("ZRot", 0.0).asDouble();
    joint.sphereRadius = 1.0;//jointJson.get("sphereRadius", 0.0).asDouble();
    joint.envelopeRadius = 1.0;//jointJson.get("envelopeRadius", 0.0).asDouble();
    tPoseJoints.push_back(joint);
    return;
}