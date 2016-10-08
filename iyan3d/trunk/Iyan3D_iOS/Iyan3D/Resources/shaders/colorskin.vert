attribute vec3 vertPosition;
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform mat4 jointTransforms[jointsSize];
uniform mat4 mvp;

void main()
{
    
    mat4 finalMatrix = mat4(1.0);
    
    int jointId = int(optionalData1.x);
    if(jointId > 0)
        finalMatrix = jointTransforms[jointId - 1] * optionalData2.x;
    
    jointId = int(optionalData1.y);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData2.y);
    
    jointId = int( optionalData1.z);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData2.z);
    
    jointId = int( optionalData1.w);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData2.w);
    
    jointId = int( optionalData3.x);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.x);
    
    jointId = int( optionalData3.y);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.y);
    
    jointId = int( optionalData3.z);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.z);
    
    jointId = int( optionalData3.w);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.w);
    
    gl_Position = mvp * finalMatrix * vec4(vertPosition, 1.0);
    
}

