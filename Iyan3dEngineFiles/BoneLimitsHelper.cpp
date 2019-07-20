#include "HeaderFiles/BoneLimitsHelper.h"


static Json::Value boneLimitsJson;
vector <BoneLimits> boneLimitsArray;
vector <Vector3> tPoseRotations;
vector <int> mirrorJoints;


int BoneLimitsHelper::getMirrorJointId(int jointId)
{
    if(jointId < mirrorJoints.size())
        return mirrorJoints[jointId];
    return -1;
}

void BoneLimitsHelper::init()
{
    
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
    }
}

BoneLimits BoneLimitsHelper::getBoneLimits(int boneId)
{
    int keyIndex = KeyHelper::getKeyIndex(boneLimitsArray, boneId);
    if(keyIndex == -1 || boneLimitsArray[keyIndex].id!= boneId)
    {
        Logger::log(ERROR, "Invalid boneId: "+to_string(boneId)+". BoneLimitsArray size : "+to_string(boneLimitsArray.size()),"BoneLimitsHelper");
        BoneLimits junk;
        return junk;
    }
    return boneLimitsArray[keyIndex];
}

BoneLimits BoneLimitsHelper::getBoneLimits(std::string name)
{
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


