#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "common.h"

struct Material
{
    Vec3fa emissionColor;
    Vec3fa diffuse;
    float emission, shininess, transparency, reflection, refraction;
	float shadowDarkness;
    bool hasTexture;
    bool hasFaceNormals;
};

#endif
