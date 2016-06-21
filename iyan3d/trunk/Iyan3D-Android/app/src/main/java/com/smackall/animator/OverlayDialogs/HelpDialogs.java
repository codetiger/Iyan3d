package com.smackall.animator.OverlayDialogs;

import android.content.Context;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;

import com.smackall.animator.R;

import java.util.ArrayList;

/**
 * Note that currently multiple Overlay doesn't work well, but multiple ToolTip is working fine
 * Therefore, if you want to use multiple ToolTip, please switch off the Overlay by .setOverlay(null)
 */
public class HelpDialogs implements Tooltip.Callback{

    public ArrayList<View> views = new ArrayList<>();
    public ArrayList<Tooltip.TooltipView> toolTips = new ArrayList<>();


    public void showPop(Context ctx)
    {
        //getAllContentDescriptionViews(view);
        for (int i = 0; i < views.size(); i++){
            if(views.get(i) != null && views.get(i).getTag() != null && views.get(i).getContentDescription() != null)
                showToolTipView(ctx,views.get(i),views.get(i).getTag().toString(),views.get(i).getContentDescription().toString(),R.color.yellow);
        }
    }

    private void getAllContentDescriptionViews(ViewGroup view)
    {
        if(view == null) return;
        for(int i = 0; i < (view.getChildCount()); i++){
            try {
                if (view.getChildAt(i).getContentDescription() != null && view.getChildAt(i).getContentDescription() != "" && view.getChildAt(i).getContentDescription().length() > 5) {
                    views.add(view.getChildAt(i));
                }
                if (((ViewGroup) view.getChildAt(i)).getChildCount() > 0) {
                    getAllContentDescriptionViews(((ViewGroup) view.getChildAt(i)));
                }
            }
            catch (ClassCastException ignored){

            }
        }
    }

    private void showToolTipView(Context context,View anchorView, String tag, CharSequence text, int backgroundColor) {
       Tooltip.Gravity gravity = Tooltip.Gravity.LEFT;

        if(tag.toLowerCase().equals("right"))
            gravity = Tooltip.Gravity.RIGHT;
        else if(tag.toLowerCase().equals("left"))
            gravity = Tooltip.Gravity.LEFT;
        else if(tag.toLowerCase().equals("top"))
            gravity = Tooltip.Gravity.TOP;
        else if(tag.toLowerCase().equals("bottom"))
            gravity = Tooltip.Gravity.BOTTOM;
        else if(tag.toLowerCase().equals("center"))
            gravity = Tooltip.Gravity.CENTER;

        DisplayMetrics metrics = context.getResources().getDisplayMetrics();

       toolTips.add(Tooltip.make(
                context,
                new Tooltip.Builder()
                        .anchor(anchorView, gravity)
                        .closePolicy(Tooltip.ClosePolicy.TOUCH_ANYWHERE_CONSUME,0)
                        .text(text)
                        .fitToScreen(true)
                        .withCallback(this)
                        .maxWidth(metrics.widthPixels / 2)
                        .floatingAnimation(Tooltip.AnimationBuilder.DEFAULT)
                        .build(),backgroundColor));
       toolTips.get(toolTips.size()-1).show();
    }

    @Override
    public void onTooltipClose(Tooltip.TooltipView tooltip, boolean fromUser, boolean containsTouch) {
        dismissTips(containsTouch,tooltip);
    }

    @Override
    public void onTooltipFailed(Tooltip.TooltipView view) {

    }

    @Override
    public void onTooltipShown(Tooltip.TooltipView view) {

    }

    @Override
    public void onTooltipHidden(Tooltip.TooltipView view) {

    }

    public void dismissTips(boolean containsTouch, Tooltip.TooltipView tooltip)
    {
        System.out.println("Contains Touch : " + containsTouch);
        for (int i = 0; i < toolTips.size(); i++){
            if(containsTouch){
                if(toolTips.get(i) == tooltip) {
                    toolTips.remove(i);
                    tooltip.hide();
                    tooltip.remove();
                    break;
                }
            }
            else if(toolTips.get(i) != null && toolTips.get(i).isShown()) {
                toolTips.get(i).hide();
                toolTips.get(i).remove();
            }
        }
        if(!containsTouch)
            toolTips.clear();
    }
}
