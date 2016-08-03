//
//  MaterialProperty.h
//  Iyan3D
//
//  Created by Karthik on 01/08/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef MaterialProperty_h
#define MaterialProperty_h

#include "Texture.h"
#include "Constants.h"

class MaterialProperty
{
    Texture* textures[MAX_TEXTURE_PER_NODE];
    std::map< PROP_INDEX, Property > props;
    NODE_TYPE nodeType;
    
public:
    
    MaterialProperty(NODE_TYPE nodeType);
    ~MaterialProperty();
    
    std::map< PROP_INDEX, Property >& getProps();
    Texture* getTextureOfType(node_texture_type nType);
    void setTextureForType(Texture* texture, node_texture_type nType);
    Property& getProperty(PROP_INDEX pIndex);
    void addOrUpdateProperty(PROP_INDEX index, Vector4 value, PROP_INDEX parentProp, PROP_TYPE type = TYPE_NONE, string title = "", string groupName = " ", string fileName = "", ICON_INDEX iconId = NO_ICON);
    void checkAndUpdatePropsMap(std::map < PROP_INDEX, Property > &propsMap, Property property);

};

#endif /* MaterialProperty_h */
