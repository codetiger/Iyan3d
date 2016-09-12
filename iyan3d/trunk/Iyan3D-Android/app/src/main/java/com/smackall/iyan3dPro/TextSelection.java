package com.smackall.iyan3dPro;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Switch;

import com.smackall.iyan3dPro.Analytics.HitScreens;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.FileHelper;
import com.smackall.iyan3dPro.Helper.Listeners.FilePickerListener;
import com.smackall.iyan3dPro.Helper.TextDB;
import com.smackall.iyan3dPro.Helper.UIHelper;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class TextSelection implements View.OnClickListener, SeekBar.OnSeekBarChangeListener {
    public TextDB textDB = new TextDB();
    ViewGroup insertPoint;
    View v;
    private EditorView editorView;

    public TextSelection(EditorView editorView) {
        this.editorView = editorView;
    }

    public void showTextSelection() {
        textDB.resetValues();
        HitScreens.TextSelectionView(editorView);
        editorView.showOrHideToolbarView(Constants.HIDE);
        insertPoint = editorView.sharedPreferenceManager.getInt(editorView, "toolbarPosition") == 1 ?
                ((ViewGroup) editorView.findViewById(R.id.leftView))
                : ((ViewGroup) editorView.findViewById(R.id.rightView));

        for (int i = 0; i < ((ViewGroup) insertPoint.getParent()).getChildCount(); i++) {
            if (((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup) insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) this.editorView.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        v = vi.inflate(R.layout.text_view, insertPoint, false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        initViews(v);

        LinearLayout gridViewHolder = (LinearLayout) insertPoint.findViewById(R.id.gridView_holder);
        List<File> topList = new ArrayList<File>();
        editorView.filePicker.showFilePicker(new String[]{"ttf", "otf"}, topList, gridViewHolder, new FilePickerListener() {
            @Override
            public void FilePickerListenerCallback(String path, boolean isCanceled, boolean isTempNode, ViewGroup insertPoint) {
                if (new File(path).isDirectory() || !FileHelper.checkValidFilePath(path)) return;
                textDB.setFilePath(path);
                textDB.setTempNode(isTempNode);
                importText();
            }
        }, false);
    }

    private void initViews(View v) {
        v.findViewById(R.id.cancel_textView).setOnClickListener(this);
        v.findViewById(R.id.add_text_btn).setOnClickListener(this);
        v.findViewById(R.id.withBone).setOnClickListener(this);
        ((SeekBar) v.findViewById(R.id.bevalSlider)).setOnSeekBarChangeListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.cancel_textView:
            case R.id.add_text_btn:

                if (v.getId() == R.id.add_text_btn) {
                    textDB.setTempNode((v.getId() == R.id.cancel_textView));
                    if (!importText())
                        return;
                }

                HitScreens.EditorView(editorView);
                editorView.renderManager.removeTempNode();
                insertPoint.removeAllViews();
                editorView.showOrHideToolbarView(Constants.SHOW);
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                break;
            case R.id.withBone:
                importText();
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

    public boolean importText() {
        if (textDB.getFilePath().equals("-1")) {
            UIHelper.informDialog(editorView, editorView.getString(R.string.choode_font_style));
            return false;
        } else if (((EditText) v.findViewById(R.id.inputText)).getText().toString().isEmpty()) {
            UIHelper.informDialog(editorView, "Empty text cannot be added.");
            return false;
        }
        editorView.showOrHideLoading(Constants.SHOW);
        textDB.setText(((EditText) v.findViewById(R.id.inputText)).getText().toString());
        textDB.setBevalValue(((SeekBar) v.findViewById(R.id.bevalSlider)).getProgress());
        textDB.setFontSize(10);
        textDB.setTextureName("-1");
        textDB.setTypeOfNode(((Switch) v.findViewById(R.id.withBone)).isChecked() ? Constants.ASSET_TEXT_RIG : Constants.ASSET_TEXT);
        editorView.renderManager.importText(textDB);
        return true;
    }
}
