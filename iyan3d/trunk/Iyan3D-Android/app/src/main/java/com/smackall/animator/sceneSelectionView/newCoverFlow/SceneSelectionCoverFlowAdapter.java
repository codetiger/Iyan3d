/*
 * Copyright 2013 David Schreiber
 *           2013 John Paul Nalog
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

package com.smackall.animator.sceneSelectionView.newCoverFlow;

import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;


public abstract class SceneSelectionCoverFlowAdapter extends BaseAdapter {

    // =============================================================================
    // Supertype overrides
    // =============================================================================

    @Override
    public final View getView(int i, View reusableView, ViewGroup viewGroup) {
        SceneSelectionCoverFlow coverFlow = (SceneSelectionCoverFlow) viewGroup;

        View wrappedView = null;
        SceneSelectionCoverFlowItemWrapper coverFlowItem;



        if (reusableView != null) {
            coverFlowItem = (SceneSelectionCoverFlowItemWrapper) reusableView;
            wrappedView = coverFlowItem.getChildAt(0);
            coverFlowItem.removeAllViews();
        } else {
            coverFlowItem = new SceneSelectionCoverFlowItemWrapper(viewGroup.getContext());
        }

        wrappedView = this.getCoverFlowItem(i, wrappedView, viewGroup);



        if (wrappedView == null) {
            throw new NullPointerException("getCoverFlowItem() was expected to return a view, but null was returned.");
        }


        coverFlowItem.addView(wrappedView);
        coverFlowItem.setLayoutParams(wrappedView.getLayoutParams());

        return coverFlowItem;
    }


    // =============================================================================
    // Abstract methods
    // =============================================================================

    public abstract View getCoverFlowItem(int position, View reusableView, ViewGroup parent);
}
