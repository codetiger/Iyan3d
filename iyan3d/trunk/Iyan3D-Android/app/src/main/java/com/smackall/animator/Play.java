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
    boolean setFrameCompleted = true;
    boolean isSceneHaveVideo = false;

    public Play(Context mContext, RecyclerView frames){
        this.mContext = mContext;
        this.frames = frames;
        this.timer = new Timer();
    }

    public void updatePhysics(final boolean isPreview)
    {
        setFrameCompleted = true;
        checkSceneContainsVideo();
        ((EditorView)((Activity)mContext)).frameAdapter.notifyDataSetChanged();
        ((EditorView)mContext).showOrHideLoading(Constants.SHOW);
        ((EditorView)mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setIsPlaying(!GL2JNILib.isPlaying(),((EditorView)mContext).nativeCallBacks);
                ((EditorView)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        play(isPreview);
                        ((EditorView) ((Activity) mContext)).showOrHideLoading(Constants.HIDE);
                        if(isPreview && ((EditorView)mContext).animationSelection!= null)
                            ((EditorView)mContext).animationSelection.processCompleted = true;
                    }
                });
            }
        });
    }

    public void play(final boolean isPreview)
    {
        playAnimation();
        LinearLayoutManager g = ((LinearLayoutManager)frames.getLayoutManager());
        clampInt = 0;
        visibleItmCount = g.findLastVisibleItemPosition();
        g.scrollToPosition(GL2JNILib.currentFrame());

        ((EditorView)(Activity)mContext).showOrHideToolbarView(Constants.HIDE);
        if(timer == null)
            timer = new Timer();
        else {
            timer.cancel();
            timer = null;
            timer = new Timer();
        }
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                            if (!GL2JNILib.isPlaying()) {
                                cancel();
                                if(!isPreview)
                                    ((EditorView) (Activity) mContext).showOrHideToolbarView(Constants.SHOW);
                            } else if(setFrameCompleted) {
                                ((Activity)mContext).runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                            highLightFrame(GL2JNILib.currentFrame());
                                    }
                                });
                            }
            }
        }, 0, (isSceneHaveVideo) ? 1000 : 1000/24);
    }

    public void highLightFrame(int currentPosition)
    {
        try {
            LinearLayoutManager g = ((LinearLayoutManager) frames.getLayoutManager());
            View previous = g.findViewByPosition(currentPosition - 1);
            View current = g.findViewByPosition(currentPosition);
            if (previous != null)
                previous.setBackgroundResource(R.drawable.border_grid_frames_non_pressed);
            else if (mContext != null && ((EditorView) (Activity) mContext).frameAdapter != null) {
                ((EditorView) (Activity) mContext).frameAdapter.notifyDataSetChanged();
            }
            if (current != null)
                current.setBackgroundResource(R.drawable.border_grid_frames_pressed);
            else if (mContext != null && ((EditorView) (Activity) mContext).frameAdapter != null) {
                ((EditorView) (Activity) mContext).frameAdapter.notifyDataSetChanged();
            }
            if (currentPosition >= visibleItmCount / 2) clampInt = visibleItmCount / 2;
            g.scrollToPosition((clampInt != 0) ? currentPosition + clampInt : currentPosition);
        }
        catch (IndexOutOfBoundsException e){e.printStackTrace();
        }
    }

    private void playAnimation(){
        if(play_animation_timer == null)
            play_animation_timer = new Timer();
        else {
            play_animation_timer.cancel();
            play_animation_timer = null;
            play_animation_timer = new Timer();
        }
        play_animation_timer.schedule(new TimerTask() {
            @Override
            public void run() {
                ((EditorView)(Activity)mContext).glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        if(setFrameCompleted) {
                            setFrameCompleted = false;
                            setFrameCompleted = GL2JNILib.play(((EditorView)mContext).nativeCallBacks);
                        }
                    }
                });
                if (!GL2JNILib.isPlaying()) {
                    ((Activity)mContext).runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if(mContext != null && ((EditorView)(Activity)mContext) != null && ((EditorView)(Activity)mContext).frameAdapter != null)
                                ((EditorView)(Activity)mContext).frameAdapter.notifyDataSetChanged();
                        }
                    });
                    if(play_animation_timer == null) return;
                    play_animation_timer.cancel();
                    play_animation_timer = null;
                }
            }
        }, 0, (isSceneHaveVideo) ? 1000 : 1000/24);
    }

    private void checkSceneContainsVideo()
    {
        for(int i = 2; i < GL2JNILib.getNodeCount(); i++){
            if(GL2JNILib.getNodeType(i) == Constants.NODE_VIDEO){
                isSceneHaveVideo = true;
                break;
            }
            else{
                isSceneHaveVideo = false;
            }
        }
    }
}
