//
//  MaterialProperty.cpp
//  Iyan3D
//
//  Created by Karthik on 01/08/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "MaterialProperty.h"
#include "FileHelper.h"

MaterialProperty::MaterialProperty(NODE_TYPE nType)
{
    for(int i = 0; i < MAX_TEXTURE_PER_NODE; i ++)
        textures[i] = NULL;
    
    nodeType = nType;
    if(nodeType == NODE_CAMERA) {
        addOrUpdateProperty(MATERIAL_PROPS, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Material Properties", "PROPERTIES");
        addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED, TYPE_NONE, "Transparency");
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Visible");
        addOrUpdateProperty(SELECTED, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
        addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Lighting");
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(true, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "IsVertexColor");
        addOrUpdateProperty(REFLECTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Reflection");
        addOrUpdateProperty(REFRACTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Glassy");
        addOrUpdateProperty(VERTEX_COLOR, Vector4(1.0), MATERIAL_PROPS, TYPE_NONE, "Color");
    } else if(nodeType == NODE_LIGHT || nodeType == NODE_ADDITIONAL_LIGHT) {
        addOrUpdateProperty(MATERIAL_PROPS, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Material Properties", "PROPERTIES");
        addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED, TYPE_NONE, "Transparency");
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Visible");
        addOrUpdateProperty(SELECTED, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
        addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, TYPE_NONE, "Lighting");
        addOrUpdateProperty(REFLECTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Reflection");
        addOrUpdateProperty(REFRACTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "Glassy");
        
    } else if(nodeType == NODE_SGM || nodeType == NODE_OBJ || nodeType == NODE_TEXT || nodeType == NODE_RIG || nodeType == NODE_TEXT_SKIN) {
        addOrUpdateProperty(MATERIAL_PROPS, Vector4(1, 0, 0, 0), UNDEFINED, PARENT_TYPE, "Material Properties", "PROPERTIES");
        addOrUpdateProperty(TRANSPARENCY, Vector4(1.0, 0.0, 0.0, 0.0), UNDEFINED, TYPE_NONE, "Transparency");
        addOrUpdateProperty(VISIBILITY, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Visible", "PROPERTIES");
        addOrUpdateProperty(SELECTED, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "Selected");
        addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Lighting", "PROPERTIES");
        addOrUpdateProperty(SPECIFIC_FLOAT, Vector4(0, 0, 0, 0), UNDEFINED, TYPE_NONE, "SpecificFloat");
        addOrUpdateProperty(VERTEX_COLOR, Vector4(1.0), MATERIAL_PROPS, TYPE_NONE, "Color");
        addOrUpdateProperty(ORIG_VERTEX_COLOR, Vector4(1.0), UNDEFINED, TYPE_NONE, "Color");
        addOrUpdateProperty(TEXT_COLOR, Vector4(1.0), UNDEFINED, TYPE_NONE, "Color");
        
        addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(false, 0, 0, 0), MATERIAL_PROPS, TYPE_NONE, "IsVertexColor");
        addOrUpdateProperty(REFLECTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, SLIDER_TYPE, "Reflection", "SAMPLE");
        addOrUpdateProperty(REFRACTION, Vector4(0, 0, 0, 0), MATERIAL_PROPS, SLIDER_TYPE, "Glassy", "SAMPLE");
        addOrUpdateProperty(TEXTURE, Vector4(1), MATERIAL_PROPS, IMAGE_TYPE, "Texture", "SKIN", "");
        addOrUpdateProperty(TEXTURE_SCALE, Vector4(1, 0, 0, true), MATERIAL_PROPS, SLIDER_TYPE, "Scale", "SKIN");
        addOrUpdateProperty(TEXTURE_SMOOTH, Vector4(1, 0, 0, 0), MATERIAL_PROPS, SWITCH_TYPE, "Smooth", "SKIN");
        addOrUpdateProperty(BUMP_MAP, Vector4(1), MATERIAL_PROPS, IMAGE_TYPE, "Bump Map", "SKIN", "");
        addOrUpdateProperty(BUMP_DEPTH, Vector4(1, 0, 0, 0), MATERIAL_PROPS, SLIDER_TYPE, "Depth", "SKIN");
        
    } else if(nodeType == NODE_IMAGE || nodeType == NODE_VIDEO || nodeType == NODE_PARTICLES) {
        if(nodeType != NODE_PARTICLES)
            addOrUpdateProperty(LIGHTING, Vector4(1, 0, 0, 0), UNDEFINED, SWITCH_TYPE, "Lighting", "PROPERTIES");
    }

}

MaterialProperty::~MaterialProperty()
{
    
}

Texture* MaterialProperty::getTextureOfType(node_texture_type texType)
{
    if(texType > NODE_TEXTURE_TYPE_REFLECTIONMAP || texType < NODE_TEXTURE_TYPE_COLORMAP)
        return NULL;
    
    return textures[texType];

}

void MaterialProperty::setTextureForType(Texture* texture, node_texture_type texType)
{
    if(texType > NODE_TEXTURE_TYPE_REFLECTIONMAP || texType < NODE_TEXTURE_TYPE_COLORMAP)
        return;
    
    textures[texType] = texture;
}

std::map< PROP_INDEX, Property >& MaterialProperty::getProps()
{
    return props;
}

Property& MaterialProperty::getProperty(PROP_INDEX pIndex)
{
    if(props.find(pIndex) != props.end()) {
        return props[pIndex];
    } else {
        std::map< PROP_INDEX, Property >::iterator pIt;
        
        for(pIt = props.begin(); pIt != props.end(); pIt++) {
            if(pIt->second.subProps.find(pIndex) != pIt->second.subProps.end()) {
                return pIt->second.subProps[pIndex];
            }
        }
        
        printf("\n No such material property ..");
    }
}

void MaterialProperty::addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE propType, string title, string groupName, string fileName, ICON_INDEX iconId)
{
    Property property;
    property.index = index;
    property.parentIndex = (nodeType == NODE_LIGHT || nodeType == NODE_ADDITIONAL_LIGHT) ? UNDEFINED : parentProp;
    property.value = value;
    property.iconId = iconId;
    
    if(title.length() > 2) {
        property.title = title;
        property.type = propType;
    }
    
    if(groupName.length() > 2)
        property.groupName = groupName;
    if(fileName.length() > 5 || propType == IMAGE_TYPE)
        property.fileName = fileName;
    
    if(property.parentIndex != UNDEFINED && props.find(property.parentIndex) != props.end()) {
        checkAndUpdatePropsMap(props[parentProp].subProps, property);
    } else if(props.find(index) == props.end()) {
        property.type = propType;
        props.insert(pair<PROP_INDEX, Property>( index, property));
    }
    else {
        props.find(index)->second.value = property.value;
        if(property.fileName.length() > 5 || propType == IMAGE_TYPE)
            props.find(index)->second.fileName = property.fileName;
    }
    
}

void MaterialProperty::checkAndUpdatePropsMap(std::map < PROP_INDEX, Property > &propsMap, Property property)
{
    if(propsMap.find(property.index) == propsMap.end()) {
        propsMap.insert(pair<PROP_INDEX, Property>( property.index, property));
    } else {
        propsMap.find(property.index)->second.value = property.value;
        if(property.fileName.length() > 5 || property.type == IMAGE_TYPE)
            propsMap.find(property.index)->second.fileName = property.fileName;
    }
}

bool MaterialProperty::IsPropertyExists(PROP_INDEX pIndex)
{
    if(props.find(pIndex) != props.end())
        return true;
    else {
        std::map<PROP_INDEX, Property>::iterator pIt;
        for(pIt = props.begin(); pIt != props.end(); pIt++) {
            if(pIt->second.subProps.find(pIndex) != pIt->second.subProps.end())
                return true;
        }
        
        return false;
    }
}

void MaterialProperty::writeProperties(ofstream *filePointer)
{
    FileHelper::writeFloat(filePointer, (IsPropertyExists(LIGHTING)) ? getProperty(LIGHTING).value.x : -1.0);
    FileHelper::writeFloat(filePointer, (IsPropertyExists(IS_VERTEX_COLOR)) ? getProperty(IS_VERTEX_COLOR).value.x : -1.0);
    FileHelper::writeFloat(filePointer, (IsPropertyExists(REFLECTION)) ? getProperty(REFLECTION).value.x : -1.0);
    FileHelper::writeFloat(filePointer, (IsPropertyExists(REFRACTION)) ? getProperty(REFRACTION).value.x : -1.0);
    FileHelper::writeFloat(filePointer, (IsPropertyExists(TEXTURE_SCALE)) ? getProperty(TEXTURE_SCALE).value.x : -1.0);
    FileHelper::writeFloat(filePointer, IsPropertyExists(TEXTURE_SMOOTH) ? getProperty(TEXTURE_SMOOTH).value.x : -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeVector4(filePointer, (IsPropertyExists(VERTEX_COLOR)) ? getProperty(VERTEX_COLOR).value : Vector4(-1.0));
    FileHelper::writeVector4(filePointer, (IsPropertyExists(TEXT_COLOR)) ? getProperty(TEXT_COLOR).value : Vector4(-1.0));
    FileHelper::writeVector4(filePointer, Vector4(-1.0));
    FileHelper::writeVector4(filePointer, Vector4(-1.0));
    FileHelper::writeString(filePointer, (IsPropertyExists(TEXTURE)) ? getProperty(TEXTURE).fileName : " ");
    FileHelper::writeString(filePointer, (IsPropertyExists(BUMP_MAP)) ? getProperty(BUMP_MAP).fileName : " ");
    FileHelper::writeString(filePointer, " ");
    FileHelper::writeString(filePointer, " ");
    FileHelper::writeString(filePointer, " ");
    FileHelper::writeString(filePointer, " ");
}

void MaterialProperty::readProperties(ifstream *filePointer)
{
    if(IsPropertyExists(LIGHTING))
        getProperty(LIGHTING).value.x = FileHelper::readFloat(filePointer);
    else
        FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(IS_VERTEX_COLOR)) {
        getProperty(IS_VERTEX_COLOR).value.x = FileHelper::readFloat(filePointer);
    } else
        FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(REFLECTION))
        getProperty(REFLECTION).value.x = FileHelper::readFloat(filePointer);
    else
        FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(REFRACTION))
        getProperty(REFRACTION).value.x = FileHelper::readFloat(filePointer);
    else
        FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(SPECIFIC_FLOAT))
        getProperty(SPECIFIC_FLOAT).value.x = FileHelper::readFloat(filePointer);
    else
        FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(TEXTURE_SCALE))
        getProperty(TEXTURE_SCALE).value.x = FileHelper::readFloat(filePointer);
    else
        FileHelper::readFloat(filePointer);
    
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    
    if(IsPropertyExists(VERTEX_COLOR))
        getProperty(VERTEX_COLOR).value = FileHelper::readVector4(filePointer);
    else
        FileHelper::readVector4(filePointer);
    
    if(IsPropertyExists(TEXT_COLOR))
        getProperty(TEXT_COLOR).value = FileHelper::readVector4(filePointer);
    else
        FileHelper::readVector4(filePointer);
    
    FileHelper::readVector4(filePointer);
    FileHelper::readVector4(filePointer);
    
    if(IsPropertyExists(TEXTURE))
        getProperty(TEXTURE).fileName = FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    else
        FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    
    if(IsPropertyExists(BUMP_MAP))
        getProperty(BUMP_MAP).fileName = FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    else
        FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    
    FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
    FileHelper::readString(filePointer, SGB_VERSION_CURRENT);
}

