#ifndef Iyan3D_AutoRigWeightsHelper_h
#define Iyan3D_AutoRigWeightsHelper_h

#include "MathHelper.h"
#include "../../SGEngine2/Utilities/Logger.h"
#include "RigKey.h"
#include <map>
#include <set>
#include <queue>
#include <vector>
using namespace std;

struct InfluencedObject{
    int id;
    float weight;
    InfluencedObject(){
        id = -1;
        weight = 0.0;
    }
};


class AutoRigHelper{
    
public:
    static void updateOBJVertexColors(shared_ptr<MeshNode> mesh, std::map<int, SGNode*> &envelopes, std::map<int, RigKey> &rigKeys,int selectedJointId = NOT_EXISTS, int mirrorJointId = NOT_EXISTS);
    static void initWeights(shared_ptr<MeshNode> meshSceneNode, std::map<int, RigKey> &rigKeys, std::map<int, vector<InfluencedObject> > &influencedVertices, std::map<int, vector<InfluencedObject> > &influencedJoints);
    static Vector3 getVertexGlobalPosition(Vector3 vertexPos,shared_ptr<MeshNode> meshSceneNode);
};

#endif