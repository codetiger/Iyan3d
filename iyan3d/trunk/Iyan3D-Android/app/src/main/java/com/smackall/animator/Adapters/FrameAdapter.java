package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.R;


/**
 * Created by Sabish.M on 5/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FrameAdapter extends RecyclerView.Adapter<FrameViewHolder> {

    private Context mContext;
    private ImageView referenceImage;
    private int currentFrame = 0;

    public FrameAdapter(Context context,ImageView referenceImage) {
        this.mContext = context;
        this.referenceImage = referenceImage;
    }
    // Create new views (invoked by the layout manager)
    @Override
    public FrameViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(mContext).inflate(R.layout.editorscene_frames, parent, false);
        view.getLayoutParams().width = this.referenceImage.getWidth();
        view.getLayoutParams().height = this.referenceImage.getHeight();
        FrameViewHolder viewHolder = new FrameViewHolder(mContext, view);
        return viewHolder;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(FrameViewHolder viewHolder, final int position) {
        viewHolder.view.setBackgroundResource((currentFrame == position) ?  R.drawable.border_grid_frames_pressed : R.drawable.border_grid_frames_non_pressed);
        if(((EditorView)((Activity)(mContext))).sharedPreferenceManager.getInt(mContext,"frameType") == Constants.FRAME_COUNT)
            viewHolder.tvName.setText(String.valueOf(position+1));
        else {
            float value = (float) ((position+1)/24.0);
            viewHolder.tvName.setText(String.format("%.2f", value )+"s");
        }

        viewHolder.view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(currentFrame == position) return;
                currentFrame = position;
                notifyDataSetChanged();
            }
        });
    }

    @Override
    public int getItemCount() {
        return ((EditorView)((Activity)mContext)).totalFrames;
    }
}
