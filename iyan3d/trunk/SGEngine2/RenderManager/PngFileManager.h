//
// Created by shankarganesh on 12/9/15.
//

#ifndef IYAN3D_ANDROID_PNGFILEMANAGER_H
#define IYAN3D_ANDROID_PNGFILEMANAGER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "../libpng/png.h"

class PNGFileManager {
public:
    static uint8_t *read_png_file(const char* file_name, int &width , int &height);
    static void write_png_file(char *filename , uint8_t * inputData , int width , int height);
    static uint8_t* getImageDataFromVideo(std::string fileName, int frame,int &width, int &height);
};
#endif //IYAN3D_ANDROID_PNGFILEMANAGER_H