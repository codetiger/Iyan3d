//
//  Shader.vsh
//  SGEngine2
//
//  Created by Harishankar on 12/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec2 texCoord1;

varying vec2 vTexCoord;

void main()
{
    vTexCoord = texCoord1;
    gl_Position = vec4(vec3(vertPosition),1.0);
}

