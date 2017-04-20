package com.bfmj.viewcore.view;

import android.content.Context;

import com.baofeng.mojing.input.base.MojingKeyCode;
import com.bfmj.distortion.Logger;
import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.util.GLFocusUtils;


public class GLSeekBarView extends GLProcessView {
    private int mResImg;
    private GLImageView mBarView;
    private int bar_width = 100;
    private int bar_height=20;

    public void setBarImage(int barImage){
        this.mResImg = barImage;
        initView();
    }
    public GLSeekBarView(Context context){
        super(context);
    }

    private void initView(){
        mBarView = new GLImageView(getContext());
        mBarView.setImage(mResImg);
        mBarView.setLayoutParams(bar_width,bar_height);
        this.addView(mBarView);
    }

    public void setBarWidth(int w)
    {
        bar_width = w;
    }

    public void setBarHeight(int h){
        bar_height = h;
    }

    @Override
    public void addView(GLRectView view) {
        view.setX(this.getX() + this.getPaddingLeft() + view.getMarginLeft());
        view.setY(this.getY() + this.getPaddingTop() + view.getMarginTop());
        super.addView(view);
    }

    public void setProcessAnimation(int process){
        float step = (this.getWidth() - this.getPaddingLeft() - this.getPaddingRight()) / process;
        GLAnimation animation = new GLTranslateAnimation(step, 0, 0);
        animation.setAnimView(mBarView);
        animation.setDuration(500);
        mBarView.startAnimation(animation);
    }

    @Override
//    public void doAnimationEnd(){
//        GLGroupView parent = getParent();
//        if( parent != null && parent instanceof GLGridViewScroll){
//            ((GLGridViewScroll) parent).animationEnd();
//        }
//    }


    public void setProcess(int process) {
        float width = (this.getWidth() - this.getPaddingLeft() - this.getPaddingRight()) / 100.0F * process;
//        this.mBarView.setLayoutParams(bar_width ,bar_height);
        this.mBarView.setX(this.getX() + this.getPaddingLeft()+width);
        this.mBarView.setY(this.getY() + this.getPaddingTop());
    }

    private GLOnKeyListener mKeyListener = new GLOnKeyListener() {
        @Override
        public boolean onKeyDown(GLRectView glRectView, int keyCode) {
            if(keyCode== MojingKeyCode.KEYCODE_ENTER){
                float[] pos = GLFocusUtils.getPosition(GLSeekBarView.this.getMatrixState().getVMatrix(), GLSeekBarView.this.getDepth());
                int x=(int)pos[0];//;,y=xy[1];
                float viewX = GLSeekBarView.this.getX();
                float viewWidth = GLSeekBarView.this.getWidth();
                if(x>=viewX&&x< viewX+viewWidth){
                    float process =(x-viewX)/viewWidth;
//                    int current = (int) (mDuration*process);
                }
            }
            return false;
        }
        @Override
        public boolean onKeyUp(GLRectView glRectView, int i) {
            return false;
        }

        @Override
        public boolean onKeyLongPress(GLRectView glRectView, int i) {
            return false;
        }
    };

}
