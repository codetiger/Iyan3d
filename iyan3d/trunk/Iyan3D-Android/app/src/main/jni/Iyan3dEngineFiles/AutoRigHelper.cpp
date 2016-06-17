#include "HeaderFiles/AutoRigHelper.h"

vector< set<int> > adjacentVertices;
std::map<int, Mat4> envelopeMatrices;

void initAdjacencyMatrix(int verticesCount, int indicesCount, u32* indices){
    adjacentVertices.clear();
    set<int> emptySet;
    emptySet.clear();
    for(int i=0; i<verticesCount; i++)
        adjacentVertices.push_back(emptySet);
    
    for(int i=0; i < indicesCount - 2; i+=3){
        adjacentVertices[indices[i]].insert(indices[i+1]);
        adjacentVertices[indices[i]].insert(indices[i+2]);
        adjacentVertices[indices[i+1]].insert(indices[i]);
        adjacentVertices[indices[i+1]].insert(indices[i+2]);
        adjacentVertices[indices[i+2]].insert(indices[i]);
        adjacentVertices[indices[i+2]].insert(indices[i+1]);
    }
}

void initUnmatchedWeights(int verticesCount,std::map<int,bool> &isWeighted, std::map<int, vector<InfluencedObject> > &influencedVertices, std::map<int, vector<InfluencedObject> > &influencedJoints){
    queue < pair<int, int> > unmatchedVertices;
    bool isVisited[verticesCount+5];
    memset(isVisited, false, sizeof(isVisited));
    
    //marking all weighted vertices as visited
    for(std::map<int,bool> :: iterator it = isWeighted.begin(); it!=isWeighted.end(); it++)
        isVisited[it->first] = isWeighted[it->second];
    
    //Adding level1 unweighted vertices along with it's parent vertexId to the queue
    for(int i=0; i < verticesCount; i++)
        if(isWeighted[i]){
            for(set<int>:: iterator it = adjacentVertices[i].begin(); it != adjacentVertices[i].end(); it++)
                if(!isWeighted[*it] && !isVisited[*it]){
                    isVisited[*it] = true;
                    unmatchedVertices.push(make_pair(*it, i));
                    isWeighted.erase(*it);  //Erasing unweighted vertices from map just to make isWeighted.size() = number of weighted vertices
                }
        }
        else isWeighted.erase(i);
    
    //Level order traversal/BFS on unmatchedVertices
    while(unmatchedVertices.size() > 0){
        int levelSize = (int)unmatchedVertices.size();
        for(int i=0; i < levelSize; i++){
            pair<int, int> vertex = unmatchedVertices.front();
            //copying weightPaints from parent
            influencedJoints[vertex.first] = influencedJoints[vertex.second];
            for(int j=0; j<influencedJoints[vertex.first].size(); j++){
                InfluencedObject influencedVertex;
                influencedVertex.id = vertex.first;
                influencedVertex.weight = influencedJoints[vertex.first][j].weight;
                influencedVertices[ influencedJoints[vertex.first][j].id ].push_back(influencedVertex);
            }
            isWeighted[vertex.first]=true;
            //add adjacent unweighted, unvisited vertices to the queue
            for(set<int>::iterator it = adjacentVertices[vertex.first].begin(); it != adjacentVertices[vertex.first].end(); it++)
                if(!isVisited[*it]){
                    isVisited[*it] = true;
                    unmatchedVertices.push(make_pair(*it, vertex.first));
                }
            //removing the first vertex from the queue
            unmatchedVertices.pop();
        }
    }
}

bool isInsideTruncatedCone(Vector3 point, float baseRadius, float topRadius, float height){
    if(point.y > height || point.y < 0.0)
        return false;
    float nearRadius = MathHelper::getLinearInterpolation(0.0, baseRadius, height,  topRadius, point.y);
    
    return (point.x*point.x + point.z * point.z) <= (nearRadius*nearRadius);
}

bool isInsideSphere(Vector3 point, float radius){
    return (point.x*point.x+point.y*point.y+point.z*point.z) <= (radius*radius);
}

bool isInsideHemiSphere(Vector3 point, Vector3 sphereOrigin, Vector3 sphereTip){
    float dy = sphereTip.y - sphereOrigin.y;
    if((point.y - sphereOrigin.y) * dy < 0.0)
        return false;
    return isInsideSphere(point,dy);
}

bool isInsideEnvelope(Vector3 point, float baseRadius, float topRadius, float height){
    if(isInsideTruncatedCone(point, baseRadius, topRadius, height))
        return true;
    if(isInsideHemiSphere(point, Vector3(0.0), Vector3(0.0,-baseRadius,0.0)))
        return true;
    if(isInsideHemiSphere(Vector3(point.x, point.y - height, point.z), Vector3(0.0), Vector3(0.0,topRadius,0.0)))
        return true;
    return false;
}


