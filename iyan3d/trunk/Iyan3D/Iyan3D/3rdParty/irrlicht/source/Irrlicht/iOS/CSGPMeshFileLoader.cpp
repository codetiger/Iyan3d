// Copyright (C) 2012 Harishankar Narayanan
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// SGM Mesh loader

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_SGP_LOADER_

#include "CSGPMeshFileLoader.h"

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
CSGPMeshFileLoader::CSGPMeshFileLoader(scene::ISceneManager* smgr) : SceneManager(smgr) {
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CSGPMeshFileLoader::isALoadableFileExtension(const io::path& filename) const {
	return core::hasFileExtension ( filename, "sgp" );
}


//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* CSGPMeshFileLoader::createMesh(io::IReadFile* f) {
	if (!f)
		return 0;

	u16 vertCount;
	f->read(&vertCount, sizeof(u16));
	printf("c %d\n", vertCount);

	SMesh * sm = new SMesh();
	SMeshBuffer *smb = new SMeshBuffer();

	irr::video::S3DVertex* verts = new irr::video::S3DVertex[vertCount];
	for(int i = 0; i < vertCount; i++) {
		SSGPVectHeader vt;
		f->read(&vt, sizeof(SSGPVectHeader));
		verts[i].Pos.X = -vt.vx;
		verts[i].Pos.Y = vt.vy;
		verts[i].Pos.Z = vt.vz;
	}

	smb->append(verts, vertCount, NULL, 0);
	sm->addMeshBuffer(smb);
	SAnimatedMesh *animMesh = new SAnimatedMesh();
	animMesh->Type = EAMT_OBJ;
	animMesh->addMesh(sm);
	animMesh->recalculateBoundingBox();

	return animMesh;
}

}

}

#endif // _IRR_COMPILE_WITH_SGM_LOADER_

