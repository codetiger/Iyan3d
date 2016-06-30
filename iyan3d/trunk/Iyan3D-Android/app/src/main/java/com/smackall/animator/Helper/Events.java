package com.smackall.animator.Helper;

import android.content.Context;

import com.crashlytics.android.answers.Answers;
import com.crashlytics.android.answers.CustomEvent;

/**
 * Created by Sabish.M on 4/6/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Events {

    public static void appStart(Context context)
    {
        Answers.getInstance().logCustom(new CustomEvent("AppStart"));
    }

    public static void loadingCompleted(Context context,int duration)
    {
        Answers.getInstance().logCustom(new CustomEvent("LoadingComplete").putCustomAttribute("duration",duration));
    }

    public static void createNewSceneInTopLeft(Context context)
    {
        Answers.getInstance().logCustom(new CustomEvent("CreateNewSceneInTopLeft"));
    }

    public static void createNewSceneInCollectionView(Context context)
    {
        Answers.getInstance().logCustom(new CustomEvent("CreateNewSceneInCollectionView"));
    }

    public static void sceneOpenedFirstTime(Context context,String isFirstTimeUser)
    {
        Answers.getInstance().logCustom(new CustomEvent("SceneOpenedFirstTime").putCustomAttribute("FirstTimeUser",isFirstTimeUser));
    }

    public static void importEvents(Context mContext,int order){
        switch (order) {
            case Constants.IMPORT_MODELS:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("Models","Import"));
                break;
            case Constants.IMPORT_IMAGES:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("Images","Import"));
                break;
            case Constants.IMPORT_VIDEOS:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("Videos","Import"));
                break;
            case Constants.IMPORT_TEXT:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("Text","Import"));
                break;
            case Constants.IMPORT_LIGHT:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("Light","Import"));
                break;
            case Constants.IMPORT_OBJ:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("OBJ","Import"));
                break;
            case Constants.IMPORT_ADD_BONE:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("AddBone","Import"));
                break;
            case Constants.IMPORT_PARTICLE:
                Answers.getInstance().logCustom(new CustomEvent("ImportAction").putCustomAttribute("Particles","Import"));
                break;
        }
    }

    public static void exportEvents(Context mContext,int order){
        switch (order) {
            case Constants.EXPORT_IMAGES:
                Answers.getInstance().logCustom(new CustomEvent("ExportAction").putCustomAttribute("Image","Export"));
                break;
            case Constants.EXPORT_VIDEO:
                Answers.getInstance().logCustom(new CustomEvent("ExportAction").putCustomAttribute("Video","Export"));
                break;
        }
    }

    public static void backToScene(Context context){
        Answers.getInstance().logCustom(new CustomEvent("BackToScenes"));
    }

    public static void exportNextAction(Context mContext){
        Answers.getInstance().logCustom(new CustomEvent("ExportNextAction"));
    }

    public static void exportCancelAction(Context mContext) {
        Answers.getInstance().logCustom(new CustomEvent("ExportCancelAction"));
    }

    public static void rigCompletedEvent(Context mContext){

        Answers.getInstance().logCustom(new CustomEvent("AutoRig-Completion"));
    }
    public static void shareEvent(Context mContext,int type){

        Answers.getInstance().logCustom(new CustomEvent("ShareOnPreview").putCustomAttribute((type == Constants.VIDEO) ? "Video" :"Image","Share"));
    }
}
