#ifndef __iyan3dengine__SGAnimationSceneHelper__
#define __iyan3dengine__SGAnimationSceneHelper__

#include <iostream>
#include "Constants.h"
#include "SGNode.h"
#include "SGJoint.h"
#include "KeyHelper.h"
//#include "BoneLimitsHelper.h"
#include "ShaderManager.h"
#include "SGRotationKey.h"
#include "../../SGEngine2/Utilities/Logger.h"

using namespace std;


class SGAnimationSceneHelper
{
    
public:
	SGAnimationSceneHelper();    
    static Quaternion readRotationFromSGAFile(float rollValue,ifstream* filePointer);
    static void writeScene(ofstream *filePointer , int totalFrames , float cameraFOV, int sceneObjectsSize);
    static void readScene(ifstream *filePointer, int &totalFrames , float &cameraFOV , int &nodeCount);
    static void storeSGRAnimations(string FilePath , SGNode* sgNode, int totalFrames, vector<int> totalKeyFrames);
    static void applySGRAnimations(string filePath, SGNode* sgNode, int &totalFrames , int currentFrame , int &animFrames);

    static void storeTextAnimations(string filePath , SGNode* sgNode, int totalFrames , vector<Vector3> jointPositions, vector<int>totalKeyFrames);
    static void applyTextAnimations(string filePath , SGNode* sgNode, int &totalFrames , int currentFrame , vector<Vector3> jointPositions, int &animFrames);
    static vector<Vector3> storeTextInitialPositions(SGNode *sgnode);
};

#endif