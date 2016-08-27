package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.SeekBar;
import android.widget.Switch;

import com.google.android.gms.analytics.Tracker;
import com.smackall.animator.Adapters.TextSelectionAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.TextDB;
import com.smackall.animator.Helper.UIHelper;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class TextSelection implements View.OnClickListener, SeekBar.OnSeekBarChangeListener {
    public TextDB textDB = new TextDB();
    TextSelectionAdapter textSelectionAdapter;
    ViewGroup insertPoint;
    View v;
    private Context mContext;
    private DatabaseHelper db;
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;

    public TextSelection(Context context, DatabaseHelper db, AddToDownloadManager addToDownloadManager, DownloadManager downloadManager) {
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    public void showTextSelection() {
        HitScreens.TextSelectionView(mContext);
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
        v = vi.inflate(R.layout.text_view, insertPoint, false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        initViews(v);
        GridView gridView = (GridView) v.findViewById(R.id.text_gridView);
        initTextGrid(gridView);
    }

    private void initViews(View v) {
        v.findViewById(R.id.cancel_textView).setOnClickListener(this);
        v.findViewById(R.id.add_text_btn).setOnClickListener(this);
        v.findViewById(R.id.color_picker_btn).setOnClickListener(this);
        v.findViewById(R.id.withBone).setOnClickListener(this);
        v.findViewById(R.id.fontstore).setOnClickListener(this);
        v.findViewById(R.id.myfont).setOnClickListener(this);
        ((SeekBar) v.findViewById(R.id.bevalSlider)).setOnSeekBarChangeListener(this);
    }

    private void initTextGrid(GridView gridView) {
        textSelectionAdapter = new TextSelectionAdapter(mContext, db, gridView, this.addToDownloadManager, this.downloadManager);
        gridView.setAdapter(textSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
        textSelectionAdapter.downloadFonts();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.color_picker_btn:
                ((EditorView) mContext).colorPicker.showColorPicker(v, null, Constants.TEXT_VIEW);
                break;
            case R.id.cancel_textView:
            case R.id.add_text_btn:
                HitScreens.EditorView(mContext);
                textDB.setTempNode((v.getId() == R.id.cancel_textView));
                if (v.getId() == R.id.add_text_btn)
                    importText();
                ((EditorView) mContext).renderManager.removeTempNode();
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                textSelectionAdapter = null;
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                break;
            case R.id.withBone:
                importText();
                break;
            case R.id.myfont:
                downloadManager.cancelAll();
                textSelectionAdapter.addFileListToAssetDb(false);
                break;
            case R.id.fontstore:
                textSelectionAdapter.addFileListToAssetDb(true);
                break;
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        textDB.setTempNode(true);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        importText();
    }

    public void importText() {
        if (textDB.getFilePath().equals("-1")) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.choode_font_style));
            return;
        } else if (!FileHelper.checkValidFilePath(PathManager.LocalFontsFolder + "/" + textDB.getFilePath()) && !FileHelper.checkValidFilePath(PathManager.LocalUserFontFolder + "/" + textDB.getFilePath())) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
            return;
        }
        ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
        textDB.setAssetName(((EditText) v.findViewById(R.id.inputText)).getText().toString());
        textDB.setBevalValue(((SeekBar) v.findViewById(R.id.bevalSlider)).getProgress());
        textDB.setFontSize(10);
        textDB.setTextureName("-1");
        textDB.setTypeOfNode(((Switch) v.findViewById(R.id.withBone)).isChecked() ? Constants.ASSET_TEXT_RIG : Constants.ASSET_TEXT);
        ((EditorView) mContext).renderManager.importText(textDB);
    }

    public void notifyDataSetChanged(boolean openWithFunction) {
        if (textSelectionAdapter != null)
            textSelectionAdapter.notifyDataSetChanged();
        if (openWithFunction)
            v.findViewById(R.id.myfont).performClick();
    }
}
