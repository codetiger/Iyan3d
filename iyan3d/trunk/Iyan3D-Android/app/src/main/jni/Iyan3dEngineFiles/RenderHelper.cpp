//
//  RenderHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 24/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "RenderHelper.h"
#include "SGEditorScene.h"


void RenderHelper::drawGrid(SceneManager* smgr)
{
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    for (int i = -20; i <= 20; i+= 5) {
        smgr->draw3DLine(Vector3(i, 0, -20), Vector3(i, 0, 20), Vector3(0.6,0.6,1.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
        smgr->draw3DLine(Vector3(-20, 0, i), Vector3(20, 0, i), Vector3(0.6,0.6,1.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    smgr->draw3DLine(Vector3(-20, 0, 0), Vector3(20, 0, 0), Vector3(1.0,0.2,0.2),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    smgr->draw3DLine(Vector3(0, 0, -20), Vector3(0, 0, 20), Vector3(0.2,1.0,0.2),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
}
