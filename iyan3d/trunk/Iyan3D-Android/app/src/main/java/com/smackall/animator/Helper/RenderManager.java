package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.ViewGroup;

import com.smackall.animator.EditorView;
import com.smackall.animator.NativeCallBackClasses.NativeCallBacks;
import com.smackall.animator.R;
import com.smackall.animator.Rig;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.List;

/**
 * Created by Sabish.M on 15/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class RenderManager {
    public GLSurfaceView glView;
    Context mContext;
    boolean wait;
    private SharedPreferenceManager sp;

    public RenderManager(Context mContext, SharedPreferenceManager sp) {
        this.mContext = mContext;
        this.sp = sp;
        this.glView = ((EditorView) mContext).glView;
    }

    public void selectObject(final int nodeId) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.selectNode(nodeId, (sp.getInt(mContext, "multiSelect") == 1));
            }
        });
        ((EditorView) mContext).reloadMyObjectList();
    }

    public void removeObject(final int nodeId, final boolean isUndoRedo) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeNode(nodeId, isUndoRedo);
            }
        });
        ((EditorView) mContext).reloadMyObjectList();
    }

    public void renderAll() {
        GL2JNILib.step();
    }

    public void panProgress(final MotionEvent event) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (event.getPointerCount() > 1)
                        GL2JNILib.panProgress(((EditorView) mContext).nativeCallBacks, event.getX(0), event.getY(0), event.getX(1), event.getY(1));
                } catch (IllegalArgumentException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void panBegin(final MotionEvent event) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (event.getPointerCount() > 1)
                        GL2JNILib.panBegin(event.getX(0), event.getY(0), event.getX(1), event.getY(1));
                } catch (IllegalArgumentException ignored) {
                }
            }
        });
    }

    public void touchBegan(final MotionEvent event) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (event.getPointerCount() == 1)
                        GL2JNILib.touchBegan(event.getX(), event.getY());
                } catch (IllegalArgumentException ignored) {
                }
            }
        });
    }

    public void swipe(final VelocityTracker velocityTracker) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    GL2JNILib.swipe(velocityTracker.getXVelocity(), velocityTracker.getYVelocity());
                } catch (IllegalArgumentException ignored) {
                }
            }
        });
    }

    public void tapMove(final MotionEvent event) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    GL2JNILib.tapMove(((EditorView) mContext).nativeCallBacks, event.getX(), event.getY());
                } catch (IllegalArgumentException ignored) {
                }
            }
        });
    }

    public void tapEnd(final MotionEvent event) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    GL2JNILib.tapEnd(event.getX(), event.getY());
                    ((Activity) mContext).runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if (mContext != null && ((EditorView) mContext).frameAdapter != null)
                                ((EditorView) mContext).frameAdapter.notifyDataSetChanged();
                        }
                    });
                } catch (IllegalArgumentException ignored) {
                }
            }
        });
    }

    public void checkControlSelection(final MotionEvent event) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    GL2JNILib.controlSelection(event.getX(), event.getY(), (sp.getInt(mContext, "multiSelect") == 1));
                } catch (IllegalArgumentException ignored) {
                }
            }
        });
    }

    public void checktapposition(final MotionEvent event) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    GL2JNILib.tap(event.getX(), event.getY(), (sp.getInt(mContext, "multiSelect") == 1));
                } catch (IllegalArgumentException ignored) {
                }
                ((Activity) mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mContext != null && ((EditorView) mContext).frameAdapter != null)
                            ((EditorView) mContext).frameAdapter.notifyDataSetChanged();
                    }
                });
            }
        });
    }

    public void removeTempNode() {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeTempNode();
            }
        });
    }

    public void importAssets(final AssetsDB assetsDB) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.importModel(assetsDB.getAssetName(),
                        assetsDB.getAssetPath(), assetsDB.getTexturePath(), assetsDB.isHasMeshColor(), assetsDB.getX(), assetsDB.getY(), assetsDB.getZ(), assetsDB.getType(), assetsDB.getIsTempNode());
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void importText(final TextDB textDB) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.loadText(textDB.getTypeOfNode(), textDB.getText()
                        , textDB.getFontSize(), textDB.getBevalValue(), textDB.getActionType(), textDB.getFilePath(), textDB.getTempNode());
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void importImage(final ImageDB imageDB) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.importImageOrVideo(imageDB.getNodeType(), imageDB.getName(), imageDB.getWidth(), imageDB.getHeight(), imageDB.getActionType(), imageDB.getIsTempNode());
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void importLight(final int lightId, final int action) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.importAdditionalLight(lightId, action);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);

            }
        });
    }

    public void changeTexture(final int selectedNodeId,final int selectedMeshBufferid, final String textureName, final float x, final float y, final float z, final boolean isTemp) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.changeTexture(selectedNodeId,selectedMeshBufferid, textureName, x, y, z, isTemp);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void removeTempTexture(final int selectedNodeId) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeTempTexture(selectedNodeId);
            }
        });
    }

    public void setCurrentFrame(final int frame, final NativeCallBacks nativeCallBacks) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setCurrentFrame(frame, nativeCallBacks);
            }
        });
    }

    public void cameraPosition(final float statusBarHeight) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ViewGroup insertPointParent = (sp.getInt(mContext, "toolbarPosition") == 1) ? (ViewGroup) ((Activity) mContext).findViewById(R.id.rightView).getParent()
                        : (ViewGroup) ((Activity) mContext).findViewById(R.id.leftView).getParent();
                ViewGroup insertPoint = null;
                for (int i = 0; i < insertPointParent.getChildCount(); i++) {
                    if (insertPointParent.getChildAt(i).getTag() != null && insertPointParent.getChildAt(i).getTag().toString().equals("-1")) {
                        insertPoint = (ViewGroup) insertPointParent.getChildAt(i);
                    }
                }
                if (insertPoint != null) {
                    GL2JNILib.cameraPositionViaToolBarPosition((sp.getInt(mContext, "toolbarPosition")), insertPoint.getWidth(), ((Activity) mContext).findViewById(R.id.recycleViewHolder).getHeight() + statusBarHeight);
                    GL2JNILib.previewPosition(sp.getInt(mContext, "previewPosition"), (sp.getInt(mContext, "previewSize") == 0) ? 1 : 2, ((Activity) mContext).findViewById(R.id.recycleViewHolder).getHeight() + statusBarHeight, insertPoint.getWidth());
                }
            }
        });
    }

    public void saveAsSGM(final String FileName, final String textureName, final int tempName, final boolean haveTexture, final float x, final float y, final float z) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.saveAsSGM(FileName, textureName, tempName, haveTexture, x, y, z);
            }
        });
    }

    public void cameraProperty(final int fov, final int resolution, final boolean storeAction) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraPropertyChanged(fov, resolution, storeAction);
            }
        });
    }

    public void beginRigging() {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.beginRigging();
                ((Activity) mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ((EditorView) mContext).rig.switchSceneMode(1);
                        ((EditorView) mContext).rig.currentSceneMode += 1;
                        ((EditorView) mContext).rig.updateText();
                    }
                });
            }
        });
    }

    public void cancelRig(final boolean completed) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cancelRig(completed);
            }
        });
    }

    public void switchSceneMode(Rig rig, final int sceneMode) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.switchRigSceneMode(((EditorView) mContext).nativeCallBacks, sceneMode);
            }
        });
    }

    public void addJoint(Rig rig) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.addJoint(((EditorView) mContext).nativeCallBacks);
            }
        });
    }

    public void setSkeletonType(final int type) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setSkeletonType(type);
            }
        });
    }

    public void unselectObjects() {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.unselectObjects();
            }
        });
    }

    public void hideNode(final int nodeId, final boolean hide) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.hideNode(nodeId, hide);
            }
        });
    }

    public void unselectObject(final int nodeId) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.unselectObject(nodeId);
            }
        });
    }

    public void setScale(final float x, final float y, final float z, final boolean store) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.setScaleValue(x, y, z, store);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void setRigScale(final float x, final float y, final float z) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.rigNodeScale(x, y, z);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void changeEnvelopScale(final float x) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.changeEnvelopScale(x);
            }
        });
    }

    public void setMove() {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.move(((EditorView) mContext).nativeCallBacks);
            }
        });
    }

    public void setRotate() {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.rotate(((EditorView) mContext).nativeCallBacks);
            }
        });
    }

    public void cameraView(final int cameraView) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraView(cameraView);
            }
        });
    }

    public void changeMeshProps(final float refraction, final float reflection, final boolean light, final boolean visible, final boolean storeAction) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.changeMeshProperty(refraction, reflection, light, visible, storeAction);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void setTextureSmoothStatus(final boolean state) {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setTextureSmoothStatus(state);
            }
        });
    }

    public void createDuplicate() {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
                GL2JNILib.createDuplicateAssets(((EditorView) mContext).nativeCallBacks);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }


    public void cloneSelectedAsset(int selectedAssetId, int selectedNodeType, int selectedNodeId) {
        if (selectedNodeType == Constants.NODE_SGM || selectedNodeType == Constants.NODE_OBJ) {
            boolean status = GL2JNILib.cloneSelectedAsset(selectedAssetId, selectedNodeType, selectedNodeId, Constants.IMPORT_ASSET_ACTION);
            if (!status)
                UIHelper.informDialog(mContext, mContext.getString(R.string.copy_limit_exceeded));

        } else if ((selectedNodeType == Constants.NODE_RIG || selectedNodeType == Constants.NODE_PARTICLES) && selectedAssetId != Constants.NOT_SELECTED) {
            ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
            List<AssetsDB> assetsDBs = null;
            assetsDBs = ((EditorView) mContext).db.getModelWithAssetId(selectedAssetId);
            if (assetsDBs == null || assetsDBs.size() <= 0)
                assetsDBs = ((EditorView) mContext).db.getMyModelWithAssetId(selectedAssetId);
            AssetsDB assetsDB = null;
            if (assetsDBs != null && assetsDBs.size() > 0) {
                assetsDB = assetsDBs.get(0);
                assetsDB.setTexture(GL2JNILib.getTexture(selectedNodeId));
                assetsDB.setX(GL2JNILib.getVertexColorXWithId(selectedNodeId));
                assetsDB.setY(GL2JNILib.getVertexColorYWithId(selectedNodeId));
                assetsDB.setZ(GL2JNILib.getVertexColorZWithId(selectedNodeId));
                assetsDB.setIsTempNode(false);
                //importAssets(assetsDB,false);
                GL2JNILib.importAsset(assetsDB.getType(), assetsDB.getAssetsId(), assetsDB.getAssetName(), assetsDB.getTexture(), 0, 0, assetsDB.getIsTempNode(), assetsDB.getX(), assetsDB.getY(), assetsDB.getZ(), assetsDB.getActionType());
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
                GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1, false);
            }
        } else if ((selectedNodeType == Constants.NODE_TEXT_SKIN || selectedNodeType == Constants.NODE_TEXT) && selectedNodeId != Constants.NOT_SELECTED) {
            ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
            TextDB textDB = new TextDB();
            textDB.setText(GL2JNILib.getNodeName(selectedNodeId));
            textDB.setFilePath(GL2JNILib.optionalFilePathWithId(selectedNodeId));
            textDB.setRed(GL2JNILib.getVertexColorXWithId(selectedNodeId));
            textDB.setGreen(GL2JNILib.getVertexColorYWithId(selectedNodeId));
            textDB.setBlue(GL2JNILib.getVertexColorZWithId(selectedNodeId));
            textDB.setBevalValue((int) GL2JNILib.nodeSpecificFloatWithId(selectedNodeId));
            textDB.setFontSize(GL2JNILib.getFontSizeWithId(selectedNodeId));
            textDB.setTextureName(GL2JNILib.getTexture(selectedNodeId));
            textDB.setTypeOfNode((selectedNodeType == Constants.NODE_TEXT_SKIN) ? Constants.ASSET_TEXT_RIG : Constants.ASSET_TEXT);
            textDB.setTempNode(false);
            GL2JNILib.loadText( textDB.getTypeOfNode(), textDB.getText()
                    , textDB.getFontSize(), textDB.getBevalValue(), textDB.getActionType(), textDB.getFilePath(), textDB.getTempNode());
            GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1, false);
            ((EditorView) mContext).showOrHideLoading(Constants.HIDE);

        } else if ((selectedNodeType == Constants.NODE_IMAGE || selectedNodeType == Constants.NODE_VIDEO)) {
            ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
            String name = GL2JNILib.getNodeName(selectedNodeId);
            int imgW = (int) GL2JNILib.getVertexColorXWithId(selectedNodeId);
            int imgH = (int) GL2JNILib.getVertexColorYWithId(selectedNodeId);
            final ImageDB imageDB = new ImageDB();
            imageDB.setNodeType((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_IMAGE) ? Constants.NODE_IMAGE : Constants.NODE_VIDEO);
            imageDB.setName(name);
            imageDB.setWidth(imgW);
            imageDB.setHeight(imgH);
            imageDB.setAssetAddType(0);
            imageDB.setTempNode(false);
            GL2JNILib.importImageOrVideo(imageDB.getNodeType(), imageDB.getName(), imageDB.getWidth(), imageDB.getHeight(), imageDB.getActionType(), imageDB.getIsTempNode());
            GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1, false);
            ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
        }
    }
}
