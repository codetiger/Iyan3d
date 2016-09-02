/*
Open Asset Import Library (assimp)
----------------------------------------------------------------------

Copyright (c) 2006-2016, assimp team
All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

/** @file  Text3DLoader.h
 *  @brief Declaration of the SGM importer class.
 */
#ifndef AI_TEXT3DLOADER_H_INCLUDED
#define AI_TEXT3DLOADER_H_INCLUDED

#include "BaseImporter.h"
#include <assimp/types.h>
#include <vector>
#include <stdint.h>
#include <assimp/Importer.hpp>

#include <ft2build.h>
#include <freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "glu.h"

#include "ftgl/FtglPoint.h"
#include "ftgl/Vectoriser.h"

using namespace std;
typedef unsigned short u16;

namespace Assimp    {

// ---------------------------------------------------------------------------
/** Importer class for the Quick3D Object and Scene formats.
*/
class Text3DImporter : public BaseImporter
{
public:
    Text3DImporter();
    ~Text3DImporter();

public:

    // -------------------------------------------------------------------
    /** Returns whether the class can handle the format of the given file.
    * See BaseImporter::CanRead() for details.  */
    bool CanRead( const std::string& pFile, IOSystem* pIOHandler,
        bool checkSig) const;

protected:

    // -------------------------------------------------------------------
    /** Return importer meta information.
     * See #BaseImporter::GetInfo for the details
     */
    const aiImporterDesc* GetInfo () const;

    void SetupProperties(const Importer* pImp);

    // -------------------------------------------------------------------
    /** Imports the given file into the given scene structure.
    * See BaseImporter::InternReadFile() for details
    */
    void InternReadFile( const std::string& pFile, aiScene* pScene, IOSystem* pIOHandler);

private:
    std::wstring text2Convert;
    std::string fontPath;
    bool shouldHaveBones;
    unsigned short bezierSteps;
    float extrude;
    double bevelRadius;
    int bevelSegments;

    double AddCharacter(FT_Face face, wchar_t ch, double offset);
    void AddBevel(FT_Face face, wchar_t ch, double offset);
    void AddCharacterSideFace(FT_Face face, wchar_t ch, double offset);
    Vectoriser* generateVertices(FT_Face face, wchar_t ch, double strength = 0);

    struct SSGRVectHeader {
        float vx, vy, vz, nx, ny, nz, s, t;
    };

    struct SSGMCountHeaderLowPoly {
        unsigned short vertCount, indCount, colCount;
    };

    struct SSGMCountHeaderHighPoly {
        unsigned int vertCount, indCount, colCount;
    };

    struct SSGMVectHeader {
        float vx, vy, vz, nx, ny, nz;
    };

    struct SSGMUVHeader {
        float s, t;
    };

    struct SSGMIndexHeaderLowPoly {
        unsigned short vtInd, colInd;
    };

    struct SSGMIndexHeaderHighPoly {
        unsigned int vtInd, colInd;
    };

    struct SSGMColHeader {
        unsigned char r, g, b;
    };

#define UV_MAPPED 1
#define VERTEX_COLORED 2


    struct Material
    {
        Material()
            :   diffuse         (0.6f,0.6f,0.6f)
            ,   transparency    (0.f)
            ,   texIdx          (UINT_MAX)
        {}

        aiString name;
        aiColor3D ambient, diffuse, specular;
        float transparency;

        unsigned int texIdx;
    };

    struct Face
    {
        explicit Face(unsigned int s)
            :   indices   (s)
            ,   uvindices (s)
            ,   mat       (0)
        {
        }

        std::vector<unsigned int> indices;
        std::vector<unsigned int> uvindices;
        unsigned int mat;
    };

    struct Mesh
    {

        std::vector<aiVector3D> verts;
        std::vector<aiVector3D> normals;
        std::vector<aiVector3D> uv;
        std::vector<Face>       faces;

        uint32_t prevUVIdx;
    };
};

} // end of namespace Assimp

#endif // AI_Text3DImporter_H_IN
