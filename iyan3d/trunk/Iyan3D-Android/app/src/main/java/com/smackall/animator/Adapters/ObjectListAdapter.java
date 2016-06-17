package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.support.v4.content.ContextCompat;
import android.util.SparseBooleanArray;
import android.view.ActionMode;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.SimpleGestureFilter;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;


/**
 * Created by Sabish.M on 10/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class ObjectListAdapter extends BaseAdapter implements SimpleGestureFilter.SimpleGestureListener,AbsListView.MultiChoiceModeListener {

    Context mContext;
    public SimpleGestureFilter detector;
    public ListView listView;
    public int objectCount = 0;
    private SparseBooleanArray mSelectedIds;
    private SparseBooleanArray mEditedIds;
    public boolean isMultiSelectEnable;
    private int selectedPosition = -1;

    public ObjectListAdapter(Context context,ListView listView,int objectCount,int isMultiSelectEnable) {
        this.mContext = context;
        this.listView = listView;
        this.objectCount = objectCount;
        detector = new SimpleGestureFilter(((Activity)mContext),ObjectListAdapter.this);
        mSelectedIds = new SparseBooleanArray();
        mEditedIds = new SparseBooleanArray();
        this.isMultiSelectEnable = (isMultiSelectEnable == 1);
    }

    @Override
    public int getCount() {
        return objectCount;
    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        final View grid;
        if (convertView == null) {
            LayoutInflater inflater = ((Activity) mContext).getLayoutInflater();
            grid = inflater.inflate(R.layout.object_list_cell, parent, false);

        } else {
            grid = (View) convertView;
        }
        ((ViewGroup) ((ViewGroup) grid).getChildAt(2)).setBackgroundColor(ContextCompat.getColor(mContext,R.color.white));

        if(GL2JNILib.isNodeSelected(position))
            ((ViewGroup) ((ViewGroup) grid).getChildAt(2)).setBackgroundColor(ContextCompat.getColor(mContext,R.color.selectList));
        if(isMultiSelectEnable && mSelectedIds.size() > 1) {
        }
        else {
            if(selectedPosition == position && mEditedIds.get(position,false))
                ((ViewGroup)grid).getChildAt(2).setX(((ViewGroup)((ViewGroup)grid).getChildAt(1)).getChildAt(1).getWidth()*-1);
            else
                ((ViewGroup)grid).getChildAt(2).setX(0);
        }
        TextView object_lable = (TextView) grid.findViewById(R.id.object_lable);
        ImageView object_img = (ImageView) grid.findViewById(R.id.object_img);
        ImageView delete_btn = (ImageView) grid.findViewById(R.id.delete_btn);

        switch (GL2JNILib.getNodeType(position)){
            case Constants.NODE_VIDEO:
            case Constants.NODE_CAMERA:
                object_img.setImageResource(R.drawable.my_object_camera);
                break;
            case Constants.NODE_ADDITIONAL_LIGHT:
            case Constants.NODE_LIGHT:
                object_img.setImageResource(R.drawable.my_object_light);
                break;
            case Constants.NODE_RIG:
            case Constants.NODE_SGM:
            case Constants.NODE_OBJ:
                object_img.setImageResource(R.drawable.my_object_model);
                break;
            case Constants.NODE_IMAGE:
                object_img.setImageResource(R.drawable.my_object_image);
                break;
            case Constants.NODE_PARTICLES:
                object_img.setImageResource(R.drawable.my_object_particle);
                break;
            case Constants.NODE_TEXT_SKIN:
            case Constants.NODE_TEXT:
                    object_img.setImageResource(R.drawable.my_object_text);
                    break;
        }

        object_lable.setText(GL2JNILib.getNodeName(position));
        ((ViewGroup)grid).getChildAt(2).setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                detector.onTouchEvent(event, position, grid);
                return true;
            }
        });
        delete_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mEditedIds.clear();
                mSelectedIds.clear();
                selectedPosition = -1;
                ((EditorView) ((Activity) mContext)).glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.removeNode(position,false);
                        objectCount = GL2JNILib.getNodeCount();
                        ((EditorView)mContext).runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                notifyDataSetChanged();
                            }
                        });
                    }
                });
            }
        });
        return grid;
    }

    @Override
    public void onSwipe(int direction,int position,View view) {
        float slideValue = 0;
        if(!(!isMultiSelectEnable || mSelectedIds.size() <= 1))
            return;
        if(position < 2) return;
        switch (direction) {
            case SimpleGestureFilter.SWIPE_LEFT :
                mEditedIds.clear();
                selectedPosition = position;
                mEditedIds.put(position,true);
                slideValue =  ((ViewGroup)((ViewGroup)view).getChildAt(1)).getChildAt(1).getWidth()*-1;
                break;
        }

        for(int i = 0; i < this.listView.getChildCount(); i++) {
            if(this.listView.getChildAt(i) == null) {
                continue;
            }
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR1) {
                ((ViewGroup)this.listView.getChildAt(i)).getChildAt(2).setBackgroundColor(ContextCompat.getColor(mContext, R.color.white));
                ((ViewGroup)this.listView.getChildAt(i)).getChildAt(2).animate().translationX(0);
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR1) {
            if(!isMultiSelectEnable || mSelectedIds.size() <= 1) {
                if(slideValue != 0)
                    ((ViewGroup)view).getChildAt(2).setBackgroundColor(ContextCompat.getColor(mContext, R.color.selectList));
                ((ViewGroup) view).getChildAt(2).animate().translationX(slideValue);
            }
            else {
//                for (int i = 0; i < this.listView.getChildCount(); i++) {
//                    if (this.listView.getChildAt(i) == null)
//                        continue;
//                    if(mSelectedIds.get(i,false)) {
//                        if (this.listView.getChildAt(i).isActivated())
//                            ((ViewGroup) this.listView.getChildAt(i)).getChildAt(2).animate().translationX(slideValue);
//                    }
//                    else
//                        ((ViewGroup) this.listView.getChildAt(i)).getChildAt(2).animate().translationX(0);
//                }
            }
        }
    }

    @Override
    public void onSingleTap(int position) {

        mEditedIds.clear();
        if(isMultiSelectEnable)
            toggleSelection(position);

        selectedPosition = position;
        ((EditorView)((Activity)mContext)).renderManager.selectObject(position);
        notifyDataSetChanged();
    }

    @Override
    public void onItemCheckedStateChanged(ActionMode mode, int position, long id, boolean checked) {

    }

    @Override
    public boolean onCreateActionMode(ActionMode mode, Menu menu) {
        return false;
    }

    @Override
    public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
        return false;
    }

    @Override
    public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
        return false;
    }

    @Override
    public void onDestroyActionMode(ActionMode mode) {

    }

    public void toggleSelection(int position) {
        selectView(position, !mSelectedIds.get(position));
    }

    public void removeSelection() {
        mSelectedIds.clear();
        notifyDataSetChanged();
    }

    public void selectView(int position, boolean value) {
        if (value)
            mSelectedIds.put(position, value);
        else {
            mSelectedIds.delete(position);
        }
        notifyDataSetChanged();
    }
}