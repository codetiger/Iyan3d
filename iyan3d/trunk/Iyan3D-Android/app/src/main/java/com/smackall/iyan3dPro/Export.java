package com.smackall.iyan3dPro;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.view.ContextThemeWrapper;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.Switch;
import android.widget.TextView;

import com.smackall.iyan3dPro.Analytics.HitScreens;
import com.smackall.iyan3dPro.Helper.BitmapUtil;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.DescriptionManager;
import com.smackall.iyan3dPro.Helper.Encoder;
import com.smackall.iyan3dPro.Helper.Events;
import com.smackall.iyan3dPro.Helper.FileHelper;
import com.smackall.iyan3dPro.Helper.MediaScannerWrapper;
import com.smackall.iyan3dPro.Helper.PathManager;
import com.smackall.iyan3dPro.Helper.RangeSeekBar;
import com.smackall.iyan3dPro.Helper.SharedPreferenceManager;
import com.smackall.iyan3dPro.Helper.UIHelper;
import com.smackall.iyan3dPro.OverlayDialogs.HelpDialogs;
import com.smackall.iyan3dPro.opengl.GL2JNILib;

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
public class Export implements View.OnClickListener {
    public Dialog dialog;
    public boolean isRendering = false;
    FFmpeg fFmpeg;
    int frameCount;
    ArrayList<Integer> renderedImg = new ArrayList<>();
    int credit;
    boolean renderingStarted = false;
    float divideValue = 0.0f;
    private Context mContext;
    private SharedPreferenceManager sp;
    private float x = 0.1f;
    private float y = 0.1f;
    private float z = 0.1f;
    private boolean waterMark = false;
    private int shader = Constants.NORMAL_SHADER;
    private int type = Constants.EXPORT_IMAGES;
    private boolean cancel = false;
    private Encoder encoder;
    private int min, max;
    private String fileName;

    public Export(Context mContext, SharedPreferenceManager sp) {
        this.mContext = mContext;
        this.sp = sp;
        fFmpeg = new FFmpeg();
    }

