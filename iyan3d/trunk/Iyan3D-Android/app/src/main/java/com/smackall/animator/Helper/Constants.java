package com.smackall.animator.Helper;

import android.content.Context;
import android.os.Environment;
import android.os.StatFs;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;

import java.io.File;
import java.io.IOException;

/**
 * Created by Sabish.M on 4/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Constants {
    public final static int ASSET_JSON = 1;
    public final static int ANIMATION_JSON = 2;
    public static int width;
    public static int height;
    public final static int SCREEN_UNDEFINED = -1;
    public final static int SCREEN_NORMAL = 0;
    public final static int SCREEN_LARGE = 1;
    public final static int SCREEN_XLARGE = 2;
    public final static int ONE_FRAME = 1;
    public final static int TWENTY_FOUR_FRAME = 24;
    public final static int TWO_FORTY_FRAME = 240;
    public final static int FRONT_VIEW = 0;
    public final static int TOP_VIEW = 1;
    public final static int LEFT_VIEW = 2;
    public final static int BACK_VIEW = 3;
    public final static int RIGHT_VIEW = 4;
    public final static int BOTTOM_VIEW = 5;

    public final static int IMPORT_MODELS = 0;
    public final static int IMPORT_IMAGES = 1;
    public final static int IMPORT_VIDEOS = 2;
    public final static int IMPORT_TEXT = 3;
    public final static int IMPORT_LIGHT = 4;
    public final static int IMPORT_OBJ = 5;
    public final static int IMPORT_ADD_BONE = 6;
    public final static int IMPORT_PARTICLE = 7;

    public final static int APPLY_ANIMATION = 0;
    public final static int SAVE_ANIMATION = 1;

    public final static int EXPORT_IMAGES = 0;
    public final static int EXPORT_VIDEO = 1;

    public final static int HIDE = 0;
    public final static int SHOW = 1;
    public final static int LOW = 0;
    public final static int NORMAL = 1;
    public final static int HIGH = 2;

    public static int VIEW_TYPE = 0;
    public final static int EDITOR_VIEW = -1;
    public final static int ASSET_VIEW = 0;
    public final static int ANIMATION_VIEW = 1;
    public final static int TEXT_VIEW = 2;
    public final static int PARTICLE_VIEW = 13;
    public final static int RENDERING_VIEW = 14;
    public final static int SETTINGS_VIEW = 15;
    public final static int OBJ_VIEW = 16;
    public final static int OBJ_TEXTURE = 17;
    public final static int CHANGE_TEXTURE = 18;
    public final static int IMAGE_VIEW = 19;
    public final static int AUTO_RIG_VIEW = 20;

    public final static int MYANIMATION_TABLE = 7;
    public final static int MYANIMATION_DOWNLOAD = 4;
    public final static int MYANIMATION_RATING = 6;
    public final static int MYANIMATION_FEATURED = 5;
    public final static int MYANIMATION_RECENT = 8;
    public final static int MYANIMATION_ALL = 9;

    public final static int FRAME_COUNT = 0;
    public final static int FRAME_DURATION = 1;

    public final static int TOOLBAR_LEFT = 0;
    public final static int TOOLBAR_RIGHT = 1;

    public final static int PREVIEW_SMALL = 0;
    public final static int PREVIEW_LARGE = 1;

    public final static int PREVIEW_LEFT_BOTTOM = 0;
    public final static int PREVIEW_LEFT_TOP = 1;
    public final static int PREVIEW_RIGHT_BOTTOM = 2;
    public final static int PREVIEW_RIGHT_TOP = 3;

    public final static int CONE = 60001;
    public final static int CUBE = 60002;
    public final static int CYLINDER = 60003;
    public final static int PLANE = 60004;
    public final static int SPHERE = 60005;
    public final static int TORUS = 60006;

    public final static int THOUSAND_EIGHTY = 0;
    public final static int SEVEN_TWENTY = 1;
    public final static int FOUR_EIGHTY = 2;
    public final static int THREE_SIXTY = 3;
    public final static int TWO_FOURTY = 4;

    public final static int OBJ_MODE = 0;
    public final static int TEXTURE_MODE = 1;
    public final static int CHANGE_TEXTURE_MODE = 3;

    public final static int NODE_CAMERA = 0;
    public final static int NODE_LIGHT = 1;
    public final static int NODE_SGM = 2;
    public final static int NODE_TEXT_SKIN = 4;
    public final static int NODE_IMAGE = 5;
    public final static int NODE_OBJ = 6;
    public final static int NODE_RIG = 3;
    public final static int NODE_UNDEFINED = -1;
    public final static int NODE_BITS = 4;
    public final static int NODE_ADDITIONAL_LIGHT = 7;
    public final static int NODE_TEXT = 8;
    public final static int NODE_VIDEO = 9;
    public final static int NODE_PARTICLES = 10;

    public final static int LONG_PRESS = 1;
    public final static int  HUMAN_JOINTS_SIZE = 54;

    public final static int ASSET_TEXT_RIG = 10;
    public final static int ASSET_TEXT = 11;

    public final static int IMAGE_IMPORT_RESPONSE = 20;
    public final static int OBJ_IMPORT_RESPONSE = 21;
    public final static int VIDEO_IMPORT_RESPONSE = 22;

    public final static int RIG_MODE_OBJVIEW = 0;
    public final static int RIG_MODE_MOVE_JOINTS = 1;
    public final static int RIG_MODE_EDIT_ENVELOPES = 2;
    public final static int RIG_MODE_PREVIEW = 3;
    public final static int OWN_RIGGING = 0;
    public final static int HUMAN_RIGGING = 1;

    public final static int NOT_SELECTED = -1;

    public final static int DO_NOTHING = 0;
    public final static int DELETE_ASSET = 1;
    public final static int ADD_ASSET_BACK = 2;
    public final static int DEACTIVATE_UNDO = 3;
    public final static int DEACTIVATE_REDO = 4;
    public final static int DEACTIVATE_BOTH = 5;
    public final static int ACTIVATE_BOTH = 6;
    public final static int ADD_TEXT_IMAGE_BACK = 7;
    public final static int SWITCH_FRAME = 8;
    public final static int RELOAD_FRAMES = 9;
    public final static int SWITCH_MIRROR = 10;
    public final static int ADD_MULTI_ASSET_BACK = 11;
    public final static int DELETE_MULTI_ASSET = 12;
    public final static int ADD_INSTANCE_BACK = 13;

    public final static int ACTION_EMPTY = -1;
    public final static int ACTION_CHANGE_MIRROR_STATE = 0;
    public final static int ACTION_CHANGE_NODE_KEYS = 1;
    public final static int ACTION_CHANGE_JOINT_KEYS = 2;
    public final static int ACTION_SWITCH_FRAME = 3;
    public final static int ACTION_CHANGE_PROPERTY_MESH = 4;
    public final static int ACTION_CHANGE_PROPERTY_LIGHT = 5;
    public final static int ACTION_CHANGE_PROPERTY_CAMERA = 6;
    public final static int ACTION_CHANGE_NODE_JOINT_KEYS = 7;
    public final static int ACTION_CHANGE_MULTI_NODE_KEYS = 8;
    public final static int ACTION_SWITCH_MODE = 9;
    public final static int ACTION_CHANGE_SKELETON_KEYS = 10;
    public final static int ACTION_CHANGE_ENVELOPE_SCALE = 11;
    public final static int ACTION_CHANGE_SGR_KEYS = 12;
    public final static int ACTION_NODE_ADDED = 13;
    public final static int ACTION_MULTI_NODE_ADDED = 14;
    public final static int ACTION_NODE_DELETED = 15;
    public final static int ACTION_MULTI_NODE_DELETED_AFTER = 16;
    public final static int ACTION_MULTI_NODE_DELETED_BEFORE = 17;
    public final static int ACTION_TEXT_IMAGE_ADD = 18;
    public final static int ACTION_TEXT_IMAGE_DELETE = 19;
    public final static int ACTION_APPLY_ANIM = 20;
    public final static int ACTION_ADD_JOINT = 21;
    public final static int ACTION_ADD_BONE = 22;
    public final static int ACTION_SGR_CREATED = 23;
    public final static int ACTION_TEXTURE_CHANGE = 24;
    public final static int IMPORT_ASSET_ACTION = 25;
    public final static int UNDO_ACTION = 26;
    public final static int REDO_ACTION = 27;
    public final static int UNDO_REDO_ACTION = 28;
    public final static int OPEN_SAVED_FILE = 29;
    public final static int OTHER_ACTION = 30;

    public final static int NORMAL_SHADER = 0;
    public final static int TOON_SHADER = 1;
    public final static int CLOUD = 2;

    public static String deviceUniqueId = "";

    public final static int TASK_PROGRESS = 0;
    public final static int TASK_COMPLETED = 1;

    public final static int GOOGLE_SIGNIN = 1;
    public final static int FACEBOOK_SIGNIN = 2;
    public final static int TWITTER_SIGNIN = 3;
    public final static int GOOGLE_SIGNIN_REQUESTCODE = 141;

    public static int currentActivity = -1;

    public static int NONE = 0;
    public static int STATIC = 1;
    public static int LIGHT = 2;
    public static int MEDIUM = 3;
    public static int HEAVY = 4;
    public static int CLOTH = 5;
    public static int BALLOON = 6;
    public static int JELLY = 7;
    
    public static int POINT = 0;
    public static int DIRECTIONAL = 1;

    public final static int REQUEST_STORAGE = 100;
    public final static int REQUEST_INTERNET = 101;
    public final static int REQUEST_GET_ACCOUNTS = 102;
    public final static int REQUEST_WAKELOCK = 103;

    public final static int STORAGE = 100;
    public final static int INTERNET = 101;
    public final static int GET_ACCOUNTS = 102;
    public final static int WAKELOCK = 103;

    public final static int IMAGE = 0;
    public final static int VIDEO = 1;

    public static boolean isFirstTimeUser = false;

    public static boolean isPremium(Context mContext) {
        File key = new File("data/data/"+mContext.getPackageName()+"/shared_prefs/key");
        try {
            if(!key.exists()) {
                return  false;
            }
            else if (KeyMaker.readFromKeyFile(mContext) == null) {
                return false;
            }
            else {
                try {
                    if (KeyMaker.readFromKeyFile(mContext).equals(FileHelper.md5(deviceUniqueId + FileHelper.md5("Iyan3dPremium" + 1)))) {
                        return true;
                    } else {
                        return false;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static boolean checkPlayServices(Context mContext) {
        int resultCode = GooglePlayServicesUtil
                .isGooglePlayServicesAvailable(mContext);
        return resultCode == ConnectionResult.SUCCESS;
    }

    public static float getFreeSpace() {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2) {
            StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getPath());
            long bytesAvailable = 0;
            bytesAvailable = (long)stat.getBlockSizeLong() * (long)stat.getAvailableBlocksLong();
            return bytesAvailable / (1024.f * 1024.f);
        }
        else{
            File path = Environment.getExternalStorageDirectory();
            StatFs stat = new StatFs(path.getPath());
            long blockSize = stat.getBlockSize();
            long availableBlocks = stat.getAvailableBlocks();
            return (availableBlocks * blockSize)/ (1024.f * 1024.f);
        }
    }
}
