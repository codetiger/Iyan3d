#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>
#include <ftoutln.h>
#include <fttrigon.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "glu.h"

#include "ftgl/FtglPoint.h"
#include "ftgl/Vectoriser.h"
#include "SkinMesh.h"

using namespace std;
typedef unsigned short u16;

class TextMesh3d {
public:
    static Vectoriser* generateVertices(FT_Face face, wchar_t ch, unsigned short bezierSteps, double strength = 0);
    static void AddCharacterSideFace(FT_Face face, wchar_t ch, unsigned short bezierSteps, double offset, float extrude, SkinMesh* mesh, double bevelRadius);
    static double AddCharacter(FT_Face face, wchar_t ch, unsigned short bezierSteps, double offset, float extrude, SkinMesh* mesh, double bevelRadius);
    static void AddBevel(FT_Face face, wchar_t ch, unsigned short bezierSteps, double offset, float extrude, SkinMesh* mesh, double bevelRadius, int bevelSegments, int height);
    static AnimatedMesh* getTextMesh(wstring text, u16 beizerSteps, float extrude, int height, char* filepath, Vector4 color, DEVICE_TYPE device, double bevelRadius, int bevelSegments);
};
