package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.BitmapFactory;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.PopupMenu;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadRequest;
import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.HashMap;
import java.util.List;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AssetSelectionAdapter extends BaseAdapter {

    public HashMap<Integer, Integer> downloadingAssets = new HashMap<>();
    public List<AssetsDB> assetsDBs;
    public int meshDownloadId = -1;
    public int textureDownloadId = -1;
    public int jsonDownloadId = -1;
    public int selectedAsset = -1;
    private Context mContext;
    private DatabaseHelper db;
    private GridView gridView;
    private HashMap<Integer, String> queueList = new HashMap<>();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private int previousPosition = -1;

    public AssetSelectionAdapter(Context c, DatabaseHelper db, GridView gridView, AddToDownloadManager addToDownloadManager
            , DownloadManager downloadManager) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.assetsDBs = this.db.getAllModelDetail(0);
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    @Override
    public int getCount() {
        if (assetsDBs == null || assetsDBs.size() <= 0) {
            DownloadHelper downloadHelper = new DownloadHelper();

        }

        return (assetsDBs != null && assetsDBs.size() > 0) ? assetsDBs.size() : 0;
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
        } else {
            grid = convertView;
        }

        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                grid.getLayoutParams().height = this.gridView.getHeight() / 4;
                break;
            default:
                grid.getLayoutParams().height = this.gridView.getHeight() / 5;
                break;
        }
        grid.findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);
        grid.findViewById(R.id.thumbnail).setVisibility(View.INVISIBLE);

        if (position >= assetsDBs.size())
            return grid;

        ((TextView) grid.findViewById(R.id.assetLable)).setText(assetsDBs.get(position).getAssetName());
        String fileName = assetsDBs.get(position).getAssetsId() + ".png";

        if (((EditorView) mContext).assetSelection.category.getSelectedItemPosition() == 6) {
            grid.findViewById(R.id.props_btn).setVisibility(View.VISIBLE);
            grid.findViewById(R.id.props_btn).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    showMyModelProps(v, position);
                }
            });
        } else
            grid.findViewById(R.id.props_btn).setVisibility(View.INVISIBLE);

        if (FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder + "/" + fileName)) {
            grid.findViewById(R.id.progress_bar).setVisibility(View.INVISIBLE);
            grid.findViewById(R.id.thumbnail).setVisibility(View.VISIBLE);
            ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + fileName));
            if (queueList != null && queueList.containsKey(position)) {
                queueList.remove(position);
            }
        } else {
            if (queueList != null && !queueList.containsKey(position)) {
                String thumbnailName = assetsDBs.get(position).getAssetsId() + ".png";
                String url = GL2JNILib.MeshThumbnail() + thumbnailName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingAssets.put(addToDownloadManager.downloadAdd(mContext, url, thumbnailName, desPath,
                        DownloadRequest.Priority.LOW, downloadManager, AssetSelectionAdapter.this, null, null, null), assetsDBs.get(position).getAssetsId());
                queueList.put(position, "1");
            }
            grid.findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);
            grid.findViewById(R.id.thumbnail).setVisibility(View.INVISIBLE);
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
                selectedAsset = position;
                importAsset();
            }
        });
        return grid;
    }

    private void importAsset() {
        if (assetsDBs == null || assetsDBs.size() <= selectedAsset) return;

        final AssetsDB assetsDB = assetsDBs.get(selectedAsset);
        String ext = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String mesh = PathManager.LocalMeshFolder + "/" + assetsDB.getAssetsId() + ext;
        String texture = PathManager.LocalTextureFolder + "/" + assetsDB.getAssetsId() + "-cm.png";

        String particleExt = ".json";
        String particleJson = assetsDB.getAssetsId() + particleExt;

        if (!FileHelper.checkValidFilePath(mesh) || !FileHelper.checkValidFilePath(texture)) {
            download(assetsDB);
            if ((assetsDB.getAssetsId() >= 50000 && assetsDB.getAssetsId() < 60000) && !FileHelper.checkValidFilePath(PathManager.LocalMeshFolder + "/" + particleJson)) {
                String desPath = PathManager.LocalMeshFolder + "/";
                String url = GL2JNILib.Particle() + particleJson;
                addToDownloadManager.lastErrorShowingTime = 0;
                downloadManager.cancel(jsonDownloadId);
                jsonDownloadId = addToDownloadManager.downloadAdd(mContext, url, particleJson, desPath, DownloadRequest.Priority.HIGH, downloadManager, AssetSelectionAdapter.this, null, null, particleJson);
            }
        } else {
            ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
            assetsDB.setIsTempNode(true);
            assetsDB.setAssetPath(PathManager.LocalMeshFolder + "/" + assetsDB.getAssetsId() + ext);
            assetsDB.setTexture(assetsDB.getAssetsId() + "-cm");
            ((EditorView) mContext).renderManager.importAssets(assetsDB);
        }
    }

    private void download(AssetsDB assetsDB) {
        if (mContext == null || downloadManager == null || assetsDB == null || addToDownloadManager == null)
            return;
        ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
        String meshExt = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String textureExt = ".png";
        downloadManager.cancel(meshDownloadId);
        downloadManager.cancel(textureDownloadId);
        String mesh = GL2JNILib.Mesh() + assetsDB.getAssetsId() + meshExt;
        String texture = GL2JNILib.Texture() + assetsDB.getAssetsId() + textureExt;
        String fileName = Integer.toString(assetsDB.getAssetsId()) + meshExt;
        String textureFileName = Integer.toString(assetsDB.getAssetsId()) + "-cm" + textureExt;
        String desPath = PathManager.LocalCacheFolder + "/";
        meshDownloadId = addToDownloadManager.downloadAdd(mContext, mesh, fileName, desPath, DownloadRequest.Priority.HIGH, downloadManager, this, null, null, null);
        addToDownloadManager.lastErrorShowingTime = 0;
        textureDownloadId = addToDownloadManager.downloadAdd(mContext, texture, textureFileName, desPath, DownloadRequest.Priority.HIGH, downloadManager, this, null, null, null);
    }

    public void downloadThumbnail() {
        if (downloadingAssets != null)
            downloadingAssets.clear();
        else
            downloadingAssets = new HashMap<>();
        if (queueList != null)
            queueList.clear();
        else
            queueList = new HashMap<>();
    }

    private void showMyModelProps(View v, final int position) {
        final PopupMenu popup = new PopupMenu(mContext, v);
        popup.getMenuInflater().inflate(R.menu.my_model_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.ID_CLONE:
                        int assetName = assetsDBs.get(position).getAssetsId();
                        int nodeType = assetsDBs.get(position).getType();
                        int assetId = ((nodeType == 1) ? 40000 : 20000) + ((db.getMYModelAssetCount() == 0) ? 1 : db.getAllMyModelDetail().get(db.getMYModelAssetCount() - 1).getID() + 1);
                        AssetsDB assetsDB;
                        assetsDB = assetsDBs.get(position);
                        assetsDB.setAssetName(assetsDB.getAssetName() + "copy");
                        assetsDB.setAssetsId(assetId);
                        String ext = (assetsDBs.get(position).getType() == 1) ? ".sgr" : ".sgm";
                        FileHelper.copy(PathManager.LocalMeshFolder + "/" + assetName + ext, PathManager.LocalMeshFolder + "/" + assetId + ext);
                        FileHelper.copy(PathManager.LocalTextureFolder + "/" + assetName + "-cm.png", PathManager.LocalTextureFolder + "/" + assetId + "-cm.png");
                        FileHelper.copy(PathManager.LocalThumbnailFolder + "/" + assetName + ".png", PathManager.LocalThumbnailFolder + "/" + assetId + ".png");
                        db.addNewMyModelAssets(assetsDB);
                        assetsDBs.clear();
                        assetsDBs = db.getAllMyModelDetail();
                        notifyDataSetChanged();
                        break;
                    case Constants.ID_RENAME:
                        renameAsset(position);
                        break;
                    case Constants.ID_DELETE:
                        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
                        builder.setTitle(mContext.getString(R.string.confirmation));
                        builder.setMessage(mContext.getString(R.string.object_will_be_deleted_msg));
                        builder.setPositiveButton(mContext.getString(R.string.ok), new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                int assetsId = assetsDBs.get(position).getAssetsId();
                                String meshExt = (assetsDBs.get(position).getType() == 1) ? ".sgr" : ".sgm";
                                FileHelper.deleteFilesAndFolder(PathManager.LocalMeshFolder + "/" + assetsId + meshExt);
                                FileHelper.deleteFilesAndFolder(PathManager.LocalThumbnailFolder + "/" + assetsId + ".png");
                                FileHelper.deleteFilesAndFolder(PathManager.LocalTextureFolder + "/" + assetsId + "-cm.png");
                                db.deleteMyMyModel(assetsId);
                                assetsDBs.remove(position);
                                selectedAsset = -1;
                                if (mContext != null && ((EditorView) mContext).renderManager != null)
                                    ((EditorView) mContext).renderManager.removeTempNode();
                                notifyDataSetChanged();
                            }
                        });
                        builder.setNegativeButton(mContext.getString(R.string.cancel), new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.cancel();
                            }
                        });
                        builder.show();
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    private void renameAsset(final int position) {
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle(mContext.getString(R.string.rename_your_asset));
        final EditText input = new EditText(mContext);
        input.setHint(R.string.asset_name);
        input.setInputType(InputType.TYPE_CLASS_TEXT);
        builder.setView(input);
        builder.setPositiveButton(mContext.getString(R.string.ok), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (input.getText().toString().length() == 0) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.asset_name_empty));
                    return;
                }
                if (FileHelper.isItHaveSpecialChar(input.getText().toString())) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.asset_name_contain_special_char));
                    return;
                }
                List<AssetsDB> tempAsset = db.getSingleMyModel(DatabaseHelper.ASSET_KEY_ASSET_NAME, input.getText().toString());
                if (tempAsset != null && tempAsset.size() > 0) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.name_already_exists));
                    return;
                }
                if (assetsDBs == null) return;
                AssetsDB assetsDB = assetsDBs.get(position);
                assetsDB.setAssetName(input.getText().toString());
                db.updateMyModelDetails(assetsDB);
                assetsDBs.clear();
                assetsDBs = db.getAllMyModelDetail();
                notifyDataSetChanged();
            }
        });
        builder.setNegativeButton(mContext.getString(R.string.cancel), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });
        builder.show();
    }
}

