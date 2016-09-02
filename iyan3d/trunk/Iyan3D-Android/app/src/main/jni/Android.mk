LOCAL_PATH := /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni

ifeq ($(TARGET_ARCH),armeabi)
    APP_PLATFORM=14
endif

ifeq ($(TARGET_ARCH),arm)
    APP_PLATFORM=14
endif

ifeq ($(TARGET_ARCH),armeabi-v7a)
    APP_PLATFORM=14
endif

ifeq ($(TARGET_ARCH),mips)
    APP_PLATFORM=14
endif

ifeq ($(TARGET_ARCH),x86)
    APP_PLATFORM=14
endif

ifeq ($(TARGET_ARCH),arm64)
    APP_PLATFORM=21
endif

ifeq ($(TARGET_ARCH),mips64)
    APP_PLATFORM=21
endif

ifeq ($(TARGET_ARCH),x86_64)
    APP_PLATFORM=21
endif

$(info $(TARGET_ARCH))
$(info $(APP_PLATFORM))

include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), mips mips64))

else
LOCAL_MODULE := libavcodec
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
LOCAL_C_INCLUDES := ../obj/local/$(TARGET_ARCH_ABI)/include
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS  += -march=armv7-a -mfloat-abi=softfp
endif
LOCAL_SRC_FILES := ../obj/local/$(TARGET_ARCH_ABI)/lib/libavcodec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libavfilter
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
LOCAL_C_INCLUDES := ../obj/local/$(TARGET_ARCH_ABI)/include
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS  += -march=armv7-a -mfloat-abi=softfp
endif
LOCAL_SRC_FILES := ../obj/local/$(TARGET_ARCH_ABI)/lib/libavfilter.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libavformat
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
LOCAL_C_INCLUDES := ../obj/local/$(TARGET_ARCH_ABI)/include
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS  += -march=armv7-a -mfloat-abi=softfp
endif
LOCAL_SRC_FILES := ../obj/local/$(TARGET_ARCH_ABI)/lib/libavformat.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libavutil
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
LOCAL_C_INCLUDES := ../obj/local/$(TARGET_ARCH_ABI)/include
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS  += -march=armv7-a -mfloat-abi=softfp
endif
LOCAL_SRC_FILES := ../obj/local/$(TARGET_ARCH_ABI)/lib/libavutil.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libswscale
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
LOCAL_C_INCLUDES := ../obj/local/$(TARGET_ARCH_ABI)/include
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS  += -march=armv7-a -mfloat-abi=softfp
endif
LOCAL_SRC_FILES := ../obj/local/$(TARGET_ARCH_ABI)/lib/libswscale.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
LOCAL_LDLIBS  := -llog -landroid -lz -lm
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections -fvisibility=hidden -flto
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
ifeq ($(TARGET_ARCH),arm)
$(info ARMFILTERWORKING)
LOCAL_CFLAGS  += -march=armv7-a -mfloat-abi=softfp
endif
LOCAL_STATIC_LIBRARIES := libavfilter libavformat libavcodec libswscale libavutil
LOCAL_C_INCLUDES := $(LOCAL_PATH)/ffmpeg
LOCAL_C_INCLUDES += /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/obj/local/$(TARGET_ARCH_ABI)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ffmpeg/$(TARGET_ARCH_ABI)-config
LOCAL_SRC_FILES := ffmpeg/FFmpegJni.c ffmpeg/ffmpeg.c ffmpeg/cmdutils.c ffmpeg/ffmpeg_filter.c ffmpeg/ffmpeg_opt.c
LOCAL_MODULE := videokit
include $(BUILD_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_PATH := /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2
LOCAL_MODULE := freetype_static

FILE_LIST := $(wildcard $(LOCAL_PATH)/src/autofit/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/base/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/cff/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/pshinter/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/psnames/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/raster/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/sfnt/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/smooth/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/truetype/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/raster/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/autofit/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/cff/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/sfnt/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/truetype/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/base/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/psnames/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/pshinter/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/smooth/*.cpp)


LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/freetype \
                    $(LOCAL_PATH)/freetype/config \
                    $(LOCAL_PATH)/freetype/internal \
                    $(LOCAL_PATH)/freetype/services \
                    $(LOCAL_PATH)/src/autofit \
                    $(LOCAL_PATH)/src/base \
                    $(LOCAL_PATH)/src/cff \
                    $(LOCAL_PATH)/src/pshinter \
                    $(LOCAL_PATH)/src/psnames \
                    $(LOCAL_PATH)/src/raster \
                    $(LOCAL_PATH)/src/sfnt \
                    $(LOCAL_PATH)/src/smooth \
                    $(LOCAL_PATH)/src/truetype

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PATH := /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master
LOCAL_MODULE := assimp_static
ASSIMP_SRC_DIR = code

FILE_LIST := $(wildcard $(LOCAL_PATH)/$(ASSIMP_SRC_DIR)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/contrib/openddlparser/code/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(ASSIMP_SRC_DIR)/ftgl/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(ASSIMP_SRC_DIR)/iGLU-1.0.0/libtess/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(ASSIMP_SRC_DIR)/iGLU-1.0.0/libutil/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/contrib/unzip/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/contrib/poly2tri/poly2tri/*/*.cc)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES += contrib/clipper/clipper.cpp \
	contrib/ConvertUTF/ConvertUTF.c \
	contrib/irrXML/irrXML.cpp

# enables -frtti and -fexceptions
LOCAL_CPP_FEATURES := exceptions
# identifier 'nullptr' will become a keyword in C++0x [-Wc++0x-compat]
# but next breaks blender and other importer
# LOCAL_CFLAGS += -std=c++11

# can't be disabled? rudamentary function?
#       -DASSIMP_BUILD_NO_FLIPWINDING_PROCESS  \
#
DontBuildProcess = \
        -DASSIMP_BUILD_NO_FLIPUVS_PROCESS  \
        -DASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS \
        -DASSIMP_BUILD_NO_CALCTANGENTS_PROCESS \
        -DASSIMP_BUILD_NO_DEBONE_PROCESS \
        -DASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS \
        -DASSIMP_BUILD_NO_FINDINSTANCES_PROCESS \
        -DASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS \
        -DASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS \
        -DASSIMP_BUILD_NO_GENFACENORMALS_PROCESS \
        -DASSIMP_BUILD_NO_GENUVCOORDS_PROCESS \
        -DASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS \
        -DASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS \
        -DASSIMP_BUILD_NO_JOINVERTICES_PROCESS \
        -DASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS \
        -DASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS \
        -DASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS \
        -DASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS \
        -DASSIMP_BUILD_NO_REMOVEVC_PROCESS \
        -DASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS \
        -DASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS \
        -DASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS \
        -DASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS \
        -DASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS \
        -DASSIMP_BUILD_NO_TRIANGULATE_PROCESS \
        -DASSIMP_BUILD_NO_VALIDATEDS_PROCESS

DontBuildImporters = \
        -DASSIMP_BUILD_NO_X_IMPORTER \
#       -DASSIMP_BUILD_NO_3DS_IMPORTER \
        -DASSIMP_BUILD_NO_MD3_IMPORTER \
        -DASSIMP_BUILD_NO_MDL_IMPORTER \
        -DASSIMP_BUILD_NO_MD2_IMPORTER \
        -DASSIMP_BUILD_NO_PLY_IMPORTER \
        -DASSIMP_BUILD_NO_ASE_IMPORTER \
        -DASSIMP_BUILD_NO_HMP_IMPORTER \
        -DASSIMP_BUILD_NO_SMD_IMPORTER \
        -DASSIMP_BUILD_NO_MDC_IMPORTER \
        -DASSIMP_BUILD_NO_MD5_IMPORTER \
        -DASSIMP_BUILD_NO_STL_IMPORTER \
        -DASSIMP_BUILD_NO_LWO_IMPORTER \
        -DASSIMP_BUILD_NO_DXF_IMPORTER \
        -DASSIMP_BUILD_NO_NFF_IMPORTER \
        -DASSIMP_BUILD_NO_RAW_IMPORTER \
        -DASSIMP_BUILD_NO_OFF_IMPORTER \
        -DASSIMP_BUILD_NO_AC_IMPORTER \
        -DASSIMP_BUILD_NO_BVH_IMPORTER \
        -DASSIMP_BUILD_NO_IRRMESH_IMPORTER \
        -DASSIMP_BUILD_NO_IRR_IMPORTER \
        -DASSIMP_BUILD_NO_Q3D_IMPORTER \
        -DASSIMP_BUILD_NO_B3D_IMPORTER \
#       -DASSIMP_BUILD_NO_COLLADA_IMPORTER \
        -DASSIMP_BUILD_NO_TERRAGEN_IMPORTER \
        -DASSIMP_BUILD_NO_CSM_IMPORTER \
        -DASSIMP_BUILD_NO_3D_IMPORTER \
        -DASSIMP_BUILD_NO_LWS_IMPORTER \
        -DASSIMP_BUILD_NO_OGRE_IMPORTER \
        -DASSIMP_BUILD_NO_MS3D_IMPORTER \
        -DASSIMP_BUILD_NO_COB_IMPORTER \
        -DASSIMP_BUILD_NO_Q3BSP_IMPORTER \
        -DASSIMP_BUILD_NO_NDO_IMPORTER \
        -DASSIMP_BUILD_NO_IFC_IMPORTER \
        -DASSIMP_BUILD_NO_XGL_IMPORTER \
#       -DASSIMP_BUILD_NO_FBX_IMPORTER \
        -DASSIMP_BUILD_NO_C4D_IMPORTER \
    	-DASSIMP_BUILD_NO_OPENGEX_IMPORTER \
        -DASSIMP_BUILD_NO_ASSBIN_IMPORTER
#       -DASSIMP_BUILD_NO_BLEND_IMPORTER \
        -DASSIMP_BUILD_NO_GEO_IMPORTER
#       -DASSIMP_BUILD_NO_OBJ_IMPORTER \

DontBuildImporters := -DASSIMP_BUILD_NO_IFC_IMPORTER -DASSIMP_BUILD_NO_IRRMESH_IMPORTER  -DASSIMP_BUILD_NO_IRR_IMPORTER -DASSIMP_BUILD_NO_C4D_IMPORTER

ASSIMP_FLAGS_3_0 = -DASSIMP_BUILD_DLL_EXPORT -DASSIMP_BUILD_NO_OWN_ZLIB -DASSIMP_BUILD_BOOST_WORKAROUND -Dassimp_EXPORTS -fPIC -fvisibility=hidden -Wall
ASSIMP_FLAGS_3_1 = $(ASSIMP_FLAGS_3_0) # -DASSIMP_BUILD_BLENDER_DEBUG

LOCAL_CFLAGS += -Os $(ASSIMP_FLAGS_3_1) -DOPENDDL_NO_USE_CPP11 $(DontBuildImporters)  # $(DontBuildProcess)

LOCAL_C_INCLUDES := /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2 \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype/config \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype/internal \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype/services \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/autofit \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/base \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/cff \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/pshinter \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/psnames \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/raster \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/sfnt \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/smooth \
                    /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/truetype

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include $(LOCAL_PATH)/contrib/rapidjson/include \
                    $(LOCAL_PATH)/$(ASSIMP_SRC_DIR)/BoostWorkaround \
                    $(LOCAL_PATH)/contrib/openddlparser/include \
                    $(LOCAL_PATH)/code \
                    $(LOCAL_PATH)/code/ftgl \
                    $(LOCAL_PATH)/code/iGLU-1.0.0/include \
                    $(LOCAL_PATH)/code/res

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/include $(LOCAL_PATH)/$(ASSIMP_SRC_DIR)/BoostWorkaround

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PATH := /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni

LOCAL_MODULE := iyan3d

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(filter-out $(LOCAL_PATH)/Iyan3dEngineFiles/SGCloudRenderingHelper.cpp, $(wildcard $(LOCAL_PATH)/Iyan3dEngineFiles/*.cpp))
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
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/common/GLKMath/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/Meshes/*.cpp)
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
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Core/common/GLKMath/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/RenderManager/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/SceneManager/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/libpng/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Loaders/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/SGEngine2/Utilities/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := libassimp_static freetype_static

LOCAL_CPPFLAGS := -Os -ffunction-sections -fdata-sections -fvisibility=hidden -std=c++11 -fpermissive
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections -fvisibility=hidden
LOCAL_LDFLAGS := -Wl,--gc-sections,--icf=safe
LOCAL_LDLIBS  := -llog -lGLESv2 -lEGL -landroid -lz -lm -lGLESv1_CM

LOCAL_C_INCLUDES :=/storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2 \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2 \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype/config \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype/internal \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/freetype/services \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/autofit \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/base \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/cff \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/pshinter \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/psnames \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/raster \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/sfnt \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/smooth \
                   /storage/Sabish/Iyan3D_6.0_SVN/Iyan3D/app/src/main/jni/freetype2/src/truetype

LOCAL_C_INCLUDES += /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master/include \
                    /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master/code \
                    /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master/code/ftgl \
                    /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master/code/iGLU-1.0.0/include \
                    /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master/code/iGLU-1.0.0/libtess \
                    /storage/Sabish/Iyan3D_6.0_SVN/trunk/assimp-master/code/iGLU-1.0.0/libutil

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    $(LOCAL_PATH)/SGEngine2 \
                    $(LOCAL_PATH)/SGEngine2/libpng \
                    $(LOCAL_PATH)/SGEngine2/Utilities \
                    $(LOCAL_PATH)/SGEngine2/SceneManager \
                    $(LOCAL_PATH)/SGEngine2/Core \
                    $(LOCAL_PATH)/SGEngine2/Core/Meshes \
                    $(LOCAL_PATH)/SGEngine2/Core/common \
                    $(LOCAL_PATH)/SGEngine2/Core/common/GLKMath \
                    $(LOCAL_PATH)/SGEngine2/Core/Material \
                    $(LOCAL_PATH)/SGEngine2/Core/Textures \
                    $(LOCAL_PATH)/SGEngine2/Core/Nodes \
                    $(LOCAL_PATH)/SGEngine2/RenderManager \
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

LOCAL_PATH := $(call my-dir)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ffmpeg
include $(all-subdir-makefiles)