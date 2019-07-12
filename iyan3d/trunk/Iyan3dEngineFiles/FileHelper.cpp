 //
//  FileHelper.cpp
//  Iyan3DEngine
//
//  Created by Karthi on 09/05/14.
//  Copyright (c) 2014 Smackall Games Pvt ltd. All rights reserved.
//


#include "HeaderFiles/FileHelper.h"
#include "HeaderFiles/Constants.h"

using namespace std;
int FileHelper::seekPosition = 0;
string FileHelper::documentsDirectory = "";

FileHelper::FileHelper()
{
    
}

void FileHelper::clearFolder()
{
    
}

void FileHelper::printStatement(char* stringToPrint)
{
    cout<<stringToPrint;
}

void FileHelper::resetSeekPosition(){
    seekPosition = 0;
}

unsigned int FileHelper::readUnsignedInt(ifstream *file)
{
    unsigned int intRead;
    file->read((char*)&intRead, sizeof(unsigned int));
    seekPosition += sizeof(unsigned int);
    return intRead;
}

int FileHelper::readInt(ifstream *file)
{
    int intRead;
    file->read((char*)&intRead, sizeof(int));
    seekPosition += sizeof(int);
    return intRead;
}

int FileHelper::readShort(ifstream *file)
{
    short shortRead;
    file->read((char*)&shortRead, sizeof(short));
    seekPosition += sizeof(short);
    return shortRead;
}

float FileHelper::readFloat(ifstream *file)
{
    float floatRead;
    file->read((char*)&floatRead, sizeof(float));
    seekPosition += sizeof(float);
    return floatRead;
}

bool FileHelper::readBool(ifstream *file)
{
    bool boolRead;
    file->read((char*)&boolRead, sizeof(bool));
    seekPosition += sizeof(bool);
    return boolRead;
}

std::string FileHelper::readString(ifstream *file,int sgbVersion)
{
    int stringSize;
    if(sgbVersion >= SGB_VERSION_1)
        stringSize = FileHelper::readInt(file);
    else
        stringSize = 128;
    
    if (stringSize <= 0)
        return "";
    
    char dataToRead[stringSize+1];
    
    file->read(dataToRead, stringSize);
    dataToRead[stringSize] = '\0';
    std::string stringRead(dataToRead);
    seekPosition += stringSize;
    return stringRead;
}

std::wstring FileHelper::readWString(ifstream *file)
{
    int stringSize = FileHelper::readInt(file);
    
    if (stringSize <= 0)
        return L"";
    
    std::wstring stringRead;
    
    for (int i = 0; i < stringSize; i++) {
        stringRead += (wchar_t)readShort(file);
    }
    //seekPosition += stringSize;
    
    return stringRead;
}

void FileHelper::writeUnsignedInt(ofstream *file, unsigned int data)
{
    file->write((char*)&data, sizeof(unsigned int));
}

void FileHelper::writeInt(ofstream *file , int data)
{
    file->write((char*)&data, sizeof(int));
}

void FileHelper::writeShort(ofstream *file , short data)
{
    file->write((char*)&data, sizeof(short));
}

void FileHelper::writeFloat(ofstream *file , float data)
{
    file->write((char*)&data, sizeof(float));
}

void FileHelper::writeBool(ofstream *file , bool data)
{
    file->write((char*)&data, sizeof(bool));
}

void FileHelper::writeString(ofstream *file , std::string data)
{
    int len = (int)data.size();
    writeInt(file, len); // Storing the string size in new sgb version
    
    if(len == 0)
        return;
    char *dataToWrite = new char[len];
    std::copy(data.begin(), data.end(), dataToWrite);
    file->write(dataToWrite,len);
    delete[] dataToWrite;
}

void FileHelper::writeWString(ofstream *file , std::wstring data)
{
    int len = (int)data.length();
    writeInt(file, len); // Storing the string size in new sgb version
    
    if(len == 0)
        return;
    for (int i = 0; i < len; i++) {
        writeShort(file, (short)data[i]);
    }
}

void FileHelper::writeVector4(ofstream *file, Vector4 value)
{
    writeFloat(file, value.x);
    writeFloat(file, value.y);
    writeFloat(file, value.z);
    writeFloat(file, value.w);
}

void FileHelper::writeVector3(ofstream *file, Vector3 value)
{
    writeFloat(file, value.x);
    writeFloat(file, value.y);
    writeFloat(file, value.z);
}

void FileHelper::writeVector2(ofstream *file, Vector2 value)
{
    writeFloat(file, value.x);
    writeFloat(file, value.y);
}


Vector4 FileHelper::readVector4(ifstream *file)
{
    Vector4 value;
    value.x = readFloat(file);
    value.y = readFloat(file);
    value.z = readFloat(file);
    value.w = readFloat(file);
    
    return value;
}

Vector3 FileHelper::readVector3(ifstream *file)
{
    Vector3 value;
    value.x = readFloat(file);
    value.y = readFloat(file);
    value.z = readFloat(file);
    
    return value;
}

Vector2 FileHelper::readVector2(ifstream *file)
{
    Vector2 value;
    value.x = readFloat(file);
    value.y = readFloat(file);
    
    return value;
}

std::string FileHelper::getFontsDirectory()
{
    return FileHelper::getDocumentsDirectory() + "Resources/Fonts/";
}


std::string FileHelper::getTexturesDirectory()
{
    return FileHelper::getDocumentsDirectory() + "Resources/Textures/";
}

std::string FileHelper::getCachesDirectory()
{
	std::string home = getenv("HOME");
	std::string caches = "/Library/Caches/";
	std::string cachesPath = home + caches;
	return cachesPath;
}

std::string FileHelper::getDocumentsDirectory()
{
    std::string home = getenv("HOME");
    std::string documents = "/Documents/";
    std::string documentsPath = home + documents;
    return documentsPath;
}

void FileHelper::setDocumentsDirectory(string documentsPath)
{
    FileHelper::documentsDirectory = documentsPath;
}

char * FileHelper::getCharacterPointer(std::string &str)
{
    return strcpy((char*)malloc(str.length()+1), str.c_str());
}

bool FileHelper::checkFileExists(std::string fileName)
{
     if(FILE *file = fopen(fileName.c_str(), "r"))
     {
         fclose(file);
         return true;
     }else {
         return false;
     }
 }
