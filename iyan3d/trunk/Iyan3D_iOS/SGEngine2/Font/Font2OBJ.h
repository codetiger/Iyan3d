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

struct Vector3df {
    float x, y, z;
};

struct Tri {
    Vector3df a, b, c;
};

class Font2OBJ {
public:
    map<string, int> vertMap;
    static float AddCharacter(FT_Face face, wchar_t ch, unsigned short bezierSteps, float offset, float extrude, SkinMesh* animMesh, float bevelRadius, int bevelSegments);
    static float AddCharacterSideFace(FT_Face face, wchar_t ch, unsigned short bezierSteps, float offset, float extrude, SkinMesh* mesh, float bevelRadius, int bevelSegments);
    static float AddBevel(FT_Face face, wchar_t ch, unsigned short bezierSteps, float offset, float extrude, SkinMesh* mesh, float bevelRadius, int bevelSegments);
    static AnimatedMesh* getTextMesh(wstring text, u16 beizerSteps, float extrude, int height, char* filepath, Vector4 color, DEVICE_TYPE device, float bevelRadius, int bevelSegments);
};
