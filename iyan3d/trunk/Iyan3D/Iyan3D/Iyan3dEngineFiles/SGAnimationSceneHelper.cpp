#include "HeaderFiles/SGAnimationSceneHelper.h"

SGAnimationSceneHelper::SGAnimationSceneHelper()
{
}

Quaternion SGAnimationSceneHelper::readRotationFromSGAFile(float rollValue,ifstream* filePointer)
{
	Quaternion rotation;
	rotation.x = FileHelper::readFloat(filePointer);
	rotation.y = FileHelper::readFloat(filePointer);
	rotation.z = FileHelper::readFloat(filePointer);
	rotation.w = FileHelper::readFloat(filePointer);
	return rotation;
}

void SGAnimationSceneHelper::writeScene(ofstream *filePointer , int totalFrames , float cameraFOV, int sceneObjectsSize)
{
    FileHelper::writeInt(filePointer,totalFrames);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].x);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].y);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].z);
    FileHelper::writeFloat(filePointer,ShaderManager::shadowDensity);
    FileHelper::writeFloat(filePointer,cameraFOV);
    FileHelper::writeInt(filePointer,sceneObjectsSize);
}

void SGAnimationSceneHelper::readScene(ifstream *filePointer, int &totalFrames , float &cameraFOV , int &nodeCount)
{
    totalFrames = FileHelper::readInt(filePointer);
    Vector3 lightColor;
    lightColor.x = FileHelper::readFloat(filePointer);
    lightColor.y = FileHelper::readFloat(filePointer);
    lightColor.z = FileHelper::readFloat(filePointer);
    ShaderManager::lightColor.push_back(lightColor);
    ShaderManager::shadowDensity = FileHelper::readFloat(filePointer);
    cameraFOV = FileHelper::readFloat(filePointer);
    nodeCount = FileHelper::readInt(filePointer);
    
}

void SGAnimationSceneHelper::storeSGRAnimations(string filePath , SGNode *sgNode , int totalFrames , vector<int>totalKeyFrames)
{
    //string filePath = FileHelper::getDocumentsDirectory() + "sample.sgra";
    ofstream sgraFile(filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    FileHelper::writeShort(&sgraFile, 1);
    FileHelper::writeShort(&sgraFile, 0);
    FileHelper::writeShort(&sgraFile, sgNode->joints.size());
    FileHelper::writeShort(&sgraFile, totalFrames);
    FileHelper::writeShort(&sgraFile, (short)totalKeyFrames.size());
    short lastKeyFrameId = totalKeyFrames[totalKeyFrames.size()-1];
    FileHelper::writeShort(&sgraFile, lastKeyFrameId);
    
    Vector3 nodeInitPos = Vector3(0.0);
    int posCount = 0;
    for(int i = 0; i < totalKeyFrames.size(); i++)
    {
        short frame = totalKeyFrames[i];
        ActionKey nodeKey = sgNode->getKeyForFrame(frame);
        short activeKeys = 0;
        if(nodeKey.isPositionKey) activeKeys++;
        if(nodeKey.isRotationKey) activeKeys++;
        if(nodeKey.isScaleKey) activeKeys++;
        
        FileHelper::writeShort(&sgraFile, frame); // Frame Id
        
        FileHelper::writeShort(&sgraFile, activeKeys);
        
        if(nodeKey.isPositionKey) {
            int type = 1;
            
            Vector3 offsetPos = Vector3();
            FileHelper::writeShort(&sgraFile, type);
            FileHelper::writeFloat(&sgraFile, (nodeKey.position.x - nodeInitPos.x));
            FileHelper::writeFloat(&sgraFile, (nodeKey.position.y - nodeInitPos.y));
            FileHelper::writeFloat(&sgraFile, (nodeKey.position.z - nodeInitPos.z));
            if(posCount == 0) nodeInitPos = nodeKey.position;
            posCount++;
        }
        if(nodeKey.isRotationKey) {
            int type = 2;
            FileHelper::writeShort(&sgraFile, type);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.x);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.y);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.z);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.w);
        }
        if(nodeKey.isScaleKey) {
            int type = 3;
            FileHelper::writeShort(&sgraFile, type);
            FileHelper::writeFloat(&sgraFile, nodeKey.scale.x);
            FileHelper::writeFloat(&sgraFile, nodeKey.scale.y);
            FileHelper::writeFloat(&sgraFile, nodeKey.scale.z);
        }
        
        for(int jointId = 1; jointId < sgNode->joints.size(); jointId++) {
            
            ActionKey frameKey = sgNode->joints[jointId]->getKeyForFrame(frame);
            short activeKeys = 0;
            if(frameKey.isPositionKey) activeKeys++;
            if(frameKey.isRotationKey) activeKeys++;
            if(frameKey.isScaleKey) activeKeys++;
            
            FileHelper::writeShort(&sgraFile, activeKeys);
            
            if(frameKey.isPositionKey) {
                int type = 1;
                FileHelper::writeShort(&sgraFile, type);
                FileHelper::writeFloat(&sgraFile, frameKey.position.x);
                FileHelper::writeFloat(&sgraFile, frameKey.position.y);
                FileHelper::writeFloat(&sgraFile, frameKey.position.z);
            }
            if(frameKey.isRotationKey) {
                int type = 2;
                FileHelper::writeShort(&sgraFile, type);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.x);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.y);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.z);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.w);
            }
            if(frameKey.isScaleKey) {
                int type = 3;
                FileHelper::writeShort(&sgraFile, type);
                FileHelper::writeFloat(&sgraFile, frameKey.scale.x);
                FileHelper::writeFloat(&sgraFile, frameKey.scale.y);
                FileHelper::writeFloat(&sgraFile, frameKey.scale.z);
            }

        }
    }
    
    sgraFile.close();
    
}


