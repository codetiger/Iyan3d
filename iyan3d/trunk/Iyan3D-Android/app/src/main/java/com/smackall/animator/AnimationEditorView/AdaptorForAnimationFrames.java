package com.smackall.animator.AnimationEditorView;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNIView;

import smackall.animator.R;


/**
 * Created by Sabish on 01/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AdaptorForAnimationFrames extends BaseAdapter {

    Context context;
    String type;
    int totalLength;

    private static LayoutInflater inflater=null;
    public AdaptorForAnimationFrames(Context mainActivity, int totalLength, String type) {
        // TODO Auto-generated constructor stub
        context = mainActivity;
        inflater = ( LayoutInflater )context.
                getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        this.type = type;
        this.totalLength = totalLength;
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return totalLength;
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    public static class Holder
    {
        TextView textView;
        LinearLayout gridLayout;
    }
    @Override
    public View getView(final int position, final View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
        final Holder holder=new Holder();
        View rowView = inflater.inflate(R.layout.animation_editor_frames, parent, false);
        if(Constant.screenInch > 5.5)
            rowView.getLayoutParams().width = (int) ((Constant.animationEditor.width*0.55)/11);
        else
            rowView.getLayoutParams().width = (int) ((Constant.animationEditor.width*0.55)/6);
        rowView.getLayoutParams().height = Constant.animationEditor.time_layout.getHeight();
        holder.textView=(TextView) rowView.findViewById(R.id.frame_text);
        holder.gridLayout = (LinearLayout) rowView.findViewById(R.id.animation_editor_frame_holder);

        if(type.equals("frames"))
            holder.textView.setText(String.valueOf(position+1));
        else if(type.equals("duration")){
            float value = (float) ((position+1)/24.0);
            holder.textView.setText(String.format("%.2f", value )+"s");
        }

        holder.textView.setTextSize(Constant.animationEditor.width / 85);
        holder.textView.setTextColor(Color.WHITE);


        if(Constant.animationEditor.selectedNodeId != -1 && GL2JNILib.isKeySetForNode(position) ) {
            if(Constant.animationEditor.selectedGridItem == position)
                holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.is_key_set_false));
                else
            holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.is_key_set_true));
        }
        else {
            if(Constant.animationEditor.selectedGridItem == position)
                holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
            else
                holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));
        }

        rowView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (position != Constant.animationEditor.previousSelectedGridItem) {
//                    System.out.println("Grid View First Value : " + gridValueText[0]);


                    Constant.animationEditor.selectedGridItem = position;
                    View gridSelected = Constant.animationEditor.gridView.getChildAt(Constant.animationEditor.selectedGridItem);
                    LinearLayout gridHolder = (LinearLayout) gridSelected.findViewById(R.id.animation_editor_frame_holder);

                    if(Constant.animationEditor.selectedNodeId != -1 && GL2JNILib.isKeySetForNode(Constant.animationEditor.selectedGridItem) ) {
                        gridHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.is_key_set_false));
                    }else{
                        gridHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
                    }

                    View gridPreviousSelected = Constant.animationEditor.gridView.getChildAt(Constant.animationEditor.previousSelectedGridItem);
                    LinearLayout gridHolderPreviousItem = (LinearLayout) gridPreviousSelected.findViewById(R.id.animation_editor_frame_holder);
                    if(Constant.animationEditor.selectedNodeId != -1 && GL2JNILib.isKeySetForNode(Constant.animationEditor.previousSelectedGridItem) ) {
                        gridHolderPreviousItem.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.is_key_set_true));
                    }else
                        gridHolderPreviousItem.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));

                    Constant.animationEditor.previousSelectedGridItem = position;
                }
                System.out.println("Clicked Frame: "+position);
                Constant.animationEditor.selectedGridItem = position;
                if(GL2JNIView.isSurfaceCreated())
                GL2JNILib.frameClicked(position);
            }
        });
        return rowView;
    }
}
