# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
TOP_LOCAL_PATH:=$(call my-dir)
include $(call all-subdir-makefiles)
LOCAL_PATH := $(TOP_LOCAL_PATH)  
include $(CLEAR_VARS)

LOCAL_SRC_FILES := Helper.cpp Logger.cpp Maths.cpp
LOCAL_CFLAGS   	+= -std=c++11 -frtti -fexceptions -fpermissive
LOCAL_LDLIBS	+= -llog -lGLESv2 -lEGL -landroid -lOpenSLES -lGLESv1_CM -lz
LOCAL_C_INCLUDES := $(LOCAL_PATH)/SGEngine2 \
        $(LOCAL_PATH)/SGEngine2/libpng \
        $(LOCAL_PATH)/SGEngine2/Utilities \
        $(LOCAL_PATH)/SGEngine2/SceneManager \
        $(LOCAL_PATH)/SGEngine2/Core \
        $(LOCAL_PATH)/SGEngine2/Core/Meshes \
        $(LOCAL_PATH)/SGEngine2/Core/common \
        $(LOCAL_PATH)/SGEngine2/Core/Material \
        $(LOCAL_PATH)/SGEngine2/Core/Textures \
        $(LOCAL_PATH)/SGEngine2/Core/Nodes \
        $(LOCAL_PATH)/SGEngine2/RenderManager \
        $(LOCAL_PATH)/SGEngine2/Font \
        $(LOCAL_PATH)/SGEngine2/Font/glu \
        $(LOCAL_PATH)/SGEngine2/Font/glu/libutil \
        $(LOCAL_PATH)/SGEngine2/Font/glu/libtess \
        $(LOCAL_PATH)/SGEngine2/Font/glu/include \
        $(LOCAL_PATH)/SGEngine2/Font/ftgl \
        $(LOCAL_PATH)/SGEngine2/Font/freeType \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/psnames \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/raster \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/sfnt \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/autofit \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/truetype \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/smooth \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/base \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/cff \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/pshinter \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/freetype \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/freetype/config \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/freetype/internal \
        $(LOCAL_PATH)/SGEngine2/Font/freeType/include/freetype/internal/services \
        $(LOCAL_PATH)/SGEngine2/Font/poly2tri \
        $(LOCAL_PATH)/SGEngine2/Font/poly2tri/common \
        $(LOCAL_PATH)/SGEngine2/Font/poly2tri/sweep \
        $(LOCAL_PATH)/SGEngine2/Loaders \
        $(LOCAL_PATH)/Iyan3dEngineFiles \
        $(LOCAL_PATH)/Iyan3dEngineFiles/HeaderFiles

include $(BUILD_SHARED_LIBRARY)
