package com.smackall.iyan3dPro.Helper;

import android.content.Context;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;

import com.smackall.iyan3dPro.R;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

public class FileHelper {
    static final String AB = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
    static SecureRandom rnd = new SecureRandom();

    public static boolean checkValidFilePath(File file) {
        return file != null && file.exists();
    }

    public static boolean checkValidFilePath(String string2) {
        return string2 != null && new File(string2).exists();
    }

    public static boolean copy(File file, File file2) {
        try {
            FileInputStream fileInputStream = new FileInputStream(file);
            FileOutputStream fileOutputStream = new FileOutputStream(file2);
            byte[] arrby = new byte[1024];
            do {
                int n;
                if ((n = fileInputStream.read(arrby)) <= 0) {
                    fileInputStream.close();
                    fileOutputStream.close();
                    return true;
                }
                fileOutputStream.write(arrby, 0, n);
            } while (true);
        } catch (Exception var4_6) {
            return false;
        }
    }

    public static boolean copy(String string2, String string3) {
        File file = new File(string2);
        File file2 = new File(string3);
        try {
            FileInputStream fileInputStream = new FileInputStream(file);
            FileOutputStream fileOutputStream = new FileOutputStream(file2);
            byte[] arrby = new byte[1024];
            do {
                int n;
                if ((n = fileInputStream.read(arrby)) <= 0) {
                    fileInputStream.close();
                    fileOutputStream.close();
                    return true;
                }
                fileOutputStream.write(arrby, 0, n);
            } while (true);
        } catch (Exception var6_8) {
            return false;
        }
    }

    public static void deleteFilesAndFolder(File file) {
        if (file.exists()) {
            if (file.isDirectory() && file.list() != null) {
                String[] arrstring = file.list();
                int n = arrstring.length;
                for (String anArrstring : arrstring) {
                    FileHelper.deleteFilesAndFolder(new File(file, anArrstring));
                }
            }
            file.delete();
        }
    }

    public static void deleteFilesAndFolder(String string2) {
        File file = new File(string2);
        if (file.exists()) {
            if (file.isDirectory() && file.list() != null) {
                String[] arrstring = file.list();
                int n = arrstring.length;
                for (String anArrstring : arrstring) {
                    FileHelper.deleteFilesAndFolder(String.valueOf(new File(file, anArrstring)));
                }
            }
            file.delete();
        }
    }

    public static String getFileExt(File file) {
        String string2 = file.getAbsolutePath();
        return string2.substring(1 + string2.lastIndexOf("."), string2.length());
    }

    public static String getFileExt(String string2) {
        return string2.substring(1 + string2.lastIndexOf("."), string2.length());
    }

    public static String getFileNameFromPath(String path) {
        return path.substring(path.lastIndexOf("/") + 1, path.length());
    }

    public static String getFileWithoutExt(File file) {
        String string2 = file.getAbsolutePath();
        return string2.substring(1 + string2.lastIndexOf("/"), string2.lastIndexOf("."));
    }

    public static String getFileWithoutExt(String string2) {
        return string2.substring(1 + string2.lastIndexOf("/"), string2.lastIndexOf("."));
    }

    public static String getFileLocation(String string) {
        return string.substring(0, string.lastIndexOf("/"));
    }

    public static String getFileLocation(File file) {
        String string = file.getAbsolutePath();
        return string.substring(0, string.lastIndexOf("/"));
    }

    public static void mkDir(File file) {
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void mkDir(String string2) {
        File file = new File(string2);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static boolean move(File file, File file2) {
        if (file.exists()) {
            file.renameTo(file2);
            return true;
        }
        return false;
    }

    public static boolean move(String string2, String string3) {
        File file = new File(string2);
        File file2 = new File(string3);
        if (file.exists()) {
            file.renameTo(file2);
            return true;
        }
        return false;
    }

    public static String md5(String s) {

        String digest = null;
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            byte[] hash = md.digest(s.getBytes("UTF-8"));

            //converting byte array to Hexadecimal String
            StringBuilder sb = new StringBuilder(2 * hash.length);
            for (byte b : hash) {
                sb.append(String.format("%02x", b & 0xff));
            }

            digest = sb.toString();

        } catch (UnsupportedEncodingException | NoSuchAlgorithmException ex) {
            ex.printStackTrace();
        }
        return digest;
    }

    public static boolean isItHaveSpecialChar(String input) {
        String charecters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 1234567890";
        String[] charArray = input.split("");
        for (String aCharArray : charArray) {
            if (!charecters.contains(aCharArray))
                return true;
        }
        return false;
    }

    public static void getObjAndTextureFromCommonIyan3dPath(final int mode) {
        final String commonFolder = PathManager.LocalImportAndExport + "/";
        final String userMeshFolder = PathManager.LocalUserMeshFolder + "/";

        final File f = new File(commonFolder);

        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.toLowerCase().endsWith((mode == Constants.OBJ_MODE) ? "obj" : "png");
            }
        };
        File files[] = f.listFiles(filenameFilter);

