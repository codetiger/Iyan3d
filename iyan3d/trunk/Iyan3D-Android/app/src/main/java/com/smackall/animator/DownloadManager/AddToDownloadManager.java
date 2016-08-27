package com.smackall.animator.DownloadManager;

import android.content.Context;
import android.net.Uri;

import com.smackall.animator.Adapters.AnimationSelectionAdapter;
import com.smackall.animator.Adapters.AssetSelectionAdapter;
import com.smackall.animator.Adapters.TextSelectionAdapter;
import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

import java.io.File;
import java.util.Calendar;

/**
 * Created by Sabish.M on 23/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AddToDownloadManager {

    public boolean errorMessageShowing = false;
    public long lastErrorShowingTime = 0;
    long MAX_INTERVAL = 120000; //2 minutes in milli seconds
    private MyDownloadStatusListener myDownloadStatusListener = new MyDownloadStatusListener();
    private Context mContext;
    private AssetSelectionAdapter assetSelectionAdapter;
    private TextSelectionAdapter textSelectionAdapter;
    private AnimationSelectionAdapter animationSelectionAdapter;

    public int downloadAdd(Context context, String url, String fileName, String destinationPath, DownloadRequest.Priority priority, DownloadManager downloadManager,
                           AssetSelectionAdapter assetSelectionAdapter, TextSelectionAdapter textSelectionAdapter, AnimationSelectionAdapter animationSelectionAdapter, String requiredFileName) {

        this.mContext = context;
        this.assetSelectionAdapter = assetSelectionAdapter;
        this.textSelectionAdapter = textSelectionAdapter;
        this.animationSelectionAdapter = animationSelectionAdapter;


        Uri uri = Uri.parse(url);
        Uri destinationUri = Uri.parse(destinationPath + ((requiredFileName != null) ? requiredFileName : fileName));

        DownloadRequest downloadRequest = new DownloadRequest(uri)
                .setDestinationURI(destinationUri).setPriority(priority)
                .setDownloadListener(myDownloadStatusListener);

        return downloadManager.add(downloadRequest);
    }

    private void importAsset() {
        if (assetSelectionAdapter == null || assetSelectionAdapter.assetsDBs == null || assetSelectionAdapter.assetsDBs.size() <= assetSelectionAdapter.selectedAsset)
            return;
        AssetsDB assetsDB = assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset);
        String ext = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String mesh = PathManager.LocalMeshFolder + "/" + assetsDB.getAssetsId() + ext;
        String texture = PathManager.LocalTextureFolder + "/" + assetsDB.getAssetsId() + "-cm.png";
        if (Constants.VIEW_TYPE == Constants.PARTICLE_VIEW && !FileHelper.checkValidFilePath(PathManager.LocalMeshFolder + "/" + assetsDB.getAssetsId() + ".json"))
            return;
        if (FileHelper.checkValidFilePath(mesh) && FileHelper.checkValidFilePath(texture)) {
            ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
            assetsDB.setIsTempNode(true);
            assetsDB.setAssetPath(PathManager.LocalMeshFolder + "/" + assetsDB.getAssetsId() + ext);
            assetsDB.setTexture(assetsDB.getAssetsId() + "-cm");
            ((EditorView) mContext).renderManager.importAssets(assetsDB);
        }
    }

    class MyDownloadStatusListener implements DownloadStatusListener {
        @Override
        public void onDownloadComplete(int id) {
            switch (Constants.VIEW_TYPE) {
                case Constants.PARTICLE_VIEW: {
                    try {
                        if (assetSelectionAdapter.meshDownloadId != id && assetSelectionAdapter.textureDownloadId != id && assetSelectionAdapter.jsonDownloadId != id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String fileName = assetSelectionAdapter.downloadingAssets.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder + "/" + fileName + ".png");
                            FileHelper.move(moveFrom, moveTo);
                            assetSelectionAdapter.notifyDataSetChanged();
                            break;
                        } else if (assetSelectionAdapter.meshDownloadId == id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String ext = (assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getType() == 1) ? ".sgr" : ".sgm";
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ext);
                            File moveTo = new File(PathManager.LocalMeshFolder + "/" + fileName + ext);
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        } else if (assetSelectionAdapter.textureDownloadId == id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + "-cm.png");
                            File moveTo = new File(PathManager.LocalTextureFolder + "/" + fileName + "-cm.png");
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        } else if (assetSelectionAdapter.jsonDownloadId == id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ".json");
                            File moveTo = new File(PathManager.LocalMeshFolder + "/" + fileName + ".json");
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                case Constants.ASSET_VIEW: {
                    try {
                        if (assetSelectionAdapter == null) break;
                        if (assetSelectionAdapter.meshDownloadId != id && assetSelectionAdapter.textureDownloadId != id && Constants.VIEW_TYPE == Constants.ASSET_VIEW) {
                            String fileName = assetSelectionAdapter.downloadingAssets.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder + "/" + fileName + ".png");
                            FileHelper.move(moveFrom, moveTo);
                            assetSelectionAdapter.notifyDataSetChanged();
                            break;
                        } else if (assetSelectionAdapter.meshDownloadId == id && Constants.VIEW_TYPE == Constants.ASSET_VIEW) {
                            String ext = (assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getType() == 1) ? ".sgr" : ".sgm";
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ext);
                            File moveTo = new File(PathManager.LocalMeshFolder + "/" + fileName + ext);
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        } else if (Constants.VIEW_TYPE == Constants.ASSET_VIEW) {
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + "-cm.png");
                            File moveTo = new File(PathManager.LocalTextureFolder + "/" + fileName + "-cm.png");
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        }
                    } catch (NullPointerException | IndexOutOfBoundsException | ClassCastException ignored) {
                        ignored.printStackTrace();
                    }
                    break;
                }
                case Constants.ANIMATION_VIEW: {
                    try {
                        if (animationSelectionAdapter == null) break;
                        if (animationSelectionAdapter.animDownloadId != id) {
                            String fileName = animationSelectionAdapter.downloadingAnimation.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder + "/" + fileName + ".png");
                            FileHelper.move(moveFrom, moveTo);
                            animationSelectionAdapter.notifyDataSetChanged();
                            break;
                        } else {
                            int animId = animationSelectionAdapter.animDBs.get(animationSelectionAdapter.selectedId).getAnimAssetId();
                            String ext = (animationSelectionAdapter.animDBs.get(animationSelectionAdapter.selectedId).getAnimType() == 0) ? ".sgra" : ".sgta";
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + animId + ext);
                            File moveTo = new File(PathManager.LocalAnimationFolder + "/" + animId + ext);
                            FileHelper.move(moveFrom, moveTo);
                            animationSelectionAdapter.importAnimation(animationSelectionAdapter.selectedId);
                        }
                    } catch (NullPointerException | IndexOutOfBoundsException | ClassCastException ignored) {
                        ignored.printStackTrace();
                    }
                    break;
                }
                case Constants.TEXT_VIEW: {
                    if (textSelectionAdapter == null) break;
                    try {
                        String fileName = textSelectionAdapter.downloadingFonts.get(id);
                        File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName);
                        File moveTo = new File(PathManager.LocalFontsFolder + "/" + fileName);
                        FileHelper.move(moveFrom, moveTo);
                        textSelectionAdapter.notifyDataSetChanged();
                    } catch (NullPointerException | IndexOutOfBoundsException | ClassCastException ignored) {
                        ignored.printStackTrace();
                    }
                    break;
                }
            }
        }

        @Override
        public void onDownloadFailed(int id, int errorCode, String errorMessage) {
            if (errorCode != DownloadManager.ERROR_DOWNLOAD_CANCELLED && !errorMessageShowing && (Calendar.getInstance().getTimeInMillis() - lastErrorShowingTime) > MAX_INTERVAL) {
                lastErrorShowingTime = Calendar.getInstance().getTimeInMillis();
                errorMessageShowing = true;
                UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
                if (Constants.VIEW_TYPE == Constants.ANIMATION_VIEW && mContext != null && ((EditorView) mContext).animationSelection != null && ((EditorView) mContext).animationSelection.animationSelectionAdapter != null) {
                    ((EditorView) mContext).animationSelection.processCompleted = true;
                    ((EditorView) mContext).animationSelection.filePath = "";
                }
            }
        }

        @Override
        public void onProgress(int id, long totalBytes, long downloadedBytes, int progress) {
        }
    }
}
