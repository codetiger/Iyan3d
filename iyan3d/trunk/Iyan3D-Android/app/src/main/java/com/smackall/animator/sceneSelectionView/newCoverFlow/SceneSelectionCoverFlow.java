
package com.smackall.animator.sceneSelectionView.newCoverFlow;

import android.content.Context;
import android.graphics.Camera;
import android.graphics.Matrix;
import android.util.AttributeSet;
import android.view.View;
import android.view.animation.Transformation;
import android.widget.Gallery;

import com.smackall.animator.common.Constant;

public class SceneSelectionCoverFlow extends Gallery {

    public static final int ACTION_DISTANCE_AUTO = Integer.MAX_VALUE;
    public static final float SCALEDOWN_GRAVITY_CENTER = 0.5f;
    private float unselectedAlpha;
    private Camera transformationCamera;
    private int maxRotation = 90;
    private float unselectedScale;
    private float scaleDownGravity = SCALEDOWN_GRAVITY_CENTER;
    private int actionDistance;
    private float unselectedSaturation;

    public SceneSelectionCoverFlow(Context context) {
        super(context);
        this.initialize();
    }

    public SceneSelectionCoverFlow(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.initialize();
        this.applyXmlAttributes(attrs);
    }

    public SceneSelectionCoverFlow(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        this.initialize();
        this.applyXmlAttributes(attrs);
    }

    private void initialize() {
        this.transformationCamera = new Camera();
        this.setSpacing(-60);
    }

    private void applyXmlAttributes(AttributeSet attrs) {
   //     TypedArray a = getContext().obtainStyledAttributes(attrs, R.styleable.FancyCoverFlow);

        this.actionDistance = 200;
        this.scaleDownGravity = 1.0f;
        this.maxRotation = 45;
        this.unselectedAlpha = 0.3f;
        this.unselectedSaturation = 0.5f;
        this.unselectedScale = 0.75f;
    }



    @Override
    protected boolean getChildStaticTransformation(View child, Transformation t) {
        // We can cast here because FancyCoverFlowAdapter only creates wrappers.
        SceneSelectionCoverFlowItemWrapper item = (SceneSelectionCoverFlowItemWrapper) child;

        // Since Jelly Bean childs won't get invalidated automatically, needs to be added for the smooth coverflow animation
        if (android.os.Build.VERSION.SDK_INT >= 16) {
            item.invalidate();
        }

        final int coverFlowWidth = this.getWidth();
        final int coverFlowCenter = coverFlowWidth / 2;
        final int childWidth = item.getWidth();
        final int childHeight = item.getHeight();
        final int childCenter = item.getLeft() + childWidth / 2;

        

        // Use coverflow width when its defined as automatic.
        final int actionDistance = (this.actionDistance == ACTION_DISTANCE_AUTO) ? (int) ((coverFlowWidth + childWidth) / 2.0f) : this.actionDistance;

        // Calculate the abstract amount for all effects.
        final float effectsAmount = Math.min(1.0f, Math.max(-1.0f, (1.0f / actionDistance) * (childCenter - coverFlowCenter)));

        // Clear previous transformations and set transformation type (matrix + alpha).
        t.clear();
        t.setTransformationType(Transformation.TYPE_BOTH);

        // Alpha
        if (this.unselectedAlpha != 1) {
            final float alphaAmount = (this.unselectedAlpha - 1) * Math.abs(effectsAmount) + 1;
            t.setAlpha(alphaAmount);
        }

        // Saturation
        if (this.unselectedSaturation != 1) {
            // Pass over saturation to the wrapper.
            final float saturationAmount = (this.unselectedSaturation - 1) * Math.abs(effectsAmount) + 1;
            item.setSaturation(saturationAmount);
        }

        final Matrix imageMatrix = t.getMatrix();

        // Apply rotation.
        if (this.maxRotation != 0) {
            final int rotationAngle = (int) (-effectsAmount * this.maxRotation);
            this.transformationCamera.save();
            this.transformationCamera.rotateY(rotationAngle);
            this.transformationCamera.getMatrix(imageMatrix);
            this.transformationCamera.restore();
        }

        // Zoom.
        if (this.unselectedScale != 1) {
            final float zoomAmount = (this.unselectedScale - 1) * Math.abs(effectsAmount) + 1;
            // Calculate the scale anchor (y anchor can be altered)
            final float translateX = childWidth / 2.0f;
            final float translateY = childHeight * this.scaleDownGravity;
            imageMatrix.preTranslate(-translateX, -translateY);
            imageMatrix.postScale(zoomAmount, zoomAmount);
            imageMatrix.postTranslate(translateX, translateY);
        }
        if(Constant.currentItemID != this.getSelectedItemPosition()) {
            Constant.currentItemID = this.getSelectedItemPosition();
            System.out.println("Current Item :"+this.getSelectedItemPosition());
        }
       return true;
    }

}
