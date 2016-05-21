//
//  exportSGR.cpp
//  Iyan3D
//
//  Created by vivek on 6/11/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#include "HeaderFiles/exportSGR.h"

exportSGR::exportSGR()
{
	
}
exportSGR::~exportSGR()
{
	
}
bool exportSGR::writeSGR(std::string filePath,shared_ptr<AnimatedMeshNode> node)
{
    ofstream filePointer(filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    
    SkinMesh *skinnedMesh = (SkinMesh*)node->getMesh();
    Mesh *mesh = node->getMesh();
    unsigned int vertexCount = mesh->getVerticesCount();
    u16 versionIdentifier = 0; // 0 for HighPoly models
    FileHelper::writeShort(&filePointer, versionIdentifier);// write VertexCount
    FileHelper::writeInt(&filePointer, vertexCount);// write VertexCount
    for(int i = 0; i < vertexCount; i++){
        vertexData *vertex = mesh->getLiteVertexByIndex(i);
        Vector3 vertexPosition = AutoRigHelper::getVertexGlobalPosition(vertex->vertPosition,node);
        // write vertex position,normal,UV Coords
        FileHelper::writeFloat(&filePointer,vertexPosition.x);
        FileHelper::writeFloat(&filePointer,vertexPosition.y);
        FileHelper::writeFloat(&filePointer,vertexPosition.z);
        FileHelper::writeFloat(&filePointer,vertex->vertNormal.x);
        FileHelper::writeFloat(&filePointer,vertex->vertNormal.y);
        FileHelper::writeFloat(&filePointer,vertex->vertNormal.z);
        FileHelper::writeFloat(&filePointer,vertex->texCoord1.x);
        FileHelper::writeFloat(&filePointer,1 - vertex->texCoord1.y);
    }
    
    
    unsigned int indicesCount = mesh->getTotalIndicesCount();
    FileHelper::writeInt(&filePointer,indicesCount); // write IndexCount
    
    u32 *indicesArr = mesh->getHighPolyIndicesArray();
    for(int k = 0; k < indicesCount; k++){
        unsigned int index = indicesArr[k];
        FileHelper::writeInt(&filePointer,index);	// write Vertex Index
    }
    
	unsigned short boneCount = node->getJointCount();
    FileHelper::writeShort(&filePointer,boneCount);	// write boneCount
    
	for(int j = 0; j < boneCount; j++){
		if(j == 0)
			FileHelper::writeShort(&filePointer,-1); // write -1 for pivot bone
		else if(j == 1)
			FileHelper::writeShort(&filePointer,0); // write 0 for hip bone
		else
			FileHelper::writeShort(&filePointer,node->getJointNode(j)->getParent()->getID()); // write bone ParentId
		
        
		Mat4 jointMatrix = (*skinnedMesh->joints)[j]->LocalAnimatedMatrix;
		for(int k = 0;k < 16;k++) // write jointMatrix
			FileHelper::writeFloat(&filePointer, jointMatrix[k]);
		
		unsigned short boneWeightCount = (*skinnedMesh->joints)[j]->PaintedVertices->size();
		//printf("\n weight %d %d %d",j,node->getJointNode(j)->getParent()->getID(),boneWeightCount);
		FileHelper::writeShort(&filePointer,boneWeightCount); // write boneWeightCount
		
		for(int w = 0;w < boneWeightCount;w++){	// write vertexIndex and strength
			unsigned int vertexIndex = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->vertexId;
			unsigned short strength = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->weight * 255.0;
			FileHelper::writeInt(&filePointer,vertexIndex);
			FileHelper::writeShort(&filePointer,strength);
		}
	}
    //node.reset();
	return true;
}
bool exportSGR::createSGR(std::string filePath,shared_ptr<MeshNode> node, std::map<int, RigKey> &rigKeys, std::map<int, vector<InfluencedObject> > &influencedVertices, std::map<int, vector<InfluencedObject> > &influencedJoints)
{
    ofstream filePointer(filePath, ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    if(!node || node->getMesh() == NULL)
        return false;
	Mesh *mesh = node->getMesh();
	unsigned int vertexCount = mesh->getVerticesCount();
    u16 versionIdentifier = 1; // 0 for HighPoly models, 1 For writing envelope radius
    FileHelper::writeShort(&filePointer, versionIdentifier);// write VertexCount
	FileHelper::writeInt(&filePointer, vertexCount);// write VertexCount
	for(int i = 0; i < vertexCount; i++){
        vertexData *vertex = mesh->getLiteVertexByIndex(i);
        Vector3 vertexPosition = AutoRigHelper::getVertexGlobalPosition(vertex->vertPosition,node);
        // write vertex position,normal,UV Coords
		FileHelper::writeFloat(&filePointer,vertexPosition.x);
		FileHelper::writeFloat(&filePointer,vertexPosition.y);
		FileHelper::writeFloat(&filePointer,vertexPosition.z);
		FileHelper::writeFloat(&filePointer,vertex->vertNormal.x);
		FileHelper::writeFloat(&filePointer,vertex->vertNormal.y);
		FileHelper::writeFloat(&filePointer,vertex->vertNormal.z);
		FileHelper::writeFloat(&filePointer,vertex->texCoord1.x);
		FileHelper::writeFloat(&filePointer,1 - vertex->texCoord1.y);
	}
    unsigned int indicesCount = mesh->getTotalIndicesCount();
	FileHelper::writeInt(&filePointer,indicesCount); // write IndexCount
	
    u32 *indicesArr = mesh->getHighPolyIndicesArray();
    for(int k = 0; k < indicesCount; k++){
        unsigned int index = indicesArr[k];
		FileHelper::writeInt(&filePointer,index);	// write Vertex Index
    }
	short boneCount = rigKeys.size();
    FileHelper::writeShort(&filePointer,boneCount);	// write boneCount
	for(int j = 0; j < boneCount; j++){
		if(j == 0)
			FileHelper::writeShort(&filePointer,-1); // write -1 for pivot bone
		else if(j == 1)
			FileHelper::writeShort(&filePointer,0); // write 0 for hip bone
        else
			FileHelper::writeShort(&filePointer,rigKeys[j].parentId); // write bone ParentId
        
        Mat4 jointMatrix = rigKeys[j].referenceNode->node->getRelativeTransformation();
        for(int k = 0;k < 16;k++){ // write jointMatrix
			FileHelper::writeFloat(&filePointer, jointMatrix[k]);
        }
        
		unsigned short influencedVerticesCount = influencedVertices[j].size();
		FileHelper::writeShort(&filePointer,influencedVerticesCount); // write boneWeightCount
		for(int w = 0;w < influencedVerticesCount;w++){	// write vertexIndex and strength
			unsigned int vertexIndex = influencedVertices[j][w].id;
			short strength = (influencedVertices[j][w].weight * 255.0);
			FileHelper::writeInt(&filePointer,vertexIndex);
			FileHelper::writeShort(&filePointer,strength);
		}
        
        FileHelper::writeFloat(&filePointer, (rigKeys[j].sphere->node) ? rigKeys[j].sphere->node->getScale().x : 0.2);
        FileHelper::writeFloat(&filePointer, rigKeys[j].envelopeRadius);
	}
        //node.reset();
	return true;
}