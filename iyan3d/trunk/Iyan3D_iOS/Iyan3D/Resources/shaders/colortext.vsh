attribute vec3 vertPosition;
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform mat4 jointTransforms[161];
uniform mat4 mvp;

void main()
{
    vec4 pos = vec4(0.0);
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0)
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition, 1.0)) * strength;
    else
        pos = vec4(vertPosition,1.0);
    
    gl_Position = (mvp) * pos;
}

