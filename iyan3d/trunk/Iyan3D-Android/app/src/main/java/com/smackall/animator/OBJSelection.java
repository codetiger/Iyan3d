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

import com.google.android.gms.analytics.Tracker;
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

    Context mContext;
    DatabaseHelper db;
    OBJSelectionAdapter objSelectionAdapter;
    ViewGroup insertPoint;
    public boolean modelImported = false;
    private Tracker mTracker;

    public OBJSelection(Context context,DatabaseHelper db){
        this.mContext = context;
        this.db = db;
    }

    public void showObjSelection(final int ViewMode)
    {
        HitScreens.OBJSelectionView(mContext);
        modelImported = false;
        ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.HIDE);
        insertPoint = (((EditorView)((Activity)mContext)).sharedPreferenceManager.getInt(mContext,"toolbarPosition") == 1 ) ?
                (ViewGroup) ((Activity)mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity)mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup)insertPoint.getParent()).getChildCount(); i++){
            if(((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup)insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        final View objView = vi.inflate(R.layout.obj_view,insertPoint,false);
        insertPoint.addView(objView, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        final TextView infoLable = (TextView)objView.findViewById(R.id.info_lable);
        ((Button)objView.findViewById(R.id.import_btn)).setVisibility(View.VISIBLE);
        final GridView gridView = (GridView)objView.findViewById(R.id.obj_grid);
        gridView.setTag(ViewMode);
        infoLable.setText((gridView.getTag() == Constants.OBJ_MODE) ? "Add OBJ files in SD-Card/Iyan3D foler." : "Add Texture files in SD-Card/Iyan3D folder.");
        initAssetGrid(gridView);
        Button cancel = (Button)objView.findViewById(R.id.cancel_obj);
        ((Button)objView.findViewById(R.id.import_btn)).setText(String.format(Locale.getDefault(),"%s","IMPORT OBJ"));
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
                ((EditorView) ((Activity) mContext)).renderManager.removeTempNode();
                insertPoint.removeAllViews();
                ((EditorView) ((Activity) mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                objSelectionAdapter = null;
            }
        });
        objView.findViewById(R.id.import_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(gridView.getTag() == Constants.OBJ_MODE){
                    getObjFromStorage();
                }
                else {
                    ((EditorView) ((Activity) mContext)).imageManager.getImageFromStorage(Constants.IMPORT_OBJ);
                }
            }
        });
        objView.findViewById(R.id.next_obj).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (gridView.getTag() == Constants.OBJ_MODE) {
                    if(!modelImported) return;
                    HitScreens.EditorView(mContext);
                    gridView.setTag(Constants.TEXTURE_MODE);
                    ((Button) v).setText(String.format(Locale.getDefault(),"%s","Add To Scene"));
                    objSelectionAdapter.files = null;
                    objSelectionAdapter.notifyDataSetChanged();
                    ((Button)objView.findViewById(R.id.import_btn)).setText(String.format(Locale.getDefault(),"%s","IMPORT IMAGE"));
                    ((TextView)objView.findViewById(R.id.info_lable)).setVisibility(View.GONE);
                } else {
                    addSgmToDatabase();
                    insertPoint.removeAllViews();
                    ((EditorView) ((Activity) mContext)).showOrHideToolbarView(Constants.SHOW);
                    mContext = null;
                    db = null;
                    objSelectionAdapter = null;
                }
            }
        });
    }

    private void initAssetGrid(GridView gridView)
    {
        objSelectionAdapter = new OBJSelectionAdapter(mContext,gridView);
        gridView.setAdapter(objSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }

    public void notifyDataChanged(){
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(objSelectionAdapter!= null)
                    objSelectionAdapter.notifyDataSetChanged();
            }
        });
    }

    private void addSgmToDatabase()
    {
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        AssetsDB assetsDB = objSelectionAdapter.assetsDB;
        String fileName = assetsDB.getAssetName();
        String textureName = (assetsDB.getTexture().equals("-1")) ? "white_texture" : assetsDB.getTexture();
        int assetId = 20000 + ((db.getMYModelAssetCount() == 0) ? 1 : db.getAllMyModelDetail().get(db.getMYModelAssetCount()-1).getID());
        String sgmFrom = (assetsDB.getAssetsId() == 123456) ? PathManager.LocalMeshFolder+"/123456.sgm" : PathManager.DefaultAssetsDir+"/"+assetsDB.getAssetsId()+".sgm";
        String sgmTo = PathManager.LocalMeshFolder+"/"+ assetId+".sgm";
        String textureFrom = (textureName.equals("white_texture") ? PathManager.DefaultAssetsDir+"/"+textureName+".png" : PathManager.LocalImportedImageFolder+"/"+textureName+".png");
        String textureTo = PathManager.LocalMeshFolder+"/"+assetId+"-cm.png";
        FileHelper.copy(sgmFrom, sgmTo);
        FileHelper.copy(textureFrom, textureTo);
        ((EditorView)((Activity)mContext)).imageManager.makeThumbnail(textureTo, Integer.toString(assetId));
        assetsDB.setAssetsId(assetId);
        assetsDB.setDateTime(new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault()).format(new Date()));
        assetsDB.setNbones(0);
        assetsDB.setHash(FileHelper.md5(fileName));
        assetsDB.setType(Constants.NODE_SGM);
        if(assetsDB.getX() == -1 && assetsDB.getY() == -1&& assetsDB.getZ() == -1)
            assetsDB.setTexture(assetId+"-cm");
        db.addNewMyModelAssets(assetsDB);
        assetsDB.setIsTempNode(false);
        ((EditorView)((Activity)mContext)).renderManager.importAssets(assetsDB,false);
    }

    public void getObjFromStorage()
    {
        try {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.setType("file/*.obj");
            ((Activity) mContext).startActivityForResult(intent, Constants.OBJ_IMPORT_RESPONSE);
        }
        catch (ActivityNotFoundException e){
            e.printStackTrace();
            UIHelper.informDialog(mContext,"File manager not found please copy your obj file into SdCard/Iyan3D folder.");
        }
    }

    public void startActivityForResult(Intent i, int requestCode,int resultCode)
    {
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        if (requestCode == Constants.OBJ_IMPORT_RESPONSE && resultCode == Activity.RESULT_OK && null != i) {
            String path = i.getData().getPath();
            if(path != null && !path.toLowerCase().equals("null")){
                if(!FileHelper.checkValidFilePath(path)){
                    UIHelper.informDialog(mContext,"Something wrong. Please manually copy your obj file to SdCard/Iyan3D folder.");
                }
                else{
                    String ext = FileHelper.getFileExt(path);
                    if(ext.toLowerCase().endsWith("obj")){
                        String fileName = FileHelper.getFileNameFromPath(path);
                        FileHelper.copy(path,PathManager.LocalUserMeshFolder+"/"+fileName);
                        notifyDataChanged();
                    }
                    else{
                        UIHelper.informDialog(mContext,"File is not a valid obj format.");
                    }
                }
            }
        }
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
    }
}
