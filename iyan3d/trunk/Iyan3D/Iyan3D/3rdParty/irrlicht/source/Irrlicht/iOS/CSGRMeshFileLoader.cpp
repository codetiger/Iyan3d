// Copyright (C) 2013 Harishankar Narayanan
// SGR Rigged Mesh loader

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_SGR_LOADER_

#include "CSGRMeshFileLoader.h"
#include "CDynamicMeshBuffer.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "os.h"
#include "SMesh.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "IMeshManipulator.h"

namespace irr {
namespace scene {

//! Constructor
CSGRMeshFileLoader::CSGRMeshFileLoader(scene::ISceneManager* smgr) : SceneManager(smgr) {
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CSGRMeshFileLoader::isALoadableFileExtension(const io::path& filename) const {
	return core::hasFileExtension ( filename, "sgr" );
}


//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* CSGRMeshFileLoader::createMesh(io::IReadFile* f) {
	if (!f)
		return 0;

    u16 vertCount;
	f->read(&vertCount, sizeof(u16));

    ISkinnedMesh *animMesh = new CSkinnedMesh();
    SSkinMeshBuffer* mb = animMesh->addMeshBuffer();
	mb->Vertices_Standard.reallocate(vertCount);

	for(int i = 0; i < vertCount; i++) {
        SSGRVectHeader vert;
		f->read(&vert, sizeof(SSGRVectHeader));

		irr::video::S3DVertex vts;
		vts.Pos.X = vert.vx;
		vts.Pos.Y = vert.vy;
		vts.Pos.Z = vert.vz;
		vts.Normal.X = vert.nx;
		vts.Normal.Y = vert.ny;
		vts.Normal.Z = vert.nz;
        
        vts.TCoords.X = vert.s;
		vts.TCoords.Y = 1-vert.t;
		vts.Color = video::SColor(255, 255, 255, 255);
        
		mb->Vertices_Standard.push_back(vts);
    }

    u16 indCount;
	f->read(&indCount, sizeof(u16));
	mb->Indices.reallocate(indCount);
    for(int i = 0; i < indCount; i++) {
        u16 ind;
		f->read(&ind, sizeof(u16));
		mb->Indices.push_back(ind);
    }

	mb->recalculateBoundingBox();

    u16 boneCount;
	f->read(&boneCount, sizeof(u16));
    
    for (int i = 0; i < boneCount; i++) {
        s16 boneParentInd;
        f->read(&boneParentInd, sizeof(s16));
        
        f32 *boneMatrix = new f32[16];
        f->read(boneMatrix, 64);
      
        ISkinnedMesh::SJoint *ibone;
        if(boneParentInd >= 0)
            ibone = animMesh->addJoint(animMesh->getAllJoints()[boneParentInd]);
        else
            ibone = animMesh->addJoint();

        for (int j = 0; j < 16; ++j)
            ibone->LocalMatrix[j] = boneMatrix[j];
		delete boneMatrix;
        
        u16 boneWeightCount;
        f->read(&boneWeightCount, sizeof(u16));

        for (int j = 0; j < boneWeightCount; ++j) {
            u16 vertInd;
            f->read(&vertInd, sizeof(u16));
            u16 vertWeight;
            f->read(&vertWeight, sizeof(u16));
            
            ISkinnedMesh::SWeight *weight = animMesh->addWeight(ibone);
            weight->buffer_id = 0;
            weight->vertex_id = vertInd;
            weight->strength = (float)vertWeight / 255.0f;
        }
    }
    animMesh->finalize();
	return animMesh;
}

}

}

#endif // _IRR_COMPILE_WITH_SGR_LOADER_

