package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.PopupMenu;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

import java.io.File;
import java.io.FilenameFilter;


/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelectionAdapter extends BaseAdapter {

    public File[] files;
    private int previousPosition = -1;
    private Context mContext;
    private GridView gridView;
    private String[] basicShapes = {"Cone", "Cube", "Cylinder", "Plane", "Sphere", "Torus"};
    private int[] basicShapesId = {60001, 60002, 60003, 60004, 60005, 60006};
    private int[] basicShapesDrawableId = {R.drawable.cone, R.drawable.cube, R.drawable.cylinder, R.drawable.plane, R.drawable.sphere, R.drawable.torus};
    public int objSelectedPosition = 0;

    public OBJSelectionAdapter(Context c, GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        this.files = getFileList();
        return ((this.files != null && this.files.length > 0) ? files.length : 0) + 6;
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
            grid = inflater.inflate(R.layout.asset_cell, parent, false);
        } else
            grid = convertView;

        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                grid.getLayoutParams().height = this.gridView.getHeight() / 4;
                break;
            default:
                grid.getLayoutParams().height = this.gridView.getHeight() / 5;
                break;
        }

        grid.findViewById(R.id.progress_bar).setVisibility(View.INVISIBLE);
        if (position <= 5) {
            ((ImageView) grid.findViewById(R.id.thumbnail)).setImageResource(basicShapesDrawableId[position]);
            ((TextView) grid.findViewById(R.id.assetLable)).setText(basicShapes[position]);
            grid.findViewById(R.id.props_btn).setVisibility(View.INVISIBLE);
        } else if (position > 5) {
            grid.findViewById(R.id.props_btn).setVisibility(View.VISIBLE);
            grid.findViewById(R.id.props_btn).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    showObjModelProps(v, position);
                }
            });

            if (files[position - 6].exists()) {
                int drawable = (files[position - 6]).getAbsolutePath().toLowerCase().endsWith(".3ds") ? R.drawable.threeds :
                        (files[position - 6]).getAbsolutePath().toLowerCase().endsWith(".dae") ? R.drawable.dae :
                                (files[position - 6]).getAbsolutePath().toLowerCase().endsWith(".fbx") ? R.drawable.fbx :
                                        R.drawable.obj;
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageResource(drawable);
                ((TextView) grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position - 6].toString())));
            }
        }

        grid.setBackgroundResource(0);
        grid.setBackgroundColor(ContextCompat.getColor(mContext, R.color.cellBg));
        if (previousPosition != -1 && position == previousPosition) {
            grid.setBackgroundResource(R.drawable.cell_highlight);
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                previousPosition = position;
                notifyDataSetChanged();
                    objSelectedPosition = position;
                String meshPath = position > 5 ? files[position - 6].toString() : PathManager.DefaultAssetsDir + "/" + basicShapesId[position] + ".sgm";
                ((EditorView) mContext).objSelection.importModel(meshPath, true, false);
            }
        });

        return grid;
    }

    private File[] getFileList() {
        FileHelper.getObjAndTextureFromCommonIyan3dPath(Constants.OBJ_MODE);
        final String userFolder = PathManager.LocalUserMeshFolder + "/";
        final File f = new File(userFolder);
        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return (filename.toLowerCase().endsWith(".obj") ||
                        filename.toLowerCase().endsWith(".3ds") ||
                        filename.toLowerCase().endsWith(".dae") ||
                        filename.toLowerCase().endsWith(".fbx"));
            }
        };
        return f.listFiles(filenameFilter);
    }

    private void showObjModelProps(View view, final int position) {
        final PopupMenu popup = new PopupMenu(mContext, view);
        popup.getMenuInflater().inflate(R.menu.my_model_menu, popup.getMenu());
        popup.getMenu().getItem(0).setVisible(false);
        popup.getMenu().getItem(1).setVisible(false);
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                int i = item.getOrder();
                if (i == Constants.ID_DELETE) {
                    FileHelper.deleteFilesAndFolder(files[position - 6].toString());
                            if (objSelectedPosition == position) {
                                ((EditorView) mContext).renderManager.removeTempNode();
                                ((EditorView) mContext).objSelection.modelImported = false;
                            }
                        files = null;
                        notifyDataSetChanged();
                }
                return true;
            }
        });
        popup.show();
    }
}
