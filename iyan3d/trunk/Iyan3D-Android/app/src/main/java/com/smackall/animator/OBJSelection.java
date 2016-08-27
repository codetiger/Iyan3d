package com.smackall.animator;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
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
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;

import java.text.SimpleDateFormat;
import java.util.Date;
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

    public OBJSelection(Context context, DatabaseHelper db) {
        this.mContext = context;
        this.db = db;
    }

    public void showObjSelection(final int ViewMode) {
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
        gridView.setTag(ViewMode);
        infoLable.setText(((Integer.parseInt(gridView.getTag().toString()) == Constants.OBJ_MODE)) ? mContext.getString(R.string.add_obj_in_sdCard) : mContext.getString(R.string.add_texture_in_sdcard));
        initAssetGrid(gridView);
        Button cancel = (Button) objView.findViewById(R.id.cancel_obj);
        ((Button) objView.findViewById(R.id.import_btn)).setText(String.format(Locale.getDefault(), "%s", mContext.getString(R.string.import_obj)));
        ((Button) objView.findViewById(R.id.next_obj)).setText(String.format(Locale.getDefault(), "%s", mContext.getString(R.string.addtoscene)));
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
                if ((Integer.parseInt(gridView.getTag().toString())) == Constants.OBJ_MODE) {
                    getObjFromStorage();
                } else {
                    ((EditorView) mContext).imageManager.getImageFromStorage(Constants.IMPORT_OBJ);
                }
            }
        });
        objView.findViewById(R.id.next_obj).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!modelImported) return;
                addSgmToDatabase();
                insertPoint.removeAllViews();
                ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                objSelectionAdapter = null;
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

    private void addSgmToDatabase() {
        ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
        AssetsDB assetsDB = objSelectionAdapter.assetsDB;
        String fileName = assetsDB.getAssetName();
        String textureName = (assetsDB.getTexture().equals("-1")) ? "white_texture" : assetsDB.getTexture();
        int assetId = 20000 + ((db.getMYModelAssetCount() == 0) ? 1 : db.getAllMyModelDetail().get(db.getMYModelAssetCount() - 1).getID() + 1);
        String sgmFrom = (assetsDB.getAssetsId() == 123456) ? PathManager.LocalMeshFolder + "/123456.sgm" : PathManager.DefaultAssetsDir + "/" + assetsDB.getAssetsId() + ".sgm";
        String sgmTo = PathManager.LocalMeshFolder + "/" + assetId + ".sgm";
        String textureFrom = (textureName.equals("white_texture") ? PathManager.DefaultAssetsDir + "/" + textureName + ".png" : PathManager.LocalImportedImageFolder + "/" + textureName + ".png");
        String textureTo = PathManager.LocalTextureFolder + "/" + assetId + "-cm.png";
        FileHelper.copy(sgmFrom, sgmTo);
        FileHelper.copy(textureFrom, textureTo);
        ((EditorView) mContext).imageManager.makeThumbnail(textureTo, Integer.toString(assetId));
        assetsDB.setAssetsId(assetId);
        assetsDB.setDateTime(new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault()).format(new Date()));
        assetsDB.setNBones(0);
        assetsDB.setHash(FileHelper.md5(fileName));
        assetsDB.setType(Constants.NODE_SGM);
        if (assetsDB.getX() == -1 && assetsDB.getY() == -1 && assetsDB.getZ() == -1)
            assetsDB.setTexture(assetId + "-cm");
        db.addNewMyModelAssets(assetsDB);
        assetsDB.setIsTempNode(false);
        ((EditorView) mContext).renderManager.importAssets(assetsDB);
    }

    public void getObjFromStorage() {
        try {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.setType("file/*.obj");
            ((Activity) mContext).startActivityForResult(intent, Constants.OBJ_IMPORT_RESPONSE);
        } catch (ActivityNotFoundException e) {
            e.printStackTrace();
            UIHelper.informDialog(mContext, mContext.getString(R.string.filemanager_not_found));
        }
    }

    public void startActivityForResult(Intent i, int requestCode, int resultCode) {
        ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
        if (requestCode == Constants.OBJ_IMPORT_RESPONSE && resultCode == Activity.RESULT_OK && null != i) {
            String path = i.getData().getPath();
            if (path != null && !path.toLowerCase().equals("null")) {
                if (!FileHelper.checkValidFilePath(path)) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.manually_copy_obj));
                } else {
                    String ext = FileHelper.getFileExt(path);
                    if (ext.toLowerCase().endsWith("obj")) {
                        String fileName = FileHelper.getFileNameFromPath(path);
                        FileHelper.copy(path, PathManager.LocalUserMeshFolder + "/" + fileName);
                        notifyDataChanged();
                    } else {
                        UIHelper.informDialog(mContext, mContext.getString(R.string.not_valid_obj_format));
                    }
                }
            }
        }
        ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
    }
}
