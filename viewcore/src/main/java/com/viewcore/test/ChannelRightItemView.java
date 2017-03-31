package com.viewcore.test;

import android.content.Context;

import com.bfmj.distortion.Logger;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRelativeView;
import com.bfmj.viewcore.view.GLTextView;

/**
 * Created by yushaochen on 2016/7/13.
 */
public class ChannelRightItemView extends GLRelativeView {

    private Context mContext;

    public ChannelRightItemView(Context context) {
        super(context);
        mContext = context;
        this.setLayoutParams(350f,168f);
        this.setBackground(new GLColor(0x767676,0.3f));
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
        glImageView.setMargin(228f,20f,0,0);
        glImageView.setId("leftIcon");

        GLTextView text1 = new GLTextView(mContext);
        text1.setWidth(230f);
        text1.setHeight(100f);
        text1.setTextSize(80);
        text1.setTextColor(new GLColor(0xffffff));
        text1.setId("title");
        text1.setMargin(30f,60f,0,0);

//        GLTextView text2 = new GLTextView(mContext);
//        text2.setWidth(300f);
//        text2.setHeight(35f);
//        text2.setTextSize(28);
//        text2.setTextColor(new GLColor(0xffffff));
//        text2.setId("subtitle");
//        text2.setMargin(30f,10f,0,0);

        this.addView(glImageView);
        this.addView(text1);
//        this.addViewOf(text2,null,null,null,text1);
    }

}
