#include "HeaderFiles/BoneLimitsHelper.h"


static Json::Value boneLimitsJson;
vector <BoneLimits> boneLimitsArray;
vector <Vector3> tPoseRotations;
vector <int> mirrorJoints;


int BoneLimitsHelper::getMirrorJointId(int jointId){
    if(jointId < mirrorJoints.size())
        return mirrorJoints[jointId];
    return -1;
}

void BoneLimitsHelper::init(){
    
    boneLimitsArray.clear();
    tPoseRotations.clear();
    
    ifstream jsonFile(constants::BundlePath + "/boneLimits45.json");
    if(!jsonFile.is_open()){
        Logger::log(ERROR, "BoneLimits json file not found", "BoneLimitsHelper");
        return;
    }
    Json::Reader reader;
    if(!reader.parse(jsonFile, boneLimitsJson, false)){
        Logger::log(ERROR, "BoneLimits json parser error", "BoneLimitsHelper");
        return;
    }
    
    Json::Value boneLimitsValues = boneLimitsJson["boneLimits"];
    for(Json::ValueIterator itr = boneLimitsValues.begin() ; itr != boneLimitsValues.end() ; itr++ ) {
        BoneLimits boneLimits = getBoneLimits(itr.key().asString());
        KeyHelper::addKey(boneLimitsArray, boneLimits); //To store in the sorted order of id.
    }
    
    Json::Value initialRotationArray;
    ifstream rotationJsonFile(constants::BundlePath + "/humanTPoseBoneLimits.json");
    if(!rotationJsonFile.is_open()){
        Logger::log(ERROR, "humanTPoseBoneLimits.json file not found", "BoneLimitsHelper");
        return;
    }
    if(!reader.parse(rotationJsonFile, initialRotationArray, false)){
        Logger::log(ERROR, "humanTPoseBoneLimits.json parser error", "BoneLimitsHelper");
        return;
    }

    int num_joints = initialRotationArray.size();
    for(int i=0; i<num_joints; i++)
        tPoseRotations.push_back(Vector3(initialRotationArray[i]["X"].asDouble(),
                                           initialRotationArray[i]["Y"].asDouble(),
                                           initialRotationArray[i]["Z"].asDouble()));
    
    
    Json::Value mirrorJointsJson;
    ifstream mirrorJointsJsonFile(constants::BundlePath + "/mirrorJoints.json");
    if(!rotationJsonFile.is_open()){
        Logger::log(ERROR, "mirrorJoints.json file not found", "BoneLimitsHelper");
        return;
    }
    if(!reader.parse(mirrorJointsJsonFile, mirrorJointsJson, false)){
        Logger::log(ERROR, "mirrorJoints.json parser error", "BoneLimitsHelper");
        return;
    }
    num_joints = mirrorJointsJson.size();
    for(int i=0; i<num_joints; i++){
        mirrorJoints.push_back(mirrorJointsJson[i].asInt());
        //cout<<mirrorJoints[i]<<endl;
    }
    
    //printBoneLimits();
}

void BoneLimitsHelper::printBoneLimits(){
    Json::Value mirrors(Json::arrayValue);
    for(int i=0;i<boneLimitsArray.size();i++){
        std::string name = boneLimitsArray[i].name;
        if(name[name.length()-1]=='L'){
            mirrors.append(boneLimitsJson["boneLimits"][name.substr(0,name.length()-1)+'R']["id"].asInt());
        }
        else if(name[name.length()-1]=='R') {
            mirrors.append(boneLimitsJson["boneLimits"][name.substr(0,name.length()-1)+'L']["id"].asInt());
        }
        else mirrors.append(-1);
    }
}

Vector3 BoneLimitsHelper::getTPoseRotationDeg(int boneId){
    Vector3 junk(-1.0);
    if(boneId < tPoseRotations.size())
        return tPoseRotations[boneId];
    Logger::log(ERROR, "Unable to get TPoseRotation for boneId: "+to_string(boneId), "BoneLimitsHelper");
    return junk;
}

BoneLimits BoneLimitsHelper::getBoneLimits(int boneId){
    int keyIndex = KeyHelper::getKeyIndex(boneLimitsArray, boneId);
    if(keyIndex == -1 || boneLimitsArray[keyIndex].id!= boneId)
    {
        Logger::log(ERROR, "Invalid boneId: "+to_string(boneId)+". BoneLimitsArray size : "+to_string(boneLimitsArray.size()),"BoneLimitsHelper");
        BoneLimits junk;
        return junk;
    }
    return boneLimitsArray[keyIndex];
}

BoneLimits BoneLimitsHelper::getBoneLimits(std::string name){
    Json::Value boneLimitsValue = boneLimitsJson["boneLimits"][name];
    BoneLimits boneLimits;
    boneLimits.name = name;
    boneLimits.id = boneLimitsValue["id"].asInt();
    boneLimits.minRotation.x = boneLimitsValue["X"]["min"].asDouble();
    boneLimits.minRotation.y = boneLimitsValue["Y"]["min"].asDouble();
    boneLimits.minRotation.z = boneLimitsValue["Z"]["min"].asDouble();
    boneLimits.maxRotation.x = boneLimitsValue["X"]["max"].asDouble();
    boneLimits.maxRotation.y = boneLimitsValue["Y"]["max"].asDouble();
    boneLimits.maxRotation.z = boneLimitsValue["Z"]["max"].asDouble();
    return boneLimits;
}

Vector3 BoneLimitsHelper::limitRotation(shared_ptr<Node> node, Vector3 expectedRotation){
    //Gets all the parameters in degrees
    Vector3 baseRotation = BoneLimitsHelper::getTPoseRotationDeg(node->getID());
    Vector3 currentRotation = node->getRotationInDegrees();
    //Converting all angles into -180 to +180 format.
    baseRotation = MathHelper::toEuler(Quaternion(baseRotation*DEGTORAD))*RADTODEG;
    currentRotation = MathHelper::toEuler(Quaternion(currentRotation*DEGTORAD))*RADTODEG;
    expectedRotation = MathHelper::toEuler(Quaternion(expectedRotation*DEGTORAD))*RADTODEG;
    
    BoneLimits boneLimits = getBoneLimits(node->getID());
    Vector3 angleDifference = expectedRotation - baseRotation;
    
    if(angleDifference.x < boneLimits.minRotation.x || angleDifference.y < boneLimits.minRotation.y || angleDifference.z < boneLimits.minRotation.z || angleDifference.x > boneLimits.maxRotation.x || angleDifference.y > boneLimits.maxRotation.y || angleDifference.z > boneLimits.maxRotation.z)
        return currentRotation;
    //node.reset();
    
    return expectedRotation;
}
