//
//  SGSceneWriter.h
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGSceneWriter_h
#define SGSceneWriter_h

#include "Constants.h"

#include "assimp/Exporter.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

class SGSceneWriter
{
private:
    SceneManager* smgr;

public:
    SGSceneWriter(SceneManager* smgr, void* scene);
    ~SGSceneWriter();
    
    void saveSceneData(std::string *filePath);
    void writeScene(ofstream *filePointer);
    void writeGlobalInfo(ofstream *filePointer);
    
};

#endif /* SGSceneWriter_h */
