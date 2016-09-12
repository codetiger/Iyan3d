package com.smackall.iyan3dPro.Helper;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.SwitchCompat;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.smackall.iyan3dPro.EditorView;
import com.smackall.iyan3dPro.Helper.CustomViews.CustomRadioButton;
import com.smackall.iyan3dPro.Helper.CustomViews.CustomSeekBar;
import com.smackall.iyan3dPro.Helper.Listeners.CustomListeners;
import com.smackall.iyan3dPro.Props;
import com.smackall.iyan3dPro.R;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Sabish.M on 23/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class PropsViewGenerator {

    Context mContext;
    LinearLayout mListView;
    EditorView editorView;
    Props props;
    boolean isSubProps;
    Map<Integer, Property> nodeProps;
    ArrayList<Property> propertyMap = new ArrayList<>();

    HashMap<String, Integer> parentGroupName = new HashMap<>();

    public PropsViewGenerator(Context context, LinearLayout listView, Props props, Map<Integer, Property> nodeProps, boolean isSubProps) {
        this.mContext = context;
        this.mListView = listView;
        this.editorView = ((EditorView) mContext);
        this.props = props;
        this.nodeProps = nodeProps;
        this.isSubProps = isSubProps;

        for (Map.Entry<Integer, Property> entry : nodeProps.entrySet()) {
            propertyMap.add(entry.getValue());
        }

        for (int i = 0; i < propertyMap.size(); i++) {
            if (!parentGroupName.containsKey(propertyMap.get(i).groupName)) {
                parentGroupName.put(propertyMap.get(i).groupName, i);
            }
        }
    }

    public int getCount() {
        return propertyMap.size();
    }


    public void getView() {

        for (int i = 0; i < getCount(); i++) {
            View subView = getViewForCell(i, propertyMap, parentGroupName);
            if (subView != null) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN)
                    subView.setBackground(ContextCompat.getDrawable(mContext, R.drawable.bottom_border));
                else
                    subView.setBackgroundDrawable(ContextCompat.getDrawable(mContext, R.drawable.bottom_border));
                mListView.addView(subView);
            }
        }
    }

    public View getViewForCell(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {

        if (map.size() > position && map.get(position).type != Constants.UNDEFINED) {

            switch (map.get(position).type) {
                case Constants.TYPE_NONE:
                    return null;
                case Constants.SLIDER_TYPE:
                    return getSliderTypeView(position, map, groupName);
                case Constants.IMAGE_TYPE:
                    return getImageTypeView(position, map, groupName);
                case Constants.COLOR_TYPE:
                    return getColorTypeView(position, map, groupName);
                case Constants.SWITCH_TYPE:
                    return getSwitchTypeView(position, map, groupName);
                case Constants.PARENT_TYPE:
                    return getParentTypeView(position, map, groupName);
                case Constants.BUTTON_TYPE:
                    return getButtonTypeView(position, map, groupName);
                case Constants.SEGMENT_TYPE:
                case Constants.LIST_TYPE:
                    return getListOrSegmentTypeView(position, map, groupName);
                case Constants.ICON_TYPE:
                    return getIconTypeView(position, map, groupName);
            }
        }
        return null;
    }


    View getSliderTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        int[] params = new int[2];

        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT, 1.0f);
        LinearLayout linearLayout = props.getLinearLayout(LinearLayout.HORIZONTAL, 1.0f, Gravity.LEFT | Gravity.CENTER, ltParams);
        if (!map.get(position).title.equals("")) {
            params[0] = 0;
            params[1] = LinearLayout.LayoutParams.MATCH_PARENT;
            TextView t = props.getTextView(0.3f, " " + map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);
            linearLayout.addView(t);
        }
        CustomSeekBar seekBar = props.getSlider((!map.get(position).title.equals("")) ? 0.7f : 1.0f);

        boolean isDynamic = map.get(position).valueW == 1;
        int x = (int) (map.get(position).valueX * 100);
        int y = (int) (map.get(position).valueY * 100);
        int z = (int) (map.get(position).valueZ * 100);
        int w = (int) (map.get(position).valueW * 100);
        seekBar.setProgress(x);
        seekBar.setMax((x >= seekBar.getMax() * 100) ? x * 2 : seekBar.getMax() * 100);
        seekBar.setDifference((x >= (seekBar.getMax()) ? Math.abs(x - 50) : 0));

        seekBar.setOnSeekBarChangeListener(new CustomListeners(propertyMap.get(position), position, mContext, props));

        linearLayout.addView(seekBar);
        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {

            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView title = props.getTextView(0.0f, " " + map.get(position).groupName, Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            title.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            title.getLayoutParams().height = measureHeight(title) + ((measureHeight(title)) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = title;

            //Switch type is horizontal view but cell has vertical so create vertical linear layout.
            ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            LinearLayout vlt = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER_VERTICAL, ltParams);
            vlt.addView(linearLayout);

            groupViews[1] = vlt;

            return addGroupName(groupViews, LinearLayout.VERTICAL);
        } else {
            linearLayout.getLayoutParams().height = measureHeight(linearLayout) + (measureHeight(linearLayout) / 2);
            return linearLayout;
        }
    }

    View getImageTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {

        int[] params = new int[2];

        Button bu = props.getIconTypeButton(0.0f, map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER_VERTICAL, null);

        try {
            if (FileHelper.checkValidFilePath(map.get(position).fileName)) {
                Bitmap bmp = BitmapFactory.decodeFile(map.get(position).fileName);
                Drawable dr = new BitmapDrawable(mContext.getResources(), Bitmap.createScaledBitmap(bmp, 48, 48, false));
                bu.setCompoundDrawablesWithIntrinsicBounds(null, null, dr, null);
            }
        } catch (NullPointerException | OutOfMemoryError | IllegalStateException e) {
            e.printStackTrace();
        }
        bu.setOnClickListener(new CustomListeners(propertyMap.get(position), position, mContext, props));
        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView t = props.getTextView(0.0f, " " + map.get(position).groupName.toUpperCase(), Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = t;
            groupViews[1] = bu;
            LinearLayout ct = addGroupName(groupViews, LinearLayout.VERTICAL);
            ct.getLayoutParams().height = measureHeight(ct) + (measureHeight(ct) / 2);
            return ct;
        } else {
            LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            LinearLayout linearLayout1 = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER | Gravity.LEFT, ltParams);
            linearLayout1.addView(bu);
            return linearLayout1;
        }
    }

    View getColorTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        final int[] params = new int[2];

        if (!isSubProps) {
            Button bu = props.getIconTypeButton(0.0f, map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER_VERTICAL, null);
            int color = Color.argb((int) map.get(position).valueW * 255, (int) map.get(position).valueX * 255, (int) map.get(position).valueY * 255,
                    (int) map.get(position).valueZ * 255);

            Bitmap bmp;
            Drawable dr = null;
            if (map.get(position).type == Constants.COLOR_TYPE) {
                bmp = Bitmap.createBitmap(48, 48, Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(bmp);
                canvas.drawColor(color);
                dr = new BitmapDrawable(mContext.getResources(), bmp);
            } else {
                if (FileHelper.checkValidFilePath(map.get(position).fileName)) {
                    bmp = BitmapFactory.decodeFile(map.get(position).fileName);
                    dr = new BitmapDrawable(mContext.getResources(), bmp);
                }
            }
            bu.setCompoundDrawablesWithIntrinsicBounds(null, null, dr, null);
            bu.setOnClickListener(new CustomListeners(propertyMap.get(position), position, mContext, props));
            if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
                params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
                params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
                TextView t = props.getTextView(0.0f, " " + map.get(position).groupName.toUpperCase(), Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
                t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
                t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);

                View[] groupViews = new View[2];
                groupViews[0] = t;
                groupViews[1] = bu;
                LinearLayout ct = addGroupName(groupViews, LinearLayout.VERTICAL);
                ct.getLayoutParams().height = measureHeight(ct) + (measureHeight(ct) / 2);
                return ct;
            } else {

                LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                LinearLayout linearLayout1 = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER | Gravity.LEFT, ltParams);
                linearLayout1.addView(bu);
                return linearLayout1;
            }
        } else {
            LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
            LinearLayout parent = props.getLinearLayout(LinearLayout.HORIZONTAL, 1.0f, Gravity.CENTER, ltParams);
            ltParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 0.7f);
            LinearLayout colorPicker = props.getLinearLayout(LinearLayout.HORIZONTAL, 1.0f, Gravity.CENTER, ltParams);
            ImageView imageView = props.getImageView();
            imageView.setImageDrawable(ContextCompat.getDrawable(mContext, R.drawable.color_picker));
            Bitmap bitmap = ((BitmapDrawable) imageView.getDrawable()).getBitmap();

            colorPicker.addView(imageView);
            parent.addView(colorPicker);
            ltParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 0.3f);
            LinearLayout preview = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER, ltParams);
            int color = Color.argb((int) map.get(position).valueW * 255, (int) map.get(position).valueX * 255, (int) map.get(position).valueY * 255,
                    (int) map.get(position).valueZ * 255);
            preview.setBackgroundColor(color);
            imageView.setOnTouchListener(new CustomListeners(map.get(position), position, mContext, props, bitmap, preview));
            parent.addView(preview);
            return parent;
        }
    }

    View getSwitchTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        final int[] params = new int[2];

        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT, 1.0f);
        LinearLayout linearLayout = props.getLinearLayout(LinearLayout.HORIZONTAL, 1.0f, Gravity.LEFT | Gravity.CENTER, ltParams);
        if (!map.get(position).title.equals("")) {
            params[0] = 0;
            params[1] = LinearLayout.LayoutParams.MATCH_PARENT;
            TextView t = props.getTextView(0.7f, " " + map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);
            linearLayout.addView(t);
        }
        ltParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 0.3f);
        SwitchCompat switchCompat = props.getSwitch(false, ltParams);
        switchCompat.setChecked((propertyMap.get(position).valueX > 0.0f));
        switchCompat.setOnCheckedChangeListener(new CustomListeners(propertyMap.get(position), position, mContext, props));
        linearLayout.addView(switchCompat);
        linearLayout.getLayoutParams().height = measureHeight(linearLayout) + (measureHeight(linearLayout) / 2);
        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView t = props.getTextView(0.0f, " " + map.get(position).groupName, Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = t;

            //Switch type is horizontal view but cell has vertical so create vertical linear layout.
            ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            LinearLayout vlt = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER_VERTICAL, ltParams);
            vlt.addView(linearLayout);
            groupViews[1] = vlt;
            return addGroupName(groupViews, LinearLayout.VERTICAL);
        } else {
            linearLayout.getLayoutParams().height = measureHeight(linearLayout) + (measureHeight(linearLayout) / 2);
            return linearLayout;
        }
    }

    View getParentTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        int[] params = new int[2];

        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
        final LinearLayout vlt = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER | Gravity.LEFT, ltParams);
        final LinearLayout hlt = props.getLinearLayout(LinearLayout.HORIZONTAL, 1.0f, Gravity.CENTER | Gravity.LEFT, ltParams);
        int hltHeight = measureHeight(props.getIconTypeButton(0.0f, map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER_VERTICAL, null));
        ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, hltHeight);
        hlt.setLayoutParams(ltParams);

        params[0] = 0;
        params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
        TextView ti = props.getTextView(0.7f, " " + map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
        ti.getLayoutParams().height = measureHeight(ti) + ((measureHeight(ti)) / 2);
        hlt.addView(ti);

        ltParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 0.3f);
        LinearLayout parentForImage = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER | Gravity.RIGHT, ltParams);
        parentForImage.setPadding(5, 5, 5, 5);

        ImageView imageView = props.getImageView();
        imageView.setImageDrawable(ContextCompat.getDrawable(mContext, R.drawable.arrow_right));
        parentForImage.addView(imageView);

        hlt.addView(parentForImage);

        final int finalPos = position;
        final ArrayList<Property> finalMap = map;
        hlt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                props.getPropsView(editorView.sgNodeOptionsMap.getSortedMap(finalMap.get(finalPos).subProps), true, true);
            }
        });

        vlt.addView(hlt);

        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView t = props.getTextView(0.0f, " " + map.get(position).groupName.toUpperCase(), Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = t;
            groupViews[1] = vlt;
            return addGroupName(groupViews, LinearLayout.VERTICAL);
        } else {
            return vlt;
        }
    }

    View getButtonTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        int[] params = new int[2];

        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        Button bu = props.getIconTypeButton(0.0f, map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER_VERTICAL, null);
        bu.setLayoutParams(ltParams);
        bu.setOnClickListener(new CustomListeners(propertyMap.get(position), position, mContext, props));
        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView t = props.getTextView(0.0f, " " + map.get(position).groupName.toUpperCase(), Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = t;
            groupViews[1] = bu;
            LinearLayout ct = addGroupName(groupViews, LinearLayout.VERTICAL);
            ct.getLayoutParams().height = measureHeight(ct) + (measureHeight(ct) / 2);
            return ct;
        } else {
            ltParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            LinearLayout linearLayout1 = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER | Gravity.LEFT, ltParams);
            linearLayout1.addView(bu);
            return linearLayout1;
        }
    }

    View getListOrSegmentTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        int[] params = new int[2];

        RadioGroup r = props.getRadioGroup(Gravity.LEFT | Gravity.CENTER, RadioGroup.VERTICAL);
        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView t = props.getTextView(0.0f, " " + map.get(position).groupName.toUpperCase(), Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            t.getLayoutParams().height = measureHeight(t) + (measureHeight(t) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = t;
            groupViews[1] = r;
            for (int i = position; i < map.size(); i++) {
                if (map.get(i).groupName.toLowerCase().equals(map.get(position).groupName.toLowerCase())) {
                    LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
                    CustomRadioButton rb = props.getRadioButton(map.get(i).title, Color.BLACK, false, ltParams);
                    rb.setPosition(i);
                    rb.getLayoutParams().height = measureHeight(rb) + ((measureHeight(rb)) / 2);
                    r.addView(rb);
                    rb.setChecked(map.get(i).valueX > 0);
                }
            }
            r.setOnCheckedChangeListener(new CustomListeners(propertyMap,mContext,props));
            return addGroupName(groupViews, LinearLayout.VERTICAL);
        } else {
            return null;
        }
    }

    View getIconTypeView(int position, ArrayList<Property> map, HashMap<String, Integer> groupName) {
        int[] params = new int[2];

        Drawable d = null;
        if (getDrawable(position, map) != 0)
            d = ContextCompat.getDrawable(mContext, getDrawable(position, map));
        Button b = props.getIconTypeButton(0.0f, map.get(position).title, Color.BLACK, Gravity.LEFT | Gravity.CENTER_VERTICAL, d);
        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT, 0.0f);
        b.setLayoutParams(ltParams);
        b.setOnClickListener(new CustomListeners(propertyMap.get(position), position, mContext, props));
        if (groupName.containsKey(map.get(position).groupName) && position == groupName.get(map.get(position).groupName)) {
            params[0] = LinearLayout.LayoutParams.MATCH_PARENT;
            params[1] = LinearLayout.LayoutParams.WRAP_CONTENT;
            TextView t = props.getTextView(0.0f, " " + map.get(position).groupName.toUpperCase(), Color.BLACK, Gravity.LEFT | Gravity.CENTER, params);
            t.setBackgroundColor(ContextCompat.getColor(mContext, R.color.title));
            t.getLayoutParams().height = measureHeight(t) + ((measureHeight(t)) / 2);

            View[] groupViews = new View[2];
            groupViews[0] = t;
            groupViews[1] = b;
            return addGroupName(groupViews, LinearLayout.VERTICAL);
        } else {
            ltParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            LinearLayout linearLayout1 = props.getLinearLayout(LinearLayout.VERTICAL, 1.0f, Gravity.CENTER | Gravity.LEFT, ltParams);
            linearLayout1.addView(b);
            return linearLayout1;
        }
    }

    public LinearLayout addGroupName(View[] views, int orientation) {
        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
        LinearLayout v = props.getLinearLayout(orientation, 1.0f, Gravity.CENTER_VERTICAL, ltParams);
        v.removeAllViews();
        for (View view : views) {
            if (view == null) continue;
            v.setWeightSum(v.getWeightSum() + ((view.getLayoutParams() != null) ?
                    ((LinearLayout.LayoutParams) view.getLayoutParams()).weight : 0.0f));
            v.addView(view);
        }
        return v;
    }

    private int getDrawable(int position, ArrayList<Property> map) {
        if (position >= map.size()) return 0;

        if (map.get(position).index == Constants.DELETE)
            return R.drawable.delete_btn;
        else if (map.get(position).index == Constants.CLONE)
            return R.drawable.clone;
        else if (map.get(position).title.toLowerCase().equals("skin"))
            return R.drawable.skin;
        return 0;
    }

    public int measureHeight(ViewGroup view) {
        if (view == null) return 0;
        int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(Constants.width, View.MeasureSpec.AT_MOST);
        int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
        view.measure(widthMeasureSpec, heightMeasureSpec);
        return view.getMeasuredHeight();
    }

    public int measureHeight(View view) {
        if (view == null) return 0;
        int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(Constants.width, View.MeasureSpec.AT_MOST);
        int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
        view.measure(widthMeasureSpec, heightMeasureSpec);
        return view.getMeasuredHeight();
    }

    public int measureHeight(RadioButton view) {
        if (view == null) return 0;
        int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(Constants.width, View.MeasureSpec.AT_MOST);
        int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
        view.measure(widthMeasureSpec, heightMeasureSpec);
        return view.getMeasuredHeight();
    }
}
