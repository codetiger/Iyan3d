//
//  SGOBJManager.h
//  Iyan3D
//
//  Created by Karthik on 14/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef SGOBJManager_h
#define SGOBJManager_h

#include "Constants.h"
#include "SGNode.h"

class SGOBJManager
{
private:
    SceneManager * smgr;
    OBJMeshFileLoader *objLoader;
    
public :
    
    SGOBJManager(SceneManager* smgr, void * scene);
    ~SGOBJManager();
    
    bool loadAndSaveAsSGM(string objPath, string texturePath, int assetId, bool isVertexColor = false, Vector3 vColor = Vector3(1.0));
    bool writeSGM(string filePath, SGNode *objNode, bool hasUV = true);
 
};

#endif /* SGOBJManager_h */
