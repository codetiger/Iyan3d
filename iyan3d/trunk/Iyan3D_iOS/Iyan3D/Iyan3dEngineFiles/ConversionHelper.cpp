//
//  ConversionHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 31/10/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "ConversionHelper.h"

wstring ConversionHelper::getWStringForString(string inputString)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
    wstring wideString = converter.from_bytes(inputString);
    
    return wideString;
}

string ConversionHelper::getStringForWString(wstring inputWString)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
    string narrowString = converter.to_bytes(inputWString);
    
    return narrowString;

}