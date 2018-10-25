package io.agora.openlive.ui;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.List;
import io.agora.openlive.R;
import io.agora.openlive.model.UserItem;

/**
 * Created by beryl on 2017/10/20.
 */

public class LiveRoom17Adapter extends RecyclerView.Adapter<LiveRoom17Adapter.MyViewHolder> {

    protected final LayoutInflater mInflater;
    private List<UserItem> userItemList;
    private onClickButtonInterface onClickButtonInterface;
    private long downTime = 0;
    private long dumpTime = 200;


    public LiveRoom17Adapter(Context context, List<UserItem> userItemList, onClickButtonInterface onClickButtonInterface) {
        mInflater = ((Activity) context).getLayoutInflater();
        this.userItemList = userItemList;
        this.onClickButtonInterface = onClickButtonInterface;

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

        View view = mInflater.inflate(R.layout.item_live_layout, parent, false);
        MyViewHolder holder = new MyViewHolder(view);
        return holder;
    }

    class MyViewHolder extends RecyclerView.ViewHolder {

        private TextView tv;
        private ImageView mBtnPlay;
        private ImageView mBtnMute;
        private RelativeLayout view;
        private RelativeLayout textParent;

        public MyViewHolder(View view) {
            super(view);
            tv = (TextView) view.findViewById(R.id.item_live_textview);
            mBtnPlay = (ImageView) view.findViewById(R.id.item_live_button);
            mBtnMute = (ImageView) view.findViewById(R.id.item_live_mute);
            textParent = (RelativeLayout) view.findViewById(R.id.item_live_text_parent);
            this.view = (RelativeLayout) view.findViewById(R.id.item_live_parent);
        }

    }

    private void changeHolderView(MyViewHolder holder, boolean mute, int position) {
        //mute state
        if (mute) {
            holder.textParent.setVisibility(View.VISIBLE);
            holder.mBtnMute.setVisibility(View.INVISIBLE);

        } else {
            holder.textParent.setVisibility(View.INVISIBLE);
            //unable state , userself unable be mute
            if (userItemList.get(position).ismIsUserSelf()) {
                holder.mBtnMute.setVisibility(View.INVISIBLE);
            } else {
                holder.mBtnMute.setVisibility(View.VISIBLE);
            }

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


        //recycleview item space,if you don't want,remove it
        changeHolderView(holder, userItem.ismIsMuteStream(), position);

        holder.tv.setText("" + (userItem.getUid() & 0XFFFFFFFFL)); //position == userItemList.size() - 1 &&


        holder.mBtnMute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                changeHolderView(holder, true, position);
                onClickButtonInterface.onMuteClick(position);
            }
        });

        holder.mBtnPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                changeHolderView(holder, false, position);
                onClickButtonInterface.onPlayClick(position);

            }
        });
        holder.view.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //double click
                        if (System.currentTimeMillis() - downTime < dumpTime) {
                            onClickButtonInterface.onSurfaceDoubleClick(position);
                        }
                        downTime = System.currentTimeMillis();

                        break;
                    case MotionEvent.ACTION_UP:

                        break;
                    case MotionEvent.ACTION_MOVE:
                        break;
                    case MotionEvent.ACTION_CANCEL:
                        break;
                    default:
                        break;
                }

                return false;
            }
        });
    }


    public interface onClickButtonInterface {
        void onMuteClick(int position);

        void onPlayClick(int position);

        void onSurfaceDoubleClick(int position);
    }

}
