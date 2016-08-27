package com.smackall.animator.Helper;

import android.content.Context;

import com.smackall.animator.EditorView;
import com.smackall.animator.opengl.GL2JNILib;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipOutputStream;

/**
 * Created by Sabish.M on 29/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ZipManager {

    private static final int BUFFER = 2048;
    Context mContext;
    DatabaseHelper db;

    public ZipManager(Context context, DatabaseHelper db) {
        this.mContext = context;
        this.db = db;
    }


    public ArrayList<String> getFiles(boolean forBackUp) {
        FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder);
        FileHelper.mkDir(PathManager.LocalCacheFolder);
        FileHelper.copy(PathManager.LocalProjectFolder + "/" + ((EditorView) mContext).projectNameHash + ".sgb", PathManager.LocalCacheFolder + "/index.sgb");
        ArrayList<String> files = new ArrayList<>();
        if (!forBackUp)
            files.add(PathManager.LocalCacheFolder + "/index.sgb");
        else
            files.add(PathManager.LocalProjectFolder + "/" + ((EditorView) mContext).projectNameHash + ".sgb");
        for (int i = 2; i < GL2JNILib.getNodeCount(); i++) {
            if (GL2JNILib.getNodeType(i) == Constants.NODE_SGM || GL2JNILib.getNodeType(i) == Constants.NODE_RIG || GL2JNILib.getNodeType(i) == Constants.NODE_OBJ) {
                if (!isStoreAsset(GL2JNILib.getAssetIdWithNodeId(i))) {
                    files.add(getPathOfAsset(Integer.toString(GL2JNILib.getAssetIdWithNodeId(i)), GL2JNILib.getNodeType(i)));
                    if (!GL2JNILib.perVertexColor(i))
                        files.add(getTexturePath(GL2JNILib.getTexture(i)));
                } else {
                    if (!GL2JNILib.getTexture(i).equals(GL2JNILib.getAssetIdWithNodeId(i) + "-cm")) {
                        if (!GL2JNILib.perVertexColor(i))
                            files.add(getTexturePath(GL2JNILib.getTexture(i)));
                    }
                }
            } else if (GL2JNILib.getNodeType(i) == Constants.NODE_IMAGE) {
                if (!GL2JNILib.perVertexColor(i))
                    files.add(getTexturePath(GL2JNILib.getNodeName(i)));
            } else if (GL2JNILib.getNodeType(i) == Constants.NODE_TEXT || GL2JNILib.getNodeType(i) == Constants.NODE_TEXT_SKIN) {
                if (!isStoreAsset(GL2JNILib.getAssetIdWithNodeId(i))) {
                    files.add(getFontPath(GL2JNILib.optionalFilePathWithId(i)));
                }
                if (!GL2JNILib.perVertexColor(i))
                    files.add(getTexturePath(GL2JNILib.getTexture(i)));
            } else if (GL2JNILib.getNodeType(i) == Constants.NODE_PARTICLES) {
                addParticlesFilesPath(files, GL2JNILib.getAssetIdWithNodeId(i), i);
            }
        }
        if (forBackUp)
            files.add(PathManager.LocalScenesFolder + "/" + ((EditorView) mContext).projectNameHash + ".png");
        return files;
    }

    private String getPathOfAsset(String fileName, int nodeType) {
        String ext = (nodeType == Constants.NODE_RIG) ? ".sgr" : (nodeType == Constants.NODE_SGM) ? ".sgm" : (nodeType == Constants.NODE_OBJ) ? ".obj" : ".sgr";
        String file1 = PathManager.LocalMeshFolder + "/" + fileName + ext;
        String file2 = PathManager.LocalUserMeshFolder + "/" + fileName + ext;
        String file3 = PathManager.DefaultAssetsDir + "/" + fileName + ext;
        if (FileHelper.checkValidFilePath(file1)) return file1;
        else if (FileHelper.checkValidFilePath(file2)) return file2;
        else if (FileHelper.checkValidFilePath(file3)) return file3;
        else return "";
    }

    private String getTexturePath(String fileName) {
        String file1 = PathManager.LocalTextureFolder + "/" + fileName + ".png";
        String file2 = PathManager.LocalUserMeshFolder + "/" + fileName + ".png";
        String file3 = PathManager.LocalImportedImageFolder + "/" + fileName + ".png";
        String file4 = PathManager.DefaultAssetsDir + "/" + fileName + ".png";
        if (FileHelper.checkValidFilePath(file1)) return file1;
        else if (FileHelper.checkValidFilePath(file2)) return file2;
        else if (FileHelper.checkValidFilePath(file3)) return file3;
        else if (FileHelper.checkValidFilePath(file4)) return file4;
        else return "";
    }

    private String getFontPath(String fileName) {
        String file1 = PathManager.LocalFontsFolder + "/" + fileName;
        String file2 = PathManager.LocalUserFontFolder + "/" + fileName;
        String file3 = PathManager.LocalImportAndExport + "/" + fileName;
        if (FileHelper.checkValidFilePath(file1)) return file1;
        else if (FileHelper.checkValidFilePath(file2)) return file2;
        else if (FileHelper.checkValidFilePath(file3)) return file3;
        else return "";
    }

    private void addParticlesFilesPath(ArrayList<String> filesArray, int assetId, int nodeId) {
        if (FileHelper.checkValidFilePath(PathManager.LocalMeshFolder + "/" + assetId + ".json")) {
            filesArray.add(PathManager.LocalMeshFolder + "/" + assetId + ".json");
            filesArray.add(PathManager.LocalMeshFolder + "/" + assetId + ".sgm");
            String texture = GL2JNILib.getTexture(nodeId);
            filesArray.add(getTexturePath(texture));
        }
    }

    private boolean isStoreAsset(int assetId) {
        return db.getModelWithAssetId(assetId) != null && db.getModelWithAssetId(assetId).size() > 0 && db.getModelWithAssetId(assetId).get(0).getAssetsId() == assetId;
    }

    public boolean packForCloudRender() {
        ArrayList<String> _files = getFiles(false);
        String zipName = "index.zip";
        String path = PathManager.LocalCacheFolder + "/";

        try {
            BufferedInputStream origin = null;
            FileOutputStream dest = new FileOutputStream(path + zipName);

            ZipOutputStream out = new ZipOutputStream(new BufferedOutputStream(dest));

            byte data[] = new byte[BUFFER];

            for (int i = 0; i < _files.size(); i++) {
                if (!FileHelper.checkValidFilePath(_files.get(i))) return false;
                FileInputStream fi = new FileInputStream(_files.get(i));
                origin = new BufferedInputStream(fi, BUFFER);
                ZipEntry entry = new ZipEntry(_files.get(i).substring(_files.get(i).lastIndexOf("/") + 1));
                try {
                    out.putNextEntry(entry);
                } catch (ZipException e) {
                    continue;
                }
                int count;
                while ((count = origin.read(data, 0, BUFFER)) != -1) {
                    out.write(data, 0, count);
                }
                origin.close();
            }

            out.finish();
            out.close();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}
