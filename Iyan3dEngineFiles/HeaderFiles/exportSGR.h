#ifndef __Iyan3D__exportSGR__
#define __Iyan3D__exportSGR__

#include <iostream>
#include "FileHelper.h"
#include "RigKey.h"
#include "BoneLimitsHelper.h"

class exportSGR{
	
public:
	exportSGR();
	~exportSGR();
	static bool writeSGR(std::string filePath,shared_ptr<AnimatedMeshNode> node);
};
#endif /* defined(__Iyan3D__exportSGR__) */
