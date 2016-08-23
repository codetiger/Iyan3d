 //
//  Node.cpp
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Node.h"
#ifdef IOS
#import "TargetConditionals.h"
#endif
Node::Node()
{
    skinType = GPU_SKIN;
    id = NOT_EXISTS;
    type = NODE_TYPE_EMPTY;
    shouldUpdateMesh = false;
    position = Vector3(0.0);
    rotation = Vector3(0.0);
    scale = Vector3(1.0);
    uvScale = 1.0f;
    instancingRenderIt = 0;
    
    for(int i = 0; i < MAX_TEXTURE_PER_NODE; i++)
        textures[i] = NULL;

    Parent = shared_ptr<Node>();
    Children = make_shared< vector< shared_ptr<Node> > >();
    isVisible = true;
    hasTransparency = hasNormalMap = hasReflectionMap = false;
    #ifdef ANDROID
    nodeData = make_shared<OGLNodeData>();
    #elif IOS
    if(common::deviceType == OPENGLES2)
        nodeData = make_shared<OGLNodeData>();
    else{
        #if !(TARGET_IPHONE_SIMULATOR)
            nodeData = static_pointer_cast<APIData>(initMetalNodeData());
        #endif
    }
    #endif
    drawMode = DRAW_MODE_TRIANGLES;
}

Node::~Node()
{
//    if(this->instancedNodes.size()){
//        for(u16 i = 0;i < instancedNodes.size();i++){
//            if(instancedNodes[i]) {
//                while (instancedNodes[i].use_count() > 0)
//                    instancedNodes[i].reset();
//                instancedNodes.erase(instancedNodes.begin() + i);
//            }
//        }
//    }
    this->instancedNodes.clear();
    
    if(this->Children->size()){
        for(u16 i = 0;i < Children->size();i++){
            if((*Children)[i]) {
                for(int j = 0; j < (*Children)[i].use_count(); j++)
                    (*Children)[i].reset();
                Children->erase(Children->begin() + i);
            }
        }
    }
    this->Children->clear();
    if(this->Children) {
        for(int j = 0; j < Children.use_count(); j++)
            Children.reset();
    }
    
    if(this->Parent)
    this->Parent.reset();
    
    if(nodeData) {
        for(int j = 0; j < nodeData.use_count(); j++)
            nodeData.reset();
    }
}

bool Node::operator==(shared_ptr<Node> n)
{
    if(n->getID() != id || n->position != position || n->scale != scale || n->rotation != rotation || n->callbackFuncName.compare(callbackFuncName) != 0 || n->type != type)
        return false;
    return true;
}

bool Node::isMetalSupported()
{
    #ifdef IOS
    size_t size;
    cpu_type_t type;
    cpu_subtype_t subtype;
    size = sizeof(type);
    sysctlbyname("hw.cputype", &type, &size, NULL, 0);
    
    size = sizeof(subtype);
    sysctlbyname("hw.cpusubtype", &subtype, &size, NULL, 0);
    if(subtype == CPU_SUBTYPE_ARM64_V8)
        return true;
    return false;
    #endif
}

void Node::setTexture(Texture *texture, int textureIndex)
{
    if(textureIndex > NODE_TEXTURE_TYPE_NORMALMAP || textureIndex < NODE_TEXTURE_TYPE_COLORMAP)
        return;

    textures[textureIndex] = texture;
    
    if(textureIndex == NODE_TEXTURE_TYPE_COLORMAP)
        hasTransparency = texture->hasTransparency;
//    else if(textureIndex == NODE_TEXTURE_TYPE_REFLECTIONMAP)
//        hasReflectionMap = (texture != NULL);
    else if(textureIndex == NODE_TEXTURE_TYPE_NORMALMAP)
        hasNormalMap = (texture != NULL);
}

void Node::setRotation(Quaternion r, bool updateBB)
{
    rotation = r;
    if(updateBB)
        FlagTransformationToChildren();
}

void Node::setPosition(Vector3 position, bool updateBB)
{
    this->position = position;
    if(updateBB)
        FlagTransformationToChildren();
}

void Node::setScale(Vector3 scale, bool updateBB)
{
    this->scale = scale;
    if(updateBB)
        FlagTransformationToChildren();
}

Quaternion Node::getRotation()
{
    return rotation;
}

Vector3 Node::getPosition()
{
    return position;
}

Vector3 Node::getScale()
{
    return scale;
}

Vector3 Node::getAbsolutePosition()
{
    return getAbsoluteTransformation().getTranslation();
}

