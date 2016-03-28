package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by Sabish.M on 11/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Play {

    Context mContext;
    RecyclerView frames;
    Timer timer;
    int visibleItmCount = 0;
    int clampInt = 0;
    Timer play_animation_timer;

    public Play(Context mContext, RecyclerView frames){
        this.mContext = mContext;
        this.frames = frames;
        this.timer = new Timer();
    }

    public void play(final boolean isPreview)
    {
        ((EditorView)((Activity)mContext)).frameAdapter.notifyDataSetChanged();
        GL2JNILib.setIsPlaying(!GL2JNILib.isPlaying());
        playAnimation();
        LinearLayoutManager g = ((LinearLayoutManager)frames.getLayoutManager());
        clampInt = 0;
        visibleItmCount = g.findLastVisibleItemPosition();

        ((EditorView)(Activity)mContext).showOrHideToolbarView(Constants.HIDE);
        if(timer == null)
            timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                            if (!GL2JNILib.isPlaying()) {
                                if(!isPreview)
                                    ((EditorView) (Activity) mContext).showOrHideToolbarView(Constants.SHOW);
                                cancel();
                            } else {
                                ((Activity)mContext).runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        highLightFrame(GL2JNILib.currentFrame());
                                    }
                                });
                            }
            }
        }, 0, 1000/24);
    }

    private void highLightFrame(final int currentPosition)
    {
        LinearLayoutManager g = ((LinearLayoutManager)frames.getLayoutManager());
        if(currentPosition >= visibleItmCount/2) clampInt = visibleItmCount/2;
        g.scrollToPosition((clampInt != 0) ? currentPosition + clampInt : currentPosition);
        View current = g.findViewByPosition(currentPosition);
        View previous = g.findViewByPosition(currentPosition - 1);
        if(previous != null) previous.setBackgroundResource(R.drawable.border_grid_frames_non_pressed);
        if(current != null) current.setBackgroundResource(R.drawable.border_grid_frames_pressed);
    }

    private void playAnimation(){
        play_animation_timer = new Timer();
        play_animation_timer.schedule(new TimerTask() {
            @Override
            public void run() {
                GL2JNILib.play();
                if (!GL2JNILib.isPlaying()) {
                    if(play_animation_timer == null) return;
                    play_animation_timer.cancel();
                    play_animation_timer = null;
                }
            }
        }, 0, 1000 / 24);
    }
}
