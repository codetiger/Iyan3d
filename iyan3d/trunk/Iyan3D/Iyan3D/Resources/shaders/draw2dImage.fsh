//
//  Shader.fsh
//  SGEngine2
//
//  Created by Harishankar on 12/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

uniform sampler2D texture1;
varying lowp vec2 vTexCoord;
void main()
{
    lowp vec4 texColor = texture2D(texture1,vTexCoord.xy);
    gl_FragColor = vec4(texColor);
}
