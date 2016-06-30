package com.smackall.animator;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.Switch;
import android.widget.TextView;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.BitmapUtil;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CreditTask;
import com.smackall.animator.Helper.DescriptionManager;
import com.smackall.animator.Helper.Encoder;
import com.smackall.animator.Helper.Events;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.HQTaskDB;
import com.smackall.animator.Helper.MediaScannerWrapper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.RangeSeekBar;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.OverlayDialogs.HelpDialogs;
import com.smackall.animator.opengl.GL2JNILib;

import org.jcodec.api.SequenceEncoder;
import org.jcodec.common.NIOUtils;
import org.jcodec.common.SeekableByteChannel;
import org.jcodec.common.model.Picture;
import org.jcodec.containers.mp4.Brand;
import org.jcodec.containers.mp4.TrackType;
import org.jcodec.containers.mp4.muxer.MP4Muxer;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;


/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Export implements View.OnClickListener,CreditTask , CompoundButton.OnCheckedChangeListener {
    private Context mContext;
    private SharedPreferenceManager sp;
    public Dialog dialog;

    private float x = 0.1f;    private float y = 0.1f;    private float z = 0.1f;
    private boolean waterMark = true;
    private int shader;
    private int type = Constants.EXPORT_IMAGES;
    private boolean cancel = false;
    private Encoder encoder;
    private int min,max;
    int frameCount;
    private String fileName;
    ArrayList<Integer> renderedImg = new ArrayList<>();
    int credit;
    String usageType;
    public boolean isRendering = false;
    TextView helpHint;
    boolean renderingStarted = false;
    float divideValue = 0.0f;

    public Export(Context mContext,SharedPreferenceManager sp){
        this.mContext = mContext;
        this.sp = sp;
    }

    public void showExport(final int type)
    {
        HitScreens.ExportVideo(mContext);
        Dialog export = new Dialog(mContext);
        export.requestWindowFeature(Window.FEATURE_NO_TITLE);
        export.setContentView(R.layout.export);
        export.setCancelable(false);
        export.setCanceledOnTouchOutside(false);
        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                export.getWindow().setLayout(Constants.width, (int) (Constants.height));
                break;
            default:
                divideValue = 1.5f;
                export.getWindow().setLayout((int) (Constants.width / 1.5), (int) (Constants.height));
                break;
        }

        dialog = export;
        x = 0.1f;
        y = 0.1f;
        z = 0.1f;
        min = 1;max =  GL2JNILib.totalFrame();
        shader = Constants.NORMAL_SHADER;
        waterMark = true;
        this.type = type;
        this.cancel =false;
        this.isRendering = false;
        renderingStarted = false;
        encoder = null;
        renderedImg.clear();
        credit = 0;
        fileName = "";
        export.show();
        initViews(export);
        setDefaultValues(export);
        updateCreditLable();
        if(type == Constants.EXPORT_VIDEO)
            initRangeBar();
        else {
            min = max = GL2JNILib.currentFrame()+1;
            hideView();
        }

        final String className = mContext.getClass().getSimpleName();
        final DescriptionManager descriptionManager = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection)(mContext)).descriptionManager : ((EditorView)(mContext)).descriptionManager);
        final HelpDialogs helpDialogs = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection)(mContext)).helpDialogs : ((EditorView)(mContext)).helpDialogs);
        export.findViewById(R.id.help).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.findViewById(R.id.viewGroup).bringToFront();
                descriptionManager.addExportViewDescriptions(mContext, dialog,type);
                helpDialogs.showPop(mContext, ((ViewGroup) dialog.findViewById(R.id.viewGroup)), (divideValue > 0) ? ((ViewGroup) dialog.findViewById(R.id.viewGroup)).getWidth() : 0);
            }
        });

        export.findViewById(R.id.viewGroup).bringToFront();
        export.findViewById(R.id.viewGroup).getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            public void onGlobalLayout() {
                if (Build.VERSION.SDK_INT < 16) {
                    dialog.findViewById(R.id.viewGroup).getViewTreeObserver().removeGlobalOnLayoutListener(this);
                } else {
                    dialog.findViewById(R.id.viewGroup).getViewTreeObserver().removeOnGlobalLayoutListener(this);
                }
                if (sp.getInt(mContext, "firstTimeUserForExport") == 0) {
                    sp.setData(mContext, "firstTimeUserForExport", 1);
                    descriptionManager.addExportViewDescriptions(mContext, dialog,type);
                    helpDialogs.showPop(mContext, (ViewGroup) dialog.getWindow().getDecorView(), (divideValue > 0) ? ((ViewGroup) dialog.findViewById(R.id.viewGroup)).getWidth() : 0);
                }
            }});
    }

    private void initViews(Dialog export)
    {
        ((LinearLayout)export.findViewById(R.id.color_picket_btn)).setOnClickListener(this);
        ((ImageView)export.findViewById(R.id.normal)).setOnClickListener(this);
        ((ImageView)export.findViewById(R.id.toon)).setOnClickListener(this);
        ((ImageView)export.findViewById(R.id.cloud)).setOnClickListener(this);
        ((Switch)export.findViewById(R.id.waterMark)).setOnCheckedChangeListener(this);
        ((RadioButton)export.findViewById(R.id.THOUSAND_EIGHTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.SEVEN_TWENTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.FOUR_EIGHTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.THREE_SIXTY)).setOnClickListener(this);
        ((RadioButton)export.findViewById(R.id.TWO_FOURTY)).setOnClickListener(this);
        ((Button)export.findViewById(R.id.cancel)).setOnClickListener(this);
        ((Button)export.findViewById(R.id.next)).setOnClickListener(this);
        ((TextView)export.findViewById(R.id.leftValue)).setText("1");
        ((TextView)export.findViewById(R.id.rightValue)).setText(String.format(Locale.getDefault(),"%d", GL2JNILib.totalFrame()));
        helpHint = ((TextView)export.findViewById(R.id.renderHelpHint));
        helpHint.setText(String.format(Locale.getDefault(),"%s","Render Normal Shader in your device."));
        highLight(((ImageView) export.findViewById(R.id.normal)));

        FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder+"/");
    }

    private void hideView()
    {
        ((TextView)dialog.findViewById(R.id.leftValue)).setVisibility(View.INVISIBLE);
        ((TextView)dialog.findViewById(R.id.rightValue)).setVisibility(View.INVISIBLE);
        ((LinearLayout)dialog.findViewById(R.id.seekbar_placeholder)).setVisibility(View.INVISIBLE);
    }

    private void initEncoder()
    {
        MP4Muxer muxer = null;
        FileHelper.mkDir(PathManager.LocalCacheFolder+"/");
        File file = new File(PathManager.LocalCacheFolder+"/"+fileName+".mp4");
        try {
            encoder = new Encoder(file);
           SeekableByteChannel ch = NIOUtils.writableFileChannel(file);
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
                if (maxValue == minValue) {
                    if (maxValue == GL2JNILib.totalFrame())
                        bar.setSelectedMinValue(minValue - 1);
                    else bar.setSelectedMaxValue(maxValue + 1);
                }
                ((TextView) dialog.findViewById(R.id.leftValue)).setText(String.format(Locale.getDefault(),"%d", minValue));
                ((TextView) dialog.findViewById(R.id.rightValue)).setText(String.format(Locale.getDefault(),"%d", maxValue));
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
        try {
            waterMark = (renderType != Constants.CLOUD);
            ((Switch) dialog.findViewById(R.id.waterMark)).setAlpha((renderType != Constants.CLOUD) ? 1.0f : 0.3f);
            ((Switch) dialog.findViewById(R.id.waterMark)).setChecked((renderType != Constants.CLOUD));
            ((Switch) dialog.findViewById(R.id.waterMark)).setClickable((renderType != Constants.CLOUD));
            ((LinearLayout) dialog.findViewById(R.id.color_picket_btn)).setAlpha((renderType != Constants.CLOUD) ? 1.0f : 0.3f);
            ((LinearLayout) dialog.findViewById(R.id.color_picket_btn)).setClickable((renderType != Constants.CLOUD));
        }
        catch (NullPointerException ignored){}
        updateCreditLable();
    }

    private void setDefaultValues(Dialog camera_prop)
    {
        int cameraResolution = GL2JNILib.resolutionType();
        ((RadioButton)camera_prop.findViewById(R.id.THOUSAND_EIGHTY)).setChecked((cameraResolution == Constants.THOUSAND_EIGHTY));
        ((RadioButton)camera_prop.findViewById(R.id.SEVEN_TWENTY)).setChecked((cameraResolution == Constants.SEVEN_TWENTY));
        ((RadioButton)camera_prop.findViewById(R.id.FOUR_EIGHTY)).setChecked((cameraResolution == Constants.FOUR_EIGHTY));
        ((RadioButton)camera_prop.findViewById(R.id.THREE_SIXTY)).setChecked((cameraResolution == Constants.THREE_SIXTY));
        ((RadioButton)camera_prop.findViewById(R.id.TWO_FOURTY)).setChecked((cameraResolution == Constants.TWO_FOURTY));
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.color_picket_btn:
                ((EditorView)((Activity)mContext)).colorPicker.showColorPicker(v,null,Constants.RENDERING_VIEW);
                break;
            case R.id.normal:
                shader = Constants.NORMAL_SHADER;
                helpHint.setText(String.format(Locale.getDefault(),"%s","Render Normal Shader in your device."));
                disableOptions(Constants.NORMAL_SHADER);
                highLight(v);
                updateCreditLable();
                break;
            case R.id.toon:
                shader = Constants.TOON_SHADER;
                helpHint.setText(String.format(Locale.getDefault(),"%s","Render Toon Shader in your device."));
                disableOptions(Constants.TOON_SHADER);
                highLight(v);
                updateCreditLable();
                break;
            case R.id.cloud:
                shader = Constants.CLOUD;
                helpHint.setText(String.format(Locale.getDefault(),"%s","Render with High Quality in cloud."));
                disableOptions(Constants.CLOUD);
                highLight(v);
                updateCreditLable();
                break;
            case R.id.waterMark:
                waterMark = ((Switch)dialog.findViewById(R.id.waterMark)).isChecked();
                updateCreditLable();
                break;
            case R.id.THOUSAND_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.THOUSAND_EIGHTY);
                if(GL2JNILib.resolutionType() != Constants.THOUSAND_EIGHTY)
                    updateCamera();
                updateCreditLable();
                break;
            case R.id.SEVEN_TWENTY:
                sp.setData(mContext, "cameraResolution", Constants.SEVEN_TWENTY);
                if(GL2JNILib.resolutionType() != Constants.SEVEN_TWENTY)
                    updateCamera();
                updateCreditLable();
                break;
            case R.id.FOUR_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.FOUR_EIGHTY);
                if(GL2JNILib.resolutionType() != Constants.FOUR_EIGHTY)
                    updateCamera();
                updateCreditLable();
                break;
            case R.id.THREE_SIXTY:
                sp.setData(mContext, "cameraResolution", Constants.THREE_SIXTY);
                if(GL2JNILib.resolutionType() != Constants.THREE_SIXTY)
                    updateCamera();
                updateCreditLable();
                break;
            case R.id.TWO_FOURTY:
                sp.setData(mContext, "cameraResolution", Constants.TWO_FOURTY);
                if(GL2JNILib.resolutionType() != Constants.TWO_FOURTY)
                    updateCamera();
                updateCreditLable();
                break;
            case R.id.cancel:
                HitScreens.EditorView(mContext);
                Events.exportCancelAction(mContext);
                cancel = true;
                isRendering = false;
                if(encoder != null && encoder.spsList.size() > 0){
                    try {
                        encoder.finish();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                GL2JNILib.setfreezeRendering(false);
                GL2JNILib.setCurrentFrame(0,((EditorView)mContext).nativeCallBacks);
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder+"/");
                if(dialog != null)
                    dialog.dismiss();
                FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".png");
                FileHelper.deleteFilesAndFolder(PathManager.RenderPath+"/"+fileName+".mp4");
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                break;
            case R.id.next:
                if(((EditorView)(Activity)mContext).userDetails.signInType <= 0){
                    informDialog(mContext, "Please SignIn to continue.");
                    dialog.findViewById(R.id.cancel).performClick();
                    return;
                }
                if(type == Constants.EXPORT_VIDEO && (min == max)){
                    UIHelper.informDialog(mContext,"Please choose minimum two frames for export Video.");
                    return;
                }
                enableControls(dialog,false);
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder+"/");
                FileHelper.mkDir(PathManager.LocalCacheFolder+"/");
                v.setVisibility(View.GONE);
                ((ProgressBar)dialog.findViewById(R.id.creditProgress)).setVisibility(View.VISIBLE);
                ((EditorView)((Activity)mContext)).creditsManager.getCreditsForUniqueId(Export.this);
                break;
            case R.id.done:
                HitScreens.EditorView(mContext);
                cancel = true;
                GL2JNILib.setfreezeRendering(false);
                GL2JNILib.setCurrentFrame(0,((EditorView)mContext).nativeCallBacks);
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
                dialog.dismiss();
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                break;
        }
    }

    private void nextBtnAction(int currentCredits,int premiumUser)
    {
        usageType = (GL2JNILib.resolutionType() == Constants.THOUSAND_EIGHTY) ? "1080P" : (GL2JNILib.resolutionType() == Constants.SEVEN_TWENTY) ? "720P" : (GL2JNILib.resolutionType() == Constants.FOUR_EIGHTY) ? "480P" : (GL2JNILib.resolutionType() == Constants.THREE_SIXTY) ? "360P" : "240P";

        if(updateCreditLable() > currentCredits && premiumUser <= 0){
            informDialog(mContext, "You are not have enough credits please recharge your account to proceed.");
            ((Button)dialog.findViewById(R.id.next)).setVisibility(View.VISIBLE);
            enableControls(dialog,true);
        }
        else if(shader != Constants.CLOUD && (type == Constants.EXPORT_VIDEO || type == Constants.EXPORT_IMAGES)) {
            Events.exportNextAction(mContext);
            GL2JNILib.setfreezeRendering(true);
            dialog.setContentView(R.layout.render_preview);
            initPreview();
            fileName =  new SimpleDateFormat("ddMMyyyyHHmmss",Locale.getDefault()).format(new Date());
            isRendering = true;
            FileHelper.mkDir(PathManager.LocalCacheFolder+"/");
            doRendering(min);
            updateprogress();
        }
        else if(updateCreditLable() > currentCredits){
            informDialog(mContext, "You are not have enough credits please recharge your account to proceed.");
            ((Button)dialog.findViewById(R.id.next)).setVisibility(View.VISIBLE);
            enableControls(dialog,true);
        }
        else if(shader == Constants.CLOUD && updateCreditLable() <= currentCredits){
            if(!canUploadToCloud()){
                ((Button)dialog.findViewById(R.id.next)).setVisibility(View.VISIBLE);
                ((ProgressBar)dialog.findViewById(R.id.creditProgress)).setVisibility(View.GONE);
                UIHelper.informDialog(mContext,"Scene contains particles which is currently not supported in HQ rendering.");
                enableControls(dialog,true);
                return;
            }
            ((EditorView)mContext).glView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    GL2JNILib.syncPhysicsWithWorld(min,max,true);
                    GL2JNILib.save(((EditorView)((Activity)mContext)).nativeCallBacks,true,((EditorView)((Activity)mContext)).projectNameHash);
                }
            });
        }
    }

    public void packCloudRender()
    {
        if(((EditorView)((Activity)mContext)).zipManager.packCloudRender()) {
            Events.exportNextAction(mContext);
            ((EditorView) ((Activity) mContext)).publish.uploadHQRender(min, max, (credit*-1), getCameraResolution()[0],getCameraResolution()[1]);
        }
        else {
            dialog.dismiss();
            UIHelper.informDialog(mContext,"Something Wrong try again.");
        }
    }

    public void updateColor(float x, float y, float z){this.x = x; this.y = y; this.z = z;}
    public void updateCamera(){cameraProperty((int) GL2JNILib.cameraFov(), sp.getInt(mContext, "cameraResolution"), false);}

    public void cameraProperty(final int fov, final int resolution, final boolean storeAction){
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraPropertyChanged(fov, resolution, storeAction);
                ((Activity) mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateCreditLable();
                    }
                });
            }
        });
    }

    private void doFinish()
    {
        isRendering = false;
        ((Button) dialog.findViewById(R.id.cancel)).setVisibility(View.GONE);
        ((ProgressBar) dialog.findViewById(R.id.roundProgress)).setVisibility(View.GONE);
        ((Button) dialog.findViewById(R.id.done)).setVisibility(View.VISIBLE);
        ((Button) dialog.findViewById(R.id.done)).setOnClickListener(Export.this);
        ((TextView) dialog.findViewById(R.id.progresstext)).setVisibility(View.INVISIBLE);
        ((ProgressBar)dialog.findViewById(R.id.previewProgress)).setVisibility(View.GONE);
    }

    private int updateCreditLable()
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
            ((TextView)dialog.findViewById(R.id.credit_lable)).setText((credits == 0 ) ? "" : String.format(Locale.getDefault(),"%d Credits", credits));
           return credit = credits;
        }
        return credit;
    }

    public void doRendering(final int frame)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (frame <= max) {
                    GL2JNILib.updatePhysics(frame);
                    GL2JNILib.renderFrame(((EditorView) mContext).nativeCallBacks, frame, shader, (type == Constants.EXPORT_IMAGES), !waterMark, x, y, z);
                }
                else {
                    ((Activity) mContext).runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ((ProgressBar) dialog.findViewById(R.id.previewProgress)).setVisibility(View.INVISIBLE);
                            ((TextView) dialog.findViewById(R.id.progresstext)).setText(String.format(Locale.getDefault(),"%s","Making Video"));
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
                if(!cancel) {
                    try {
                        ((ImageView) dialog.findViewById(R.id.preview)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalCacheFolder + "/" + frame + ".png"));
                    }
                    catch (OutOfMemoryError e){
                        UIHelper.informDialog(mContext,"Out of memory.");
                    }
                }
                if (!cancel && type != Constants.EXPORT_IMAGES) {
                    updateprogress();
                    doRendering(frame + 1);
                    renderedImg.add(renderedImg.size(), frame);
                }
                if(!renderingStarted){
                    renderingStarted = true;
                    if (type == Constants.EXPORT_VIDEO) {
                        initEncoder();
                        encodeImage(min);
                    }
                }
                if (type == Constants.EXPORT_IMAGES) {
                    if(usageType.equals("240P")) finishExport(frame,true,"");
                    else ((EditorView) ((Activity) mContext)).creditsManager.useOrRechargeCredits(credit*-1,usageType,"","",frame,Export.this);
                }
            }
        });
    }

    private void updateprogress()
    {
        ((ProgressBar)dialog.findViewById(R.id.previewProgress)).setMax((max - min) + 1);
        ((ProgressBar)dialog.findViewById(R.id.previewProgress)).setProgress(((ProgressBar) dialog.findViewById(R.id.previewProgress)).getProgress() + 1);
        ((TextView)dialog.findViewById(R.id.progresstext)).setText(String.format(Locale.getDefault(),"%d%s%d", ((ProgressBar) dialog.findViewById(R.id.previewProgress)).getProgress(), "/", (max - min) + 1));
    }

    private void encodeImage(final int frame){
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
                        if(cancel) break;
                        Picture pic = null;
                        try {
                            if(renderedImg.size() > 0)
                                pic = BitmapUtil.fromBitmap(BitmapFactory.decodeFile(PathManager.LocalCacheFolder + "/" + renderedImg.get(0) + ".png"));
                        }
                        catch(OutOfMemoryError e){
                            e.printStackTrace();
                            UIHelper.informDialog(mContext,"Out Of memory.");
                            isRendering = false;
                            cancel = true;
                            dialog.dismiss();
                            break;
                        }
                        if(pic != null && !cancel)
                            encoder.encodeNativeFrame(pic);
                    } catch (IOException | IllegalStateException e) {
                        e.printStackTrace();
                    }
                    try {
                        if (frameCount >= max && !cancel) {
                            if (usageType.equals("240P")) finishExport(0, true, "");
                            else
                                ((EditorView) ((Activity) mContext)).creditsManager.useOrRechargeCredits(credit * -1, usageType, "", "", 0, Export.this);
                            cancel = true;
                            return;
                        }
                    }
                    catch (IllegalStateException ignored){}
                    if(renderedImg.size() > 0) {
                        try {
                            FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/" + renderedImg.get(0) + ".png");
                            renderedImg.remove(0);
                        }
                        catch (IndexOutOfBoundsException ignored){

                        }
                    }
                    frameCount++;
                }
            }
        })).start();
    }

    @Override
    public void finishExport(int frame,boolean status,String msg)
    {
        if(!status){
            encoder = null;
            cancel = true;
            dialog.dismiss();
            FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
            UIHelper.informDialog(mContext,"Check your Internet connection it was slow. So unable to complete the operation.");
            dialog.findViewById(R.id.cancel).performClick();
            return;
        }

        if(type == Constants.EXPORT_IMAGES){
            FileHelper.move(PathManager.LocalCacheFolder + "/" + frame + ".png", PathManager.RenderPath + "/" + fileName + ".png");
            doFinish();
            MediaScannerWrapper.scan(mContext,PathManager.RenderPath + "/" + fileName + ".png");
            renderCompletedDialog(mContext,"Image Successfully saved in your SDCard/Iyan3D/Render");
        }
        else if(type == Constants.EXPORT_VIDEO){
            try {
                if(encoder != null)
                    encoder.finish();
                else{
                    UIHelper.informDialog(mContext, "Unable to continue try again.");
                    isRendering = false;
                    cancel = true;
                    dialog.dismiss();
                    return;
                }
                FileHelper.move(PathManager.LocalCacheFolder + "/" + fileName + ".mp4", PathManager.RenderPath + "/" + fileName + ".mp4");
                MediaScannerWrapper.scan(mContext,PathManager.RenderPath + "/" + fileName + ".mp4");
                renderCompletedDialog(mContext,"Video successfully saved in your SDCard/Iyan3D/Render");
                ((Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        doFinish();
                    }
                });
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void failed() {
        ((Button)dialog.findViewById(R.id.next)).setVisibility(View.VISIBLE);
        ((ProgressBar)dialog.findViewById(R.id.creditProgress)).setVisibility(View.GONE);
        enableControls(dialog,true);
    }

    @Override
    public void onCreditRequestCompleted(int credit,int premiumUser) {
        nextBtnAction(credit,premiumUser);
    }

    @Override
    public void onCheckProgressCompleted(int progress, int taskId) {

    }
    @Override
    public void onTaskFileDownloadCompleted(HQTaskDB taskDB, View list, boolean downloadComplete) {

    }

    public int[] getCameraResolution()
    {
        int[] resolution = new int[2];
        switch (GL2JNILib.resolutionType()){
            case Constants.THOUSAND_EIGHTY:
                resolution[0] = 1920; resolution[1] = 1080;
                break;
            case Constants.SEVEN_TWENTY:
                resolution[0] = 1280; resolution[1] = 720;
                break;
            case Constants.FOUR_EIGHTY:
                resolution[0] = 848; resolution[1] = 480;
                break;
            case Constants.THREE_SIXTY:
                resolution[0] = 640; resolution[1] = 360;
                break;
            case Constants.TWO_FOURTY:
                resolution[0] = 352; resolution[1] = 240;
                break;
        }
        return resolution;
    }

    public static void informDialog(final Context context, final String msg){
        ((Activity)context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface informDialog, int id) {
                                informDialog.dismiss();
                                ((EditorView)(Activity)context).showLogin(((Activity) context).findViewById(R.id.login));
                                ((EditorView)(Activity)context).export.dialog.findViewById(R.id.cancel).performClick();
                            }
                        });
                informDialog.create();
                try {
                    informDialog.show();
                } catch (WindowManager.BadTokenException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void renderCompletedDialog(final Context context, final String msg){
        ((Activity)context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface informDialog, int id) {
                                Intent i = new Intent(((EditorView)mContext),Preview.class);
                                i.putExtra("path",fileName);
                                ((Activity)mContext).startActivity(i);
                                informDialog.dismiss();
                            }
                        });
                informDialog.create();
                try {
                    informDialog.show();
                } catch (WindowManager.BadTokenException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public boolean canUploadToCloud()
    {
        for (int i = 2; i < GL2JNILib.getNodeCount(); i++){
            int nodeType = GL2JNILib.getNodeType(i);
            if(nodeType == Constants.NODE_VIDEO || nodeType == Constants.NODE_PARTICLES){
                return false;
            }
        }
        return true;
    }

    public void destroyRendering()
    {
        cancel = true;
        GL2JNILib.setfreezeRendering(false);
        GL2JNILib.setCurrentFrame(0,((EditorView)mContext).nativeCallBacks);
        FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
        if(dialog != null) {
            ((Button)dialog.findViewById(R.id.cancel)).performClick();
        }
        FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".png");
        FileHelper.deleteFilesAndFolder(PathManager.RenderPath+"/"+fileName+".mp4");
        FileHelper.mkDir(PathManager.LocalCacheFolder+"/");
    }

    private void enableControls(Dialog export, boolean state){
        ((LinearLayout)export.findViewById(R.id.color_picket_btn)).setEnabled(state);
        ((ImageView)export.findViewById(R.id.normal)).setEnabled(state);
        ((ImageView)export.findViewById(R.id.toon)).setEnabled(state);
        ((ImageView)export.findViewById(R.id.cloud)).setEnabled(state);
        ((Switch)export.findViewById(R.id.waterMark)).setEnabled(state);
        ((RadioButton)export.findViewById(R.id.THOUSAND_EIGHTY)).setEnabled(state);
        ((RadioButton)export.findViewById(R.id.SEVEN_TWENTY)).setEnabled(state);
        ((RadioButton)export.findViewById(R.id.FOUR_EIGHTY)).setEnabled(state);
        ((RadioButton)export.findViewById(R.id.THREE_SIXTY)).setEnabled(state);
        ((RadioButton)export.findViewById(R.id.TWO_FOURTY)).setEnabled(state);
        ((Button)export.findViewById(R.id.cancel)).setEnabled(state);
        ((Button)export.findViewById(R.id.next)).setEnabled(state);

        ((LinearLayout)export.findViewById(R.id.color_picket_btn)).setAlpha((state) ? 1.0f : 0.5f);
        ((ImageView)export.findViewById(R.id.normal)).setAlpha((state) ? 1.0f : 0.5f);
        ((ImageView)export.findViewById(R.id.toon)).setAlpha((state) ? 1.0f : 0.5f);
        ((ImageView)export.findViewById(R.id.cloud)).setAlpha((state) ? 1.0f : 0.5f);
        ((Switch)export.findViewById(R.id.waterMark)).setAlpha((state) ? 1.0f : 0.5f);
        ((RadioButton)export.findViewById(R.id.THOUSAND_EIGHTY)).setAlpha((state) ? 1.0f : 0.5f);
        ((RadioButton)export.findViewById(R.id.SEVEN_TWENTY)).setAlpha((state) ? 1.0f : 0.5f);
        ((RadioButton)export.findViewById(R.id.FOUR_EIGHTY)).setAlpha((state) ? 1.0f : 0.5f);
        ((RadioButton)export.findViewById(R.id.THREE_SIXTY)).setAlpha((state) ? 1.0f : 0.5f);
        ((RadioButton)export.findViewById(R.id.TWO_FOURTY)).setAlpha((state) ? 1.0f : 0.5f);
        ((Button)export.findViewById(R.id.cancel)).setAlpha((state) ? 1.0f : 0.5f);
        ((Button)export.findViewById(R.id.next)).setAlpha((state) ? 1.0f : 0.5f);
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if(buttonView.getId() == R.id.waterMark){
            waterMark = ((Switch)dialog.findViewById(R.id.waterMark)).isChecked();
            updateCreditLable();
        }
    }
}
