//
// Created by shankarganesh on 22/9/15.
//

#ifndef IYAN3D_ANDROID_PREVIEW_SCENE_H
#define IYAN3D_ANDROID_PREVIEW_SCENE_H

#include <HeaderFiles/Constants.h>
#include "SceneManager/SceneManager.h"








void initpreviewScene(int width, int height);
void renderPreviewScene();
void DisplayAsset(int AssetId,string AssetName,int AssetType);
void removeCurrentObject();
void swipeRotate(float x, float y);
void swipeEnd();
void setViewType(VIEW_TYPE viewType);
void animationSelectionAssetItemToBeDisplayed(NODE_TYPE nodeType,int assetId,string assetName);
void animationSelectionTextItemToBeDisplayed(string text,int fontSize, float bevelValue,Vector3 color,string filePath);
void applyAnimationToNode(int assetid);
void playAnimation();
void stopAllAnimations();
void remove3DText();
void load3DText(string displayText,int fontSize,int bevelValue,int R,int G, int B,string filenmame);
void panBegin(float x1, float y1, float x2, float y2);
void panprogress(float x1, float y1, float x2, float y2);
void destroyScene();
bool isPreviewScene();

#endif //IYAN3D_ANDROID_PREVIEW_SCENE_H
