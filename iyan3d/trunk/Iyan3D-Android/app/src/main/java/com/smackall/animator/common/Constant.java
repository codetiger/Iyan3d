package com.smackall.animator.common;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.WindowManager;
import android.widget.ProgressBar;

import com.smackall.animator.AnimationEditorView.AnimationEditor;
import com.smackall.animator.AnimationEditorView.ExportDialog;
import com.smackall.animator.AnimationEditorView.RenderingDialog;
import com.smackall.animator.AssetsViewController.AssetsViewController;
import com.smackall.animator.DownloadManager.DownloadCallBack;
import com.smackall.animator.ImportAndRig.ImportObjAndTexture;
import com.smackall.animator.ImportAndRig.RigModel;
import com.smackall.animator.SplashActivity;
import com.smackall.animator.TextAssetsView.TextAssetsSelectionView;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.sceneSelectionView.SceneSelectionView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.nio.channels.FileChannel;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.List;
import java.util.Scanner;

import smackall.animator.R;


/**
 * Created by Sabish on 22/09/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class Constant {

    public static NumberProgressBar bnp;
    public static int width, height, currentItemID;
    //public static List<SceneDB> scenes;
    public static boolean isPremium= false;
    public static String sceneDatabaseDirectory = "/data/data/com.smackall.animator/databases/";
    public static String sceneDatabaseLocation = "/data/data/com.smackall.animator/databases/iyan3dScenesDB";
    public static String assetsDatabaseDirectory = "/data/data/com.smackall.animator/databases/";
    public static String assetsDatabaseLocation = "/data/data/com.smackall.animator/databases/iyan3dAssetsDB";
    public static String animDatabaseLocation = "/data/data/com.smackall.animator/databases/iyan3dAnimDB";
    public static String defaultAssetsDir = "/data/data/com.smackall.animator/files/assets";

    public static String animationsDir = "";
    public static String importedImagesdir = "";
    public static String defaultLocalAssetsDir = "";
    public static String defaultCacheDir = "";
    public static String localProjectDir = "";
    public static String downloadViewType = "";

    public static int assetJsonId;
    public static int animationJsonId;


    public static SQLiteDatabase assetsDatabase;
    public static SQLiteDatabase animDatabase;
    public static SharedPreferences assetsPrefs;
    public static boolean assetsDBAdded = true;
    public static SharedPreferences animPrefs;
    public static boolean animDBAdded = true;

    public static Dialog uiBlocker;

    public static double screenInch;
    static int dublicateNameCount = 0;

    public static RigModel rigModel;
    public static GL2JNILib gl2JNILib = new GL2JNILib();
    public static ExportDialog exportDialog;
    public static RenderingDialog renderingDialog;
    public static AnimationEditor animationEditor;
    public static ImportObjAndTexture importObjAndTexture;
    public static TextAssetsSelectionView textAssetsSelectionView;
    public static AssetsViewController assetsViewController;
    public static SplashActivity splashActivity;
    public static SceneSelectionView sceneSelectionView;
    public static DownloadCallBack downloadCallBack = new DownloadCallBack();

    public static String deviceUniqueId;

    public static AlertDialog progressAlertDialog;
    public static int unDefined = -1;
    public static AlertDialog.Builder informDialog;


    public enum TYPE
    {
        NODE_RIG("0"),
        NODE_TEXT("1");
        private String value;
        TYPE(String i) {
            value = i;
        }
        public String getValue(){
            return value;
        }
    }


    public enum ASSETS_DB
    {
        DATABASE_NAME("assets"),
        ASSETS_TABLE("Assets"),
        MY_LIBRARY_TABLE("MyLibrary"),
        KEY_ID("id"),
        KEY_ASSET_NAME("assetName"),
        KEY_IAP("iap"),
        KEY_ASSETSID("assetsId"),
        KEY_TYPE("type"),
        KEY_NBONES("nbones"),
        KEY_KEYWORDS("keywords"),
        KEY_HASH("hash"),
        KEY_TIME("time");

        private String value;
        ASSETS_DB(String i) {
            value = i;
        }
        public String getValue(){
            return value;
        }
    }

    public enum AUTORIG_SCENE_MODE
    {
        RIG_MODE_OBJVIEW(0),
        RIG_MODE_MOVE_JOINTS(1),
        RIG_MODE_EDIT_ENVELOPES(2),
        RIG_MODE_PREVIEW(3);

        private int value;
        AUTORIG_SCENE_MODE(int i) {
            value = i;
        }
        public int getValue(){
            return value;
        }
    }



    public enum ANIMATION_DB
    {
        DATABASE_NAME("animation"),
        TABLE_ANIMASSETS("AnimAssets"),
        TABLE_MYANIMASSETS("MyAnimation"),
        KEY_ID("id"),
        KEY_ASSETSID("animAssetId"),
        KEY_ANIM_NAME("animName"),
        KEY_KEYWORD("keyword"),
        KEY_USERID("userid"),
        KEY_USERNAME("username"),
        KEY_TYPE("type"),
        KEY_BONECOUNT("bonecount"),
        KEY_FEATUREDINDEX("featuredindex"),
        KEY_UPLOADEDTIME("uploaded"),
        KEY_DOWNLOADS("downloads"),
        KEY_RATING("rating"),
        PUBLISH_ID("publishId");

        private String value;
        ANIMATION_DB(String i) {
            value = i;
        }
        public String getValue(){
            return value;
        }
    }

   public enum ACTION_TYPE
    {
        DO_NOTHING(0),
        DELETE_ASSET(1),
        ADD_ASSET_BACK(2),
        DEACTIVATE_UNDO(3),
        DEACTIVATE_REDO(4),
        DEACTIVATE_BOTH(5),
        ACTIVATE_BOTH(6),
        ADD_TEXT_IMAGE_BACK(7),
        SWITCH_FRAME(8),
        RELOAD_FRAMES(9),
        SWITCH_MIRROR(10);
        private int value;
        ACTION_TYPE(int i) {
            value = i;
        }
        public int getValue(){
            return value;
        }
    }

    public enum NODE_TYPE
    {
        NODE_CAMERA (0),
        NODE_LIGHT (1),
        NODE_SGM (2),
        NODE_TEXT (4),
        NODE_IMAGE (5),
        NODE_OBJ (6),
        NODE_RIG (3),
        NODE_UNDEFINED(1),
        NODE_BITS (4),
        NODE_ADDITIONAL_LIGHT(7);
        private int value;
        NODE_TYPE(int i) {
            value = i;
        }
        public int getValue(){
            return value;
        }
    }


   public static void deleteDir(File deletePath) {
       File cacheDir = deletePath;
       if (cacheDir.exists()) {
           if (cacheDir.isDirectory())
               if(cacheDir.list() != null)
               for (String child : cacheDir.list())
                   deleteDir(new File(cacheDir, child));
           cacheDir.delete();  // delete child file or empty directory
       }
   }

    public static void mkDir(String folder){
        File newDir = new File(folder);
        if(!newDir.exists()){
            newDir.mkdir();
        }
    }

    public static void mkDir(File folder){

        if(!folder.exists()){
            folder.mkdir();
        }
    }

    public static void mkFile(String folder){
        File newDir = new File(folder);
        if(!newDir.exists()){
            try {
                newDir.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static String md5(String s) {

        try {
            // Create MD5 Hash
            MessageDigest digest = java.security.MessageDigest.getInstance("MD5");
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuffer hexString = new StringBuffer();
            for (int i=0; i<messageDigest.length; i++)
                hexString.append(Integer.toHexString(0xFF & messageDigest[i]));
            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        return "";
    }

    public static String getRealPathFromURI(Uri contentURI, Context context) {
        String result;
        Cursor cursor =context.getContentResolver().query(contentURI, null, null, null, null);
        if (cursor == null) { // Source is Dropbox or other similar local file path
            result = contentURI.getPath();
        } else {
            cursor.moveToFirst();
            int idx = cursor.getColumnIndex(MediaStore.Images.ImageColumns.DATA);
            result = cursor.getString(idx);
            cursor.close();
        }
        return result;
    }

    public static Bitmap getRoundedCornerBitmap(Bitmap bitmap) {
        Bitmap output;
        if(bitmap != null) {
            final Rect rect = new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight());
            final RectF rectF = new RectF(rect);

            output = Bitmap.createBitmap(bitmap.getWidth() + 8,
                    bitmap.getHeight() + 8, Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(output);
            final float roundPx = bitmap.getWidth() / 8;
            final Paint paint = new Paint();
            paint.setAntiAlias(true);
            canvas = new Canvas(output);
            canvas.drawARGB(0, 0, 0, 0);
            paint.setStyle(Paint.Style.FILL);
            canvas.drawRoundRect(rectF, roundPx, roundPx, paint);
            paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_IN));
            canvas.drawBitmap(bitmap, 4, 4, paint);
            paint.setXfermode(null);
            paint.setStyle(Paint.Style.STROKE);
            paint.setColor(Color.WHITE);
            paint.setStrokeWidth(5);
            canvas.drawRoundRect(rectF, roundPx, roundPx, paint);
            return output;
        }
        else 
        return null;
    }

    public static String getFileExt(String fileName) {
        return fileName.substring((fileName.lastIndexOf(".") + 1), fileName.length());
    }

    public static void copy(File src, File dst) throws IOException {
        InputStream in = new FileInputStream(src);
        OutputStream out = new FileOutputStream(dst);

        // Transfer bytes from in to out
        byte[] buf = new byte[1024];
        int len;
        while ((len = in.read(buf)) > 0) {
            out.write(buf, 0, len);
        }
        in.close();
        out.close();
    }

    public static void moveFile(File file, File dir) throws IOException {
        File newFile = new File(dir, file.getName());
        FileChannel outputChannel = null;
        FileChannel inputChannel = null;
        try {
            outputChannel = new FileOutputStream(newFile).getChannel();
            inputChannel = new FileInputStream(file).getChannel();
            inputChannel.transferTo(0, inputChannel.size(), outputChannel);
            inputChannel.close();
            file.delete();
        } finally {
            if (inputChannel != null) inputChannel.close();
            if (outputChannel != null) outputChannel.close();
        }
    }

    public static List getAssetsDetail(String columnName, String value, String databaseName, String tableName){

        if(databaseName.toLowerCase().startsWith("animation") ||
                databaseName.toLowerCase().startsWith("animations") || databaseName.toLowerCase().startsWith("anim")){
            System.out.println("Function Called");
            GetAssetsAllDetail getAssetsAllDetail = new GetAssetsAllDetail();
            return getAssetsAllDetail.getAnimAssets(columnName, value, tableName);
        }

       else if(databaseName.toLowerCase().equals("assets") || databaseName.toLowerCase().equals("text")
                || databaseName.toLowerCase().equals("mesh") || databaseName.toLowerCase().equals("asset")){
            GetAssetsAllDetail getAssetsAllDetail = new GetAssetsAllDetail();
            return getAssetsAllDetail.getAssetsDetail(columnName, value, tableName);
        }
        return null;
    }

    public static void uiBlocker(Context context){
        uiBlocker = new Dialog(context, android.R.style.Theme_Panel);
        uiBlocker.setCancelable(true);
        uiBlocker.show();
    }

    public static double textSizeChooser(Activity activity){
        DisplayMetrics dm = new DisplayMetrics();
        activity.getWindowManager().getDefaultDisplay().getMetrics(dm);
        int width=dm.widthPixels;
        int height=dm.heightPixels;
        int dens=dm.densityDpi;
        double wi=(double)width/(double)dens;
        double hi=(double)height/(double)dens;
        double x = Math.pow(wi,2);
        double y = Math.pow(hi,2);
        double screenInches = Math.sqrt(x+y);
        double inch =  (screenInches/2);
        return inch;
    }

    public static int[] helpImages(String type){

        int[] IMAGES_ID = new int[0];

        if(type.equals("common")){
            IMAGES_ID = new int[]{R.drawable.helpimage1, R.drawable.helpimage2,
                    R.drawable.helpimage3, R.drawable.helpimage4,
                    R.drawable.helpimage5, R.drawable.helpimage6, R.drawable.helpimage7};

        }
        else if(type.equals("autorig")){
            IMAGES_ID = new int[]{R.drawable.autorighelp1, R.drawable.autorighelp2,
                    R.drawable.autorighelp3, R.drawable.autorighelp4,
                    R.drawable.autorighelp5, R.drawable.autorighelp6, R.drawable.autorighelp7};
        }

        return IMAGES_ID;
    }

    public static void FullScreencall(final Activity activity,Context context) {

        if(hasSoftKeys(activity)) {
            final View decorView = activity.getWindow().getDecorView();
            if (android.os.Build.VERSION.SDK_INT > 18) {
                decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
            } else if (android.os.Build.VERSION.SDK_INT > 15) {
                decorView.setSystemUiVisibility(
                        View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                | View.SYSTEM_UI_FLAG_FULLSCREEN
                                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                );
            }
        }

            activity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
 }

    public static void screenInch(Context context){
        DisplayMetrics dm =context.getResources().getDisplayMetrics();
        double density = dm.density * 160;
        double x = Math.pow(dm.widthPixels / density, 2);
        double y = Math.pow(dm.heightPixels / density, 2);
        screenInch = Math.sqrt(x + y);
    }

    public static float getTextSize(int percentage){
        float size = (float) (((float)percentage/100.0f)*(screenInch));
        System.out.println("Text Size before Return Value : " + size);
        return size;
    }

    public static String getScreenCategory(Context context){
        if ((context.getResources().getConfiguration().screenLayout &
                Configuration.SCREENLAYOUT_SIZE_MASK) ==
                Configuration.SCREENLAYOUT_SIZE_LARGE) {
                return "large";
        }
        else if((context.getResources().getConfiguration().screenLayout &
                Configuration.SCREENLAYOUT_SIZE_MASK) ==
                Configuration.SCREENLAYOUT_SIZE_NORMAL){
            return "normal";
        }
        else if((context.getResources().getConfiguration().screenLayout &
                Configuration.SCREENLAYOUT_SIZE_MASK) ==
                Configuration.SCREENLAYOUT_SIZE_XLARGE){
            return "xlarge";
        }
        else {
            return "undefined";
        }
    }

    public static String dublicateFileNameFinder(String input){
        String fileName = "";

        File inputFile = new File(input);

        dublicateNameCount++;

        if(inputFile.exists()){
           fileName = dublicateFileNameFinder(String.valueOf(inputFile+"("+dublicateNameCount+")"));
        }

        return fileName;
    }

    public static int getNavigationWidth(Activity activity){
        if(Build.VERSION.SDK_INT > 15)
        {
            Resources resources = activity.getResources();
            int resourceId = resources.getIdentifier("navigation_bar_width", "dimen", "android");
            if (resourceId > 0) {
                System.out.println("Navigation Width : " + resources.getDimensionPixelSize(resourceId));
                return resources.getDimensionPixelSize(resourceId);
            }
        }
        return 0;
    }

    public static boolean showBetaRestrictionDialog(Context context) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder
                .setMessage("This Feature is not available for beta version.")
                .setCancelable(false)
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.dismiss();
                    }
                });
        builder.create().show();
        return true;
    }

    public static String replaceSpecialCharFromString(String string){
        String replacedString = "";
                    //if(replacedString.contains("-"))
                    replacedString = string.replaceAll("-","");
                    //if(replacedString.contains(":"))
                    replacedString = replacedString.replaceAll(":","");
                    //if(replacedString.contains(" "))
                    replacedString = replacedString.replaceAll(" ","");
        return replacedString;
    }

    public static void informDialog(Context context, String msg){
        informDialog = new AlertDialog.Builder(context);
        informDialog
                .setMessage(msg)
                .setCancelable(false)
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.dismiss();
                        informDialog = null;
                    }
                });

        informDialog.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                informDialog = null;
            }
        });
        informDialog.create();
        informDialog.show();
    }

    public static boolean hasSoftKeys(Activity c){
        boolean hasSoftwareKeys = true;
        if(Build.VERSION.SDK_INT>=Build.VERSION_CODES.JELLY_BEAN_MR1){
            Display d = c.getWindowManager().getDefaultDisplay();

            DisplayMetrics realDisplayMetrics = new DisplayMetrics();
            d.getRealMetrics(realDisplayMetrics);

            int realHeight = realDisplayMetrics.heightPixels;
            int realWidth = realDisplayMetrics.widthPixels;

            DisplayMetrics displayMetrics = new DisplayMetrics();
            d.getMetrics(displayMetrics);

            int displayHeight = displayMetrics.heightPixels;
            int displayWidth = displayMetrics.widthPixels;

            hasSoftwareKeys =  (realWidth - displayWidth) > 0 || (realHeight - displayHeight) > 0;
        }else{
            boolean hasMenuKey = false;
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.ICE_CREAM_SANDWICH) {
                hasMenuKey = ViewConfiguration.get(c).hasPermanentMenuKey();
            }
            boolean hasBackKey = KeyCharacterMap.deviceHasKey(KeyEvent.KEYCODE_BACK);
            hasSoftwareKeys = !hasMenuKey && !hasBackKey;
        }
        return hasSoftwareKeys;
    }

   public static String readFile(String fileName) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(fileName));

        try {
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();

            while (line != null) {
                sb.append(line);
                sb.append("\n");
                line = br.readLine();
            }
            return sb.toString();
        } finally {
            br.close();
        }
    }

    public static boolean isPremium(Context mContext) {
        File key = new File("data/data/"+mContext.getPackageName()+"/shared_prefs/key");
        try {
            if(!key.exists()) {
                Constant.isPremium = false;
                return Constant.isPremium;
            }
           else if (KeyMaker.readFromKeyFile(mContext) == null) {
                Constant.isPremium = false;
                return Constant.isPremium;
            }
            else {
                try {
                    if (KeyMaker.readFromKeyFile(mContext).equals(Constant.md5(Constant.deviceUniqueId + Constant.md5("Iyan3dPremium" + 1)))) {
                        Constant.isPremium = true;
                        return Constant.isPremium;
                    } else {
                        Constant.isPremium = false;
                        return Constant.isPremium;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return Constant.isPremium;
    }

    public static String getPathFromUri(Uri uri, Context context) {
        if (Build.VERSION.SDK_INT < 11)
            return RealPathUtil.getRealPathFromURI_BelowAPI11(context, uri);
        else if (Build.VERSION.SDK_INT < 19)
            return RealPathUtil.getRealPathFromURI_API11to18(context, uri);
        else
            return RealPathUtil.getRealPathFromURI_API19(context, uri);
    }

    public static boolean isOnline(Context context) {
        ConnectivityManager cm =
                (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo netInfo = cm.getActiveNetworkInfo();
        return netInfo != null && netInfo.isConnectedOrConnecting();
    }

    public static void showProgressDialog(Activity activity,Context context, String message,boolean isCancelable){
        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage(message);
        final ProgressBar progressBar = new ProgressBar(context);
        alert.setView(progressBar);
        Constant.FullScreencall(activity, context);
        alert.setCancelable(isCancelable);
        progressAlertDialog = alert.create();
        progressAlertDialog.show();
    }

    public static String readFromUrl(String urlLink){

       String strForAnim = "";

        try {
            URL url = new URL(urlLink);
            Scanner s = new Scanner(url.openStream());
            strForAnim = s.toString();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
        return strForAnim;
    }
}
