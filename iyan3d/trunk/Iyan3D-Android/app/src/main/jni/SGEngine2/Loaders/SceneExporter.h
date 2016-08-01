//
//  SceneExporter.hpp
//  Iyan3D
//
//  Created by sabish on 29/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef SceneExporter_hpp
#define SceneExporter_hpp

#include <stdio.h>

#include "assimp/Exporter.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

class SceneExporter {
public:
    
    SceneExporter();
    ~SceneExporter();
    
private:
    
    const aiScene* scene = NULL;
    Assimp::Exporter *exporter = NULL;

};

#endif /* SceneExporter_hpp */
