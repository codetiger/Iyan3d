#ifndef newiyan3d_constants_h
#define newiyan3d_constants_h

#include "SGEngineCommon.h"
#include "SGEngineOGL.h"
#include "ConversionHelper.h"
#define LOG_TYPE LOG_ERROR
#define BITSPERPIXEL 16
#define LIGHT_INIT_POS_X 10.0
#define LIGHT_INIT_POS_Y 15.0
#define LIGHT_INIT_POS_Z 0.0
#define VIEW_CAM_INIT_POS_X 0.0
#define VIEW_CAM_INIT_POS_Y 15.0
#define VIEW_CAM_INIT_POS_Z 30.0
#define RENDER_CAM_INIT_POS_X 0.0
#define RENDER_CAM_INIT_POS_Y 5.0
#define RENDER_CAM_INIT_POS_Z 15.0
#define OBJ_IMPORT_IAP @"objimport"
#define VIEWCAMERA_ZOOM_START 1.89
#define CONTROLS_DEFAULT_DISTANCE 7.0
#define TOTAL_MATERIALS 31
#define SELECTION_COLOR_R 0.0
#define SELECTION_COLOR_G 1.0
#define SELECTION_COLOR_B 0.0
#define NODE_SELECTION_TRANSPARENCY 0.2
#define SGB_VERSION 201511
#define SGB_VERSION_1 201506

#define DEFAULT_LIGHT_COLOR Vector3(1.0)
#define DEFAULT_FADE_DISTANCE 300.0

#define CIRCLE_NODE_ID 511
#define OBJ_ID 512
#define SGR_ID 1024
#define BONE_START_ID 100000
#define JOINT_START_ID 200000
#define ENVELOPE_START_ID 300000
#define REFERENCE_NODE_START_ID 400000
#define JOINT_SPHERES_START_ID 500000
#define CONTROLS_START_ID 600000
#define LIGHT_STARTING_ID 900

#define DEFAULT_FONT_FILE "Hemi Head bd.ttf"

#define MAX_LIGHTS_COUNT 5
#define OBJ_MAX_VERTICES_ALLOWED 8721192
#define OBJ_CROSSED_MAX_VERTICES_LIMIT 0
#define OBJ_LOADED 1
#define OBJ_FILE_READ_ERROR 2
#define OBJ_NOT_LOADED 3

#define SGR_JOINT_DEFAULT_COLOR_R 0.6
#define SGR_JOINT_DEFAULT_COLOR_G 0.6
#define SGR_JOINT_DEFAULT_COLOR_B 1.0

#define ANIMATIONS_ID 80000

#define UV_MAPPED 1
#define VERTEX_COLORED 2

