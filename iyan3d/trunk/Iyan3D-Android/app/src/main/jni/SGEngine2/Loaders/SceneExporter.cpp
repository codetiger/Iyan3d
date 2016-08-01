//
//  SceneExporter.cpp
//  Iyan3D
//
//  Created by sabish on 29/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "SceneExporter.h"

SceneExporter::SceneExporter()
{
    scene = NULL;
    exporter = new Assimp::Exporter();
}

SceneExporter::~SceneExporter()
{
    delete exporter;
}
