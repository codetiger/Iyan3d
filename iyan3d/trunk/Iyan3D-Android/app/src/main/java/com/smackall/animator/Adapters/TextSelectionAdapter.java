package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.Typeface;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadRequest;
import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

import java.io.File;
import java.io.FilenameFilter;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;


/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class TextSelectionAdapter extends BaseAdapter {

    public HashMap<Integer, String> downloadingFonts = new HashMap<>();
    private List<AssetsDB> assetsDBs;
    private int previousPosition = -1;
    private Context mContext;
    private DatabaseHelper db;
    private GridView gridView;
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private boolean isStore = true;

    public TextSelectionAdapter(Context c, DatabaseHelper db, GridView gridView, AddToDownloadManager addToDownloadManager, DownloadManager downloadManager) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.assetsDBs = this.db.getAllModelDetail(50);
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    @Override
    public int getCount() {
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
        View grid;

        if (convertView == null) {
            grid = new View(mContext);
            LayoutInflater inflater = ((Activity) mContext).getLayoutInflater();
            grid = inflater.inflate(R.layout.text_cell, parent, false);
        } else {
            grid = convertView;
        }
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                grid.getLayoutParams().height = this.gridView.getHeight() / 3;
                break;
            default:
                grid.getLayoutParams().height = this.gridView.getHeight() / 5;
                break;
        }
        grid.findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);
        grid.findViewById(R.id.display_text).setVisibility(View.INVISIBLE);

        String DisplayName = assetsDBs.get(position).getAssetName();
        ((TextView) grid.findViewById(R.id.lable_text)).setText(FileHelper.getFileWithoutExt(DisplayName));
        String downloadFileName = (isStore) ? assetsDBs.get(position).getAssetsId() + "." + FileHelper.getFileExt(DisplayName) :
                assetsDBs.get(position).getAssetName();

        if (FileHelper.checkValidFilePath(PathManager.LocalFontsFolder + "/" + downloadFileName) || FileHelper.checkValidFilePath(PathManager.LocalUserFontFolder + "/" + downloadFileName)) {
            grid.findViewById(R.id.progress_bar).setVisibility(View.INVISIBLE);
            grid.findViewById(R.id.display_text).setVisibility(View.VISIBLE);
            Typeface fontType;
            try {
                fontType = FileHelper.checkValidFilePath(PathManager.LocalFontsFolder + "/" + downloadFileName) ? Typeface.createFromFile(PathManager.LocalFontsFolder + "/" + downloadFileName) : Typeface.createFromFile(PathManager.LocalUserFontFolder + "/" + downloadFileName);
            } catch (RuntimeException e) {
                fontType = null;
                e.printStackTrace();
            }
            ((TextView) grid.findViewById(R.id.display_text)).setText(String.format(Locale.getDefault(), "%s", "TEXT"));
            if (fontType != null)
                ((TextView) grid.findViewById(R.id.display_text)).setTypeface(fontType);
        } else {
            grid.findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);
            grid.findViewById(R.id.display_text).setVisibility(View.INVISIBLE);
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
                importText((isStore) ? assetsDBs.get(position).getAssetsId() + "." + FileHelper.getFileExt(assetsDBs.get(position).getAssetName()) :
                        assetsDBs.get(position).getAssetName());
            }
        });
        return grid;
    }

    private void importText(String file) {
        ((EditorView) mContext).textSelection.textDB.setFilePath(file);
        ((EditorView) mContext).textSelection.importText();
    }

    public void downloadFonts() {
        if (downloadingFonts != null) {
            downloadingFonts.clear();
            downloadingFonts = new HashMap<>();
        }
        for (int i = 0; i < assetsDBs.size(); ++i) {
            if (i >= assetsDBs.size()) continue;
            String DisplayName = assetsDBs.get(i).getAssetName();
            String downloadFileName = assetsDBs.get(i).getAssetsId() + "." + FileHelper.getFileExt(DisplayName);
            if (!FileHelper.checkValidFilePath(PathManager.LocalFontsFolder + "/" + downloadFileName)) {
                String url = GL2JNILib.Font() + downloadFileName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingFonts.put(addToDownloadManager.downloadAdd(mContext, url, downloadFileName, desPath,
                        DownloadRequest.Priority.HIGH, downloadManager, null, TextSelectionAdapter.this, null, null), downloadFileName);
            }
        }
    }

    private File[] getFileList() {
        FileHelper.getFontFromCommonIyan3dPath();
        final String userFolder = PathManager.LocalUserFontFolder + "/";
        final File f = new File(userFolder);
        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.toLowerCase().endsWith("otf") || filename.toLowerCase().endsWith("ttf");
            }
        };
        return f.listFiles(filenameFilter);
    }

    public void addFileListToAssetDb(boolean isStore) {
        if (assetsDBs != null) {
            assetsDBs.clear();
            notifyDataSetChanged();
        }
        if (isStore) {
            this.isStore = true;
            assetsDBs = db.getAllModelDetail(50);
        } else {
            this.isStore = false;
            File[] filelist = getFileList();
            if (filelist == null || filelist.length <= 0) {
                UIHelper.informDialog(mContext, mContext.getString(R.string.copy_fonts_to_local));
                return;
            }
            for (File aFilelist : filelist) {
                AssetsDB assetsDB = new AssetsDB();
                assetsDB.setAssetsId(0);
                assetsDB.setAssetName(FileHelper.getFileNameFromPath(aFilelist.getPath()));
                assetsDBs.add(assetsDB);
            }
        }
        notifyDataSetChanged();
    }
}

