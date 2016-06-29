#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "common.h"

struct SGRMaterial
{
    Vec3fa emissionColor;
    Vec3fa diffuse;
    float emission, transparency, reflection, refraction;
	float shadowDarkness;
    bool hasTexture, hasLighting;
    float reflectionSharpness;
    int lightType;
    Vec3fa lightDirection;
    bool isSmoothTexture;
};

#endif
