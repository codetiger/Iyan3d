attribute vec3 vertPosition;
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform mat4 jointTransforms[57];
uniform mat4 mvp;

void main()
{
    vec4 pos = vec4(0.0);
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    else
        pos = vec4(vertPosition,1.0);

    jointId = int(optionalData1.y);
    strength = optionalData2.y;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    jointId = int( optionalData1.z);
    strength = optionalData2.z;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    jointId = int( optionalData1.w);
    strength = optionalData2.w;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    jointId = int( optionalData3.x);
    strength = optionalData4.x;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    jointId = int( optionalData3.y);
    strength = optionalData4.y;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    jointId = int( optionalData3.z);
    strength = optionalData4.z;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    jointId = int( optionalData3.w);
    strength = optionalData4.w;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    
    gl_Position = (mvp) * pos;
}