const char textCharArray[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9','_','?','*','%','(',')',':',',','$','!','.','<','#','/','@','&','^','"','>'};

const float RESOLUTION[3][2] = {
    {1920.0f,1080.0f},
    {1280.0f,720.0f},
    {720.0f,480.0f}
};


const string OGLMaterialAndShaderNames[TOTAL_MATERIALS][3] =
{
    {"SHADER_COMMON_L1","shader.vsh","commonL1.fsh"},
    {"SHADER_COMMON_L2","shader.vsh","commonL2.fsh"},
    {"SHADER_COMMON_L3","shader.vsh","commonL3.fsh"},
    {"SHADER_COMMON_L4","shader.vsh","commonL4.fsh"},
    {"SHADER_COMMON_L5","shader.vsh","commonL5.fsh"},
    {"SHADER_COMMON_SKIN_L1","GPUSkinShader.vsh","commonL1.fsh"},
    {"SHADER_COMMON_SKIN_L2","GPUSkinShader.vsh","commonL2.fsh"},
    {"SHADER_COMMON_SKIN_L3","GPUSkinShader.vsh","commonL3.fsh"},
    {"SHADER_COMMON_SKIN_L4","GPUSkinShader.vsh","commonL4.fsh"},
    {"SHADER_COMMON_SKIN_L5","GPUSkinShader.vsh","commonL5.fsh"},
    {"SHADER_COLOR", "color.vert","color.frag"},
    {"SHADER_COLOR_SKIN", "colorskin.vert","color.frag"},
    {"SHADER_DRAW_2D_IMAGE", "draw2dImage.vsh","draw2dImage.fsh"},
    {"SHADER_TOON", "shader.vsh","toon.fsh"},
    {"SHADER_TOON_SKIN","GPUSkinShader.vsh","toon.fsh"},
    {"SHADER_VERTEX_COLOR_SKIN_TOON", "vertexColorSkin.vert","vertexColorToon.frag"},
    {"SHADER_SHADOW_DEPTH_PASS","depthPass.vsh","depthPass.fsh"},
    {"SHADER_SHADOW_DEPTH_PASS_SKIN","depthPassSkin.vsh","depthPass.fsh"},
    {"SHADER_DRAW_2D_IMAGE_DEPTH", "draw2dImage.vsh","draw2dImage.fsh"},
    {"SHADER_VERTEX_COLOR_L1", "vertexColor.vert","commonL1.fsh"},
    {"SHADER_VERTEX_COLOR_L2", "vertexColor.vert","commonL2.fsh"},
    {"SHADER_VERTEX_COLOR_L3", "vertexColor.vert","commonL3.fsh"},
    {"SHADER_VERTEX_COLOR_L4", "vertexColor.vert","commonL4.fsh"},
    {"SHADER_VERTEX_COLOR_L5", "vertexColor.vert","commonL5.fsh"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L1", "vertexColorSkin.vert" , "commonL1.fsh"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L2", "vertexColorSkin.vert" , "commonL2.fsh"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L3", "vertexColorSkin.vert" , "commonL3.fsh"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L4", "vertexColorSkin.vert" , "commonL4.fsh"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L5", "vertexColorSkin.vert" , "commonL5.fsh"},
    {"SHADER_SHADOW_DEPTH_PASS_TEXT","depthPassText.vsh","depthPass.fsh"},
    {"SHADER_COLOR_TEXT", "depthPassText.vsh","color.frag"}
};
const string MTLMaterialAndShaderNames[TOTAL_MATERIALS][3] =
{
    {"SHADER_COMMON_L1","Common_Vertex","Common_Fragment_L1"},
    {"SHADER_COMMON_L2","Common_Vertex","Common_Fragment_L2"},
    {"SHADER_COMMON_L3","Common_Vertex","Common_Fragment_L3"},
    {"SHADER_COMMON_L4","Common_Vertex","Common_Fragment_L4"},
    {"SHADER_COMMON_L5","Common_Vertex","Common_Fragment_L5"},
    {"SHADER_COMMON_SKIN_L1","Common_Skin_Vertex","Common_Fragment_L1"},
    {"SHADER_COMMON_SKIN_L2","Common_Skin_Vertex","Common_Fragment_L2"},
    {"SHADER_COMMON_SKIN_L3","Common_Skin_Vertex","Common_Fragment_L3"},
    {"SHADER_COMMON_SKIN_L4","Common_Skin_Vertex","Common_Fragment_L4"},
    {"SHADER_COMMON_SKIN_L5","Common_Skin_Vertex","Common_Fragment_L5"},
    {"SHADER_COLOR", "Color_Vertex","Color_Fragment"},
    {"SHADER_COLOR_SKIN","Color_Skin_Vertex","Color_Fragment"},
    {"SHADER_DRAW_2D_IMAGE", "Draw2DImage_Vertex","Draw2DImage_Fragment"},
    {"SHADER_TOON", "Common_Vertex","Common_Toon_Fragment"},
    {"SHADER_TOON_SKIN","Common_Skin_Vertex","Common_Toon_Fragment"},
    {"SHADER_VERTEX_COLOR_SKIN_TOON","Per_Vertex_Color_Skin","Common_Fragment_L1"},
    {"SHADER_SHADOW_DEPTH_PASS","Depth_Pass_vert",""},
    {"SHADER_SHADOW_DEPTH_PASS_SKIN","Depth_Pass_Skin_vert",""},
    {"SHADER_DRAW_2D_IMAGE_DEPTH", "Draw2DImage_Vertex","Draw2DImage_Fragment_Depth"},
    {"SHADER_VERTEX_COLOR_L1","Per_Vertex_Color","Common_Fragment_L1"},
    {"SHADER_VERTEX_COLOR_L2","Per_Vertex_Color","Common_Fragment_L2"},
    {"SHADER_VERTEX_COLOR_L3","Per_Vertex_Color","Common_Fragment_L3"},
    {"SHADER_VERTEX_COLOR_L4","Per_Vertex_Color","Common_Fragment_L4"},
    {"SHADER_VERTEX_COLOR_L5","Per_Vertex_Color","Common_Fragment_L5"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L1","Per_Vertex_Color_Skin","Common_Fragment_L1"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L2","Per_Vertex_Color_Skin","Common_Fragment_L2"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L3","Per_Vertex_Color_Skin","Common_Fragment_L3"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L4","Per_Vertex_Color_Skin","Common_Fragment_L4"},
    {"SHADER_VERTEX_COLOR_SHADOW_SKIN_L5","Per_Vertex_Color_Skin","Common_Fragment_L5"},
    {"SHADER_SHADOW_DEPTH_PASS_TEXT","Depth_Pass_Text_vert",""},
    {"SHADER_COLOR_TEXT","Color_Skin_Vertex","Color_Fragment"}
};


const Vector3 ENVELOPE_BASE_DIRECTION = Vector3(0.0,1.0,0.0);
const Vector3 BONE_BASE_DIRECTION = Vector3(0.0,1.0,0.0);

enum VIEW_TYPE {
    AUTO_RIG,
    ANIMATION,
    SCENE_SELECTION = 2,
    ASSET_SELECTION = 3,
    ASSET_DETAIL = 4,
    ALL_ANIMATION_VIEW = 5,
    MY_ANIMATION_VIEW = 6,
    TEXT_VIEW = 7
};
enum JOINT_ID{
    JOINT_PIVOT = 0,
    JOINT_HIP = 1,
    JOINT_R_HAND = 9,
    JOINT_L_HAND = 28,
    JOINT_L_LEG = 53,
    JOINT_R_LEG = 48,
    HUMAN_JOINTS_SIZE = 54
};
const int IK_JOINTS[] = {JOINT_R_HAND, JOINT_L_HAND, JOINT_R_LEG, JOINT_L_LEG};

enum NODE_TYPE
{
    NODE_CAMERA = 0,
    NODE_LIGHT = 1,
    NODE_SGM = 2,
    NODE_TEXT = 4,
    NODE_IMAGE = 5,
    NODE_OBJ = 6,
    NODE_RIG = 3,
    NODE_UNDEFINED = -1,
    NODE_BITS = 4,
    NODE_ADDITIONAL_LIGHT = 7
};

enum Slidername
{
    BRIGHTNESS_SLIDER=2,
    SPECULAR_SLIDER=3,
    RED_SLIDER=4,
    GREEN_SLIDER=5,
    BLUE_SLIDER=6,
    SHAWDOW_SLIDER=7,
    FOV_SLIDER=8
};

enum MATERIAL_TYPE
{
    SHADER_COMMON_L1,
    SHADER_COMMON_L2,
    SHADER_COMMON_L3,
    SHADER_COMMON_L4,
    SHADER_COMMON_L5,
    SHADER_COMMON_SKIN_L1,
    SHADER_COMMON_SKIN_L2,
    SHADER_COMMON_SKIN_L3,
    SHADER_COMMON_SKIN_L4,
    SHADER_COMMON_SKIN_L5,
    SHADER_COLOR,
    SHADER_COLOR_SKIN,
    SHADER_DRAW_2D_IMAGE,
    SHADER_TOON,
    SHADER_TOON_SKIN,
    SHADER_VERTEX_COLOR_SKIN_TOON,
    SHADER_SHADOW_DEPTH_PASS,
    SHADER_SHADOW_DEPTH_PASS_SKIN,
    SHADER_DRAW_2D_IMAGE_DEPTH,
    SHADER_VERTEX_COLOR_L1,
    SHADER_VERTEX_COLOR_L2,
    SHADER_VERTEX_COLOR_L3,
    SHADER_VERTEX_COLOR_L4,
    SHADER_VERTEX_COLOR_L5,
    SHADER_VERTEX_COLOR_SHADOW_SKIN_L1,
    SHADER_VERTEX_COLOR_SHADOW_SKIN_L2,
    SHADER_VERTEX_COLOR_SHADOW_SKIN_L3,
    SHADER_VERTEX_COLOR_SHADOW_SKIN_L4,
    SHADER_VERTEX_COLOR_SHADOW_SKIN_L5,
    SHADER_SHADOW_DEPTH_PASS_TEXT,
    SHADER_COLOR_TEXT
};
enum ActionType
{
    ACTION_EMPTY = -1,
    //Common actions
    ACTION_CHANGE_MIRROR_STATE,
    
    //SGSceneActions
    ACTION_CHANGE_NODE_KEYS,
    ACTION_CHANGE_JOINT_KEYS,
    ACTION_SWITCH_FRAME,
    ACTION_CHANGE_PROPERTY_MESH,
    ACTION_CHANGE_PROPERTY_LIGHT,
    ACTION_CHANGE_PROPERTY_CAMERA,
    ACTION_CHANGE_NODE_JOINT_KEYS,
    //AutoRigSceneActions
    ACTION_SWITCH_MODE,
    ACTION_CHANGE_SKELETON_KEYS,
    ACTION_CHANGE_ENVELOPE_SCALE,
    ACTION_CHANGE_SGR_KEYS,
    
    ACTION_NODE_ADDED,
    ACTION_NODE_DELETED,
    ACTION_TEXT_IMAGE_ADD,
    ACTION_TEXT_IMAGE_DELETE,
    ACTION_APPLY_ANIM,
    ACTION_ADD_JOINT,
    //Mesh setup actions
    IMPORT_ASSET_ACTION,
    UNDO_ACTION,
    REDO_ACTION,
    UNDO_REDO_ACTION,
    OPEN_SAVED_FILE,
    OTHER_ACTION
};

enum ACTION_TYPE
{
    DO_NOTHING,
    DELETE_ASSET,
    ADD_ASSET_BACK,
    DEACTIVATE_UNDO,
    DEACTIVATE_REDO,
    DEACTIVATE_BOTH,
    ACTIVATE_BOTH,
    ADD_TEXT_IMAGE_BACK,
    SWITCH_FRAME,
    RELOAD_FRAMES,
    SWITCH_MIRROR
};

enum ResolutionType
{
    
};
enum Controlls
{
    X_MOVE = 0,
	Y_MOVE,
	Z_MOVE,
	X_ROTATE,
	Y_ROTATE,
	Z_ROTATE,
    X_SCALE,
    Y_SCALE,
    Z_SCALE
};
enum CONTROL_TYPE
{
    MOVE = 0,
	ROTATE,
    SCALE
};
enum LoggingType
{
    LOG_WARN,
    LOG_NONE,
    LOG_INFO,
    LOG_ERROR,
    LOG_ALL
};
enum Defaults
{
	NOT_SELECTED = -1,
    SCENE_THUMBNAIL = 1,
    RENDER_IMAGE = 2
};

enum ENVELOPE_JOINT_ID
{
    ENVELOPE_TOP_JOINT_ID=2,
    ENVELOPE_BOTTOM_JOINT_ID=3
};

enum AUTORIG_TYPE{
    RIG_OBJ                         = 1<<0,
    RIG_SKELETON                = 1<<1,
    RIG_SGR                         = 1<<2,
    RIG_REFERENCE              = 1<<3,
    RIG_SKELETON_SPHERE   = 1<<4,
    RIG_BONE                       = 1<<5,
    RIG_ENVELOPE                = 1<<6,
    RIG_SGR_SPHERE            = 1<<7,
    RIG_MASK_BITS              = 8
};

enum AUTORIG_SCENE_MODE{
    RIG_MODE_OBJVIEW,
    RIG_MODE_MOVE_JOINTS,
    RIG_MODE_EDIT_ENVELOPES,
    RIG_MODE_PREVIEW
};

enum MIRROR_SWITCH_STATE{
    MIRROR_OFF = 0,
    MIRROR_ON
};

enum CAMERA_VIEW_MODE{
    VIEW_TOP = 0,
    VIEW_RIGHT,
    VIEW_BOTTOM,
    VIEW_LEFT,
    VIEW_FRONT,
    VIEW_BACK
};
enum BUTTON_TYPES{
    SCROLL_FIRST_FRAME,
    SCROLL_LAST_FRAME
};
enum SLIDER_VIEW_ID{
    OBJECT_SELECTION,
    CAMERA_PROPERTY,
    MESH_PROPERTY,
    LIGHT_PROPERTY,
    SCALE_PROPERTY,
    VARIOUS_VIEW
};
enum  IK_JOINT_LIST {
    HAND_LEFT = 28,
    HAND_RIGHT = 9,
    LEG_LEFT = 48,
    LEG_RIGHT = 53,
    LEG_RIGHT1 = 46,
    LEG_LEFT1 = 51,
    HEAD = 5,
    ANKLE = 52,
    HIP = 1
};
enum assetType
{
    ALLMODELS = 0,
    CHARACTERS = 1,
    BACKGROUNDS,
    ACCESSORIES,
    ANIMATIONS,
    ASSET_LIBRARY = 4,
    MY_ANIMATIONS = 5,
    OBJ_FILE = 6,
    FONT = 50,
    MY_LIBRARY_TYPE = -1,
    MY_ANIMATIONS_LIBRARY_TYPE = -2   
    
};

enum Constants
{
    SLOW_INTERNET,
    NO_INTERNET
};

enum OBJ_LOADER_ERROR{
    OBJ_NULL,
    OBJ_HIGH_POLY
};

enum USER_ACTION{
    NO_ACTION = -1,
    SCENE_BEGINS,
    OBJECT_IMPORTED,
    OBJECT_IMPORTED_HUMAN,
    OBJECT_SELECTED,
    OBJECT_SELECTED_HUMAN,
    OBJECT_SELECTED_LIGHT,
    OBJECT_SELECTED_CAMERA,
    OBJECT_MOVED,
    OBJECT_RIG_MOVED,
    JOINT_SELECTED,
    JOINT_MOVED,
    ANIMATION_APPLIED,
    CAMERA_PROPS,
    LIGHT_PROPS,
    MESH_PROPS,
    CAM_PERSPECTIVE,
    SCALE_ACTION,
    SELECT_OBJECT,
    FRAMES_HELP,
    SCENE_PLAYING,
    UNDO_PRESSED,
    REDO_PRESSED,
    CAM_LIGHT_DELETE,
    DELETE_PRESSED
};

enum ANIMATION_TYPE {
    RIG_ANIMATION = 0,
    TEXT_ANIMATION = 1
};

enum OPERATION_TYPE {
    DOWNLOAD_AND_WRITE,
    DOWNLOAD_AND_READ,
    DOWNLOAD_AND_WRITE_IMAGE,
    DOWNLOAD_AND_RETURN_OBJ
};

class constants{
    
public:
    static string BundlePath;
    static string CachesStoragePath;
    static string DocumentsStoragePath;
    static string BundlePathRig;
    static string CachesStoragePathRig;
    static string impotedImagesPathAndroid;
    static string DocumentsStoragePathRig;
    static float iOS_Version;
    static Vector3 sgrJointDefaultColor;
    static Vector3 selectionColor;
};

#endif
