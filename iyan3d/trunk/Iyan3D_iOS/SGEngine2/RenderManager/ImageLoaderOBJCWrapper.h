//
//  MetalWrapper.h
//  SGEngine2
//
//  Created by Vivek on 09/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
#include<iostream>
#ifndef SGEngine2_ImageLoaderOBJCWrapper_h
#define SGEngine2_ImageLoaderOBJCWrapper_h
uint8_t* loadPNGImage(std::string filePath,int &width,int &height);
void writePNGImage(uint8_t *imageData , int width , int height , char* filePath);
#endif
