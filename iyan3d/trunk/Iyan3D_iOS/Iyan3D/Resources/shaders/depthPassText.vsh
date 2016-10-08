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
    
    gl_Position = mvp * finalMatrix * vec4(vertPosition, 1.0);
}