float getInterpolatedWeight(Vector3 point, float baseRadius, float topRadius, float height){
    if(!isInsideEnvelope(point,baseRadius,topRadius,height))
        return 0.0;
    Vector3 envelopeBase = Vector3(0.0,-baseRadius,0.0);
    Vector3 envelopeTop = Vector3(0.0,height+topRadius,0.0);
    Vector3 envelopeMidPoint = (envelopeBase+envelopeTop)/2.0;
    point.y += baseRadius;      //point with respect to envelopeBase
    if(point.y < envelopeMidPoint.y)
        return MathHelper::getLinearInterpolation(envelopeMidPoint.y, 1.0, envelopeBase.y, 0.0, point.y);
    else
        return MathHelper::getLinearInterpolation(envelopeMidPoint.y, 1.0, envelopeTop.y, 0.0, point.y);
}

void initEnvelopeMatrices(std::map<int,RigKey> &rigKeys){
    envelopeMatrices.clear();
    for(std::map<int, RigKey>::iterator it = rigKeys.begin(); it!=rigKeys.end(); it++){
        int parentId = it->second.parentId;
        if(parentId == -1) continue;
        Mat4 envelopeMatrix;
        envelopeMatrix.translate(rigKeys[parentId].referenceNode->node->getAbsolutePosition());
        Vector3 currentDir = ENVELOPE_BASE_DIRECTION;
        Vector3 targetDir = (rigKeys[it->first].referenceNode->node->getAbsolutePosition() - rigKeys[parentId].referenceNode->node->getAbsolutePosition() ).normalize();
        envelopeMatrix.setRotationRadians((MathHelper::toEuler(MathHelper::rotationBetweenVectors(targetDir,currentDir))));
        envelopeMatrices[it->first] = envelopeMatrix;
    }
}

bool isInsideEnvelope(Vector3 vertexGlobalPosition, int jointId, std::map<int, RigKey> &rigKeys){
    int parentId = rigKeys[jointId].parentId;
    Vector3 vertexLocalPosition = MathHelper::getRelativePosition(envelopeMatrices[jointId], vertexGlobalPosition);
    
    float baseRadius = rigKeys[parentId].envelopeRadius;
    float topRadius = rigKeys[jointId].envelopeRadius;
    float height = rigKeys[jointId].referenceNode->node->getAbsolutePosition().getDistanceFrom(rigKeys[parentId].referenceNode->node->getAbsolutePosition());
    
    return isInsideEnvelope(vertexLocalPosition,baseRadius,topRadius,height);
}

void AutoRigHelper::updateOBJVertexColors(shared_ptr<MeshNode> objNode, std::map<int, SGNode*> &envelopes, std::map<int, RigKey> &rigKeys,int selectedJointId, int mirrorJointId){
    
    
    initEnvelopeMatrices(rigKeys);
    int verticesCount = objNode->getMesh()->getVerticesCount();
    for(int i = 0; i < verticesCount; i++){
        vertexData *vertex = objNode->getMesh()->getLiteVertexByIndex(i);
        vertex->optionalData1 = Vector4(255/255.0,0,0,0.0);
    }
    for(int i = 0; i < verticesCount; i++){
        vertexData *vertex = objNode->getMesh()->getLiteVertexByIndex(i);
        for(std::map<int, SGNode *>::iterator it = envelopes.begin(); it!=envelopes.end(); it++)
        {
            if(it->first == selectedJointId || selectedJointId == rigKeys[it->first].parentId){
                if(isInsideEnvelope(getVertexGlobalPosition(vertex->vertPosition, objNode),it->first, rigKeys)){
                    vertex->optionalData1 = Vector4(0, 0, 255/255.0,0.0);
                    break;
                }
            }
            
            if(mirrorJointId != NOT_EXISTS && (it->first == mirrorJointId || mirrorJointId == rigKeys[it->first].parentId)){
                if(isInsideEnvelope(getVertexGlobalPosition(vertex->vertPosition, objNode),it->first, rigKeys)){
                    vertex->optionalData1 = Vector4(0, 0, 255/255.0,0.0);
                    break;
                }
            }
        }
    }
    //objNode.reset();
}


float getWeight(Vector3 &vertexGlobalPosition, int jointId, std::map<int, RigKey> &rigKeys){
    int parentId = rigKeys[jointId].parentId;
    if(parentId <= 0)       //Ignoring weights for pivot joint.
        return 0.0;
    
    Vector3 vertexLocalPosition = MathHelper::getRelativePosition(envelopeMatrices[jointId], vertexGlobalPosition);
    
    float baseRadius = rigKeys[parentId].envelopeRadius;
    float topRadius = rigKeys[jointId].envelopeRadius;
    float height = rigKeys[jointId].referenceNode->node->getAbsolutePosition().getDistanceFrom(rigKeys[parentId].referenceNode->node->getAbsolutePosition());
    
    float weight = getInterpolatedWeight(vertexLocalPosition, baseRadius, topRadius, height);
    return weight;
}


