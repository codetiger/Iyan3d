package com.smackall.iyan3dPro.Adapters;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.smackall.iyan3dPro.EditorView;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.R;
import com.smackall.iyan3dPro.opengl.GL2JNILib;

import java.util.Locale;


/**
 * Created by Sabish.M on 5/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FrameAdapter extends RecyclerView.Adapter<FrameViewHolder> {

    private Context mContext;
    private ImageView referenceImage;

    public FrameAdapter(Context context, ImageView referenceImage) {
        this.mContext = context;
        this.referenceImage = referenceImage;
    }

    // Create new views (invoked by the layout manager)
    @Override
    public FrameViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(mContext).inflate(R.layout.editorscene_frames, parent, false);
        view.getLayoutParams().width = this.referenceImage.getWidth();
        view.getLayoutParams().height = this.referenceImage.getHeight();
        return new FrameViewHolder(mContext, view);
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(FrameViewHolder viewHolder, int position) {
        final int finalPosition = viewHolder.getAdapterPosition();
        viewHolder.view.setBackgroundResource((GL2JNILib.currentFrame() == finalPosition) ? R.drawable.border_grid_frames_pressed : R.drawable.border_grid_frames_non_pressed);
        if (GL2JNILib.editorScene() && GL2JNILib.hasNodeSelected()) {
            if (GL2JNILib.isHaveKey(finalPosition) && (GL2JNILib.currentFrame() == finalPosition))
                viewHolder.view.setBackgroundResource(R.drawable.key_pressed);
            else if (GL2JNILib.isHaveKey(finalPosition))
                viewHolder.view.setBackgroundResource(R.drawable.key_nonpressed);
        }

        if (((EditorView) mContext).sharedPreferenceManager.getInt(mContext, "frameType") == Constants.FRAME_COUNT)
            viewHolder.tvName.setText(String.valueOf(finalPosition + 1));
        else {
            float value = (float) ((finalPosition + 1) / 24.0);
            viewHolder.tvName.setText(String.format(Locale.getDefault(), "%.2f%s", value, "s"));
        }

        viewHolder.view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!((EditorView) mContext).isDisplayPrepared) return;
                if (GL2JNILib.isPlaying()) return;
                ((EditorView) mContext).glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        if (GL2JNILib.currentFrame() == finalPosition || GL2JNILib.isPlaying())
                            return;
                        GL2JNILib.setCurrentFrame(finalPosition, ((EditorView) mContext).nativeCallBacks);
                        //GL2JNILib.switchFrame();
                    }
                });
                notifyDataSetChanged();
            }
        });
    }

    @Override
    public int getItemCount() {
        return GL2JNILib.totalFrame();
    }
}
