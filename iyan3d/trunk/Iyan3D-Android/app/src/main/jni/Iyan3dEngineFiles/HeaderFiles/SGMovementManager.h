//
//  SGMovementManager.h
//  Iyan3D
//
//  Created by Karthik on 27/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGMovementManager_h
#define SGMovementManager_h

#define CAM_SWIPE_SPEED 20.0
#define CAM_ZOOM_SPEED 30.0
#define CAM_PAN_SPEED 30.0
#define CAM_ZOOM_IN_LIMIT 5.0
#define CAM_UPVEC_UPREAL_MAX_DIF 0.80
#define CAM_PREV_PERCENT 0.18
#define CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE 1.5
#define CAM_PREV_BORDER_WIDTH 0.01

#include "Constants.h"

class SGMovementManager
{
private:
    int swipeTiming;
    float xAcceleration, yAcceleration;
    Vector2 panBeganPoints[2];
    Vector2 prevTouchPoints[2];
    Vector3 previousTarget;
    float previousRadius;
    SceneManager *smgr;

    
public:
    SGMovementManager(SceneManager* smgr, void* scene);
    ~SGMovementManager();
    
    /* Swipe functions */
    
    void touchBegan(Vector2 curTouchPos);
    void swipeProgress(float angleX , float angleY);
    void swipeToRotate();
    
    /* Pan functions */
    void panBegan(Vector2 touch1, Vector2 touch2);
    void panProgress(Vector2 touch1, Vector2 touch2);
    
    void touchEnd(Vector2 curTouchPos);
};

#endif /* SGMovementManager_h */
