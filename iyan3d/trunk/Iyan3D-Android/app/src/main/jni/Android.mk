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

LOCAL_MODULE    := iyan3d

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/BroadphaseCollision/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/CollisionDispatch/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/CollisionShapes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/Gimpact/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/NarrowPhaseCollision/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Character/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/ConstraintSolver/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Dynamics/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Featherstone/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/MLCPSolvers/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Vehicle/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletSoftBody/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/LinearMath/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Material/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Nodes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Textures/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/common/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Meshes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/raster/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/autofit/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/cff/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/sfnt/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/truetype/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/base/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/psnames/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/pshinter/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/smooth/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/glu/libutil/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/glu/libtess/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/ftgl/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/RenderManager/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/SceneManager/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/libpng/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Loaders/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Utilities/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/BroadphaseCollision/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/CollisionDispatch/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/CollisionShapes/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/Gimpact/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/NarrowPhaseCollision/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Character/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/ConstraintSolver/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Dynamics/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Featherstone/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/MLCPSolvers/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Vehicle/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletSoftBody/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/LinearMath/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Material/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Nodes/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Textures/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/common/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/raster/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/autofit/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/cff/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/sfnt/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/truetype/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/base/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/psnames/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/pshinter/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/freeType/include/src/smooth/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/glu/libutil/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/glu/libtess/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Font/ftgl/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/RenderManager/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/SceneManager/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/libpng/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Loaders/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Utilities/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)


LOCAL_CFLAGS  := -fexceptions
LOCAL_CPPFLAGS := -std=c++11 -fpermissive
LOCAL_LDLIBS  := -llog -lGLESv2 -lEGL -landroid -lOpenSLES -lz -lGLESv1_CM -lm
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
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3 \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/BroadphaseCollision \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/CollisionDispatch \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/CollisionShapes \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/Gimpact \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletCollision/NarrowPhaseCollision \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Character \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/ConstraintSolver \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Dynamics \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Featherstone \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/MLCPSolvers \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletDynamics/Vehicle \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/BulletSoftBody \
        $(LOCAL_PATH)/Iyan3dEngineFiles/bullet3/src/LinearMath \
        $(LOCAL_PATH)/Iyan3dEngineFiles/HeaderFiles
include $(BUILD_SHARED_LIBRARY)






























