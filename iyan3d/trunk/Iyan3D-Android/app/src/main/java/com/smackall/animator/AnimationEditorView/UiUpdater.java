package com.smackall.animator.AnimationEditorView;

import android.content.Context;
import android.view.View;
import android.widget.LinearLayout;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;

import smackall.animator.R;


/**
 * Created by Sabish.M on 14/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class UiUpdater {

    public static void frameUpdater(String firstOrlast, Context context){

        if (firstOrlast.equals("first")){
            View gridFirst = Constant.animationEditor.gridView.getChildAt(0);
            LinearLayout gridFirstHolder = (LinearLayout) gridFirst.findViewById(R.id.animation_editor_frame_holder);
            if(gridFirstHolder != null)
                gridFirstHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
            System.out.println("Current Grid Value : " + Constant.animationEditor.selectedGridItem);
            View gridPrevious = Constant.animationEditor.gridView.getChildAt(Constant.animationEditor.selectedGridItem);
            LinearLayout gridPreviousHolder = null;
            if(gridPrevious != null)
                    gridPreviousHolder = (LinearLayout) gridPrevious.findViewById(R.id.animation_editor_frame_holder);
            if(gridPreviousHolder != null)
                gridPreviousHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));
        } else {
            View gridLast =Constant.animationEditor.gridView.getChildAt(Constant.animationEditor.frameCount-1);
            LinearLayout gridLastHolder = null;
            if(gridLast != null)
                gridLastHolder = (LinearLayout) gridLast.findViewById(R.id.animation_editor_frame_holder);
            if(gridLastHolder != null)
                gridLastHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
        }
    }

    public static void switchFrameUI(int current, Context context){

            View gridFirst = Constant.animationEditor.gridView.getChildAt(current);
            LinearLayout gridFirstHolder = (LinearLayout) gridFirst.findViewById(R.id.animation_editor_frame_holder);
            if(gridFirstHolder != null)
                gridFirstHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
            System.out.println("Current Grid Value : " + Constant.animationEditor.selectedGridItem);
            View gridPrevious = Constant.animationEditor.gridView.getChildAt(Constant.animationEditor.selectedGridItem);
            LinearLayout gridPreviousHolder = null;
            if(gridPrevious != null)
                gridPreviousHolder = (LinearLayout) gridPrevious.findViewById(R.id.animation_editor_frame_holder);
            if(gridPreviousHolder != null)
                gridPreviousHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));

            Constant.animationEditor.selectedGridItem = current;
            assert gridFirstHolder != null;
            Constant.animationEditor.horizontalScrollView.smoothScrollTo((int) gridFirstHolder.getX(), 0);
            GL2JNILib.frameClicked(current);

    }

    }