    public static void informDialog(final Context context, final String msg) {
        ((Activity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface informDialog, int id) {
                                if (informDialog != null)
                                    informDialog.dismiss();
                                ((EditorView) context).export.dialog.findViewById(R.id.cancel).performClick();
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

    public void showExport(final int type) {
        HitScreens.ExportVideo(mContext);
        Dialog export = new Dialog(mContext);
        export.requestWindowFeature(Window.FEATURE_NO_TITLE);
        export.setContentView(R.layout.export);
        export.setCancelable(false);
        export.setCanceledOnTouchOutside(false);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                export.getWindow().setLayout(Constants.width, Constants.height);
                break;
            default:
                divideValue = 1.5f;
                export.getWindow().setLayout((int) (Constants.width / 1.5), Constants.height);
                break;
        }

        dialog = export;
        x = 0.1f;
        y = 0.1f;
        z = 0.1f;
        min = 1;
        max = GL2JNILib.totalFrame();
        shader = Constants.NORMAL_SHADER;
        this.type = type;
        this.cancel = false;
        this.isRendering = false;
        renderingStarted = false;
        encoder = null;
        renderedImg.clear();
        credit = 0;
        fileName = "";
        export.show();
        initViews(export);
        setDefaultValues(export);
        if (type == Constants.EXPORT_VIDEO)
            initRangeBar();
        else {
            min = max = GL2JNILib.currentFrame() + 1;
            hideView();
        }

        final String className = mContext.getClass().getSimpleName();
        final DescriptionManager descriptionManager = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection) (mContext)).descriptionManager : ((EditorView) (mContext)).descriptionManager);
        final HelpDialogs helpDialogs = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection) (mContext)).helpDialogs : ((EditorView) (mContext)).helpDialogs);
        export.findViewById(R.id.help).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.findViewById(R.id.viewGroup).bringToFront();
                descriptionManager.addExportViewDescriptions(mContext, dialog, type);
                helpDialogs.showPop(mContext, ((ViewGroup) dialog.findViewById(R.id.viewGroup)), (divideValue > 0) ? dialog.findViewById(R.id.viewGroup).getWidth() : 0);
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
                    descriptionManager.addExportViewDescriptions(mContext, dialog, type);
                    helpDialogs.showPop(mContext, (ViewGroup) dialog.getWindow().getDecorView(), (divideValue > 0) ? dialog.findViewById(R.id.viewGroup).getWidth() : 0);
                }
            }
        });
    }

    private void initViews(Dialog export) {
        export.findViewById(R.id.color_picket_btn).setOnClickListener(this);
        export.findViewById(R.id.THOUSAND_EIGHTY).setOnClickListener(this);
        export.findViewById(R.id.SEVEN_TWENTY).setOnClickListener(this);
        export.findViewById(R.id.FOUR_EIGHTY).setOnClickListener(this);
        export.findViewById(R.id.THREE_SIXTY).setOnClickListener(this);
        export.findViewById(R.id.TWO_FOURTY).setOnClickListener(this);
        export.findViewById(R.id.cancel).setOnClickListener(this);
        export.findViewById(R.id.next).setOnClickListener(this);
        ((TextView) export.findViewById(R.id.leftValue)).setText("1");
        ((TextView) export.findViewById(R.id.rightValue)).setText(String.format(Locale.getDefault(), "%d", GL2JNILib.totalFrame()));

        FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/");
    }

    private void hideView() {
        dialog.findViewById(R.id.leftValue).setVisibility(View.INVISIBLE);
        dialog.findViewById(R.id.rightValue).setVisibility(View.INVISIBLE);
        dialog.findViewById(R.id.seekbar_placeholder).setVisibility(View.INVISIBLE);
    }

    private void initEncoder() {
        MP4Muxer muxer = null;
        FileHelper.mkDir(PathManager.LocalCacheFolder + "/");
        File file = new File(PathManager.LocalCacheFolder + "/" + fileName + ".mp4");
        try {
            encoder = new Encoder(file);
            SeekableByteChannel ch = NIOUtils.writableFileChannel(file);
            muxer = new MP4Muxer(ch, Brand.MP4);
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (muxer != null)
            muxer.addTrack(TrackType.VIDEO, 25);
    }

    private void initPreview() {
        dialog.findViewById(R.id.cancel).setOnClickListener(this);
        dialog.findViewById(R.id.done).setOnClickListener(this);
    }

    private void initRangeBar() {
        final RangeSeekBar<Integer> seekBar = new RangeSeekBar<Integer>(1, GL2JNILib.totalFrame(), mContext);
        seekBar.setOnRangeSeekBarChangeListener(new RangeSeekBar.OnRangeSeekBarChangeListener<Integer>() {
            @Override
            public void onRangeSeekBarValuesChanged(RangeSeekBar<?> bar, Integer minValue, Integer maxValue) {
                if (maxValue == minValue) {
                    if (maxValue == GL2JNILib.totalFrame())
                        bar.setSelectedMinValue(minValue - 1);
                    else bar.setSelectedMaxValue(maxValue + 1);
                }
                ((TextView) dialog.findViewById(R.id.leftValue)).setText(String.format(Locale.getDefault(), "%d", minValue));
                ((TextView) dialog.findViewById(R.id.rightValue)).setText(String.format(Locale.getDefault(), "%d", maxValue));
                min = minValue;
                max = maxValue;
            }
        });
        ViewGroup rangeBarHolder = (ViewGroup) dialog.findViewById(R.id.seekbar_placeholder);
        rangeBarHolder.addView(seekBar);
    }

    private void setDefaultValues(Dialog camera_prop) {
        int cameraResolution = GL2JNILib.resolutionType();
        ((RadioButton) camera_prop.findViewById(R.id.THOUSAND_EIGHTY)).setChecked((cameraResolution == Constants.THOUSAND_EIGHTY));
        ((RadioButton) camera_prop.findViewById(R.id.SEVEN_TWENTY)).setChecked((cameraResolution == Constants.SEVEN_TWENTY));
        ((RadioButton) camera_prop.findViewById(R.id.FOUR_EIGHTY)).setChecked((cameraResolution == Constants.FOUR_EIGHTY));
        ((RadioButton) camera_prop.findViewById(R.id.THREE_SIXTY)).setChecked((cameraResolution == Constants.THREE_SIXTY));
        ((RadioButton) camera_prop.findViewById(R.id.TWO_FOURTY)).setChecked((cameraResolution == Constants.TWO_FOURTY));
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.color_picket_btn:
                ((EditorView) mContext).colorPicker.showColorPicker(v, null, Constants.RENDERING_VIEW);
                break;
            case R.id.THOUSAND_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.THOUSAND_EIGHTY);
                if (GL2JNILib.resolutionType() != Constants.THOUSAND_EIGHTY)
                    updateCamera();
                break;
            case R.id.SEVEN_TWENTY:
                sp.setData(mContext, "cameraResolution", Constants.SEVEN_TWENTY);
                if (GL2JNILib.resolutionType() != Constants.SEVEN_TWENTY)
                    updateCamera();
                break;
            case R.id.FOUR_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.FOUR_EIGHTY);
                if (GL2JNILib.resolutionType() != Constants.FOUR_EIGHTY)
                    updateCamera();
                break;
            case R.id.THREE_SIXTY:
                sp.setData(mContext, "cameraResolution", Constants.THREE_SIXTY);
                if (GL2JNILib.resolutionType() != Constants.THREE_SIXTY)
                    updateCamera();
                break;
            case R.id.TWO_FOURTY:
                sp.setData(mContext, "cameraResolution", Constants.TWO_FOURTY);
                if (GL2JNILib.resolutionType() != Constants.TWO_FOURTY)
                    updateCamera();
                break;
            case R.id.cancel:
                HitScreens.EditorView(mContext);
                Events.exportCancelAction(mContext);
                cancel = true;
                isRendering = false;
                if (encoder != null && encoder.spsList.size() > 0) {
                    try {
                        encoder.finish();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                GL2JNILib.setfreezeRendering(false);
                GL2JNILib.setCurrentFrame(0, ((EditorView) mContext).nativeCallBacks);
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/");
                if (dialog != null && dialog.isShowing()) dialog.dismiss();
                FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".png");
                FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".mp4");
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                break;
            case R.id.next:
                if (type == Constants.EXPORT_VIDEO && (min == max)) {
                    UIHelper.informDialog(mContext, mContext.getResources().getString(R.string.chooseMinTwoFrame));
                    return;
                }
                enableControls(dialog, false);
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/");
                FileHelper.mkDir(PathManager.LocalCacheFolder + "/");
                v.setVisibility(View.GONE);
                nextBtnAction();
                break;
            case R.id.done:
                HitScreens.EditorView(mContext);
                cancel = true;
                GL2JNILib.setfreezeRendering(false);
                GL2JNILib.setCurrentFrame(0, ((EditorView) mContext).nativeCallBacks);
                FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
                if (dialog != null && dialog.isShowing())
                    dialog.dismiss();
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                break;
        }
    }

    private void nextBtnAction() {

       if(type == Constants.EXPORT_VIDEO || type == Constants.EXPORT_IMAGES) {
            Events.exportNextAction(mContext);
            GL2JNILib.setfreezeRendering(true);
            dialog.setContentView(R.layout.render_preview);
            initPreview();
            fileName = new SimpleDateFormat("ddMMyyyyHHmmss", Locale.getDefault()).format(new Date());
            isRendering = true;
            FileHelper.mkDir(PathManager.LocalCacheFolder + "/");
            doRendering(min);
            updateprogress();
        }
    }


    public void updateColor(float x, float y, float z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public void updateCamera() {
        cameraProperty((int) GL2JNILib.cameraFov(), sp.getInt(mContext, "cameraResolution"), false);
    }

    public void cameraProperty(final int fov, final int resolution, final boolean storeAction) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraPropertyChanged(fov, resolution, storeAction);
            }
        });
    }

    private void doFinish() {
        isRendering = false;
        dialog.findViewById(R.id.cancel).setVisibility(View.GONE);
        dialog.findViewById(R.id.roundProgress).setVisibility(View.GONE);
        dialog.findViewById(R.id.done).setVisibility(View.VISIBLE);
        dialog.findViewById(R.id.done).setOnClickListener(Export.this);
        dialog.findViewById(R.id.progresstext).setVisibility(View.INVISIBLE);
        dialog.findViewById(R.id.previewProgress).setVisibility(View.GONE);
    }

    public void doRendering(final int frame) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (frame <= max) {
                    GL2JNILib.updatePhysics(frame);
                    GL2JNILib.renderFrame(((EditorView) mContext).nativeCallBacks, frame, shader, (type == Constants.EXPORT_IMAGES), !waterMark, x, y, z);
                } else {
                    ((Activity) mContext).runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            dialog.findViewById(R.id.previewProgress).setVisibility(View.INVISIBLE);
                            ((TextView) dialog.findViewById(R.id.progresstext)).setText(String.format(Locale.getDefault(), "%s", "Making Video"));
                            dialog.findViewById(R.id.roundProgress).setVisibility(View.VISIBLE);
                            try {
                                if (Constants.isFfmpegSupport() && !Constants.forceToJCodec)
                                    runFFmpeg();
                            } catch (Exception ignored) {
                            }
                        }
                    });
                }
            }
        });
    }

    public void updatePreview(final int frame) {
        ((Activity) mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!cancel) {
                    try {
                        System.out.println("Frame Name : " + frame);
                        ((ImageView) dialog.findViewById(R.id.preview)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalCacheFolder + "/" + frame + ".png"));
                    } catch (OutOfMemoryError e) {
                        UIHelper.informDialog(mContext, mContext.getResources().getString(R.string.outOfMemory));
                    }
                }
                if (!cancel && type != Constants.EXPORT_IMAGES) {
                    updateprogress();
                    doRendering(frame + 1);
                    renderedImg.add(renderedImg.size(), frame);
                }
                if (!renderingStarted) {
                    renderingStarted = true;
                    if (type == Constants.EXPORT_VIDEO && (!Constants.isFfmpegSupport() || Constants.forceToJCodec)) {
                        initEncoder();
                        encodeImage(min);
                    }
                }
                if (type == Constants.EXPORT_IMAGES) {
                    finishExport(frame, true, "");
                }
            }
        });
    }

    private void updateprogress() {
        ((ProgressBar) dialog.findViewById(R.id.previewProgress)).setMax((max - min) + 1);
        ((ProgressBar) dialog.findViewById(R.id.previewProgress)).setProgress(((ProgressBar) dialog.findViewById(R.id.previewProgress)).getProgress() + 1);
        ((TextView) dialog.findViewById(R.id.progresstext)).setText(String.format(Locale.getDefault(), "%d%s%d", ((ProgressBar) dialog.findViewById(R.id.previewProgress)).getProgress(), "/", (max - min) + 1));
    }

    private void encodeImage(final int frame) {
        frameCount = frame;
        (new Thread(new Runnable() {
            @Override
            public void run() {
                while (frameCount <= max) {
                    if (cancel) break;
                    try {
                        while (renderedImg.size() < 1) {
                            if (cancel) break;
                            try {
                                Thread.sleep(10);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        if (cancel) break;
                        Picture pic = null;
                        try {
                            if (renderedImg.size() > 0)
                                pic = BitmapUtil.fromBitmap(BitmapFactory.decodeFile(PathManager.LocalCacheFolder + "/" + renderedImg.get(0) + ".png"));
                        } catch (OutOfMemoryError e) {
                            e.printStackTrace();
                            UIHelper.informDialog(mContext, mContext.getResources().getString(R.string.outOfMemory));
                            isRendering = false;
                            cancel = true;
                            if (dialog != null && dialog.isShowing())
                                dialog.dismiss();
                            break;
                        }
                        if (pic != null && !cancel)
                            encoder.encodeNativeFrame(pic);
                    } catch (IOException | IllegalStateException e) {
                        e.printStackTrace();
                    }
                    try {
                        if (frameCount >= max && !cancel) {
                            finishExport(0, true, "");
                            cancel = true;
                            return;
                        }
                    } catch (IllegalStateException ignored) {
                    }
                    if (renderedImg.size() > 0) {
                        try {
                            FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/" + renderedImg.get(0) + ".png");
                            renderedImg.remove(0);
                        } catch (IndexOutOfBoundsException ignored) {

                        }
                    }
                    frameCount++;
                }
            }
        })).start();
    }

    public void finishExport(int frame, boolean status, String msg) {
        if (!status) {
            encoder = null;
            cancel = true;
            if (dialog != null && dialog.isShowing())
                dialog.dismiss();
            FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
            UIHelper.informDialog(mContext, mContext.getResources().getString(R.string.unableToCompleteExportMsg));
            dialog.findViewById(R.id.cancel).performClick();
            return;
        }

        if (type == Constants.EXPORT_IMAGES) {
            FileHelper.move(PathManager.LocalCacheFolder + "/" + frame + ".png", PathManager.RenderPath + "/" + fileName + ".png");
            doFinish();
            MediaScannerWrapper.scan(mContext, PathManager.RenderPath + "/" + fileName + ".png");
            renderCompletedDialog(mContext, mContext.getResources().getString(R.string.imgSavedSuccessMsg));
        } else if (type == Constants.EXPORT_VIDEO) {
            if (!Constants.isFfmpegSupport() || Constants.forceToJCodec) {
                if (encoder != null)
                    try {
                        encoder.finish();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                else {
                    UIHelper.informDialog(mContext, mContext.getResources().getString(R.string.unableToContinue));
                    isRendering = false;
                    cancel = true;
                    if (dialog != null && dialog.isShowing())
                        dialog.dismiss();
                    return;
                }
            }
            FileHelper.move(PathManager.LocalCacheFolder + "/" + fileName + ".mp4", PathManager.RenderPath + "/" + fileName + ".mp4");
            MediaScannerWrapper.scan(mContext, PathManager.RenderPath + "/" + fileName + ".mp4");
            renderCompletedDialog(mContext, mContext.getResources().getString(R.string.vidSavedSuccessMsg));
            ((Activity) mContext).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    doFinish();
                }
            });
        }
    }

    public int[] getCameraResolution() {
        int[] resolution = new int[2];
        switch (GL2JNILib.resolutionType()) {
            case Constants.THOUSAND_EIGHTY:
                resolution[0] = 1920;
                resolution[1] = 1080;
                break;
            case Constants.SEVEN_TWENTY:
                resolution[0] = 1280;
                resolution[1] = 720;
                break;
            case Constants.FOUR_EIGHTY:
                resolution[0] = 848;
                resolution[1] = 480;
                break;
            case Constants.THREE_SIXTY:
                resolution[0] = 640;
                resolution[1] = 360;
                break;
            case Constants.TWO_FOURTY:
                resolution[0] = 352;
                resolution[1] = 240;
                break;
        }
        return resolution;
    }

    public void renderCompletedDialog(final Context context, final String msg) {
        ((Activity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface informDialog, int id) {
                                if (dialog != null && dialog.isShowing())
                                    dialog.findViewById(R.id.done).performClick();
                                Intent i = new Intent(mContext, Preview.class);
                                i.putExtra("path", fileName);
                                mContext.startActivity(i);
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

    public void destroyRendering() {
        cancel = true;
        GL2JNILib.setfreezeRendering(false);
        GL2JNILib.setCurrentFrame(0, ((EditorView) mContext).nativeCallBacks);
        FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
        if (dialog != null) {
            dialog.findViewById(R.id.cancel).performClick();
        }
        FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".png");
        FileHelper.deleteFilesAndFolder(PathManager.RenderPath + "/" + fileName + ".mp4");
        FileHelper.mkDir(PathManager.LocalCacheFolder + "/");
    }

    private void enableControls(Dialog export, boolean state) {
        export.findViewById(R.id.color_picket_btn).setEnabled(state);
        export.findViewById(R.id.THOUSAND_EIGHTY).setEnabled(state);
        export.findViewById(R.id.SEVEN_TWENTY).setEnabled(state);
        export.findViewById(R.id.FOUR_EIGHTY).setEnabled(state);
        export.findViewById(R.id.THREE_SIXTY).setEnabled(state);
        export.findViewById(R.id.TWO_FOURTY).setEnabled(state);
        export.findViewById(R.id.cancel).setEnabled(state);
        export.findViewById(R.id.next).setEnabled(state);

        export.findViewById(R.id.color_picket_btn).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.THOUSAND_EIGHTY).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.SEVEN_TWENTY).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.FOUR_EIGHTY).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.THREE_SIXTY).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.TWO_FOURTY).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.cancel).setAlpha((state) ? 1.0f : 0.5f);
        export.findViewById(R.id.next).setAlpha((state) ? 1.0f : 0.5f);
    }

    private void runFFmpeg() {
        String widthHeight = getCameraResolution()[0] + "x" + getCameraResolution()[1];
        if (fFmpeg == null) fFmpeg = new FFmpeg();
        int status = fFmpeg.ffmpegRunCommand(Integer.toString(min), fileName);
        if (status == 0) {
                finishExport(0, true, "");
        } else {
            UIHelper.informDialog(mContext, mContext.getResources().getString(R.string.unableToContinue));
            isRendering = false;
            cancel = true;
            if (dialog != null && dialog.isShowing())
                dialog.dismiss();
        }
    }
}
