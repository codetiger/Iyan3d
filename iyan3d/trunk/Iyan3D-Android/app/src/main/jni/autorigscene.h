//
// Created by shankarganesh on 20/10/15.
//


#ifndef IYAN3D_ANDROID_AUTORIGSCENE_H
#define IYAN3D_ANDROID_AUTORIGSCENE_H

#include <Constants.h>
#include <jni.h>

#include <jni.h>
#include "SceneManager/SceneManager.h"

bool initAutorig(JNIEnv *env, jclass type,int width, int height);

bool renderAutoRig();
void addObjToScene(string objPath,string texturePath);
void panBeginAutorig(float x1, float y1, float x2, float y2);
void panProgressAutorig(float cordX1, float cordY1,float cordX2, float cordY2);
void swipe(float velocityX, float velocityY);
void tap(Vector2 tapPosition);
void rotateaction();
void moveAction();
void ownRigging();
void humanRigging();
void checkcontrolSelection(Vector2 coords);
void  tapAutoRig(Vector2 tapPositionAutorig,JNIEnv *env, jclass type);
void tapBegin(float x , float y,Vector2 coords, float width, float height);
void tapEnd(float x , float y);
void nextBtnAction(string cacheDir);
void ScaleAutoRig(float x,bool state);
float getSelectedJointScale();
void moveToPrevMode();
void undoFunction();
void redoFunction();
void topViewAutoRig();
void bottomViewAutoRig();
void leftViewAutoRig();
void rightViewAutoRig();
void frontViewAutoRig();
void backViewAutoRig();
void ScaleboneAutoRig(float x,float y,float z,bool state);
void addNewJoint();
void deleteAutoRigScene();
void boneLimitCallbackInit();
bool isJointSelected();
bool updateMirror();
Vector3 getSelectedNodeScale();
int getSceneMode();
int getskeletonType();
int getselectedNodeId();
string getUndoOrRedoButtonAction();
string getRedoButtonAction();




#endif //IYAN3D_ANDROID_AUTORIGSCENE_H
