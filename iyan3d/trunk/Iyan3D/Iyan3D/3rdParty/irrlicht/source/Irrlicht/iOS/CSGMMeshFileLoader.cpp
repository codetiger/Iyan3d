	// Copyright (C) 2012 Harishankar Narayanan
	// This file is part of the "Irrlicht Engine".
	// For conditions of distribution and use, see copyright notice in irrlicht.h
	// SGM Mesh loader

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_SGM_LOADER_

#include "CSGMMeshFileLoader.h"
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
		CSGMMeshFileLoader::CSGMMeshFileLoader(scene::ISceneManager* smgr) : SceneManager(smgr) {
		}
		
		
			//! returns true if the file maybe is able to be loaded by this class
			//! based on the file extension (e.g. ".bsp")
		bool CSGMMeshFileLoader::isALoadableFileExtension(const io::path& filename) const {
			return core::hasFileExtension ( filename, "sgm" );
		}
		
		
			//! creates/loads an animated mesh from the file.
			//! \return Pointer to the created mesh. Returns 0 if loading failed.
			//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
			//! See IReferenceCounted::drop() for more information.
		IAnimatedMesh* CSGMMeshFileLoader::createMesh(io::IReadFile* f) {
			if (!f)
				return 0;
			
			u8 hasUV = 0;
			f->read(&hasUV, sizeof(u8));
			
			SSGMCountHeader counts;
			f->read(&counts, sizeof(SSGMCountHeader));
				//printf("c %d, %d, %d\n", counts.vertCount, counts.indCount, counts.uvCount);
			
			SMesh * sm = new SMesh();
			CDynamicMeshBuffer *mb = new CDynamicMeshBuffer(video::EVT_STANDARD, video::EIT_16BIT);
			mb->getVertexBuffer().reallocate(counts.vertCount);
			mb->getIndexBuffer().reallocate(counts.vertCount);
			mb->setHardwareMappingHint(EHM_STATIC);
			
			SSGMVectHeader* verts = new SSGMVectHeader[counts.vertCount];
			for(int i = 0; i < counts.vertCount; i++)
				f->read(&verts[i], sizeof(SSGMVectHeader));
			
			SSGMUVHeader* texs = new SSGMUVHeader[counts.colCount];
			SSGMColHeader* col = new SSGMColHeader[counts.colCount];
			if(hasUV == 1) {
				for(int i = 0; i < counts.colCount; i++)
					f->read(&texs[i], sizeof(SSGMUVHeader));
			} else if(hasUV == 2) {
				for(int i = 0; i < counts.colCount; i++)
					f->read(&col[i], sizeof(SSGMColHeader));
			}
			
			for(int i = 0; i < counts.indCount; i++) {
				SSGMIndexHeader ind;
				f->read(&ind, sizeof(SSGMIndexHeader));
				irr::video::S3DVertex vts;
				vts.Pos.X = -verts[ind.vtInd].vx;
				vts.Pos.Y = verts[ind.vtInd].vy;
				vts.Pos.Z = verts[ind.vtInd].vz;
				vts.Normal.X = -verts[ind.vtInd].nx;
				vts.Normal.Y = verts[ind.vtInd].ny;
				vts.Normal.Z = verts[ind.vtInd].nz;
				if(hasUV == 1) {
					vts.TCoords.X = texs[ind.colInd].s;
					vts.TCoords.Y = 1-texs[ind.colInd].t;
					vts.Color = video::SColor(255, 255, 255, 255);
				} else if(hasUV == 2) {
					vts.Color = irr::video::SColor(255, col[ind.colInd].r, col[ind.colInd].g, col[ind.colInd].b);
				}
				
				mb->getVertexBuffer().push_back(vts);
				mb->getIndexBuffer().push_back(i);
			}
			
			delete verts;
			delete texs;
			delete col;
			
			mb->recalculateBoundingBox();
			sm->addMeshBuffer(mb);
			sm->recalculateBoundingBox();
			mb->drop();
			SceneManager->getMeshManipulator()->flipSurfaces(sm);
			
			SAnimatedMesh *animMesh = new SAnimatedMesh();
			animMesh->Type = EAMT_OBJ;
			animMesh->addMesh(sm);
			sm->drop();
			animMesh->recalculateBoundingBox();
			
			return animMesh;
		}
		
	}
	
}

#endif // _IRR_COMPILE_WITH_SGM_LOADER_