void AutoRigHelper::initWeights(shared_ptr<MeshNode> meshSceneNode, std::map<int, RigKey> &rigKeys, std::map<int, vector<InfluencedObject> > &influencedVertices, std::map<int, vector<InfluencedObject> > &influencedJoints){
    
    influencedJoints.clear();
    influencedVertices.clear();
    if(!meshSceneNode)
        return;
    
    std::map<int,bool> isWeighted;
    Mesh* mesh = meshSceneNode->getMesh();
    
    if(mesh == NULL)
        return;
    
    initEnvelopeMatrices(rigKeys);
    
    int verticesCount = mesh->getVerticesCount();
    vertexData* vertices = (vertexData*)mesh->getLiteVertexByIndex(0);
    
    for(int j=0; j < verticesCount; j++){
        vertexData* vertex = mesh->getLiteVertexByIndex(j);
        float totalWeight = 0.0;
        //printf("\n vertexPositionBefore %f %f %f ",vertex.Pos.x,vertex.Pos.y,vertex.Pos.z);
        Vector3 vertexPosition = getVertexGlobalPosition(vertex->vertPosition,meshSceneNode);
        for(std::map<int, RigKey>::iterator it=rigKeys.begin();it!=rigKeys.end();it++){
            float weight = getWeight(vertexPosition, it->first, rigKeys);
            if(weight > 0.0){
                totalWeight += weight;
                InfluencedObject influencedJoint;
                influencedJoint.id = it->second.parentId;
                influencedJoint.weight = weight;
                influencedJoints[j].push_back(influencedJoint);
            }
        }
        
        //cout<<"vertex "<<j<<" influenced by "<<influencedJoints[j].size()<<" joints"<<endl;
        
        for(int k=0; k<influencedJoints[j].size(); k++){
            influencedJoints[j][k].weight /= totalWeight;
            InfluencedObject influencedVertex;
            influencedVertex.id = j;
            influencedVertex.weight = influencedJoints[j][k].weight;
            influencedVertices[influencedJoints[j][k].id].push_back(influencedVertex);
        }
        
        if(influencedJoints[j].size() != 0)
            isWeighted[j]=true;
    }
    int indicesCount =  mesh->getTotalIndicesCount();
    u32* indices =  mesh->getHighPolyIndicesArray();
    initAdjacencyMatrix(verticesCount, indicesCount, indices);
    initUnmatchedWeights(verticesCount,isWeighted, influencedVertices, influencedJoints);
    int weightedCount = (int)isWeighted.size();
    int unweightedCount = verticesCount - weightedCount;
    
    if(weightedCount !=0 && (long long)weightedCount*(long long)unweightedCount < (long long)5e7){
        //assign vertices to nearest weighted vertex
        vector<int> weightedVertices;
        vector<int> unweightedVertices;
        float dist;
        for(int i=0; i < verticesCount; i++){
            if(isWeighted[i])
                weightedVertices.push_back(i);
            else{
                unweightedVertices.push_back(i);
                isWeighted.erase(i);
            }
        }
        for(int i=0; i < unweightedCount; i++){
            int unweightedVertex = unweightedVertices[i];
            int closestVertex = -1;
            float shortestDistance = 1e9;
            for(int j=0; j < weightedCount; j++){
                dist = getVertexGlobalPosition(vertices[unweightedVertex].vertPosition, meshSceneNode).getDistanceFrom(getVertexGlobalPosition(vertices[weightedVertices[j]].vertPosition, meshSceneNode));
                if(shortestDistance > dist){
                    shortestDistance = dist;
                    closestVertex = weightedVertices[j];
                }
            }
            
            influencedJoints[unweightedVertex] = influencedJoints[closestVertex];
            for(int k=0; k<influencedJoints[unweightedVertex].size(); k++){
                InfluencedObject influencedVertex;
                influencedVertex.id = unweightedVertex;
                influencedVertex.weight = influencedJoints[unweightedVertex][k].weight;
                influencedVertices[ influencedJoints[unweightedVertex][k].id ].push_back(influencedVertex);
            }
            isWeighted[unweightedVertex]=true;
        }
        
    }
    else{
        //assign vertices to hip
        for(int i=0; i<verticesCount; i++)
            if(!isWeighted[i])
            {
                InfluencedObject influencedJoint ;
                influencedJoint.id = 1; //hip joint
                influencedJoint.weight = 1.0;
                influencedJoints[i].push_back(influencedJoint);
                
                InfluencedObject influencedVertex;
                influencedVertex.id = i;
                influencedVertex.weight = 1.0;
                influencedVertices[1].push_back(influencedVertex);
            }
    }
    //meshSceneNode.reset();
}


Vector3 AutoRigHelper::getVertexGlobalPosition(Vector3 vertexPos,shared_ptr<MeshNode> meshSceneNode){
    Mat4 vertexMat;
    vertexMat.translate(vertexPos);
    vertexMat = meshSceneNode->getAbsoluteTransformation() * vertexMat;
    //meshSceneNode.reset();
    
    return vertexMat.getTranslation();
}

