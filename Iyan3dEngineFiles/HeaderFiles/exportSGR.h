#ifndef __Iyan3D__exportSGR__
#define __Iyan3D__exportSGR__

#include <iostream>
#include "FileHelper.h"
#include "AutoRigJointsDataHelper.h"
#include "RigKey.h"
#include "AutoRigHelper.h"
#include "BoneLimitsHelper.h"

class exportSGR{
	
public:
	exportSGR();
	~exportSGR();
	static bool writeSGR(std::string filePath,shared_ptr<AnimatedMeshNode> node);
	static bool createSGR(std::string filePath,shared_ptr<MeshNode> node, std::map<int, RigKey> &rigKeys, std::map<int, vector<InfluencedObject> > &influencedVertices, std::map<int, vector<InfluencedObject> > &influencedJoints);
};
#endif /* defined(__Iyan3D__exportSGR__) */
