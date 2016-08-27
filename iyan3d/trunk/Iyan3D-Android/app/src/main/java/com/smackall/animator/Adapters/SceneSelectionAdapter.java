package com.smackall.animator.Adapters;

import android.content.Context;
import android.graphics.BitmapFactory;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.R;
import com.smackall.animator.SceneSelection;

import java.util.List;


/**
 * Created by Sabish.M on 5/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SceneSelectionAdapter extends RecyclerView.Adapter<SceneFrameHolder> {

    private static final int ID_RENAME = 0;
    private static final int ID_DELETE = 1;
    private static final int ID_CLONE = 2;
    private static final int ID_BACKUP = 3;
    public List<SceneDB> sceneDBs;
    public View firstScene;
    public View firstSceneProps;
    public View newScene;
    private Context mContext;
    private RecyclerView gridView;

    public SceneSelectionAdapter(Context c, DatabaseHelper db, RecyclerView gridView) {
        mContext = c;
        this.gridView = gridView;
        this.sceneDBs = db.getAllScenes();
    }

    @Override
    public SceneFrameHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(mContext).inflate(R.layout.scene_selection_cell, parent, false);
        int gridViewWidth = this.gridView.getWidth();
        view.getLayoutParams().width = (gridViewWidth / 5);
        view.getLayoutParams().height = (gridViewWidth / 5);
        return new SceneFrameHolder(mContext, view);
    }

    @Override
    public void onBindViewHolder(SceneFrameHolder holder, int positio) {

        ImageView imageView = (ImageView) holder.view.findViewById(R.id.imagepart);
        TextView textView = (TextView) holder.view.findViewById(R.id.textpart);
        ImageView sceneProps = (ImageView) holder.view.findViewById(R.id.scene_props_btn);
        textView.setText((sceneDBs == null || sceneDBs.size() == holder.getAdapterPosition()) ? "" : sceneDBs.get(holder.getAdapterPosition()).getName());
        if (sceneDBs == null || (sceneDBs.size() == holder.getAdapterPosition())) {
            newScene = holder.view;
            imageView.setImageResource(R.drawable.new_scene_grid);
            sceneProps.setVisibility(View.INVISIBLE);
        } else if (sceneDBs != null && sceneDBs.size() > 0) {
            imageView.setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalScenesFolder + "/" + sceneDBs.get(holder.getAdapterPosition()).getImage() + ".png"));
            sceneProps.setVisibility(View.VISIBLE);
            if (holder.getAdapterPosition() == 0) {
                firstSceneProps = sceneProps;
                firstScene = holder.view;
            }
        }
        final int finalPosition = holder.getAdapterPosition();
        holder.view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                addNewScene(finalPosition);
            }
        });

        sceneProps.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openSceneProps(v, finalPosition, (sceneDBs != null && sceneDBs.size() > 0 && FileHelper.checkValidFilePath(PathManager.LocalProjectFolder + "/" + sceneDBs.get(finalPosition).getImage() + ".i3d")));
            }
        });
    }

    @Override
    public int getItemCount() {
        return (sceneDBs != null && sceneDBs.size() > 0) ? sceneDBs.size() + 1 : 1;
    }

    private void addNewScene(int position) {
        if (sceneDBs == null || (sceneDBs.size() == position)) {
            ((SceneSelection) (mContext)).addNewScene(null);
        } else
            ((SceneSelection) (mContext)).loadScene(position);
    }

    private void openSceneProps(View v, final int position, boolean validForBackup) {
        PopupMenu popup = new PopupMenu(mContext, v);
        popup.getMenuInflater().inflate(R.menu.scene_props_menu, popup.getMenu());
        popup.getMenu().getItem(3).setVisible(validForBackup);
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
                    case ID_BACKUP:
                        if (sceneDBs.size() > position)
                            ((SceneSelection) (mContext)).backUp(sceneDBs.get(position).getImage() + ".i3d");
                        break;
                }
                return true;
            }
        });
        popup.show();
    }
}

