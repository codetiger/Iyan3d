package com.smackall.animator.TextAssetsView;

/**
 * Created by Sabish on 01/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.AssetsViewController.AssetsDB;
import com.smackall.animator.AssetsViewController.AssetsJsonParsing;
import com.smackall.animator.AssetsViewController.AssetsViewDatabaseHandler;
import com.smackall.animator.InAppPurchase.PremiumUpgrade;
import com.smackall.animator.common.Constant;
import com.smackall.animator.common.FileHelper;
import com.smackall.animator.common.Services;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNILibAssetView;
import com.smackall.animator.opengl.GL2JNIViewAssetView;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import smackall.animator.R;


public class TextAssetsSelectionView extends Activity {
    TextView seek_bar_value_text, number_picker_text, char_count, assetsText_addText, fonts_store_text, fonts_my_text;

    Button assets_view_cancel_btn, assets_view_add_to_scene, import_font_btn;

    EditText assetsText_edit_text;

    int progressChanged = 0;
    public static int width, height,heighOfGridViewHolder;

    public static ProgressBar textAssetsProgressBar;

    ImageView color_picker, color_picker_btn;

    Drawable imgDrawable;

    Bitmap bitmap;

     static Context context;
    public static Activity mActivity;

    public static GridView gridView;

    LinearLayout number_picker_btn_holder , font_store_btn, my_font_btn;
    FrameLayout glview_textAsset;
    GLSurfaceView glSurfaceViewTextAsset;
    ListView listView ;
    VelocityTracker velocityTracker;
    public static List<AssetsDB> fontAssets;
    AssetsViewDatabaseHandler assetsViewDatabaseHandler;

    public static String fontStyle[] = {};
    public static String fontName[] = {};
    public static Integer[] assetsId ={};
    public static HashMap hashMap;

    String[] valuesStr = new String[] {"10","12","14","16","18","20","22","24","26","28","30","32","34","36","38","40","42","44","46","48"};

    CustomAdapterForTextAssets  customAdapterForTextAssets;
    CustomAdapterForMyFonts customAdapterForMyFonts;
    boolean isMyFonts = false;
    public boolean isRederingFinishWithText = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_text_assets_selection_view);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Display d = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        width = d.getWidth();
        height = d.getHeight();
        //Constant.downloadViewType = "text";
        context = this;
        GL2JNIViewAssetView.callBackSufaceCreated(false);
        isRederingFinishWithText = false;
        if(!Constant.isOnline(context))
            Constant.informDialog(context, "Failed connecting to the server! Please try again later.");

        Constant.textAssetsSelectionView = TextAssetsSelectionView.this;
        Constant.FullScreencall(TextAssetsSelectionView.this,context);
        mActivity = TextAssetsSelectionView.this;
        assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(context);
        hashMap = new HashMap();
        customAdapterForTextAssets =  new CustomAdapterForTextAssets(mActivity);
        customAdapterForMyFonts = new CustomAdapterForMyFonts(mActivity);


        SeekBar text_assets_seekbar = (SeekBar) findViewById(R.id.text_assets_seekbar);

        seek_bar_value_text =   (TextView) findViewById(R.id.seekbar_value_text);
        number_picker_text =    (TextView) findViewById(R.id.number_picker_btn);
        char_count =            (TextView) findViewById(R.id.char_count);
        assetsText_addText =    (TextView) findViewById(R.id.assetsText_addText);
        fonts_store_text = (TextView) findViewById(R.id.font_store_text);
        fonts_my_text = (TextView) findViewById(R.id.my_fonts_text);

        glview_textAsset = (FrameLayout) findViewById(R.id.glView_textAssets);
        assetsText_edit_text = (EditText) findViewById(R.id.textAssets_edit_text);

        color_picker_btn = (ImageView) findViewById(R.id.color_picker_btn);

        assets_view_cancel_btn =    (Button) findViewById(R.id.assets_view_cancel_btn);
        assets_view_add_to_scene =  (Button) findViewById(R.id.assets_view_add_to_scene);
        import_font_btn = (Button) findViewById(R.id.import_font_btn);

        number_picker_btn_holder =    (LinearLayout) findViewById(R.id.number_picker_btn_holder);
        LinearLayout gridViewHolder = (LinearLayout) findViewById(R.id.grid_view_holder_text_assets);
        font_store_btn = (LinearLayout) findViewById(R.id.font_store_btn);
        my_font_btn = (LinearLayout) findViewById(R.id.my_font_btn);

        textAssetsProgressBar = (ProgressBar) findViewById(R.id.textAssetsProgressBar);

        gridView = (GridView) this.findViewById(R.id.gridViewForTextAssets);

        text_assets_seekbar.setProgressDrawable(getResources().getDrawable(R.drawable.styled_progress));

        heighOfGridViewHolder = gridViewHolder.getHeight();

        ImageView imageView = new ImageView(this);
        imageView.setImageDrawable(getResources().getDrawable(R.drawable.color_picker));

        imgDrawable = (imageView).getDrawable();
        bitmap = ((BitmapDrawable)imgDrawable).getBitmap();
        GL2JNILibAssetView.setViewType(3);
        glSurfaceViewTextAsset=new GL2JNIViewAssetView(this);
        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        glview_textAsset.addView(glSurfaceViewTextAsset, glParams);
        velocityTracker=  VelocityTracker.obtain();
        glSurfaceViewTextAsset.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (GL2JNIViewAssetView.isSurfaceCreated())
                    return false;
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    velocityTracker.addMovement(event);
                    velocityTracker.computeCurrentVelocity(1000);
                    float xVelocity = velocityTracker.getXVelocity();
                    float yVelocity = velocityTracker.getYVelocity();
                    GL2JNILibAssetView.assetSwipetoRotate(xVelocity, yVelocity);
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    GL2JNILibAssetView.assetSwipetoEnd();
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                    velocityTracker.addMovement(event);
                    velocityTracker.computeCurrentVelocity(1000);
                    float xVelocity = velocityTracker.getXVelocity();
                    float yVelocity = velocityTracker.getYVelocity();
                    GL2JNILibAssetView.assetSwipetoRotate(xVelocity, yVelocity);
                    return true;
                }

                return false;
            }
        });
            assets_view_add_to_scene.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (!(assetsText_edit_text.getText().toString().length() > 0)) {
                        Constant.informDialog(context, "Please enter some text to add.");
                        return;
                    }
                    if (!isRederingFinishWithText)
                        return;
                    GL2JNILibAssetView.add3DTextToScene();
                   // Constant.animationEditor.importedItems.add("TEXT: " + assetsText_edit_text.getText().toString());//Select List
                    GL2JNILibAssetView.assetViewFinish();
                    GL2JNILib.resetContext();
                    TextAssetsSelectionView.this.finish();
                }
            });

        text_assets_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                seek_bar_value_text.setText(Integer.toString(progressChanged));
                hashMap.put("bivel", progress);
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            public void onStopTrackingTouch(SeekBar seekBar) {
                if (isRederingFinishWithText)
                    textUpdater();
            }
        });

        assetsText_edit_text.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if ((event != null && (event.getKeyCode() == KeyEvent.KEYCODE_ENTER)) || (actionId == EditorInfo.IME_ACTION_DONE)) {
                    String string = assetsText_edit_text.getText().toString();
                    if (isRederingFinishWithText)
                        textUpdater();
                }
                return false;
            }
        });

        color_picker_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                colorPicker(context, color_picker_btn.getLeft(),
                        color_picker_btn.getTop() + (color_picker_btn.getHeight() * 4));
            }
        });

        number_picker_btn_holder.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                numberPicker(context, number_picker_btn_holder.getLeft(), number_picker_btn_holder.getTop() + (number_picker_btn_holder.getHeight() * 3));
            }
        });

        assets_view_cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GL2JNILibAssetView.assetViewFinish();
                customAdapterForTextAssets.downloadManager.cancelAll();
                GL2JNILib.resetContext();
                TextAssetsSelectionView.this.finish();

            }
        });

        font_store_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                import_font_btn.setVisibility(View.GONE);
                font_store_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_left_pressed));
                my_font_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_right_radi));
                isMyFonts = false;
                loadAssetsFromDatabase("50");
            }
        });

        my_font_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (Constant.isPremium(context)) {
//                    Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//                    intent.setType("file/*");
//                    if (intent.resolveActivity(getPackageManager()) == null) {
//                        FileHelper.getFontsFromCommonIyan3dPath(context);
//                    } else
                    Constant.informDialog(context, "Please copy Fonts to SD-Card/Iyan3D folder.");
                    updateMyFontsList();
//                }
                    import_font_btn.setVisibility(View.VISIBLE);
                    font_store_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_left_radi));
                    my_font_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_right_pressed));

                } else {
                    Intent in = new Intent(context, PremiumUpgrade.class);
                    in.putExtra("actiontype", "myfonts");
                    in.putExtra("class", "TextAssetsSelectionView");
                    context.startActivity(in);
                }
//                if (!Constant.isPremium(context)) {
//                    if(Constant.isTempPremium){
//                        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//                        intent.setType("file/*");
//                        if (intent.resolveActivity(getPackageManager()) == null) {
//                            FileHelper.getFontsFromCommonIyan3dPath(context);
//                        } else
//                            updateMyFontsList();
////                }
//                        import_font_btn.setVisibility(View.VISIBLE);
//                        font_store_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_left_radi));
//                        my_font_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_right_pressed));
//                          Constant.isTempPremium=false;
//                    }
//                    else{
//                        Intent in = new Intent(getApplicationContext(), PremiumUpgrade.class);
//                        startActivity(in);
//                    }
//
//                } else {
//                    Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//                    intent.setType("file/*");
//                    if (intent.resolveActivity(getPackageManager()) == null) {
//                        FileHelper.getFontsFromCommonIyan3dPath(context);
//                    } else
//                        updateMyFontsList();
////                }
//                    import_font_btn.setVisibility(View.VISIBLE);
//                    font_store_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_left_radi));
//                    my_font_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_right_pressed));
//                }
            }
        });

        import_font_btn.setText("Refresh");
        import_font_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//                intent.setType("file/*");
//                if(intent.resolveActivity(getPackageManager()) != null) {
//                    startActivityForResult(intent, 1);
//                }
//                else{
//                    import_font_btn.setVisibility(View.INVISIBLE);
                    FileHelper.getFontsFromCommonIyan3dPath(context);
//                }
            }
        });

        assetsText_edit_text.addTextChangedListener(mTextEditorWatcher);

        loadAssetsFromDatabase("50");


    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        // TODO Auto-generated method stub
        switch(requestCode){
            case 1:
                if(resultCode==RESULT_OK){
                    Intent data = intent;
                    Uri selectedFont = data.getData();
                    String filePath = Constant.getPathFromUri(selectedFont, context);
                    String fileName = new File(filePath).getName();
                    String ext = Constant.getFileExt(new File(filePath).getName());
                    System.out.println("Font Path : " + filePath);
                    System.out.println("Font Name : " + fileName);
                    System.out.println("File Extension : " + ext);
                    if(ext.equals("ttf") || ext.equals("otf")){
                        addToMyFonts(filePath, fileName);
                    }
                    else {
                        System.out.println("File Not Valid!");
                    }
                }
                break;
        }
    }

    private final TextWatcher mTextEditorWatcher = new TextWatcher() {
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        public void onTextChanged(CharSequence s, int start, int before, int count) {
            //This sets a textview to the current length
            hashMap.put("text", s);
            char_count.setText(String.valueOf(s.length()) + "/50");
        }

        public void afterTextChanged(Editable s) {
        }
    };


    public void colorPicker(Context context, float x, float y){
        Dialog color_picker_dialog = new Dialog(context);
        color_picker_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        color_picker_dialog.setContentView(R.layout.color_picker_layout);
        color_picker_dialog.setCancelable(false);
        color_picker_dialog.setCanceledOnTouchOutside(true);

        color_picker_dialog.getWindow().setLayout(width / 3, height / 2);

        Window window = color_picker_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity= Gravity.TOP | Gravity.RIGHT;
        wlp.dimAmount=0.0f;
        wlp.x = (int) x;
        wlp.y = (int) y;
        window.setAttributes(wlp);

        color_picker = (ImageView) color_picker_dialog.findViewById(R.id.color_picker);
        final LinearLayout color_getter = (LinearLayout) color_picker_dialog.findViewById(R.id.color_geter);
        color_picker.setImageBitmap(bitmap);
        color_picker_dialog.show();

        color_picker.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {

                float eventX = event.getX();
                float eventY = event.getY();
                float[] eventXY = new float[]{eventX, eventY};

                Matrix invertMatrix = new Matrix();
                ((ImageView) v).getImageMatrix().invert(invertMatrix);

                invertMatrix.mapPoints(eventXY);
                int x = (int) eventXY[0];
                int y = (int) eventXY[1];

                //Limit x, y range within bitmap
                if (x < 0) {
                    x = 0;
                } else if (x > bitmap.getWidth() - 1) {
                    x = bitmap.getWidth() - 1;
                }

                if (y < 0) {
                    y = 0;
                } else if (y > bitmap.getHeight() - 1) {
                    y = bitmap.getHeight() - 1;
                }

                int touchedRGB = bitmap.getPixel(x, y);

                int red = Color.red(touchedRGB);
                int green = Color.green(touchedRGB);
                int blue = Color.blue(touchedRGB);

                hashMap.put("red", red);
                hashMap.put("green", green);
                hashMap.put("blue", blue);


                color_getter.setBackgroundColor(touchedRGB);

                textColorChanger(touchedRGB);
                if (event.getAction() == android.view.MotionEvent.ACTION_UP) {
                    if(isRederingFinishWithText)
                    textUpdater();
                }
                return true;
            }
        });
    }

    public void numberPicker(Context context, float x, float y) {
        final Dialog number_picker_dialog = new Dialog(context);
        number_picker_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        number_picker_dialog.setContentView(R.layout.font_size_vertical_list);
        number_picker_dialog.setCancelable(false);
        number_picker_dialog.setCanceledOnTouchOutside(true);

        number_picker_dialog.getWindow().setLayout(number_picker_btn_holder.getWidth(), height / 2);

        Window window = number_picker_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity= Gravity.TOP | Gravity.LEFT;
        wlp.dimAmount=0.0f;
        wlp.x = (int) x;
        wlp.y = (int) y;
        window.setAttributes(wlp);

        listView = (ListView) number_picker_dialog.findViewById(R.id.listView);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,R.layout.list_items, R.id.text1, valuesStr);
        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                System.out.println("List Number : " + Integer.parseInt(valuesStr[position]));
                hashMap.put("size", Integer.parseInt(valuesStr[position]));
                if(isRederingFinishWithText)
                textUpdater();
                number_picker_text.setText(valuesStr[position]);
                number_picker_dialog.dismiss();
            }
        });
        number_picker_dialog.show();
    }

    public void gridViewHorizontally(int color)
    {
            defaultHashMapValues();
            gridView.getLayoutParams().width = (width / 6) * fontName.length;
            gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
                    System.out.println("You clicked : " + position);
                }
            });

            if (!isMyFonts) {
                customAdapterForTextAssets.context = mActivity;
                customAdapterForTextAssets.fonts = fontStyle;
                customAdapterForTextAssets.fontName = fontName;
                customAdapterForTextAssets.color = color;
                gridView.setAdapter(customAdapterForTextAssets);
            } else {
                customAdapterForMyFonts.context = mActivity;
                customAdapterForMyFonts.fonts = fontStyle;
                customAdapterForMyFonts.fontName = fontName;
                customAdapterForMyFonts.color = color;
                gridView.setAdapter(customAdapterForMyFonts);
            }

            int cols = (int) Math.ceil((double) fontName.length / 1);
            System.out.println("No of Column : " + cols);
            gridView.setNumColumns(cols);

    }

    public void loadAssetsFromDatabase(String type){

        if(assetsViewDatabaseHandler.getAssetsCount("Assets") !=0) {
            textAssetsProgressBar.setVisibility(View.GONE);
            fontAssets = assetsViewDatabaseHandler.getAllAssets("", type, "Assets");
            System.out.println("Assets Size : " + fontAssets.size());

            fontName = new String[fontAssets.size()];
            fontStyle = new String[fontAssets.size()];

            assetsId = new Integer[fontAssets.size()];
            for (int n = 0; n < fontAssets.size(); n++) {
                fontStyle[n] = "Text";
                fontName[n] = fontAssets.get(n).getAssetName();
                assetsId[n] = fontAssets.get(n).getAssetsId();
            }
            isMyFonts = false;
            gridViewHorizontally(Color.WHITE);
        }

        else{
            try {
                textAssetsProgressBar.setVisibility(View.VISIBLE);
                Constant.assetsPrefs.edit().putBoolean("added", true).apply();
                AssetsJsonParsing assetsJsonParsing = new AssetsJsonParsing();
                assetsJsonParsing.loadDatabaseFromJson(context, TextAssetsSelectionView.this, "TextView",assetsViewDatabaseHandler,null,this);
            }
            catch (Exception e){

            }
            finally {

            }
        }
    }

    @Override
    public void onBackPressed()
    {
        GL2JNILibAssetView.assetViewFinish();
        customAdapterForTextAssets.downloadManager.cancelAll();
        TextAssetsSelectionView.this.finish();
    }

    public void textColorChanger(int color){
        for (int i = 0; i < assetsId.length; i++){
         View view =  gridView.getChildAt(i);
            if (view != null) {
                TextView textView = (TextView) view.findViewById(i);
                textView.setTextColor(color);
            }
        }
    }

    private void defaultHashMapValues(){
        hashMap.put("size", 10);
        hashMap.put("text", "Text");
        hashMap.put("red", 255);
        hashMap.put("green", 255);
        hashMap.put("blue", 255);
        hashMap.put("bivel", 0);
        if(fontName.length > 0)
        hashMap.put("fileName", fontName[0]);
    }

    public static void textUpdater(){
        int fontSize = (int) hashMap.get("size");
        String text = String.valueOf(hashMap.get("text"));
        int red = (int) hashMap.get("red");
        int green = (int) hashMap.get("green");
        int blue = (int) hashMap.get("blue");
        int bivelValue = (int) hashMap.get("bivel");
        String fileName = String.valueOf(hashMap.get("fileName"));

        System.out.println("Text Sequance : " + text);
        System.out.println("Bivel Value : " + bivelValue);
        System.out.println("Text Size : " + fontSize);
        System.out.println("Char Sequance : " + "Red : " + red + "  Green : " + green + " Blue : " + blue);

        GL2JNILibAssetView.remove3dText();
        System.out.println("3D Text Removed");
        GL2JNILibAssetView.load3DText(text, fontSize, bivelValue, red, blue, green, fileName);

    }

    private void addToMyFonts(String path, String fileName){

        String userFonts = Environment.getExternalStorageDirectory()+"/Android/data/" +getPackageName()+"/fonts/userfonts/";
        Constant.mkDir(userFonts);

        File from = new File(path);
        File to = new File(userFonts + fileName);

        try {
            Constant.copy(from, to);
        } catch (IOException e) {
            e.printStackTrace();
        }
        finally {
            updateMyFontsList();
        }
    }

    public void updateMyFontsList(){
        String userFonts = Environment.getExternalStorageDirectory()+"/Android/data/" +getPackageName()+"/fonts/userfonts/";
        Constant.mkDir(userFonts);
        File f = new File(userFonts);
        File file[] = f.listFiles();
        if (file != null) {
            fontName = new String[file.length];
            fontStyle = new String[file.length];
            for (int i=0; i < file.length; i++)
            {
                fontStyle[i] = "Text";
                fontName[i] = file[i].toString().substring(file[i].toString().lastIndexOf("/")+1,file[i].toString().length());
                assetsId[i] = 0;
            }
        }
        else {
            fontName = new String[0];
            fontStyle = new String[0];
            fontName[0] = "Text";
        }

        isMyFonts = true;
        //if(file.length != 0)
        gridViewHorizontally(Color.WHITE);
    }

    @Override
    public void onResume(){
        super.onResume();
        if(isMyFonts)
        FileHelper.getFontsFromCommonIyan3dPath(context);
        stopService(new Intent(this, Services.class));
        }

    public void myFontstab(){
        Constant.informDialog(context, "Please copy Fonts to SD-Card/Iyan3D folder.");
        updateMyFontsList();
        import_font_btn.setVisibility(View.VISIBLE);
        font_store_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_left_radi));
        my_font_btn.setBackgroundDrawable(getResources().getDrawable(R.drawable.pink_border_right_pressed));

    }

    @Override
    protected void onPause() {
        super.onPause();
        startService(new Intent(this, Services.class));
    }

}
