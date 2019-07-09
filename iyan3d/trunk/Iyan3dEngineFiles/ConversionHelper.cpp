//
//  ConversionHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 31/10/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/ConversionHelper.h"

std::wstring ConversionHelper::getWStringForString(string inputString)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
    std::wstring wideString = converter.from_bytes(inputString);
    
    return wideString;
}

string ConversionHelper::getStringForWString(std::wstring inputWString)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
    string narrowString = converter.to_bytes(inputWString);
    
    return narrowString;
    
}
