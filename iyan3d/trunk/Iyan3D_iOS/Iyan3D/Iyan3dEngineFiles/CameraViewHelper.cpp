#include "HeaderFiles/CameraViewHelper.h"

Json::Value cameraViewData;

void CameraViewHelper::readData(){
    
    ifstream jsonFile(constants::BundlePath + "/viewCameraModes.json");
    Json::Reader reader;
    if(!reader.parse(jsonFile, cameraViewData, false)){
        Logger::log(ERROR, "Unable to parse viewCameraModes.json", "CameraViewHelper");
    }
}
Vector3 CameraViewHelper::getCameraPosition(CAMERA_VIEW_MODE mode){
    return Vector3(cameraViewData[mode]["X"].asDouble(),cameraViewData[mode]["Y"].asDouble(),cameraViewData[mode]["Z"].asDouble());
}

Vector2 CameraViewHelper::getCameraAngle(CAMERA_VIEW_MODE mode){
    return Vector2(cameraViewData[mode]["AngleX"].asDouble(),cameraViewData[mode]["AngleY"].asDouble());
}