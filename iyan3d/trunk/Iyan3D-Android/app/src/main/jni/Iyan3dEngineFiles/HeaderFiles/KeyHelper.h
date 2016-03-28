#ifndef __iyan3dengine__KeyHelper__
#define __iyan3dengine__KeyHelper__

#include <vector>
#include <sstream>
#include "FileHelper.h"
#include "MathHelper.h"
#include "SGPositionKey.h"
#include "SGRotationKey.h"
#include "SGScaleKey.h"
#include "SGVisibilityKey.h"
#include <algorithm>

#ifdef ANDROID
#include "../../opengl.h"
#endif

class KeyHelper
{
    
private:
    
    template <typename T, typename T2>
    static int getKeyIndex(vector<T> &keys, T2 &id, int start, int end)
    {
        //returns the last index with frameId<= search frameId. Returns -1 if no such index exist.
        if(end < start){
            return -1;
        }
        int mid = (start+end)/2;
        if(keys[mid].id == id)
            return mid;
        else if(keys[mid].id > id)
            return getKeyIndex(keys, id, start, mid-1);
        else {
            if(mid == (int)keys.size()-1 || keys[mid+1].id > id)
                return mid;
            return getKeyIndex(keys,id,mid+1,end);
        }
    }
    
    
    
public:
    
    static void readData(ifstream* filePointer, int numKeys, vector<SGPositionKey> &, vector<SGRotationKey> &, vector<SGScaleKey> &, vector<SGVisibilityKey> &);
    
    static void writeData(ofstream* filePointer, vector<SGPositionKey> &, vector<SGRotationKey> &, vector<SGScaleKey> &, vector<SGVisibilityKey> &);
    
    template <typename T, typename T2>
    static int getKeyIndex(vector<T> &keys, T2 &id)
    {
        if(keys.size() > 0)
            return getKeyIndex(keys, id, 0, (int)keys.size()-1);
        else
            return -1;
    }
    
    template<typename T>
    static void addKey(vector<T> &keys, T &key){
        int keyIndex = getKeyIndex(keys, key.id);
            keys.insert(keys.begin() + keyIndex + 1, key);
    }
    
    
    template <typename T1,typename T2, typename T3>
    static T3 getKeyInterpolationForFrame(T1 frame,vector<T2> &keys, bool isRotationKey = false)
    {
        //Interpolates position, rotation and scale for any required frame. Interpolation is not required for visibiliy
        
        int keyIndex = getKeyIndex(keys, frame);
        if(keyIndex == -1){
            T3 junk;
            //if(keys.size()==0) printf("Keys array is empty");
            return junk;
        }
        
        if(keys[keyIndex].id == frame)                //Frame exist. No need of interpolation
            return keys[keyIndex].getValue();
        
        if(keyIndex+1 == (int)keys.size())               //Interpolation is not possible. Should return the last frame's value.
            return keys[keys.size()-1].getValue();
        
        //Interpolate
        if(isRotationKey){
            if((keys[keyIndex].getValue()).dotProduct(keys[keyIndex + 1].getValue()) <= 0)
                return (MathHelper::getLinearInterpolation(keys[keyIndex].id, keys[keyIndex].getValue() * (-1), keys[keyIndex + 1].id, keys[keyIndex + 1].getValue(), frame));
        }
        
        return (MathHelper::getLinearInterpolation(keys[keyIndex].id, keys[keyIndex].getValue(), keys[keyIndex + 1].id, keys[keyIndex + 1].getValue(), frame));
    }
};

#endif
