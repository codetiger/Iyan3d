//
//  SGAnimationManager.h
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGAnimationManager_h
#define SGAnimationManager_h

#include "SGNode.h"
#include "Constants.h"

class SGAnimationManager
{
private:
    SceneManager* smgr;
public:
    
    string animFilePath;
    int animStartFrame;
    int animTotalFrames;

    SGAnimationManager(SceneManager* smgr, void* scene);
    ~SGAnimationManager();
    
    vector<Vector3> storeTextInitialPositions(SGNode *sgnode);
    void applyAnimations(string filePath , int nodeIndex);
    void copyKeysOfNode(int fromNodeId, int toNodeId);
    void copyPropsOfNode(int fromNodeId, int toNodeId, bool excludeKeys = false);
    void applySGRAnimations(string filePath, SGNode *sgNode, int &totalFrames , int currentFrame , int &animFrames);
    void applyTextAnimations(string filePath, SGNode *sgNode, int &totalFrames, int currentFrame , vector<Vector3> jointBasePositions, int &animFrames);
    bool removeAnimationForSelectedNodeAtFrame(int selectedFrame);
    void removeAppliedAnimation(int startFrame, int endFrame);    
    bool storeAnimations(int assetId);
    void storeSGRAnimations(string filePath , SGNode *sgNode , int totalFrames , vector<int>totalKeyFrames);
    void storeTextAnimations(string filePath, SGNode *sgNode, int totalFrames , vector<Vector3> jointBasePositions, vector<int>totalKeyFrames);
};

#endif /* SGAnimationManager_h */
