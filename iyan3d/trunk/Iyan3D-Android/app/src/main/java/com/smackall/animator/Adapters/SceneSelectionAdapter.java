package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.PopupMenu;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.R;
import com.smackall.animator.SceneSelection;

import java.util.List;


/**
 * Created by Sabish.M on 5/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SceneSelectionAdapter extends BaseAdapter {

    private Context mContext;
    private DatabaseHelper db;
    private GridView gridView;
    public List<SceneDB> sceneDBs;

    private static final int ID_RENAME  = 0;
    private static final int ID_DELETE = 1;
    private static final int ID_CLONE = 2;

    public SceneSelectionAdapter(Context c,DatabaseHelper db,GridView gridView) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.sceneDBs = this.db.getAllScenes();

    }

    @Override
    public int getCount() {
        return (sceneDBs != null && sceneDBs.size() > 0) ? sceneDBs.size()+1 : 1;
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
        View grid;

        if(convertView==null){
            grid = new View(mContext);
            LayoutInflater inflater=((Activity)mContext).getLayoutInflater();
            grid=inflater.inflate(R.layout.scene_selection_cell, parent, false);
        }else{
            grid = (View)convertView;
        }
        int gridViewWidth = this.gridView.getWidth();
        grid.getLayoutParams().width = (gridViewWidth/5);
        grid.getLayoutParams().height= (gridViewWidth/5);

        ImageView imageView = (ImageView)grid.findViewById(R.id.imagepart);
        TextView textView = (TextView)grid.findViewById(R.id.textpart);
        ImageView sceneProps = (ImageView)grid.findViewById(R.id.scene_props_btn);
        imageView.setImageResource(R.drawable.new_scene_grid);
        textView.setText((sceneDBs.size() == position) ? "" : sceneDBs.get(position).getName());

        if((sceneDBs.size() == position))
            sceneProps.setVisibility(View.INVISIBLE);
        else
            sceneProps.setVisibility(View.VISIBLE);

            grid.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            addNewScene(position);
            }
        });

        sceneProps.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openSceneProps(v,position);
            }
        });
        return grid;
    }

    private void addNewScene(int position){
        if((sceneDBs.size() == position)) {
            ((SceneSelection) (mContext)).addNewScene(null);
        }
        else
            ((SceneSelection) (mContext)).loadScene(position);
    }

    private void openSceneProps(View v, final int position){
        PopupMenu popup = new PopupMenu(mContext, v);
        popup.getMenuInflater().inflate(R.menu.scene_props_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case ID_RENAME:
                        ((SceneSelection) (mContext)).showRenameDialog(position);
                        break;
                    case ID_DELETE:
                        ((SceneSelection) (mContext)).deleteScene(position);
                        break;
                    case ID_CLONE:
                        ((SceneSelection) (mContext)).cloneScene(position);
                        break;
                }
                return true;
            }
        });
        popup.show();
    }
}

