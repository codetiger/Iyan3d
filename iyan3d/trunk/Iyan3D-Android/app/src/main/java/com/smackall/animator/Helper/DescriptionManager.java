package com.smackall.animator.Helper;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.View;

import com.smackall.animator.EditorView;
import com.smackall.animator.OverlayDialogs.HelpDialogs;
import com.smackall.animator.R;
import com.smackall.animator.SceneSelection;

/**
 * Created by Sabish.M on 20/6/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class DescriptionManager {

    public int actionType = 0;

    public void addSceneSelectionDescriptions(Context context) {
        ((SceneSelection) context).helpDialogs.views.clear();
        for (int i = 0; i < ((SceneSelection) context).helpDialogs.toolTips.size(); i++) {
            if (((SceneSelection) context).helpDialogs.toolTips.get(i).isShown())
                ((SceneSelection) context).helpDialogs.toolTips.get(i).remove();
        }
        ((SceneSelection) context).helpDialogs.toolTips.clear();
        try {
            ((SceneSelection) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.new_scene_btn));
            ((SceneSelection) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.login_btn));
            ((SceneSelection) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.info_btn));

            ((SceneSelection) context).helpDialogs.views.add(((SceneSelection) context).sceneAdapter.newScene);

            if (((SceneSelection) context).sceneAdapter.sceneDBs.size() > 0) {
                ((SceneSelection) context).helpDialogs.views.add(((SceneSelection) context).sceneAdapter.firstScene);
                ((SceneSelection) context).helpDialogs.views.add(((SceneSelection) context).sceneAdapter.firstSceneProps);
                ((SceneSelection) context).sceneAdapter.firstSceneProps.setContentDescription(context.getString(R.string.tap_to_clone_share_delete_msg));
                ((SceneSelection) context).sceneAdapter.firstScene.setContentDescription(context.getString(R.string.tap_to_open_scene));
                ((SceneSelection) context).sceneAdapter.firstScene.setTag("right");
                ((SceneSelection) context).sceneAdapter.firstSceneProps.setTag("right");
            }

            ((SceneSelection) context).sceneAdapter.newScene.setContentDescription(context.getString(R.string.tap_to_create_scene));
            ((Activity) context).findViewById(R.id.new_scene_btn).setContentDescription(context.getString(R.string.create_a_new_scene));
            ((Activity) context).findViewById(R.id.login_btn).setContentDescription(context.getString(R.string.manage_your_account));
            ((Activity) context).findViewById(R.id.info_btn).setContentDescription(context.getString(R.string.settings_tutorial_contact));

            ((Activity) context).findViewById(R.id.new_scene_btn).setTag("right");
            ((Activity) context).findViewById(R.id.login_btn).setTag("left");
            ((Activity) context).findViewById(R.id.info_btn).setTag("bottom");
            ((SceneSelection) context).sceneAdapter.newScene.setTag("bottom");

        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    public void addEditorViewDescription(Context context) {
        ((EditorView) context).helpDialogs.views.clear();
        for (int i = 0; i < ((EditorView) context).helpDialogs.toolTips.size(); i++) {
            if (((EditorView) context).helpDialogs.toolTips.get(i).isShown())
                ((EditorView) context).helpDialogs.toolTips.get(i).remove();
        }
        ((EditorView) context).helpDialogs.toolTips.clear();

        try {
            if (Constants.VIEW_TYPE != Constants.AUTO_RIG_VIEW)
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_frameBtn));
            ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.view_btn));

            ((Activity) context).findViewById(R.id.add_frameBtn).setContentDescription(context.getString(R.string.add_frames));
            ((Activity) context).findViewById(R.id.view_btn).setContentDescription(context.getString(R.string.standard_view_angles));

            ((Activity) context).findViewById(R.id.add_frameBtn).setTag("left");
            ((Activity) context).findViewById(R.id.view_btn).setTag("bottom");

            //Asset View
            if (Constants.VIEW_TYPE == Constants.ASSET_VIEW || Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {

                if (Constants.VIEW_TYPE == Constants.ASSET_VIEW)
                    ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.model_categary));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.asset_grid));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_asset_btn));

                ((Activity) context).findViewById(R.id.model_categary).setContentDescription(context.getString(R.string.tap_to_change_model_category));

                if (Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                    ((Activity) context).findViewById(R.id.asset_grid).setContentDescription(context.getString(R.string.tap_on_particle_to_preview));
                    ((Activity) context).findViewById(R.id.add_asset_btn).setContentDescription(context.getString(R.string.tap_to_import_selected_particle));
                } else {
                    ((Activity) context).findViewById(R.id.asset_grid).setContentDescription(context.getString(R.string.tap_on_model_to_preview));
                    ((Activity) context).findViewById(R.id.add_asset_btn).setContentDescription(context.getString(R.string.tap_to_import_selected_model));
                }

                //Asset View
                ((Activity) context).findViewById(R.id.model_categary).setTag("left");
                ((Activity) context).findViewById(R.id.asset_grid).setTag("left");
                ((Activity) context).findViewById(R.id.add_asset_btn).setTag("left");
            } else if (Constants.VIEW_TYPE == Constants.ANIMATION_VIEW) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.animation_categary));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.animation_grid));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_animation_btn));

                ((Activity) context).findViewById(R.id.animation_categary).setContentDescription(context.getString(R.string.tap_to_change_animation_category));
                ((Activity) context).findViewById(R.id.animation_grid).setContentDescription(context.getString(R.string.select_an_animation_preview));
                ((Activity) context).findViewById(R.id.add_animation_btn).setContentDescription(context.getString(R.string.tap_to_apply_animation_to));

                ((Activity) context).findViewById(R.id.animation_categary).setTag("left");
                ((Activity) context).findViewById(R.id.animation_grid).setTag("left");
                ((Activity) context).findViewById(R.id.add_animation_btn).setTag("left");
            } else if (Constants.VIEW_TYPE == Constants.TEXT_VIEW) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.inputText));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.bevalSlider));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.fontstore));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.text_gridView));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_text_btn));

                ((Activity) context).findViewById(R.id.inputText).setContentDescription(context.getString(R.string.enter_your_text_here));
                ((Activity) context).findViewById(R.id.bevalSlider).setContentDescription(context.getString(R.string.add_bevel_to_3d_text));
                ((Activity) context).findViewById(R.id.fontstore).setContentDescription(context.getString(R.string.toggle_between_store_fonts_and_local));
                ((Activity) context).findViewById(R.id.text_gridView).setContentDescription(context.getString(R.string.choose_a_font_to_preview));
                ((Activity) context).findViewById(R.id.add_text_btn).setContentDescription(context.getString(R.string.import_selected_3d_text));

                ((Activity) context).findViewById(R.id.inputText).setTag("left");
                ((Activity) context).findViewById(R.id.bevalSlider).setTag("left");
                ((Activity) context).findViewById(R.id.fontstore).setTag("left");
                ((Activity) context).findViewById(R.id.text_gridView).setTag("left");
                ((Activity) context).findViewById(R.id.add_text_btn).setTag("left");
            } else if (Constants.VIEW_TYPE == Constants.OBJ_VIEW) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.obj_grid));
                ((EditorView) context).helpDialogs.views.add(((EditorView) context).objSelection.objView.findViewById(R.id.import_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.import_model));

                ((Activity) context).findViewById(R.id.obj_grid).setContentDescription(context.getString(R.string.select_and_obj));
                ((EditorView) context).objSelection.objView.findViewById(R.id.import_btn).setContentDescription(context.getString(R.string.tap_to_import_obj_from_local));
                ((Activity) context).findViewById(R.id.import_model).setContentDescription(context.getString(R.string.tap_to_choose_texture_or_color));

                ((EditorView) context).objSelection.objView.findViewById(R.id.import_btn).setTag("left");
                ((Activity) context).findViewById(R.id.import_model).setTag("left");
            } else if (Constants.VIEW_TYPE == Constants.CHANGE_TEXTURE) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.image_grid));
                ((EditorView) context).helpDialogs.views.add(((EditorView) context).textureSelection.v.findViewById(R.id.import_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_image));

                ((Activity) context).findViewById(R.id.image_grid).setContentDescription(context.getString(R.string.choose_a_texture_to_apply));
                ((EditorView) context).textureSelection.v.findViewById(R.id.import_btn).setContentDescription(context.getString(R.string.tap_to_import_texture));
                ((Activity) context).findViewById(R.id.add_image).setContentDescription(context.getString(R.string.import_the_model_with_selected_texture_color));

                ((Activity) context).findViewById(R.id.image_grid).setTag("left");
                ((EditorView) context).textureSelection.v.findViewById(R.id.import_btn).setTag("left");
                ((Activity) context).findViewById(R.id.add_image).setTag("left");
            } else if (Constants.VIEW_TYPE == Constants.IMAGE_VIEW) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.image_grid));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_image));
                ((EditorView) context).helpDialogs.views.add(((EditorView) context).imageSelection.v.findViewById(R.id.import_btn));

                ((Activity) context).findViewById(R.id.image_grid).setContentDescription(context.getString(R.string.select_an_image_to_preview));
                ((Activity) context).findViewById(R.id.add_image).setContentDescription(context.getString(R.string.tap_to_import_the_selected_image));
                ((EditorView) context).imageSelection.v.findViewById(R.id.import_btn).setContentDescription(context.getString(R.string.tap_to_import_image_from_gallery));

                ((Activity) context).findViewById(R.id.image_grid).setTag("left");
                ((Activity) context).findViewById(R.id.add_image).setTag("left");
                ((EditorView) context).imageSelection.v.findViewById(R.id.import_btn).setTag("left");
            } else if (Constants.VIEW_TYPE == Constants.AUTO_RIG_VIEW) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.add_joint));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rig_rotate));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rig_mirror));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rig_cancel));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rig_scene_lable));
                if (((Activity) context).findViewById(R.id.rig_add_to_scene).getVisibility() == View.VISIBLE)
                    ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rig_add_to_scene));

                ((Activity) context).findViewById(R.id.add_joint).setContentDescription(context.getString(R.string.tap_to_add_a_joint));
                ((Activity) context).findViewById(R.id.rig_rotate).setContentDescription(context.getString(R.string.switch_to_move_roate_scale));
                ((Activity) context).findViewById(R.id.rig_mirror).setContentDescription(context.getString(R.string.enable_mirror_to_move));
                ((Activity) context).findViewById(R.id.rig_cancel).setContentDescription(context.getString(R.string.cancel_rigging));
                ((Activity) context).findViewById(R.id.rig_scene_lable).setContentDescription(context.getString(R.string.current_mode_in_rigging));
                ((Activity) context).findViewById(R.id.rig_add_to_scene).setContentDescription(context.getString(R.string.tap_add_rigged_model));

                ((Activity) context).findViewById(R.id.add_joint).setTag("left");
                ((Activity) context).findViewById(R.id.rig_rotate).setTag("left");
                ((Activity) context).findViewById(R.id.rig_mirror).setTag("left");
                ((Activity) context).findViewById(R.id.rig_cancel).setTag("top");
                ((Activity) context).findViewById(R.id.rig_scene_lable).setTag("top");
                ((Activity) context).findViewById(R.id.rig_add_to_scene).setTag("top");
            } else if (Constants.VIEW_TYPE == Constants.EDITOR_VIEW) {
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.play_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.my_object_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.import_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.animation_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.option_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.export_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rotate_btn));
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.undo));

                ((Activity) context).findViewById(R.id.play_btn).setContentDescription(context.getString(R.string.play_animation_preview));
                ((Activity) context).findViewById(R.id.my_object_btn).setContentDescription(context.getString(R.string.manage_objects_in_scene));
                ((Activity) context).findViewById(R.id.import_btn).setContentDescription(context.getString(R.string.import_objects_into_the_scene));
                ((Activity) context).findViewById(R.id.animation_btn).setContentDescription(context.getString(R.string.apply_animation_to_the_model));
                ((Activity) context).findViewById(R.id.option_btn).setContentDescription(context.getString(R.string.change_properties_of_the_camera));
                ((Activity) context).findViewById(R.id.export_btn).setContentDescription(context.getString(R.string.export_image_or_video));
                ((Activity) context).findViewById(R.id.rotate_btn).setContentDescription(context.getString(R.string.switch_to_move_rotate_scale));
                ((Activity) context).findViewById(R.id.undo).setContentDescription(context.getString(R.string.undo_redo_actions));

                ((Activity) context).findViewById(R.id.play_btn).setTag("left");
                ((Activity) context).findViewById(R.id.my_object_btn).setTag("left");
                ((Activity) context).findViewById(R.id.import_btn).setTag("left");
                ((Activity) context).findViewById(R.id.animation_btn).setTag("left");
                ((Activity) context).findViewById(R.id.option_btn).setTag("left");
                ((Activity) context).findViewById(R.id.export_btn).setTag("left");
                ((Activity) context).findViewById(R.id.rotate_btn).setTag("left");
                ((Activity) context).findViewById(R.id.undo).setTag("left");
            }
        } catch (NullPointerException ignored) {
            ignored.printStackTrace();
        }
    }

    public void addExportViewDescriptions(Context context, Dialog view, int exportType) {
        ((EditorView) context).helpDialogs.views.clear();
        for (int i = 0; i < ((EditorView) context).helpDialogs.toolTips.size(); i++) {
            if (((EditorView) context).helpDialogs.toolTips.get(i).isShown())
                ((EditorView) context).helpDialogs.toolTips.get(i).remove();
        }
        ((EditorView) context).helpDialogs.toolTips.clear();

        ((EditorView) context).helpDialogs.views.add(view.findViewById(R.id.waterMark));
        if (exportType == Constants.EXPORT_VIDEO)
            ((EditorView) context).helpDialogs.views.add(view.findViewById(R.id.seekbar_placeholder));
        ((EditorView) context).helpDialogs.views.add(view.findViewById(R.id.credit_lable));
        ((EditorView) context).helpDialogs.views.add(view.findViewById(R.id.next));

        (view.findViewById(R.id.waterMark)).setContentDescription(context.getString(R.string.enable_disable_watermark));
        (view.findViewById(R.id.seekbar_placeholder)).setContentDescription(context.getString(R.string.limit_the_number_of_frames));
        (view.findViewById(R.id.credit_lable)).setContentDescription(context.getString(R.string.credits_required_for_the_export));
        (view.findViewById(R.id.next)).setContentDescription(context.getString(R.string.tap_to_start_export));


        (view.findViewById(R.id.waterMark)).setTag("top");
        (view.findViewById(R.id.seekbar_placeholder)).setTag("top");
        (view.findViewById(R.id.credit_lable)).setTag("left");
        (view.findViewById(R.id.next)).setTag("top");
    }

    public void addSettingsViewDescriptions(Context context, Dialog view) {
        final String className = context.getClass().getSimpleName();
        HelpDialogs helpDialogs = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection) (context)).helpDialogs : ((EditorView) (context)).helpDialogs);

        helpDialogs.views.clear();
        for (int i = 0; i < helpDialogs.toolTips.size(); i++) {
            if (helpDialogs.toolTips.get(i).isShown())
                helpDialogs.toolTips.get(i).remove();
        }
        helpDialogs.toolTips.clear();

        helpDialogs.views.add(view.findViewById(R.id.textView5));
        helpDialogs.views.add(view.findViewById(R.id.textView3));
        helpDialogs.views.add(view.findViewById(R.id.mutiSelect_switch));
        helpDialogs.views.add(view.findViewById(R.id.restore_btn));
        helpDialogs.views.add(view.findViewById(R.id.settings_done_btn));

        view.findViewById(R.id.textView3).setContentDescription(context.getString(R.string.choose_wheather_toolbar_should));
        view.findViewById(R.id.textView5).setContentDescription(context.getString(R.string.choose_to_display_time));
        view.findViewById(R.id.mutiSelect_switch).setContentDescription(context.getString(R.string.enable_disable_multiselect));
        view.findViewById(R.id.restore_btn).setContentDescription(context.getString(R.string.tap_to_restore_purchase));
        view.findViewById(R.id.settings_done_btn).setContentDescription(context.getString(R.string.tap_to_save));

        view.findViewById(R.id.textView3).setTag("right");
        view.findViewById(R.id.textView5).setTag("left");
        view.findViewById(R.id.mutiSelect_switch).setTag("top");
        view.findViewById(R.id.restore_btn).setTag("top");
        view.findViewById(R.id.settings_done_btn).setTag("top");
    }

    public void helpForFirstTimeUser(Context context) {
        if (!Constants.isFirstTimeUser) return;
        ((EditorView) context).helpDialogs.views.clear();
        for (int i = 0; i < ((EditorView) context).helpDialogs.toolTips.size(); i++) {
            if (((EditorView) context).helpDialogs.toolTips.get(i).isShown())
                ((EditorView) context).helpDialogs.toolTips.get(i).remove();
        }
        ((EditorView) context).helpDialogs.toolTips.clear();
        switch (actionType) {
            case 0:
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.import_btn));
                ((Activity) context).findViewById(R.id.import_btn).setContentDescription(context.getString(R.string.import_objects_into_scene));
                ((Activity) context).findViewById(R.id.import_btn).setTag("left");
                actionType += 1;
                break;
            case 1:
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.animation_btn));
                ((Activity) context).findViewById(R.id.animation_btn).setContentDescription(context.getString(R.string.apply_animation_to_text_or_model));
                ((Activity) context).findViewById(R.id.animation_btn).setTag("left");
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.option_btn));
                ((Activity) context).findViewById(R.id.option_btn).setContentDescription(context.getString(R.string.change_properties_of_camera));
                ((Activity) context).findViewById(R.id.option_btn).setTag("left");
                ((EditorView) context).helpDialogs.views.add(((Activity) context).findViewById(R.id.rotate_btn));
                ((Activity) context).findViewById(R.id.rotate_btn).setContentDescription(context.getString(R.string.switch_to_move_roate_scale));
                ((Activity) context).findViewById(R.id.rotate_btn).setTag("left");
                actionType += 1;
                break;
        }
    }
}
