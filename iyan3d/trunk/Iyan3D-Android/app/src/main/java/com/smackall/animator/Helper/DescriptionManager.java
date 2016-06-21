package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;

import com.smackall.animator.EditorView;
import com.smackall.animator.R;
import com.smackall.animator.SceneSelection;

/**
 * Created by Sabish.M on 20/6/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class DescriptionManager {

    public int actionType = 0;

    public void addSceneSelectionDescriptions(Context context)
    {
        ((SceneSelection)context).helpDialogs.views.clear();
        for (int i= 0; i < ((SceneSelection)context).helpDialogs.toolTips.size(); i++){
            if(((SceneSelection)context).helpDialogs.toolTips.get(i).isShown())
                ((SceneSelection)context).helpDialogs.toolTips.get(i).remove();
        }
        ((SceneSelection)context).helpDialogs.toolTips.clear();
        try {
            ((SceneSelection)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.new_scene_btn));
            ((SceneSelection)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.login_btn));
            ((SceneSelection)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.info_btn));

            ((SceneSelection)context).helpDialogs.views.add(((SceneSelection)context).sceneAdapter.newScene);

            if(((SceneSelection)context).sceneAdapter.sceneDBs.size() >0) {
                ((SceneSelection) context).helpDialogs.views.add(((SceneSelection) context).sceneAdapter.firstScene);
                ((SceneSelection) context).helpDialogs.views.add(((SceneSelection) context).sceneAdapter.firstSceneProps);
                ((SceneSelection) context).sceneAdapter.firstSceneProps.setContentDescription("Tap to Clone / Delete / Share the scene.");
                ((SceneSelection) context).sceneAdapter.firstScene.setContentDescription("Tap to open the scene");
                ((SceneSelection) context).sceneAdapter.firstScene.setTag("right");
                ((SceneSelection) context).sceneAdapter.firstSceneProps.setTag("right");
            }

            ((SceneSelection)context).sceneAdapter.newScene.setContentDescription("Tap to create a new scene");
            ((Activity)context).findViewById(R.id.new_scene_btn).setContentDescription("Creates a new scene.");
            ((Activity)context).findViewById(R.id.login_btn).setContentDescription("Manage your account.");
            ((Activity)context).findViewById(R.id.info_btn).setContentDescription("Settings, Tutorials & contact us.");

            ((Activity)context).findViewById(R.id.new_scene_btn).setTag("right");
            ((Activity)context).findViewById(R.id.login_btn).setTag("left");
            ((Activity)context).findViewById(R.id.info_btn).setTag("bottom");

            ((SceneSelection)context).sceneAdapter.newScene.setTag("right");

        }catch (NullPointerException e){e.printStackTrace();}
    }

    public void addEditorViewDescription(Context context){
        ((EditorView)context).helpDialogs.views.clear();
        for (int i= 0; i < ((EditorView)context).helpDialogs.toolTips.size(); i++){
            if(((EditorView)context).helpDialogs.toolTips.get(i).isShown())
                ((EditorView)context).helpDialogs.toolTips.get(i).remove();
        }
        ((EditorView)context).helpDialogs.toolTips.clear();

        try {

            ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.add_frameBtn));
            ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.view_btn));

            ((Activity)context).findViewById(R.id.add_frameBtn).setContentDescription("Add Frames.");
            ((Activity)context).findViewById(R.id.view_btn).setContentDescription("Standard view angles.");

            ((Activity)context).findViewById(R.id.add_frameBtn).setTag("left");
            ((Activity)context).findViewById(R.id.view_btn).setTag("bottom");

            //Asset View
            if(Constants.VIEW_TYPE == Constants.ASSET_VIEW || Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {

                if(Constants.VIEW_TYPE == Constants.ASSET_VIEW)
                    ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.model_categary));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.asset_grid));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.add_asset_btn));

                ((Activity)context).findViewById(R.id.model_categary).setContentDescription("Tap to change model category.");
                ((Activity)context).findViewById(R.id.asset_grid).setContentDescription("Tap on a "+ ((Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) ? "particle" : "model")+" to preview.");
                ((Activity)context).findViewById(R.id.add_asset_btn).setContentDescription("Tap to import the selected "+ ((Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) ? "particle" : "model")+" into your scene.");

                //Asset View
                ((Activity)context).findViewById(R.id.model_categary).setTag("left");
                ((Activity)context).findViewById(R.id.asset_grid).setTag("left");
                ((Activity)context).findViewById(R.id.add_asset_btn).setTag("left");
            }
            else if(Constants.VIEW_TYPE == Constants.ANIMATION_VIEW){
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.animation_categary));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.animation_grid));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.add_animation_btn));

                ((Activity)context).findViewById(R.id.animation_categary).setContentDescription("Tap to change animation category.");
                ((Activity)context).findViewById(R.id.animation_grid).setContentDescription("Select an animation to preview.");
                ((Activity)context).findViewById(R.id.add_animation_btn).setContentDescription("Tap to apply animation to the selected object.");

                ((Activity)context).findViewById(R.id.animation_categary).setTag("left");
                ((Activity)context).findViewById(R.id.animation_grid).setTag("left");
                ((Activity)context).findViewById(R.id.add_animation_btn).setTag("left");
            }
            else if(Constants.VIEW_TYPE == Constants.TEXT_VIEW){
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.inputText));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.bevalSlider));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.color_picker_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.fontstore));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.text_gridView));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.add_text_btn));

                ((Activity)context).findViewById(R.id.inputText).setContentDescription("Enter your text here.");
                ((Activity)context).findViewById(R.id.bevalSlider).setContentDescription("Add bevel to 3D text.");
                ((Activity)context).findViewById(R.id.color_picker_btn).setContentDescription("Choose color for 3D text");
                ((Activity)context).findViewById(R.id.fontstore).setContentDescription("Toggle between store fonts and the fonts you have imported.");
                ((Activity)context).findViewById(R.id.text_gridView).setContentDescription("Choose a font to preview 3D text.");
                ((Activity)context).findViewById(R.id.add_text_btn).setContentDescription("Import selected 3D text into the scene.");

                ((Activity)context).findViewById(R.id.inputText).setTag("left");
                ((Activity)context).findViewById(R.id.bevalSlider).setTag("left");
                ((Activity)context).findViewById(R.id.color_picker_btn).setTag("left");
                ((Activity)context).findViewById(R.id.fontstore).setTag("left");
                ((Activity)context).findViewById(R.id.text_gridView).setTag("left");
                ((Activity)context).findViewById(R.id.add_text_btn).setTag("left");
            }
            else if(Constants.VIEW_TYPE == Constants.OBJ_VIEW){
                boolean objMode = (((EditorView)context).objSelection.gridView.getTag() == Constants.OBJ_MODE);
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.obj_grid));
                ((EditorView)context).helpDialogs.views.add(((EditorView)context).objSelection.objView.findViewById(R.id.import_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.next_obj));

                ((Activity)context).findViewById(R.id.obj_grid).setContentDescription((objMode) ?  "Select an OBJ to preview" : "Choose a texture to apply for the selected model.");
                ((EditorView)context).objSelection.objView.findViewById(R.id.import_btn).setContentDescription((objMode) ? "Tap to import OBJ from Local Storage." : "Tap to import texture from Gallery.");
                ((Activity)context).findViewById(R.id.next_obj).setContentDescription((objMode) ? "Tap to choose texture / color for the selected OBJ Model." : "Import the model with the selected texture / color.");

                ((EditorView)context).objSelection.objView.findViewById(R.id.import_btn).setTag("left");
                ((Activity)context).findViewById(R.id.next_obj).setTag("left");
            }
            else if(Constants.VIEW_TYPE == Constants.OBJ_TEXTURE){

            }
            else if(Constants.VIEW_TYPE == Constants.CHANGE_TEXTURE){
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.image_grid));
                ((EditorView)context).helpDialogs.views.add(((EditorView)context).textureSelection.v.findViewById(R.id.import_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.add_image));

                ((Activity)context).findViewById(R.id.image_grid).setContentDescription("Choose a texture to apply for the selected model.");
                ((EditorView)context).textureSelection.v.findViewById(R.id.import_btn).setContentDescription("Tap to import texture from Gallery.");
                ((Activity)context).findViewById(R.id.add_image).setContentDescription("Import the model with the selected texture / color.");

                ((Activity)context).findViewById(R.id.image_grid).setTag("left");
                ((EditorView)context).textureSelection.v.findViewById(R.id.import_btn).setTag("left");
                ((Activity)context).findViewById(R.id.add_image).setTag("left");
            }
            else if(Constants.VIEW_TYPE == Constants.IMAGE_VIEW){
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.image_grid));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.add_image));
                ((EditorView)context).helpDialogs.views.add(((EditorView)context).imageSelection.v.findViewById(R.id.import_btn));

                ((Activity)context).findViewById(R.id.image_grid).setContentDescription("Select an image to preview.");
                ((Activity)context).findViewById(R.id.add_image).setContentDescription("Tap to import the selected image into your scene.");
                ((EditorView)context).imageSelection.v.findViewById(R.id.import_btn).setContentDescription("Tap to import image from gallery.");

                ((Activity)context).findViewById(R.id.image_grid).setTag("left");
                ((Activity)context).findViewById(R.id.add_image).setTag("left");
                ((EditorView)context).imageSelection.v.findViewById(R.id.import_btn).setTag("left");
            }
            else if(Constants.VIEW_TYPE == Constants.EDITOR_VIEW){
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.play_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.my_object_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.import_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.animation_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.option_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.export_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.rotate_btn));
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.undo));

                ((Activity)context).findViewById(R.id.play_btn).setContentDescription("Play the animation preview.");
                ((Activity)context).findViewById(R.id.my_object_btn).setContentDescription("Manage objects in the scene.");
                ((Activity)context).findViewById(R.id.import_btn).setContentDescription("Import objects into the scene.");
                ((Activity)context).findViewById(R.id.animation_btn).setContentDescription("Apply animation to the selected model/text.");
                ((Activity)context).findViewById(R.id.option_btn).setContentDescription("Change properties of the Camera roll.");
                ((Activity)context).findViewById(R.id.export_btn).setContentDescription("Export image/video to the Camera roll.");
                ((Activity)context).findViewById(R.id.rotate_btn).setContentDescription("Switch to Move / Rotate / Scale mode.");
                ((Activity)context).findViewById(R.id.undo).setContentDescription("Undo / Redo your actions.");

                ((Activity)context).findViewById(R.id.play_btn).setTag("left");
                ((Activity)context).findViewById(R.id.my_object_btn).setTag("left");
                ((Activity)context).findViewById(R.id.import_btn).setTag("left");
                ((Activity)context).findViewById(R.id.animation_btn).setTag("left");
                ((Activity)context).findViewById(R.id.option_btn).setTag("left");
                ((Activity)context).findViewById(R.id.export_btn).setTag("left");
                ((Activity)context).findViewById(R.id.rotate_btn).setTag("left");
                ((Activity)context).findViewById(R.id.undo).setTag("left");
            }
        }
        catch (NullPointerException ignored){}
    }

    public void helpForFirstTimeUser(Context context)
    {
        if(!Constants.isFirstTimeUser) return;
        ((EditorView)context).helpDialogs.views.clear();
        for (int i= 0; i < ((EditorView)context).helpDialogs.toolTips.size(); i++){
            if(((EditorView)context).helpDialogs.toolTips.get(i).isShown())
                ((EditorView)context).helpDialogs.toolTips.get(i).remove();
        }
        ((EditorView)context).helpDialogs.toolTips.clear();
        switch (actionType){
            case 0:
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.import_btn));
                ((Activity)context).findViewById(R.id.import_btn).setContentDescription("Import objects into the scene.");
                ((Activity)context).findViewById(R.id.import_btn).setTag("left");
                actionType += 1;
                break;
            case 1:
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.animation_btn));
                ((Activity)context).findViewById(R.id.animation_btn).setContentDescription("Apply animation to the selected model/text.");
                ((Activity)context).findViewById(R.id.animation_btn).setTag("left");
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.option_btn));
                ((Activity)context).findViewById(R.id.option_btn).setContentDescription("Change properties of the Camera roll.");
                ((Activity)context).findViewById(R.id.option_btn).setTag("left");
                ((EditorView)context).helpDialogs.views.add(((Activity)context).findViewById(R.id.rotate_btn));
                ((Activity)context).findViewById(R.id.rotate_btn).setContentDescription("Switch to Move / Rotate / Scale mode.");
                ((Activity)context).findViewById(R.id.rotate_btn).setTag("left");
                actionType +=1;
                break;
        }
    }
}
