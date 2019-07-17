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

#include "../Textures/Texture.h"
#include "../Material/Material.h"
#include <sys/types.h>
#include <memory>
#include <sys/sysctl.h>
#include <mach/machine.h>

#include "../../RenderManager/MTLNodeDataWrapper.h"

typedef enum {
    NODE_TYPE_EMPTY,
    NODE_TYPE_CAMERA,
    NODE_TYPE_LIGHT,
    NODE_TYPE_MESH,
    NODE_TYPE_SKINNED,
    NODE_TYPE_PARTICLES,
    NODE_TYPE_LINES,
    NODE_TYPE_INSTANCED
} node_type;

typedef enum {
    NODE_GPUMEM_TYPE_STATIC,
    NODE_GPUMEM_TYPE_DYNAMIC,
} node_gpumem_type;

typedef enum {
    NODE_TEXTURE_TYPE_COLORMAP      = 0,
    NODE_TEXTURE_TYPE_NORMALMAP     = 1,
    NODE_TEXTURE_TYPE_SHADOWMAP     = 2,
    NODE_TEXTURE_TYPE_REFLECTIONMAP = 3
} node_texture_type;

enum DRAW_MODE {
    DRAW_MODE_POINTS,
    DRAW_MODE_LINES,
    DRAW_MODE_LINE_STRIP,
    DRAW_MODE_TRIANGLES,
    DRAW_MODE_TRIANGLE_STRIP,
};

class Node : public enable_shared_from_this<Node> {
private:
    int   id;
    bool  isMetalSupported();
    bool  isVisible;
    void* userPtr;

protected:
    Mat4       AbsoluteTransformation, relativeTransform;
    Vector3    position, scale;
    Quaternion rotation;

public:
    bool                                 hasTransparency;
    int                                  instancingRenderIt;
    vector<shared_ptr<Node> >             instancedNodes;
    BoundingBox                          bBox;
    skin_type                            skinType;
    bool                                 shouldUpdateMesh;
    DRAW_MODE                            drawMode;
    string                               callbackFuncName;
    node_type                            type;
    node_gpumem_type                     memtype;
    shared_ptr<APIData>                  nodeData;
    shared_ptr<Node>                     Parent;
    shared_ptr<Node>                     original;
    shared_ptr<vector<shared_ptr<Node> > > Children;
    Material*                            material;

    Node();
    virtual ~Node();
    virtual void update() = 0;
    bool         operator==(shared_ptr<Node> n);

    void setPosition(Vector3 position, bool updateBB = false);
    void setScale(Vector3 scale, bool updateBB = false);
    void setRotation(Quaternion r, bool updateBB = false);

    void    setParent(shared_ptr<Node> parent);
    void    setVisible(bool isVisible);
    Vector3 getAbsolutePosition();
    void    FlagTransformationToChildren(); // NOT FIXED
    void    setMaterial(Material* mat, bool isTransparentMaterial = false);
    void    setID(int id);

    virtual void updateBoundingBox();
    void         updateAbsoluteTransformation(bool updateFromRoot = false);
    void         updateAbsoluteTransformationOfChildren();
    void         updateRelativeTransformation();
    void         detachFromParent();
    void         detachAllChildren();

    BoundingBox getBoundingBox();
    bool        getVisible();
    int         getID();

    Quaternion getRotation();
    Vector3    getPosition();
    Vector3    getScale();

    Mat4 getRelativeTransformation();
    Mat4 getModelMatrix();
    Mat4 getAbsoluteTransformation();

    shared_ptr<Node> getParent();

    void  setUserPointer(void* userPtr);
    void* getUserPointer();
};

#endif
