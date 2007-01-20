//
//  Node.h
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Node__
#define __SGEngine2__Node__

#include "../common/common.h"
#include "iostream"
#include "../Meshes/Mesh.h"
#include "APIData.h"

#ifndef UBUNTU
#include "OGLNodeData.h"
#endif

#include "../Textures/Texture.h"
#include "InstanceNode.h"
#include "../Material/Material.h"
#include <sys/types.h>
#include <memory>
#ifdef IOS
#include <sys/sysctl.h>
#include <mach/machine.h>

#if !(TARGET_IPHONE_SIMULATOR)
#include "../../RenderManager/MTLNodeDataWrapper.h"
#endif

#endif
typedef enum {
    NODE_TYPE_EMPTY,
    NODE_TYPE_CAMERA,
    NODE_TYPE_LIGHT,
    NODE_TYPE_MESH,
    NODE_TYPE_MORPH,
    NODE_TYPE_ANIMATED,
    NODE_TYPE_SKINNED,
    NODE_TYPE_MORPH_SKINNED
} node_type;


typedef enum {
    FM_NODE_TYPE_PLATFORM_SINGLE,
    FM_NODE_TYPE_PLATFORM_REPEATE,
    FM_NODE_TYPE_ENVIRONMENT,
    FM_NODE_TYPE_FIXED,
    FM_NODE_TYPE_REPEATED
} FM_NODE_TYPE;

typedef enum {
    NODE_GPUMEM_TYPE_STATIC,
    NODE_GPUMEM_TYPE_DYNAMIC,
} node_gpumem_type;

enum DRAW_MODE{
    DRAW_MODE_POINTS,
    DRAW_MODE_LINES,
    DRAW_MODE_LINE_LOOP,
    DRAW_MODE_LINE_STRIP,
    DRAW_MODE_TRIANGLES,
    DRAW_MODE_TRIANGLE_STRIP,
    DRAW_MODE_TRIANGLE_FAN
};

class Node : public enable_shared_from_this<Node>{

private:
    shared_ptr< vector< shared_ptr<InstanceNode> > > instancedNodes;
    int id,textureCount;
    bool isMetalSupported();
    
protected:
    Mat4 AbsoluteTransformation;
    Vector3 position,scale,rotation;
    Texture* textures[MAX_TEXTURE_PER_NODE];
    bool isVisible;
    
public:
    BoundingBox bBox;
    skin_type skinType;
    bool needsVertexPosition, needsVertexNormal, needsVertexColor, needsUV1, needsUV2, needsUV3, shouldUpdateMesh;
    bool needsIndexBuf;
    DRAW_MODE drawMode;
    u16 activeTextureIndex;
    u16 instanceCount;
    string callbackFuncName;
    node_type type;
    node_gpumem_type memtype;
    shared_ptr<APIData> nodeData;
    shared_ptr<Node> Parent;
    shared_ptr< vector< shared_ptr<Node> > > Children;
    Material *material;
    
    Node();
    virtual ~Node();
    virtual void update() = 0;
    bool operator==(shared_ptr<Node> n);
    void setRotationInRadians(Vector3 rotation);
    void setRotationInDegrees(Vector3 rotation);
    void setPosition(Vector3 position);
    void setScale(Vector3 scale);
    void setParent(shared_ptr<Node> parent);
    void setVisible(bool isVisible);
    Vector3 getAbsolutePosition();
    void setTexture(Texture *texture,int textureIndex);
    void setActiveTexture(Texture *texture);
    void setActiveTextureByName(string *textureName);
    void setActiveTextureByIndex(int textureIndex);
    void FlagTransformationToChildren(); // NOT FIXED
    void RemoveNodeInstanceByIndex(u16 index);
    void RemoveAllInstanceOfNode();
    void setMaterial(Material* mat,bool isTransparentMaterial = false);
    void setID(int id);
    
    virtual void updateBoundingBox();
    void updateAbsoluteTransformation(bool updateFromRoot = false);
    void updateAbsoluteTransformationOfChildren();
    void updateRelativeTransformation();
    void detachFromParent();
    void detachAllChildren();
    
    bool CreateNodeInstance(u16 instanceCount);
    BoundingBox getBoundingBox();
    bool getVisible();
    int getID();
    int getTextureCount();
    
    u16 getBufferCount();
    
    Vector3 getRotationInDegrees();
    Vector3 getRotationInRadians();
    Vector3 getPosition();
    Vector3 getScale();

    Mat4 getRelativeTransformation();
    Mat4 getModelMatrix();
    Mat4 getAbsoluteTransformation();

    Texture* getTextureByIndex(u16 textureIndex);
    Texture* getActiveTexture();
    
    shared_ptr<InstanceNode> getNodeInstanceByIndex(u16 index);
    shared_ptr<Node> getParent();
    
};

#endif /* defined(__SGEngine2__Node__) */