void Node::updateBoundingBox()
{
    BoundingBox bb;
    
    if(Children->size()){
        for(unsigned short i = 0; i < Children->size();i++){
            if((*Children)[i]) {
                (*Children)[i]->updateBoundingBox();
                if((*Children)[i]->getBoundingBox().isValid()) {
                    for (int j = 0; j < 8; j++) {
                        Vector3 edge = (*Children)[i]->getBoundingBox().getEdgeByIndex(j);
                        Vector4 newEdge = AbsoluteTransformation *  Vector4(edge.x, edge.y, edge.z, 1.0);
                        bb.addPointsToCalculateBoundingBox(Vector3(newEdge.x, newEdge.y, newEdge.z));
                    }
                }
            }
        }
    }
    bb.calculateEdges();
    bBox = bb;
}

void Node::updateAbsoluteTransformation(bool updateFromRoot)
{
    if(Parent)
        AbsoluteTransformation = Parent->getAbsoluteTransformation() * getRelativeTransformation();
    else
        AbsoluteTransformation = getRelativeTransformation();
}

void Node::updateAbsoluteTransformationOfChildren()
{
    updateAbsoluteTransformation();
    if(Children->size()) {
        for(unsigned short i = 0; i < Children->size();i++) {
            if((*Children)[i])
                (*Children)[i]->updateAbsoluteTransformation();
        }
    }
}

Mat4 Node::getAbsoluteTransformation()
{
    updateAbsoluteTransformation();
    return AbsoluteTransformation;
}

void Node::updateRelativeTransformation()
{
}

Mat4 Node::getRelativeTransformation()
{
    Mat4 localMat = Mat4();
    localMat.translate(position);
    localMat.setRotation(rotation);
    
    if(scale != Vector3(1.0f))
        localMat.scale(scale);
    
    return localMat;
}

Mat4 Node::getModelMatrix()
{
    return getAbsoluteTransformation();
}

void Node::FlagTransformationToChildren()
{
    updateAbsoluteTransformation();
    update();
    updateAbsoluteTransformationOfChildren();
    updateBoundingBox();
    return;
}

Texture* Node::getTextureByIndex(u16 textureIndex)
{
    if(textureIndex > NODE_TEXTURE_TYPE_NORMALMAP|| textureIndex < NODE_TEXTURE_TYPE_COLORMAP)
        return NULL;

    return textures[textureIndex];
}

void Node::setMaterial(Material *mat, bool isTransparentMaterial)
{    
#ifndef UBUNTU
    if(type == NODE_TYPE_PARTICLES && mat->name != "SHADER_PARTICLES" && mat->name != "SHADER_PARTICLES_RTT")
        return;
    
    this->material = mat;
    this->material->isTransparent = isTransparentMaterial;
#endif
}

void Node::setID(int id)
{
    this->id = id;
}

int Node::getID()
{
    return id;
}

void Node::setParent(shared_ptr<Node> parent)
{
    if(this->Parent && this->Parent->Children && this->Parent->Children->size() > 0) { // remove from child list of previous parent
        for(int i = 0; i < this->Parent->Children->size();i++) {
            if((*this->Parent->Children)[i] && (*this->Parent->Children)[i]->getID() == id) {
                (this->Parent->Children)->erase(this->Parent->Children->begin() + i);
                this->Parent.reset();
                break;
            }
        }
    }
    
    if(this->Parent && parent && this->Parent->getID() == parent->getID()) // returns if present parent is same
           return;
    
    this->Parent = parent;
    if(parent)
        this->Parent->Children->push_back(shared_from_this());
}

shared_ptr<Node> Node::getParent()
{
    return Parent;
}

void Node::setVisible(bool isVisible)
{
    this->isVisible = isVisible;
}

BoundingBox Node::getBoundingBox()
{
    return bBox;
}

bool Node::getVisible()
{
    return this->isVisible;
}

void Node::detachFromParent()
{
    if(getParent()) {
        for(int c = 0; c < getParent()->Children->size(); c++) {
            if((*getParent()->Children)[c]->getID() == getID()) {
                getParent()->Children->erase(getParent()->Children->begin() + c);
                return;
            }
        }
    }
}

void Node::detachAllChildren()
{
    for(int c = 0;c < Children->size();c++)
        (*Children)[c]->setParent(shared_ptr<Node>());
    
    Children->clear();
}

void Node::setUserPointer(void* userPtr)
{
    this->userPtr = userPtr;
}

void* Node::getUserPointer()
{
    return userPtr;
}

