package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridView;
import android.widget.TextView;

import com.smackall.animator.Adapters.OBJSelectionAdapter;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelection {

    Context mContext;
    DatabaseHelper db;
    OBJSelectionAdapter objSelectionAdapter;
    ViewGroup insertPoint;
    public OBJSelection(Context context,DatabaseHelper db){
        this.mContext = context;
        this.db = db;
    }

    public void showObjSelection(final int ViewMode)
    {
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
        ((Button)objView.findViewById(R.id.import_btn)).setVisibility(View.GONE);
        final GridView gridView = (GridView)objView.findViewById(R.id.obj_grid);
        gridView.setTag(ViewMode);
        infoLable.setText((gridView.getTag() == Constants.OBJ_MODE) ? "Add OBJ files in SD-Card/Iyan3D foler." : "Add Texture files in SD-Card/Iyan3D folder.");
        initAssetGrid(gridView);
        Button cancel = (Button)objView.findViewById(R.id.cancel_obj);

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
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
                ((EditorView)((Activity)mContext)).imageManager.getImageFromStorage(Constants.IMPORT_OBJ);
            }
        });
        objView.findViewById(R.id.next_obj).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (gridView.getTag() == Constants.OBJ_MODE) {
                    gridView.setTag(Constants.TEXTURE_MODE);
                    ((Button) v).setText("Add To Scene");
                    objSelectionAdapter.files = null;
                    objSelectionAdapter.notifyDataSetChanged();
                    ((TextView)objView.findViewById(R.id.info_lable)).setVisibility(View.GONE);
                    ((Button)objView.findViewById(R.id.import_btn)).setVisibility(View.VISIBLE);
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
                objSelectionAdapter.notifyDataSetChanged();
            }
        });
    }

    private void addSgmToDatabase()
    {
        AssetsDB assetsDB = objSelectionAdapter.assetsDB;
        String fileName = assetsDB.getAssetName();
        String textureName = (assetsDB.getTexture().equals("-1")) ? "white_texture" : assetsDB.getTexture();
        int assetId = 20000 + ((db.getMYModelAssetCount() == 0) ? 1 : db.getAllMyModelDetail().get(db.getMYModelAssetCount()-1).getID());
        String sgmFrom = (assetsDB.getAssetsId() == 123456) ? PathManager.LocalMeshFolder+"/123456.sgm" : PathManager.DefaultAssetsDir+"/"+assetsDB.getAssetsId()+".sgm";
        System.out.println("Sgm Path " + sgmFrom);
        if(!FileHelper.checkValidFilePath(sgmFrom)) System.out.println("File Not Exits");
        String sgmTo = PathManager.LocalMeshFolder+"/"+ assetId+".sgm";
        String textureFrom = (textureName.equals("white_texture") ? PathManager.DefaultAssetsDir+"/"+textureName+".png" : PathManager.LocalImportedImageFolder+"/"+textureName+".png");
        String textureTo = PathManager.LocalMeshFolder+"/"+assetId+"-cm.png";
        FileHelper.copy(sgmFrom, sgmTo);
        FileHelper.copy(textureFrom, textureTo);
        ((EditorView)((Activity)mContext)).imageManager.makeThumbnail(textureTo, Integer.toString(assetId));
        assetsDB.setAssetsId(assetId);
        assetsDB.setDateTime(new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date()));
        assetsDB.setNbones(0);
        assetsDB.setHash(FileHelper.md5(fileName));
        assetsDB.setType(Constants.NODE_SGM);
        db.addNewMyModelAssets(assetsDB);
        assetsDB.setIsTempNode(false);
        ((EditorView)((Activity)mContext)).renderManager.importAssets(assetsDB,false);
    }
}
