#include "HeaderFiles/KeyHelper.h"
#include "HeaderFiles/Constants.h"

struct Key
{
    int id;
    bool isPositionKey;
    bool isRotationKey;
    bool isScaleKey;
    bool isVisibleKey;
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    bool isVisible;
    
    Key()
    {
        id = -1;
        isPositionKey = isRotationKey = isScaleKey = isVisibleKey = false;
    }
};

bool operator < (Key A, Key B)
{
    return A.id > B.id;   //To sort in descending order. Lowest element is picked first in heap!
}

void KeyHelper::readData(ifstream* filePointer, int numKeys, vector<SGPositionKey> &positionKeys, vector<SGRotationKey> &rotationKeys, vector<SGScaleKey> &scaleKeys, vector<SGVisibilityKey> &visibilityKeys)
{
    vector<Key> keys;
    positionKeys.clear(); rotationKeys.clear(); scaleKeys.clear(); visibilityKeys.clear();
    int i;
    for(i = 0; i < numKeys; i++) {
        Key key;
        key.id = FileHelper::readInt(filePointer);
        key.isPositionKey = FileHelper::readBool(filePointer);
        key.isRotationKey = FileHelper::readBool(filePointer);
        key.isScaleKey = FileHelper::readBool(filePointer);
        key.isVisibleKey = FileHelper::readBool(filePointer);
        key.isVisible = FileHelper::readBool(filePointer);
        key.position.x = FileHelper::readFloat(filePointer);
        key.position.y = FileHelper::readFloat(filePointer);
        key.position.z = FileHelper::readFloat(filePointer);
        key.rotation.x = FileHelper::readFloat(filePointer);
        key.rotation.y = FileHelper::readFloat(filePointer);
        key.rotation.z = FileHelper::readFloat(filePointer);
        key.rotation.w = FileHelper::readFloat(filePointer);
        key.scale.x = FileHelper::readFloat(filePointer);
        key.scale.y = FileHelper::readFloat(filePointer);
        key.scale.z = FileHelper::readFloat(filePointer);
        keys.push_back(key);
    }
    sort(keys.begin(),keys.end());
    reverse(keys.begin(),keys.end());//converting keys array in the order of ascending order of frameId
    
    for( i = 0; i < keys.size(); i++ ){
        
        if(keys[i].isPositionKey){
            SGPositionKey positionKey;
            positionKey.id = keys[i].id;
            positionKey.position = keys[i].position;
            positionKeys.push_back(positionKey);
        }
        if(keys[i].isRotationKey){
            SGRotationKey rotationKey;
            rotationKey.id = keys[i].id;
            rotationKey.rotation = keys[i].rotation;
            rotationKeys.push_back(rotationKey);
        }
        if(keys[i].isScaleKey){
            SGScaleKey scaleKey;
            scaleKey.id = keys[i].id;
            scaleKey.scale = keys[i].scale;
            scaleKeys.push_back(scaleKey);
        }
        if(keys[i].isVisibleKey){
            SGVisibilityKey visibilityKey;
            visibilityKey.id = keys[i].id;
            visibilityKey.visibility = keys[i].isVisible;
            visibilityKeys.push_back(visibilityKey);
        }
    }
}

void KeyHelper::writeData(ofstream* filePointer, vector<SGPositionKey> &positionKeys, vector<SGRotationKey> &rotationKeys, vector<SGScaleKey> &scaleKeys, vector<SGVisibilityKey> &visibilityKeys)
{
    
    vector<Key> keys;
    int i;
    for(i = 0; i < positionKeys.size(); i++){
        Key key;
        key.isPositionKey = true;
        key.position = positionKeys[i].position;
        key.id = positionKeys[i].id;
        keys.push_back(key);
    }
    for(i = 0; i < rotationKeys.size(); i++){
        int keyIndex = getKeyIndex(keys, rotationKeys[i].id);
        if(keyIndex == -1 || keys[keyIndex].id != rotationKeys[i].id){ //frameId not found
            Key key;
            key.isRotationKey = true;
            key.rotation = rotationKeys[i].rotation;
            key.id = rotationKeys[i].id;
            addKey(keys,key);
        }
        else{
            keys[keyIndex].isRotationKey = true;
            keys[keyIndex].rotation = rotationKeys[i].rotation;
        }
    }
    for(i = 0; i < scaleKeys.size(); i++){
        int keyIndex = getKeyIndex(keys, scaleKeys[i].id);
        if(keyIndex == -1 || keys[keyIndex].id != scaleKeys[i].id){ //frameId not found
            Key key;
            key.isScaleKey = true;
            key.scale = scaleKeys[i].scale;
            key.id = scaleKeys[i].id;
            addKey(keys,key);
        }
        else{
            keys[keyIndex].isScaleKey = true;
            keys[keyIndex].scale = scaleKeys[i].scale;
        }
    }
    for(i = 0; i < visibilityKeys.size(); i++){
        int keyIndex = getKeyIndex(keys, visibilityKeys[i].id);
        if(keyIndex == -1 || keys[keyIndex].id != visibilityKeys[i].id){ //frameId not found
            Key key;
            key.isVisibleKey = true;
            key.isVisible = visibilityKeys[i].visibility;
            key.id = visibilityKeys[i].id;
            addKey(keys,key);
        }
        else{
            keys[keyIndex].isVisibleKey = true;
            keys[keyIndex].isVisible = visibilityKeys[i].visibility;
        }
    }
    
    FileHelper::writeInt(filePointer, (int)keys.size());
    for(i = 0; i < keys.size(); i++) {
        FileHelper::writeInt(filePointer, keys[i].id);
        FileHelper::writeBool(filePointer, keys[i].isPositionKey);
        FileHelper::writeBool(filePointer, keys[i].isRotationKey);
        FileHelper::writeBool(filePointer, keys[i].isScaleKey);
        FileHelper::writeBool(filePointer, keys[i].isVisibleKey);
        FileHelper::writeBool(filePointer, keys[i].isVisible);
        FileHelper::writeFloat(filePointer, keys[i].position.x);
        FileHelper::writeFloat(filePointer, keys[i].position.y);
        FileHelper::writeFloat(filePointer, keys[i].position.z);
        FileHelper::writeFloat(filePointer, keys[i].rotation.x);
        FileHelper::writeFloat(filePointer, keys[i].rotation.y);
        FileHelper::writeFloat(filePointer, keys[i].rotation.z);
        FileHelper::writeFloat(filePointer, keys[i].rotation.w);
        FileHelper::writeFloat(filePointer, keys[i].scale.x);
        FileHelper::writeFloat(filePointer, keys[i].scale.y);
        FileHelper::writeFloat(filePointer, keys[i].scale.z);
    }
    
}
