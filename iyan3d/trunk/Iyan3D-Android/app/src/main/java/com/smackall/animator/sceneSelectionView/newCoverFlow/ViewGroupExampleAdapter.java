package com.smackall.animator.sceneSelectionView.newCoverFlow;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Environment;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.smackall.animator.common.Constant;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

/**
 * Created by android-developer on 9/9/15.
 */
public class ViewGroupExampleAdapter extends SceneSelectionCoverFlowAdapter {

    // =============================================================================
    // Private members
    // =============================================================================

    public static String[] sceneImages;
    public static String[] sceneNames;
    public static String[] sceneCreatedTime;

    // =============================================================================
    // Supertype overrides
    // =============================================================================
    public int getCount() {
        int imageLength;
        if(sceneImages != null)
            imageLength = sceneImages.length;
        else
            imageLength = 0;
        return imageLength;
    }

    @Override
    public Object getItem(int position) {
        return position;
    }

    // @Override
   // public Integer getItem(int i) {
   //     return sceneImages[i];
   // }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getCoverFlowItem(final int i, final View reuseableView, final ViewGroup viewGroup) {
        CustomViewGroup customViewGroup = null;

        if (reuseableView != null) {
            customViewGroup = (CustomViewGroup) reuseableView;
        } else {
            customViewGroup = new CustomViewGroup(viewGroup.getContext());
            customViewGroup.setLayoutParams(new SceneSelectionCoverFlow.LayoutParams(Constant.width/4, (int) (Constant.height/1.5)));
        }

        File imagefile = new File(Environment.getExternalStorageDirectory() + "/Android/data/com.smackall.animator/scenes/"+sceneImages[i]+".png");
        FileInputStream fis = null;
        try
        {
            fis = new FileInputStream(imagefile);
        }
        catch (FileNotFoundException e1)
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }

        Bitmap bi = BitmapFactory.decodeStream(fis);
        Bitmap roundCornerBitmap = Constant.getRoundedCornerBitmap(bi);
        if(roundCornerBitmap != null)
        customViewGroup.getImageView().setImageBitmap(roundCornerBitmap);
        else
            customViewGroup.getImageView().setImageBitmap(bi);
        customViewGroup.getSceneName().setText(sceneNames[i]);
        customViewGroup.getSceneTime().setText(sceneCreatedTime[i]);

        return customViewGroup;
    }

    private static class CustomViewGroup extends LinearLayout {

        // =============================================================================
        // Child views
        // =============================================================================

        private TextView sceneName;

        private ImageView imageView;

        private TextView sceneCreatedTime;

        // =============================================================================
        // Constructor
        // =============================================================================

        private CustomViewGroup(Context context) {
            super(context);

            this.setOrientation(VERTICAL);

            this.sceneName = new TextView(context);
            this.imageView = new ImageView(context);
            this.sceneCreatedTime = new TextView(context);

            LayoutParams layoutParams = new LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT);
            layoutParams.gravity = Gravity.CENTER_VERTICAL;

            this.sceneName.setLayoutParams(layoutParams);
            this.sceneName.setGravity(Gravity.CENTER);
            this.sceneName.setTextColor(Color.WHITE);
            this.sceneName.setTextSize(Constant.width / 100);
            this.sceneName.setPadding(0, Constant.height / 8, 0, Constant.height / 100);
            this.sceneName.setMaxLines(1);
            this.sceneName.setEllipsize(TextUtils.TruncateAt.END);
            this.addView(this.sceneName);

            this.imageView.setLayoutParams(layoutParams);
            this.imageView.setScaleType(ImageView.ScaleType.MATRIX);
            this.imageView.setAdjustViewBounds(true);
            this.addView(this.imageView);

            this.sceneCreatedTime.setLayoutParams(layoutParams);
            this.sceneCreatedTime.setGravity(Gravity.CENTER);
            this.sceneCreatedTime.setTextColor(Color.WHITE);
            this.sceneCreatedTime.setTextSize(Constant.width / 100);
            this.sceneCreatedTime.setPadding(0, Constant.height / 100, 0, 0);
            this.addView(this.sceneCreatedTime);
        }

        // =============================================================================
        // Getters
        // =============================================================================

        private TextView getSceneName() {
            return sceneName;
        }
        private TextView getSceneTime() {
            return sceneCreatedTime;
        }
        private ImageView getImageView() {
            return imageView;
        }
    }
}

