package com.viewcore.test;

import android.content.Context;

import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLLinearView;
import com.bfmj.viewcore.view.GLTextView;

/**
 * Created by yushaochen on 2016/7/13.
 */
public class ChannelLeftItemView extends GLLinearView {

    private Context mContext;

    public ChannelLeftItemView(Context context) {
        super(context);
        mContext = context;
        this.setLayoutParams(350f,168f);
        this.setBackground(new GLColor(0x767676,0.3f));
//        setMargin(270f,0,0,0);
        //创建子view
        createAllView();
    }

    /**
     * 创建列表item
     */
    public void createAllView(){

        GLImageView glImageView = new GLImageView(mContext);
        glImageView.setWidth(80f);
        glImageView.setHeight(80f);
//        glImageView.setMargin(80f,0,0,0);
        glImageView.setId("leftIcon");
        glImageView.setImage(R.drawable.calendar);

        GLTextView glTextView = new GLTextView(mContext);

        glTextView.setWidth(98f);
        glTextView.setHeight(60f);
        glTextView.setTextSize(48);
        glTextView.setTextColor(new GLColor(0xFFFFFF));
        glTextView.setId("leftText");
        glTextView.setText("test");
        glTextView.setMargin(30f,0,0,0);
//        glTextView.setVisible( false);

        this.addViewCenterVertical(glImageView);
        this.addViewCenterVertical(glTextView);
    }

}
