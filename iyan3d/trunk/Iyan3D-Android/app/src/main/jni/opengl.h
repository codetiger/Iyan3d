//
// Created by shankarganesh on 12/9/15.
//

#ifndef IYAN3D_ANDROID_OPENGL_H
#define IYAN3D_ANDROID_OPENGL_H

#endif //IYAN3D_ANDROID_OPENGL_H

#include <jni.h>
#include <android/log.h>

//TODO es 3.0
//#define __gl2_h_
//#include <GLES3/gl3.h>
//#include <GLES2/gl2ext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)