void SGAnimationSceneHelper::applySGRAnimations(string filePath, SGNode *sgNode, int &totalFrames , int currentFrame , int &animFrames)
{
    //string filePath = FileHelper::getDocumentsDirectory() + "sample.sgra";
    ifstream sgraFile(filePath,ios::in | ios::binary);
    
    FileHelper::resetSeekPosition();
    short versionNumber = FileHelper::readShort(&sgraFile);
    short type = FileHelper::readShort(&sgraFile);
    short numberOfJoints = FileHelper::readShort(&sgraFile);
    short numberOfFrames = FileHelper::readShort(&sgraFile);
    short numberOfKeyFrames = FileHelper::readShort(&sgraFile);
    short lastFrameId = FileHelper::readShort(&sgraFile);
    
   
    if((currentFrame + lastFrameId+1) > totalFrames)
        totalFrames = currentFrame + lastFrameId+1;
     animFrames = totalFrames;
    
    Vector3 nodeInitPos = sgNode->node->getAbsolutePosition();
    int posCount = 0;
    //printf("Node Init Pos %f %f %f ",nodeInitPos.x, nodeInitPos.y, nodeInitPos.z);
    for (int i = 0; i < numberOfKeyFrames; i++) {
        short keyFrameId = currentFrame + FileHelper::readShort(&sgraFile);
        
        ActionKey nodeKey;
        
        short activeKeys = FileHelper::readShort(&sgraFile);
        int keyType = 0;
        
        for (short j = 0; j < activeKeys; j++) {
            
            keyType = FileHelper::readShort(&sgraFile);
            
            if(keyType == 1) {
                nodeKey.isPositionKey = true;
                Vector3 position;
                position.x = (posCount==0) ? nodeInitPos.x : (nodeInitPos.x +  FileHelper::readFloat(&sgraFile));
                position.y = (posCount==0) ? nodeInitPos.y : (nodeInitPos.y + FileHelper::readFloat(&sgraFile));
                position.z = (posCount==0) ? nodeInitPos.z : (nodeInitPos.z + FileHelper::readFloat(&sgraFile));
                if(posCount == 0){
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                }
                posCount++;
                nodeKey.position = position;
            }
            if(keyType == 2) {
                nodeKey.isRotationKey = true;
                Quaternion rotation;
                rotation.x = FileHelper::readFloat(&sgraFile);
                rotation.y = FileHelper::readFloat(&sgraFile);
                rotation.z = FileHelper::readFloat(&sgraFile);
                rotation.w = FileHelper::readFloat(&sgraFile);
                nodeKey.rotation = rotation;
            }
            if(keyType == 3) {
                nodeKey.isScaleKey = true;
                Vector3 scale;
                scale.x = FileHelper::readFloat(&sgraFile);
                scale.y = FileHelper::readFloat(&sgraFile);
                scale.z = FileHelper::readFloat(&sgraFile);
                nodeKey.scale = scale;
            }
        }
        
        sgNode->setKeyForFrame(keyFrameId, nodeKey);
        
        for (int jointId = 1; jointId < numberOfJoints; jointId++) {
            
            ActionKey jointKey;
            
            short activeKeys = FileHelper::readShort(&sgraFile);
            int keyType = 0;
            for (short j = 0; j < activeKeys; j++) {
                keyType = FileHelper::readShort(&sgraFile);
                if(keyType == 1) {
                    jointKey.isPositionKey = true;
                    Vector3 position;
                    position.x = FileHelper::readFloat(&sgraFile);
                    position.y = FileHelper::readFloat(&sgraFile);
                    position.z = FileHelper::readFloat(&sgraFile);
                    jointKey.position = position;
                }
                if(keyType == 2) {
                    jointKey.isRotationKey = true;
                    Quaternion rotation;
                    rotation.x = FileHelper::readFloat(&sgraFile);
                    rotation.y = FileHelper::readFloat(&sgraFile);
                    rotation.z = FileHelper::readFloat(&sgraFile);
                    rotation.w = FileHelper::readFloat(&sgraFile);
                    jointKey.rotation = rotation;
                }
                if(keyType == 3) {
                    jointKey.isScaleKey = true;
                    Vector3 scale;
                    scale.x = FileHelper::readFloat(&sgraFile);
                    scale.y = FileHelper::readFloat(&sgraFile);
                    scale.z = FileHelper::readFloat(&sgraFile);
                    jointKey.scale = scale;
                }
            }
            
            sgNode->joints[jointId]->setKeyForFrame(keyFrameId, jointKey);

        }
    }
    
    sgraFile.close();

}

