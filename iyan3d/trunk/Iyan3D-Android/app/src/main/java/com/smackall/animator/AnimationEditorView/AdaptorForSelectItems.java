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

import smackall.animator.R;


/**
 * Created by Sabish.M on 14/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AdaptorForSelectItems extends BaseAdapter {

        String [] selectItems;
        Context context;

        int color;
        private static LayoutInflater inflater=null;
        public AdaptorForSelectItems(Context mainActivity, String[] gridValue) {
            // TODO Auto-generated constructor stub
            selectItems = gridValue;
            context = mainActivity;
            inflater = ( LayoutInflater )context.
                    getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public int getCount() {
            // TODO Auto-generated method stub
            return selectItems.length;
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
            View rowView = inflater.inflate(R.layout.import_object_list_items, parent, false);

            switch (Constant.getScreenCategory(context)){
                case "normal":
                    rowView.getLayoutParams().height = (int) (Constant.animationEditor.height*0.075)+2;
                    break;
                case "large":
                    rowView.getLayoutParams().height = (int) (Constant.animationEditor.height*0.075)+2;
                    break;
                case "xlarge":
                    rowView.getLayoutParams().height = (int) (Constant.animationEditor.height*0.075)+2;
                    break;
                case "undefined":
                    rowView.getLayoutParams().height = (int) (Constant.animationEditor.height*0.075)+2;
                    break;
            }

            holder.textView=(TextView) rowView.findViewById(R.id.slected_item_text);
            holder.gridLayout = (LinearLayout) rowView.findViewById(R.id.animation_editor_slected_item_holder);

            holder.textView.setText(selectItems[position]);
            holder.textView.setTextColor(Color.WHITE);

            if(Constant.animationEditor.gridView.getSelectedItemPosition() == position){
                holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
                Constant.animationEditor.previousSelectedGridItem = position;
            }
            else {
                holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));
            }

            rowView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    AnimationEditor.selectItem(position, selectItems[position]);
                }
            });
            return rowView;
        }
    }


