package io.agora.openlive.ui;

import android.content.Context;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.AttributeSet;
import android.util.Log;

/**
 * Created by beryl on 2017/10/30.
 */

public class RecyclerViewNoBugLinearLayoutManager extends LinearLayoutManager {

    public RecyclerViewNoBugLinearLayoutManager(Context context) {
        super(context);
    }

    public RecyclerViewNoBugLinearLayoutManager(Context context, int orientation, boolean reverseLayout) {
        super(context, orientation, reverseLayout);
    }

    public RecyclerViewNoBugLinearLayoutManager(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    @Override
    public void onLayoutChildren(RecyclerView.Recycler recycler, RecyclerView.State state) {

        try {
            //try catch一下
            super.onLayoutChildren(recycler, state);
        } catch (IndexOutOfBoundsException e) {
            Log.e("LinearLayoutManager", "RecyclerViewNoBugLinearLayoutManager onLayoutChildren error");
            e.printStackTrace();
        }

    }
}
