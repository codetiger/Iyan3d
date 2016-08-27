package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.BitmapFactory;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.PopupMenu;
import android.text.InputType;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadRequest;
import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.HashMap;
import java.util.List;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AnimationSelectionAdapter extends BaseAdapter {


    public HashMap<Integer, Integer> downloadingAnimation = new HashMap<>();
    public List<AnimDB> animDBs;
    public int animDownloadId = -1;
    public int selectedId = -1;
    private Context mContext;
    private DatabaseHelper db;
    private SharedPreferenceManager sp;
    private GridView gridView;
    private HashMap<Integer, String> queueList = new HashMap<>();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private int previousPosition = -1;

    public AnimationSelectionAdapter(Context c, DatabaseHelper db, GridView gridView, AddToDownloadManager addToDownloadManager
            , DownloadManager downloadManager, SharedPreferenceManager sp) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.animDBs = this.db.getAllMyAnimation(4, 0, "");
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
        this.sp = sp;
    }

    @Override
    public int getCount() {
        return (animDBs == null || animDBs.size() == 0) ? 0 : animDBs.size();
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

        if (position >= animDBs.size())
            return grid;

        ((TextView) grid.findViewById(R.id.assetLable)).setText(animDBs.get(position).getAnimName());
        String fileName = animDBs.get(position).getAnimAssetId() + ".png";

        if (((EditorView) mContext).animationSelection.category.getSelectedItemPosition() == 4) {
            grid.findViewById(R.id.props_btn).setVisibility(View.VISIBLE);
            grid.findViewById(R.id.props_btn).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    showMyAnimationProps(v, position);
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
                String thumbnailName = animDBs.get(position).getAnimAssetId() + ".png";
                String url = GL2JNILib.AnimationThumbnail() + thumbnailName;
                String desPath = PathManager.LocalCacheFolder + "/";
                addToDownloadManager.lastErrorShowingTime = 0;
                downloadingAnimation.put(addToDownloadManager.downloadAdd(mContext, url, thumbnailName, desPath,
                        DownloadRequest.Priority.LOW, downloadManager, null, null, AnimationSelectionAdapter.this, null), animDBs.get(position).getAnimAssetId());
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
                if (!((EditorView) mContext).animationSelection.processCompleted) return;
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                previousPosition = position;
                notifyDataSetChanged();
                GL2JNILib.setIsPlaying(false, null);
                importAnimation(position);
            }
        });
        return grid;
    }

    public void importAnimation(int position) {
        int animId = animDBs.get(position).getAnimAssetId();
        String ext = (animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
        String url = GL2JNILib.Animation() + animId + ext;
        String dest = PathManager.LocalCacheFolder + "/";
        if (!FileHelper.checkValidFilePath(PathManager.LocalAnimationFolder + "/" + animId + ext)) {
            downloadManager.cancel(animDownloadId);
            selectedId = position;
            animDownloadId = addToDownloadManager.downloadAdd(mContext, url, Integer.toString(animId) + ext, dest, DownloadRequest.Priority.HIGH, downloadManager, null, null, this, null);
        } else {
            ((EditorView) mContext).animationSelection.processCompleted = false;
            ((EditorView) mContext).animationSelection.applyAnimation(position);
            if ((((EditorView) mContext).animationSelection.animationType[((EditorView) mContext).animationSelection.category.getSelectedItemPosition()]) == 7
                    && animDBs.get(position).getPublishedId() <= 0)
                showOrHidePublish(position);
            else
                ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.GONE);
        }
    }

    public void downloadThumbnail() {
        if (downloadingAnimation != null)
            downloadingAnimation.clear();
        else
            downloadingAnimation = new HashMap<>();
        if (queueList != null)
            queueList.clear();
        else
            queueList = new HashMap<>();
    }

    private void showOrHidePublish(final int position) {
        ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.VISIBLE);
        if (sp.getInt(mContext, "toolbarPosition") == 1)
            ((LinearLayout) ((ViewGroup) ((FrameLayout) ((Activity) mContext).findViewById(R.id.publishFrame)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.END);
        else
            ((LinearLayout) ((ViewGroup) ((FrameLayout) ((Activity) mContext).findViewById(R.id.publishFrame)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.START);

        ((Activity) mContext).findViewById(R.id.publish).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (((EditorView) mContext).userDetails.signInType <= 0) {
                    UIHelper.showSignInPanelWithMessage(mContext, mContext.getString(R.string.sign_in_to_publish));
                    return;
                }
                ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.GONE);
                ((EditorView) mContext).publish.publishAnimation(position);
            }
        });
    }

    private void showMyAnimationProps(View v, final int position) {
        final PopupMenu popup = new PopupMenu(mContext, v);
        popup.getMenuInflater().inflate(R.menu.my_animation_menu, popup.getMenu());
        popup.getMenu().getItem(3).setVisible(animDBs.get(position).getPublishedId() <= 0);
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.ID_CLONE:
                        int animName = animDBs.get(position).getAnimAssetId();
                        int assetId = 80000 + db.getNextAnimationAssetId();
                        AnimDB animDB;
                        animDB = animDBs.get(position);
                        animDB.setAnimName(animDB.getAnimName() + "copy");
                        animDB.setAssetsId(assetId);
                        String ext = (animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
                        FileHelper.copy(PathManager.LocalAnimationFolder + "/" + animName + ext, PathManager.LocalAnimationFolder + "/" + assetId + ext);
                        FileHelper.copy(PathManager.LocalThumbnailFolder + "/" + animName + ".png", PathManager.LocalThumbnailFolder + "/" + assetId + ".png");
                        FileHelper.copy(PathManager.LocalAnimationFolder + "/" + animName + ".png", PathManager.LocalAnimationFolder + "/" + assetId + ".png");
                        db.addNewMyAnimationAssets(animDB);
                        animDBs.clear();
                        animDBs = db.getAllMyAnimation((GL2JNILib.getNodeType(((EditorView) mContext).animationSelection.selectedNodeId) == Constants.NODE_RIG) ? 0 : 1);
                        notifyDataSetChanged();
                        break;
                    case Constants.ID_RENAME:
                        renameAnimation(position);
                        break;
                    case Constants.ID_DELETE:
                        int animId = animDBs.get(position).getAnimAssetId();
                        String extension = (animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
                        FileHelper.deleteFilesAndFolder(PathManager.LocalAnimationFolder + "/" + animId + extension);
                        FileHelper.deleteFilesAndFolder(PathManager.LocalThumbnailFolder + "/" + animId + ".png");
                        db.deleteMyAnimation(animDBs.get(position).getAnimAssetId());
                        animDBs.remove(position);
                        if (mContext != null && ((EditorView) mContext).animationSelection != null)
                            ((EditorView) mContext).animationSelection.filePath = "";
                        notifyDataSetChanged();
                        break;
                    case Constants.ID_PUBLISH:
                        if (((EditorView) mContext).userDetails.signInType <= 0) {
                            UIHelper.showSignInPanelWithMessage(mContext, mContext.getString(R.string.sign_in_to_publish));
                            break;
                        }
                        ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.GONE);
                        ((EditorView) mContext).publish.publishAnimation(position);
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    private void renameAnimation(final int position) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle(mContext.getString(R.string.rename_animation_msg));
        final EditText input = new EditText(mContext);
        input.setHint(R.string.animation_name);
        input.setInputType(InputType.TYPE_CLASS_TEXT);
        builder.setView(input);
        builder.setPositiveButton(mContext.getString(R.string.ok), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (input.getText().toString().length() == 0) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.animation_name_empty));
                    return;
                }
                if (FileHelper.isItHaveSpecialChar(input.getText().toString())) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.animation_name_special_char));
                    return;
                }
                List<AnimDB> animDBs1 = db.getSingleMyAnimationDetail(DatabaseHelper.ANIM_KEY_ANIM_NAME, input.getText().toString());
                if (animDBs1 != null && animDBs1.size() > 0) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.name_already_exists));
                    return;
                }
                AnimDB animDB = animDBs.get(position);
                animDB.setAnimName(input.getText().toString());
                db.updateMyAnimationDetails(animDB);
                animDBs.clear();
                animDBs = db.getAllMyAnimation((GL2JNILib.getNodeType(((EditorView) mContext).animationSelection.selectedNodeId) == Constants.NODE_RIG) ? 0 : 1);
                notifyDataSetChanged();
            }
        });
        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });
        builder.show();
    }
}

