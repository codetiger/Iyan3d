//
//  FileHelper.h
//  Iyan3DEngine
//
//  Created by Karthi on 09/05/14.
//  Copyright (c) 2014 Smackall Games Pvt ltd. All rights reserved.
//

#ifndef __Iyan3DEngine__FileHelper__
#define __Iyan3DEngine__FileHelper__

#include <iostream>
#include <fstream>


using namespace std;

class FileHelper
{
    
public:
    
    static int seekPosition;
    static string documentsDirectory;
    FileHelper();
    static void resetSeekPosition();
    static void clearFolder();
    static int readInt(ifstream *file);
    static int readShort(ifstream *file);
    static float readFloat(ifstream *file);
    static bool readBool(ifstream *file);
    static std::string readString(ifstream *file, int sgbVersion = -1);
    static std::wstring readWString(ifstream *file);
    static void writeInt(ofstream *file , int data);
	static void writeShort(ofstream *file , short data);
    static void writeFloat(ofstream *file , float data);
    static void writeBool(ofstream *file , bool data);
    static void writeString(ofstream *file , std::string data);
    static void writeWString(ofstream *file , std::wstring data);
    static void printStatement(char* stringToPrint);
    static std::string getCachesDirectory();
    static std::string getDocumentsDirectory();
    static void setDocumentsDirectory(string documentPath);
    
    static char * getCharacterPointer(std::string &str);
};

#endif /* defined(__Iyan3DEngine__FileHelper__) */