        if (files != null) {
            for (File file1 : files) {
                File file = new File(userMeshFolder + getFileNameFromPath(file1.getAbsolutePath()));
                copy(file1.getAbsoluteFile(), file);
                file1.getAbsoluteFile().delete();
            }
        }
    }

    public static void getFontFromCommonIyan3dPath() {
        final String commonFolder = PathManager.LocalImportAndExport + "/";
        final String userFontFolder = PathManager.LocalUserFontFolder + "/";

        final File f = new File(commonFolder);

        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.toLowerCase().endsWith("otf") || filename.toLowerCase().endsWith("ttf");
            }
        };
        File files[] = f.listFiles(filenameFilter);

        if (files != null) {
            for (File file1 : files) {
                File file = new File(userFontFolder + getFileNameFromPath(file1.getAbsolutePath()));
                copy(file1.getAbsoluteFile(), file);
                file1.getAbsoluteFile().delete();
            }
        }
    }

    public static void copyAssetsDirToLocal(Context context) {
        AssetManager assetManager = context.getAssets();

        String[] assets = null;
        try {
            assets = assetManager.list("");
        } catch (IOException ignored) {

        }

        if (assets != null) for (String filename : assets) {
            InputStream in = null;
            OutputStream out = null;
            try {
                in = assetManager.open(filename);
                File outFile = new File(PathManager.DefaultAssetsDir + "/", filename);
                out = new FileOutputStream(outFile);
                copyFile(in, out);

            } catch (IOException ignored) {

            } finally {
                if (in != null) {
                    try {
                        in.close();
                    } catch (IOException e) {
                        // NOOP
                    }
                }
                if (out != null) {
                    try {
                        out.close();
                    } catch (IOException e) {
                        // NOOP
                    }
                }

            }
        }
    }

    private static void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    public static void copySingleAssetFile(Context context, String filename,String toPath,String outName) {
        AssetManager assetManager = context.getAssets();

        File fileDir = new File(toPath);
        if (!fileDir.exists())
            fileDir.mkdirs();

        InputStream in = null;
        OutputStream out = null;
        try {
            in = assetManager.open(filename);
            out = new FileOutputStream(toPath+"/"+outName);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (Exception ignored) {
        }
    }

    public static String randomString(int len) {
        StringBuilder sb = new StringBuilder(len);
        for (int i = 0; i < len; i++)
            sb.append(AB.charAt(rnd.nextInt(AB.length())));
        return sb.toString();
    }

    public static String getFilePathFromUri(Uri uri, Context mContext, String type) {
        String[] filePathColumn = {type};

        Cursor cursor = mContext.getContentResolver().query(uri,
                filePathColumn, null, null, null);
        if (cursor != null) {
            cursor.moveToFirst();
        } else {
            return null;
        }

        int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
        String picturePath = cursor.getString(columnIndex);
        cursor.close();
        return picturePath;
    }

    public static void manageImageFile(String path, Context mContext) {
        boolean exits = FileHelper.checkValidFilePath(path);
        if (exits) {
            Bitmap bmp = null;
            try {
                bmp = BitmapFactory.decodeFile(path);
            } catch (OutOfMemoryError e) {
                UIHelper.informDialog(mContext, mContext.getString(R.string.outOfMemory));
            }
            if (bmp == null) return;
            savePng(bmp, PathManager.LocalThumbnailFolder + "/original" + FileHelper.getFileWithoutExt(path));
            bmp = null;
            makeThumbnail(path, "", mContext);
            scaleToSquare(path, mContext);
        }
    }

    public static void makeThumbnail(String path, String fileName, Context mContext) {
        Bitmap bmp = null;
        try {
            bmp = BitmapFactory.decodeFile(path);
        } catch (OutOfMemoryError e) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.outOfMemory));
        }
        if (bmp == null)
            return;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, 128, 128, false);
        savePng(scaledBitmap, PathManager.LocalThumbnailFolder + "/" + ((fileName.length() > 0) ? fileName : FileHelper.getFileWithoutExt(path)));
        scaledBitmap = null;
    }

    public static void makeThumbnail(String path, Context mContext) {
        Bitmap bmp = null;
        try {
            bmp = BitmapFactory.decodeFile(path);
        } catch (OutOfMemoryError e) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.outOfMemory));
        }
        if (bmp == null)
            return;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, 128, 128, false);
        savePng(scaledBitmap, PathManager.LocalThumbnailFolder + "/" + FileHelper.getFileWithoutExt(path));
        scaledBitmap = null;
    }

    private static void scaleToSquare(String path, Context mContext) {
        Bitmap bmp = null;
        try {
            bmp = BitmapFactory.decodeFile(path);
        } catch (OutOfMemoryError e) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.outOfMemory));
        }
        if (bmp == null) return;
        final int oriWidth = bmp.getWidth();
        final int oriHeight = bmp.getHeight();
        int maxSize = Math.max(oriHeight, oriWidth);
        int targetSize = 0;
        if (maxSize <= 128)
            targetSize = 128;
        else if (maxSize <= 256)
            targetSize = 256;
        else if (maxSize <= 512)
            targetSize = 512;
        else
            targetSize = 1024;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, targetSize, targetSize, false);
        savePng(scaledBitmap, PathManager.LocalImportedImageFolder + "/" + FileHelper.getFileWithoutExt(path));
        scaledBitmap = null;
    }

    public static void savePng(Bitmap bitmap, String filePath) {
        try {
            File temp = new File(filePath);
            FileOutputStream os = new FileOutputStream(temp + ".png");
            bitmap.compress(Bitmap.CompressFormat.PNG, 50, os);
            os.flush();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}