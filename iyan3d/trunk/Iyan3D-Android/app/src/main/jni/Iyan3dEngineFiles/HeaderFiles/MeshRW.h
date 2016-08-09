//
//  MeshRW.h
//  Iyan3D
//
//  Created by Karthik on 04/08/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef MeshRW_h
#define MeshRW_h

#include "Constants.h"

class MeshRW
{
public:
    static void writeMeshData(ofstream *filePointer, Mesh* mesh);
    static void writeSkinMeshData(ofstream *filePointer, Mesh* mesh, shared_ptr< AnimatedMeshNode > aNode);
    static Mesh* readMeshData(ifstream *filePointer);
    static Mesh* readSkinMeshData(ifstream *filePointer);
};

#endif /* MeshRW_h */
