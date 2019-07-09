//
//  ConversionHelper.h
//  Iyan3D
//
//  Created by Karthik on 31/10/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef ConversionHelper_h
#define ConversionHelper_h

#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>

using namespace std;

class ConversionHelper
{
public:
    static std::wstring getWStringForString(string inputString);
    static string getStringForWString(std::wstring inputWString);
};

#endif /* ConversionHelper_h */
