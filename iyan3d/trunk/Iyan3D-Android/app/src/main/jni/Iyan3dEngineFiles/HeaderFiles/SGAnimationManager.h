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
    string animFilePath;
    int animStartFrame;
    int animTotalFrames;
public:
    
    SGAnimationManager(SceneManager* smgr, void* scene);
    ~SGAnimationManager();
    
    vector<Vector3> storeTextInitialPositions(SGNode *sgnode);
    void applyAnimations(string filePath , int nodeIndex);
    void applySGRAnimations(string filePath, SGNode *sgNode, int &totalFrames , int currentFrame , int &animFrames);
    void applyTextAnimations(string filePath, SGNode *sgNode, int &totalFrames, int currentFrame , vector<Vector3> jointBasePositions, int &animFrames);
    bool removeAnimationForSelectedNodeAtFrame(int selectedFrame);
    void removeAppliedAnimation(int startFrame, int endFrame);
};

#endif /* SGAnimationManager_h */
