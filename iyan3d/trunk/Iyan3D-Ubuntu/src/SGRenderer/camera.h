#ifndef CAMERA_H_
#define CAMERA_H_

#include "common.h"

struct Camera {
    Vec3fa position, direction;
    float fovDist, aspectRatio;
    double imgWidth, imgHeight;
    Mat4 camMatrix;

    Camera(Vec3fa pos, Vec3fa cRot, Vec3fa cDir, float cfov, int width, int height) {
        position = pos;
        aspectRatio = width / (float)height;
        imgWidth = width;
        imgHeight = height;

        Vec3fa angle = Vec3fa(cRot.x, cRot.y, -cRot.z);
        camMatrix.setRotationRadians(angle * M_PI / 180.0f);

        direction = Vec3fa(0.0f, 0.0f, -1.0f);
        camMatrix.rotateVect(direction);

        fovDist = 2.0f * tan(M_PI * 0.5f * cfov / 180.0);
    }

    Vec3fa getRayDirection(float x, float y) {
        Vec3fa delta = Vec3fa((x-0.5f) * fovDist * aspectRatio, (y-0.5f) * fovDist, 0.0f);
        camMatrix.rotateVect(delta);
        return (direction + delta);
    }

    Vec3fa getRayDirection1(double x, double y) {
        Vec3fa delta = direction;
	    Mat4 cM;
        Vec3fa angle = Vec3fa((y-0.5) * 180.0f, x * 360.0f, 0.0f);
        cM.setRotationRadians(angle * M_PI / 180.0f);
        cM.rotateVect(delta);
        return delta;
    }
};

#endif