void SGAnimationSceneHelper::storeTextAnimations(string filePath, SGNode *sgNode, int totalFrames , vector<Vector3> jointBasePositions, vector<int>totalKeyFrames)
{
    ofstream sgtaFile(filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    FileHelper::writeShort(&sgtaFile, 1);
    FileHelper::writeShort(&sgtaFile, 1);
    FileHelper::writeShort(&sgtaFile, sgNode->joints.size());
    FileHelper::writeShort(&sgtaFile, totalFrames);
    FileHelper::writeShort(&sgtaFile, (short)totalKeyFrames.size());
    short lastKeyFrameId = totalKeyFrames[totalKeyFrames.size()-1];
    FileHelper::writeShort(&sgtaFile, lastKeyFrameId);
    
    Vector3 nodeInitPos = Vector3(0.0);
    int posCount = 0;

        for(int i = 0; i < totalKeyFrames.size(); i++)
        {
            short frame = totalKeyFrames[i];
            ActionKey nodeKey = sgNode->getKeyForFrame(frame);
            short activeKeys = 0;
            if(nodeKey.isPositionKey) activeKeys++;
            if(nodeKey.isRotationKey) activeKeys++;
            if(nodeKey.isScaleKey) activeKeys++;
            
            FileHelper::writeShort(&sgtaFile, frame); // Frame Id
            
            FileHelper::writeShort(&sgtaFile, activeKeys);
            
            if(nodeKey.isPositionKey) {
                int type = 1;
                FileHelper::writeShort(&sgtaFile, type);
                FileHelper::writeFloat(&sgtaFile, (nodeKey.position.x - nodeInitPos.x));
                FileHelper::writeFloat(&sgtaFile, (nodeKey.position.y - nodeInitPos.y));
                FileHelper::writeFloat(&sgtaFile, (nodeKey.position.z - nodeInitPos.z));
                if(posCount == 0) nodeInitPos = nodeKey.position;
                posCount++;
            }
            if(nodeKey.isRotationKey) {
                int type = 2;
                FileHelper::writeShort(&sgtaFile, type);
                FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.x);
                FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.y);
                FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.z);
                FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.w);
            }
            if(nodeKey.isScaleKey) {
                int type = 3;
                FileHelper::writeShort(&sgtaFile, type);
                FileHelper::writeFloat(&sgtaFile, nodeKey.scale.x);
                FileHelper::writeFloat(&sgtaFile, nodeKey.scale.y);
                FileHelper::writeFloat(&sgtaFile, nodeKey.scale.z);
            }
            
            for(int jointId = 1; jointId < sgNode->joints.size(); jointId++) {
                
                ActionKey frameKey = sgNode->joints[jointId]->getKeyForFrame(frame);
                short activeKeys = 0;
                if(frameKey.isPositionKey) activeKeys++;
                if(frameKey.isRotationKey) activeKeys++;
                if(frameKey.isScaleKey) activeKeys++;
                
                FileHelper::writeShort(&sgtaFile, activeKeys);
                
                if(frameKey.isPositionKey) {
                    int type = 1;
                    
                    Vector3 offsetPos = Vector3(frameKey.position.x-jointBasePositions[jointId].x,frameKey.position.y-jointBasePositions[jointId].y,frameKey.position.z-jointBasePositions[jointId].z);

                    FileHelper::writeShort(&sgtaFile, type);
                    FileHelper::writeFloat(&sgtaFile, offsetPos.x);
                    FileHelper::writeFloat(&sgtaFile, offsetPos.y);
                    FileHelper::writeFloat(&sgtaFile, offsetPos.z);
                }
                if(frameKey.isRotationKey) {
                    int type = 2;
                    FileHelper::writeShort(&sgtaFile, type);
                    FileHelper::writeFloat(&sgtaFile, frameKey.rotation.x);
                    FileHelper::writeFloat(&sgtaFile, frameKey.rotation.y);
                    FileHelper::writeFloat(&sgtaFile, frameKey.rotation.z);
                    FileHelper::writeFloat(&sgtaFile, frameKey.rotation.w);
                }
                if(frameKey.isScaleKey) {
                    int type = 3;
                    FileHelper::writeShort(&sgtaFile, type);
                    FileHelper::writeFloat(&sgtaFile, frameKey.scale.x);
                    FileHelper::writeFloat(&sgtaFile, frameKey.scale.y);
                    FileHelper::writeFloat(&sgtaFile, frameKey.scale.z);
                }
                
            }
        }

    sgtaFile.close();
}

