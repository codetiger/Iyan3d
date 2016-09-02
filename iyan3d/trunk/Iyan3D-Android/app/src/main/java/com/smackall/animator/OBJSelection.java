package com.smackall.animator;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridView;
import android.widget.TextView;

import com.smackall.animator.Adapters.OBJSelectionAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.UIHelper;

import java.util.Locale;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelection {

    public boolean modelImported = false;
    public boolean isImporting = false;
    public GridView gridView;
    public View objView;
    Context mContext;
    DatabaseHelper db;
    OBJSelectionAdapter objSelectionAdapter;
    ViewGroup insertPoint;
    AssetsDB assetsDB = new AssetsDB();

    public OBJSelection(Context context, DatabaseHelper db) {
        this.mContext = context;
        this.db = db;
    }

    public void showObjSelection() {
        HitScreens.OBJSelectionView(mContext);
        modelImported = isImporting = false;
        ((EditorView) mContext).showOrHideToolbarView(Constants.HIDE);
        insertPoint = (((EditorView) mContext).sharedPreferenceManager.getInt(mContext, "toolbarPosition") == 1) ?
                (ViewGroup) ((Activity) mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity) mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup) insertPoint.getParent()).getChildCount(); i++) {
            if (((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup) insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        objView = vi.inflate(R.layout.obj_view, insertPoint, false);
        insertPoint.addView(objView, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        TextView infoLable = (TextView) objView.findViewById(R.id.info_lable);
        objView.findViewById(R.id.import_btn).setVisibility(View.VISIBLE);
        gridView = (GridView) objView.findViewById(R.id.obj_grid);
        infoLable.setText(mContext.getString(R.string.add_obj_in_sdCard));
        initAssetGrid(gridView);
        Button cancel = (Button) objView.findViewById(R.id.cancel_obj);
        ((Button) objView.findViewById(R.id.import_btn)).setText(String.format(Locale.getDefault(), "%s", mContext.getString(R.string.import_obj)));
        ((Button) objView.findViewById(R.id.import_model)).setText(String.format(Locale.getDefault(), "%s", mContext.getString(R.string.addtoscene)));
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (isImporting)
                    return;
                HitScreens.EditorView(mContext);
                ((EditorView) mContext).renderManager.removeTempNode();
                insertPoint.removeAllViews();
                ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                objSelectionAdapter = null;
            }
        });
        objView.findViewById(R.id.import_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getModelFromStorage();
            }
        });
        objView.findViewById(R.id.import_model).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                importModel(assetsDB.getAssetPath(), false, false);
            }
        });
    }

    private void initAssetGrid(GridView gridView) {
        objSelectionAdapter = new OBJSelectionAdapter(mContext, gridView);
        gridView.setAdapter(objSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }

    public void notifyDataChanged() {
        ((Activity) mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (objSelectionAdapter != null)
                    objSelectionAdapter.notifyDataSetChanged();
            }
        });
    }

    public void getModelFromStorage() {
        try {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);

            String[] ACCEPT_MIME_TYPES = {
                    "file/*.obj",
                    "file/*.3ds",
                    "file/*.dae",
                    "file/*.fbx"
            };

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_MIME_TYPES, ACCEPT_MIME_TYPES);
            } else
                intent.setType("file/*.*");

            ((Activity) mContext).startActivityForResult(intent, Constants.OBJ_IMPORT_RESPONSE);
        } catch (ActivityNotFoundException e) {
            e.printStackTrace();
            UIHelper.informDialog(mContext, mContext.getString(R.string.filemanager_not_found));
        }
    }

    public void startActivityForResult(final Intent i, final int requestCode, final int resultCode) {


        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (requestCode == Constants.OBJ_IMPORT_RESPONSE && resultCode == Activity.RESULT_OK && null != i) {
                    final String path = i.getData().getPath();
                    if (path != null && !path.toLowerCase().equals("null")) {
                        if (!FileHelper.checkValidFilePath(path)) {
                            UIHelper.informDialog(mContext, mContext.getString(R.string.manually_copy_obj));
                        } else {
                            String ext = FileHelper.getFileExt(path);

                            if (ext.toLowerCase().endsWith("obj") ||
                                    ext.toLowerCase().endsWith("3ds") ||
                                    ext.toLowerCase().endsWith("dae") ||
                                    ext.toLowerCase().endsWith("fbx")) {
                                ((EditorView) mContext).runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        importModel(path, false, false);
                                    }
                                });
                            } else {
                                UIHelper.informDialog(mContext, mContext.getString(R.string.not_valid_obj_format));
                            }
                        }
                    }
                }
            }
        }, 500);
    }

    public void importModel(String meshPath, boolean isTempNode, boolean isColorChanged) {
        String fileName = FileHelper.getFileNameFromPath(meshPath);

        objSelectionAdapter.objSelectedPosition = -1;
        objSelectionAdapter.notifyDataSetChanged();
        if (!isColorChanged)
            assetsDB.resetValues();
        assetsDB.setAssetPath(meshPath);
        assetsDB.setAssetName(fileName);
        assetsDB.setTexture("-1");
        assetsDB.setX(1.0f);
        assetsDB.setY(1.0f);
        assetsDB.setZ(1.0f);
        assetsDB.setHasMeshColor(false);
        assetsDB.setIsTempNode(isTempNode);
        assetsDB.setTexturePath(FileHelper.getFileLocation(meshPath) + "/");

        ((EditorView) mContext).renderManager.importAssets(assetsDB);

        if (!isTempNode) {
            insertPoint.removeAllViews();
            ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
            mContext = null;
            db = null;
            objSelectionAdapter = null;
        }
    }
}
