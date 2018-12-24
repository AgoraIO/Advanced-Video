package io.agora.largegroupcall.ui;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

import java.util.List;

import io.agora.largegroupcall.R;
import io.agora.largegroupcall.model.UserItem;

/**
 * Created by beryl on 2017/10/20.
 */

public class LiveRoomShowFourAdapter extends RecyclerView.Adapter<LiveRoomShowFourAdapter.MyViewHolder> {

    protected final LayoutInflater mInflater;
    private List<UserItem> userItemList;
    private long downTime = 0;
    private long dumpTime = 200;
    protected int mItemWidth;
    protected int mItemHeight;


    public LiveRoomShowFourAdapter(Context context, List<UserItem> userItemList,  int width, int height) {
        mInflater = ((Activity) context).getLayoutInflater();
        this.userItemList = userItemList;
        mItemWidth = width / 2;
        mItemHeight = height / 2;

    }


    @Override
    public void onBindViewHolder(final MyViewHolder holder, final int position) {

        setupView(holder, position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public int getItemCount() {
        return userItemList.size();
    }

    //Override onCreateViewHolder function，return ViewHolder
    @Override
    public MyViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {

        View view = mInflater.inflate(R.layout.item_live_layout_show_four, parent, false);
        view.getLayoutParams().width = mItemWidth;
        view.getLayoutParams().height = mItemHeight;

        MyViewHolder holder = new MyViewHolder(view);
        return holder;
    }

    class MyViewHolder extends RecyclerView.ViewHolder {

        private RelativeLayout view;


        public MyViewHolder(View view) {
            super(view);

            this.view = (RelativeLayout) view.findViewById(R.id.item_live_parent);
        }

    }

    private void setupView(final MyViewHolder holder, final int position) {

        UserItem userItem = userItemList.get(position);

        if (holder.view.getChildCount() != 0) {

            holder.view.removeAllViews();
        }

        if (userItem.getSurfaceView().getParent() != null) {
            ((RelativeLayout) userItem.getSurfaceView().getParent()).removeAllViews();
        }
        holder.view.addView(userItem.getSurfaceView());


    }


}