void SGAnimationSceneHelper::applyTextAnimations(string filePath, SGNode *sgNode, int &totalFrames, int currentFrame , vector<Vector3> jointBasePositions, int &animFrames)
{
    if(jointBasePositions.size() < sgNode->joints.size())
        return;
    
    ifstream sgraFile(filePath,ios::in | ios::binary);
    
    FileHelper::resetSeekPosition();
    short versionNumber = FileHelper::readShort(&sgraFile);
    short type = FileHelper::readShort(&sgraFile);
    short numberOfJoints = FileHelper::readShort(&sgraFile);
    short numberOfFrames = FileHelper::readShort(&sgraFile);
    short numberOfKeyFrames = FileHelper::readShort(&sgraFile);
    short lastFrameId = FileHelper::readShort(&sgraFile);
    
    if((currentFrame + lastFrameId+1) > totalFrames)
        totalFrames = currentFrame + lastFrameId+1;
    animFrames = totalFrames;
    Vector3 nodeInitPos = sgNode->node->getAbsolutePosition();
    int posCount = 0;

    for (int i = 0; i < numberOfKeyFrames; i++) {
        short keyFrameId = currentFrame + FileHelper::readShort(&sgraFile);
        
        ActionKey nodeKey;
        
        short activeKeys = FileHelper::readShort(&sgraFile);
        int keyType = 0;
        
        for (short j = 0; j < activeKeys; j++) {
            
            keyType = FileHelper::readShort(&sgraFile);
            if(keyType == 1) {
                nodeKey.isPositionKey = true;
                Vector3 position;
                position.x = (posCount==0) ? nodeInitPos.x : (nodeInitPos.x +  FileHelper::readFloat(&sgraFile));
                position.y = (posCount==0) ? nodeInitPos.y : (nodeInitPos.y + FileHelper::readFloat(&sgraFile));
                position.z = (posCount==0) ? nodeInitPos.z : (nodeInitPos.z + FileHelper::readFloat(&sgraFile));
                if(posCount == 0){
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                }
                posCount++;
                nodeKey.position = position;
            }
            if(keyType == 2) {
                nodeKey.isRotationKey = true;
                Quaternion rotation;
                rotation.x = FileHelper::readFloat(&sgraFile);
                rotation.y = FileHelper::readFloat(&sgraFile);
                rotation.z = FileHelper::readFloat(&sgraFile);
                rotation.w = FileHelper::readFloat(&sgraFile);
                nodeKey.rotation = rotation;
            }
            if(keyType == 3) {
                nodeKey.isScaleKey = true;
                Vector3 scale;
                scale.x = FileHelper::readFloat(&sgraFile);
                scale.y = FileHelper::readFloat(&sgraFile);
                scale.z = FileHelper::readFloat(&sgraFile);
                nodeKey.scale = scale;
            }
        }
        
        sgNode->setKeyForFrame(keyFrameId, nodeKey);
        
        for (int jointId = 1; jointId < numberOfJoints; jointId++) {
            
            ActionKey jointKey;
            
            short activeKeys = FileHelper::readShort(&sgraFile);
            int keyType = 0;
            for (short j = 0; j < activeKeys; j++) {
                keyType = FileHelper::readShort(&sgraFile);
                if(keyType == 1) {
                    jointKey.isPositionKey = true;
                    Vector3 position;
                    position.x = FileHelper::readFloat(&sgraFile);
                    position.y = FileHelper::readFloat(&sgraFile);
                    position.z = FileHelper::readFloat(&sgraFile);
                    jointKey.position = position;
                }
                if(keyType == 2) {
                    jointKey.isRotationKey = true;
                    Quaternion rotation;
                    rotation.x = FileHelper::readFloat(&sgraFile);
                    rotation.y = FileHelper::readFloat(&sgraFile);
                    rotation.z = FileHelper::readFloat(&sgraFile);
                    rotation.w = FileHelper::readFloat(&sgraFile);
                    jointKey.rotation = rotation;
                }
                if(keyType == 3) {
                    jointKey.isScaleKey = true;
                    Vector3 scale;
                    scale.x = FileHelper::readFloat(&sgraFile);
                    scale.y = FileHelper::readFloat(&sgraFile);
                    scale.z = FileHelper::readFloat(&sgraFile);
                    jointKey.scale = scale;
                }
            }
            
            //sgNode->joints[jointId]->setKeyForFrame(keyFrameId, jointKey);
            
            int j = jointId;
            while (j < (int)sgNode->joints.size()) {
                
                ActionKey actualKey;
                actualKey.isPositionKey = jointKey.isPositionKey;
                actualKey.isRotationKey = jointKey.isRotationKey;
                actualKey.isScaleKey = jointKey.isScaleKey;
                actualKey.rotation = jointKey.rotation;actualKey.scale = jointKey.scale;
                
                actualKey.position.x = jointBasePositions[j].x + jointKey.position.x;
                actualKey.position.y = jointBasePositions[j].y + jointKey.position.y;
                actualKey.position.z = jointBasePositions[j].z + jointKey.position.z;
                
                sgNode->joints[j]->setKeyForFrame(keyFrameId, actualKey);
                
                j += (numberOfJoints -1);
            }

        }
    }

    sgraFile.close();

}

vector<Vector3> SGAnimationSceneHelper::storeTextInitialPositions(SGNode *sgnode)
{
    int jointCount = (dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node))->getJointCount();
    
    vector<Vector3> jointPositions;
    for (int i = 0; i < jointCount; i++) {
        jointPositions.push_back((dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node))->getJointNode(i)->getPosition());
    }
    return jointPositions;
}
