// Copyright (C) 2012 Harishankar Narayanan
// SGM Mesh loader

#include "IrrCompileConfig.h"

#ifndef __C_SGP_MESH_LOADER_H_INCLUDED__
#define __C_SGP_MESH_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "ISceneManager.h"
#include "IReadFile.h"

namespace irr {

namespace scene {

//! Meshloader for SGM format
class CSGPMeshFileLoader : public IMeshLoader {
public:

	//! Constructor
	CSGPMeshFileLoader(scene::ISceneManager* smgr);

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".bsp")
	virtual bool isALoadableFileExtension(const io::path& filename) const;

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file);

private:
	struct SSGPVectHeader {
		f32 vx, vy, vz;
	};

	scene::ISceneManager* SceneManager;
};


} // end namespace scene
} // end namespace irr

#endif // __C_SGP_MESH_LOADER_H_INCLUDED__

