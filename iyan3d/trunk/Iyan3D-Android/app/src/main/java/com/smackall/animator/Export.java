package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.Switch;
import android.widget.TextView;

import com.smackall.animator.Helper.BitmapUtil;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.RangeSeekBar;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import org.jcodec.api.SequenceEncoder;
import org.jcodec.common.NIOUtils;
import org.jcodec.common.SeekableByteChannel;
import org.jcodec.containers.mp4.Brand;
import org.jcodec.containers.mp4.TrackType;
import org.jcodec.containers.mp4.muxer.MP4Muxer;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;


/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Export implements View.OnClickListener {
    private Context mContext;
    private SharedPreferenceManager sp;
    private Dialog dialog;
    private float x = 0.1f;
    private float y = 0.1f;
    private float z = 0.1f;
    private boolean waterMark = true;
    private int shader;
    private int type = Constants.EXPORT_IMAGES;
    private boolean cancel = false;
    private SequenceEncoder encoder;
    private int min,max;
    int frameCount;
    private String fileName;
    ArrayList<Integer> renderedImg = new ArrayList<>();


    public Export(Context mContext,SharedPreferenceManager sp){
        this.mContext = mContext;
        this.sp = sp;
    }

    public void showExport(int type)
    {
        Dialog export = new Dialog(mContext);
        export.requestWindowFeature(Window.FEATURE_NO_TITLE);
        export.setContentView(R.layout.export);
        export.setCancelable(false);
        export.setCanceledOnTouchOutside(false);
        export.getWindow().setLayout(Constants.width / 2, (int) (Constants.height / 1.14285714286));
        dialog = export;
        x = 0.1f;
        y = 0.1f;
        z = 0.1f;
        min = 1;max =  GL2JNILib.totalFrame();
        shader = Constants.NORMAL_SHADER;
        waterMark = true;
        this.type = type;
        this.cancel =false;
        encoder = null;
        renderedImg.clear();
        export.show();
        initViews(export);
        updateCreditLable();
        if(type == Constants.EXPORT_VIDEO)
            initRangeBar();
        else
            hideView();
    }

    private void initViews(Dialog export)
    {
        ((LinearLayout)export.findViewById(R.id.color_picket_btn)).setOnClickListener(this);
        ((ImageView)export.findViewById(R.id.normal)).setOnClickListener(this);
        ((ImageView)export.findViewById(R.id.toon)).setOnClickListener(this);
        ((ImageView)export.findViewById(R.id.cloud)).setOnClickListener(this);
        ((Switch)export.findViewById(R.id.waterMark)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.THOUSAND_EIGHTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.SEVEN_TWENTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.FOUR_EIGHTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.THREE_SIXTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.TWO_FOURTY)).setOnClickListener(this);
        ((Button)export.findViewById(R.id.cancel)).setOnClickListener(this);
        ((Button)export.findViewById(R.id.next)).setOnClickListener(this);
        ((TextView)export.findViewById(R.id.leftValue)).setText("1");
        ((TextView)export.findViewById(R.id.rightValue)).setText(String.format("%d", GL2JNILib.totalFrame()));
        highLight(((ImageView) export.findViewById(R.id.normal)));
    }

    private void hideView()
    {
        ((TextView)dialog.findViewById(R.id.leftValue)).setVisibility(View.INVISIBLE);
        ((TextView)dialog.findViewById(R.id.rightValue)).setVisibility(View.INVISIBLE);
        ((LinearLayout)dialog.findViewById(R.id.seekbar_placeholder)).setVisibility(View.INVISIBLE);
    }

    private void initEncoder()
    {
        SeekableByteChannel ch;
        MP4Muxer muxer = null;
        System.out.println("File Name " + fileName);
        File file = new File(PathManager.LocalCacheFolder+"/"+fileName+".mp4");
        try {
            encoder = new SequenceEncoder(file);
            ch = NIOUtils.writableFileChannel(file);
            muxer = new MP4Muxer(ch, Brand.MP4);
        } catch (IOException e) {
            e.printStackTrace();
        }
        if(muxer != null)
            muxer.addTrack(TrackType.VIDEO, 25);
    }

    private void initPreview()
    {
        ((Button)dialog.findViewById(R.id.cancel)).setOnClickListener(this);
        ((Button)dialog.findViewById(R.id.done)).setOnClickListener(this);
    }

    private void initRangeBar()
    {
        final RangeSeekBar<Integer> seekBar = new RangeSeekBar<Integer>(1, GL2JNILib.totalFrame(), mContext);
        seekBar.setOnRangeSeekBarChangeListener(new RangeSeekBar.OnRangeSeekBarChangeListener<Integer>() {
            @Override
            public void onRangeSeekBarValuesChanged(RangeSeekBar<?> bar, Integer minValue, Integer maxValue) {
                if(maxValue == minValue){
                    if(maxValue == GL2JNILib.totalFrame())
                        bar.setSelectedMinValue(minValue-1);
                    else bar.setSelectedMaxValue(maxValue + 1);
                }
                ((TextView) dialog.findViewById(R.id.leftValue)).setText(String.format("%d", minValue));
                ((TextView) dialog.findViewById(R.id.rightValue)).setText(String.format("%d", maxValue));
                min = minValue;
                max = maxValue;
                updateCreditLable();
            }
        });
        ViewGroup rangeBarHolder = (ViewGroup) dialog.findViewById(R.id.seekbar_placeholder);
        rangeBarHolder.addView(seekBar);
    }

    private void highLight(View v){
        ((ViewGroup)((ImageView)dialog.findViewById(R.id.normal)).getParent()).setBackgroundColor((v.getId() == R.id.normal) ? Color.WHITE : 0x00000000);
        ((ViewGroup)((ImageView)dialog.findViewById(R.id.toon)).getParent()).setBackgroundColor((v.getId() == R.id.toon) ? Color.WHITE : 0x00000000);
        ((ViewGroup)((ImageView)dialog.findViewById(R.id.cloud)).getParent()).setBackgroundColor((v.getId() == R.id.cloud) ? Color.WHITE : 0x00000000);
    }

    private void disableOptions(int renderType)
    {
        waterMark = (renderType != Constants.CLOUD);
        ((Switch) dialog.findViewById(R.id.waterMark)).setChecked((renderType != Constants.CLOUD));
        ((Switch) dialog.findViewById(R.id.waterMark)).setClickable((renderType != Constants.CLOUD));
        ((LinearLayout) dialog.findViewById(R.id.color_picket_btn)).setClickable((renderType != Constants.CLOUD));
        updateCreditLable();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.color_picket_btn:
                ((EditorView)((Activity)mContext)).colorPicker.showColorPicker(v,null,Constants.RENDERING_VIEW);
                break;
            case R.id.normal:
                shader = Constants.NORMAL_SHADER;
                disableOptions(Constants.NORMAL_SHADER);
                highLight(v);
                break;
            case R.id.toon:
                shader = Constants.TOON_SHADER;
                disableOptions(Constants.TOON_SHADER);
                highLight(v);
                break;
            case R.id.cloud:
                shader = Constants.CLOUD;
                disableOptions(Constants.CLOUD);
                highLight(v);
                break;
            case R.id.waterMark:
                waterMark = ((Switch)dialog.findViewById(R.id.waterMark)).isChecked();
                break;
            case R.id.THOUSAND_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.THOUSAND_EIGHTY);
                if(GL2JNILib.resolutionType() != Constants.THOUSAND_EIGHTY)
                    updateCamera();
                break;
            case R.id.SEVEN_TWENTY:
                sp.setData(mContext, "cameraResolution", Constants.SEVEN_TWENTY);
                if(GL2JNILib.resolutionType() != Constants.SEVEN_TWENTY)
                    updateCamera();
                break;
            case R.id.FOUR_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.FOUR_EIGHTY);
                if(GL2JNILib.resolutionType() != Constants.FOUR_EIGHTY)
                    updateCamera();
                break;
            case R.id.THREE_SIXTY:
                sp.setData(mContext, "cameraResolution", Constants.THREE_SIXTY);
                if(GL2JNILib.resolutionType() != Constants.THREE_SIXTY)
                    updateCamera();
                break;
            case R.id.TWO_FOURTY:
                sp.setData(mContext, "cameraResolution", Constants.TWO_FOURTY);
                if(GL2JNILib.resolutionType() != Constants.TWO_FOURTY)
                    updateCamera();
                break;
            case R.id.cancel:
                cancel = true;
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
                dialog.dismiss();
                FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".png");
                FileHelper.deleteFilesAndFolder(PathManager.RenderPath+"/"+fileName+".mp4");
                break;
            case R.id.next:
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
                FileHelper.mkDir(PathManager.LocalCacheFolder);
                dialog.setContentView(R.layout.render_preview);

                initPreview();
                fileName = new SimpleDateFormat("ddMMyyyyHHmmss").format(new Date());
                doRendering(GL2JNILib.currentFrame());
                if(type == Constants.EXPORT_VIDEO){
                    updateprogress();
                    initEncoder();
                    encodeImage(min);
                }
                break;
            case R.id.done:
                cancel = true;
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
                dialog.dismiss();
                break;

        }
        updateCreditLable();
    }

    public void updateColor(float x, float y, float z)
    {
        this.x = x; this.y = y; this.z = z;
    }

    public void updateCamera()
    {
        cameraProperty((int) GL2JNILib.cameraFov(), sp.getInt(mContext, "cameraResolution"), false);
    }

    public void cameraProperty(final int fov, final int resolution, final boolean storeAction){
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraPropertyChanged(fov, resolution, storeAction);
                ((Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateCreditLable();
                    }
                });
            }
        });
    }

    public void doRendering(final int frame)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (frame <= max)
                    GL2JNILib.renderFrame(Export.this, frame, shader, (type == Constants.EXPORT_IMAGES), !waterMark, x, y, z);
                else {
                    ((Activity) mContext).runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ((ProgressBar) dialog.findViewById(R.id.previewProgress)).setVisibility(View.INVISIBLE);
                            ((TextView) dialog.findViewById(R.id.progresstext)).setText("Making Video");
                            ((ProgressBar) dialog.findViewById(R.id.roundProgress)).setVisibility(View.VISIBLE);
                        }
                    });
                }
            }
        });
    }

    public void updatePreview(final int frame)
    {
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!cancel && type != Constants.EXPORT_IMAGES) {
                    renderedImg.add(renderedImg.size(), frame);
                    updateprogress();
                    doRendering(frame + 1);
                }
                ((ImageView) dialog.findViewById(R.id.preview)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalCacheFolder + "/" + frame + ".png"));
                if (type == Constants.EXPORT_IMAGES) {
                    FileHelper.move(PathManager.LocalCacheFolder + "/" + frame + ".png", PathManager.RenderPath + "/" + fileName + ".png");
                    doFinish();
                    UIHelper.informDialog(mContext, "Image Successfully saved in your SDCard/Iyan3D/Render");
                }
            }
        });
    }

    private void updateprogress()
    {
        ((ProgressBar)dialog.findViewById(R.id.previewProgress)).setMax((max - min) + 1);
        ((ProgressBar)dialog.findViewById(R.id.previewProgress)).setProgress(((ProgressBar) dialog.findViewById(R.id.previewProgress)).getProgress() + 1);
        ((TextView)dialog.findViewById(R.id.progresstext)).setText(String.format("%d%s%d", ((ProgressBar) dialog.findViewById(R.id.previewProgress)).getProgress(), "/", (max - min) + 1));
    }

    private void encodeImage(int frame){
        frameCount = frame;
        (new Thread(new Runnable() {
            @Override
            public void run() {
                while (frameCount <= max ) {
                    if(cancel) break;
                    try {
                        while (renderedImg.size() < 1) {
                            if(cancel) break;
                            try {
                                Thread.sleep(10);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        org.jcodec.common.model.Picture pic = BitmapUtil.fromBitmap(BitmapFactory.decodeFile(PathManager.LocalCacheFolder + "/" + renderedImg.get(0) + ".png"));
                        encoder.encodeNativeFrame(pic);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if (frameCount >= max && !cancel) {
                        try {
                            encoder.finish();
                            FileHelper.move(PathManager.LocalCacheFolder + "/" + fileName + ".mp4", PathManager.RenderPath + "/" + fileName + ".mp4");
                            UIHelper.informDialog(mContext, "Video successfully saved in your SDCard/Iyan3D/Render");
                            ((Activity)mContext).runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    doFinish();
                                }
                            });
                            cancel = true;
                            return;
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                    FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/" + renderedImg.get(0) + ".png");
                    renderedImg.remove(0);
                    frameCount++;
                }
            }
        })).start();
    }

    private void doFinish()
    {
        ((ProgressBar) dialog.findViewById(R.id.roundProgress)).setVisibility(View.GONE);
        ((Button) dialog.findViewById(R.id.done)).setVisibility(View.VISIBLE);
        ((Button) dialog.findViewById(R.id.done)).setOnClickListener(Export.this);
        ((TextView) dialog.findViewById(R.id.progresstext)).setVisibility(View.INVISIBLE);
        ((ProgressBar)dialog.findViewById(R.id.previewProgress)).setVisibility(View.GONE);
    }

    private void updateCreditLable()
    {
            if(Constants.isPremium(mContext) && (shader != Constants.CLOUD))
                ((TextView)dialog.findViewById(R.id.credit_lable)).setVisibility(View.INVISIBLE);
            else {
                ((TextView)dialog.findViewById(R.id.credit_lable)).setVisibility(View.VISIBLE);
            int valueForRender = 0;
            if((shader != Constants.CLOUD))
                valueForRender = (GL2JNILib.resolutionType() == Constants.THOUSAND_EIGHTY) ? 8 : (GL2JNILib.resolutionType() == Constants.SEVEN_TWENTY) ? 4 : (GL2JNILib.resolutionType() == Constants.FOUR_EIGHTY) ? 2 : (GL2JNILib.resolutionType() == Constants.THREE_SIXTY) ? 1 : 0;
            else
                valueForRender = (GL2JNILib.resolutionType() == Constants.THOUSAND_EIGHTY) ? 230 :(GL2JNILib.resolutionType() == Constants.SEVEN_TWENTY) ? 100 : (GL2JNILib.resolutionType() == Constants.FOUR_EIGHTY) ? 45 : (GL2JNILib.resolutionType() == Constants.THREE_SIXTY) ? 25 : 10;

            int frames = (type == Constants.EXPORT_IMAGES) ? 1 : ((max - min) +1);
            int waterMarkCredit = ((!waterMark && (shader != Constants.CLOUD)) ? 50 : 0);
            int creditsForFrames = frames * valueForRender;
            int credits = (creditsForFrames + waterMarkCredit);
                ((TextView)dialog.findViewById(R.id.credit_lable)).setText((credits == 0 ) ? "" : String.format("%d Credits", credits));
        }
    }
}
