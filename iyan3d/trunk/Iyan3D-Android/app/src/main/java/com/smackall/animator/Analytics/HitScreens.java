package com.smackall.animator.Analytics;

import android.app.Activity;
import android.content.Context;

import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Tracker;
import com.smackall.animator.Helper.Constants;

/**
 * Created by Sabish.M on 5/5/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */

public class HitScreens {

    public static void LoadingView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Loading View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void SettingsView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Settings View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void AutoRigView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("AutoRig View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void VideoSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Video Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void SceneSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Scene Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void EditorView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Editor View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void AssetsSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Assets Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void AnimationSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Animation Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void ImageSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Image Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void OBJSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("OBJ Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void TextSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Text Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void TextureSelectionView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Texture Selection View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void CameraPropsView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Camera Props View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void MeshPropsView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Mesh Props View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void LightPropsView(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Light Props View - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void ExportVideo(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("Exporting Video... - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }

    public static void Preview(Context mContext,int type)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName(((type == Constants.IMAGE) ? "Image Preview" : "Video Preview")+ " - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }
    public static void UserOnBoarding(Context mContext)
    {
        AnalyticsApplication application = (AnalyticsApplication) ((Activity)mContext).getApplication();
        Tracker mTracker = application.getDefaultTracker();
        mTracker.setScreenName("User On Boarding"+ " - Android");
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }
}
