package com.viewcore.test;

import android.content.Context;

import com.bfmj.distortion.Logger;
import com.bfmj.viewcore.animation.GLAlphaAnimation;
import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLTransformation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRectView;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by yushaochen on 2016/11/8.
 */

public class DownloadIconView extends GLImageView {

    private Timer timer;
    //private float y;
    public DownloadIconView(Context context) {
        super(context);
        setLayoutParams(200f,200f);
        setBackground(R.drawable.state_icon_download);
        setVisible(false);
//        AlphaAnimation_show(DownloadIconView.this);
//        TranslateAnimation_down1(DownloadIconView.this);
    }

    private boolean mbPause = false;
    @Override
    public void onPause(){
        super.onPause();
        mbPause = true;
        setY(1000);
        GLTransformation transform = animation_down1.getGlTransformation();
        if( null != transform){
            transform.setXYZ(0, 0, 0);
        }
        this.stopAnimation(animation_down1);
        Logger.printTime();
    }

    public void onResume() {
        super.onResume();
        mbPause = false;
        this.stopAnimation(animation_down1);
        setY(1000);
        GLTransformation transform = animation_down1.getGlTransformation();
        if( null != transform){
            transform.setXYZ(0, 0, 0);
        }
        this.startAnimation(animation_down1);
        Logger.printTime();
    }

//    @Override
//    public void doAnimationEnd(){
//        Logger.printTime();
//        setY(1000);
////        AlphaAnimation_show(DownloadIconView.this);
//        TranslateAnimation_down1(DownloadIconView.this);
//    }


    public void start(){
//        y = getY();
        AlphaAnimation_show(DownloadIconView.this);
        TranslateAnimation_down1(DownloadIconView.this);

//        if(null == timer) {
//            timer = new Timer();
//            timer.schedule(new TimerTask() {
//
//                @Override
//                public void run() {
//                    setY(1000);
//                    AlphaAnimation_show(DownloadIconView.this);
//                    TranslateAnimation_down1(DownloadIconView.this);
//                }
//            }, 0, 1200);
//        }
    }

    public void close(){
        //setVisible(false);
        if(null != timer) {
            timer.cancel();
            timer = null;
        }
    }

    private GLAnimation animation_hide;
    private void AlphaAnimation_hide(GLRectView view) {
        // 淡出
        animation_hide = new GLAlphaAnimation(1, 0);
        animation_hide.setAnimView(view);
        animation_hide.setDuration(600);
        view.startAnimation(animation_hide);
    }
    private GLAnimation animation_show;
    private void AlphaAnimation_show(final GLRectView view) {
        // 淡入
        animation_show = new GLAlphaAnimation(0, 1);
        animation_show.setAnimView(view);
        animation_show.setDuration(600);
        animation_show.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
            @Override
            public void onAnimationStart(GLAnimation glAnimation) {

            }

            @Override
            public void onAnimationEnd(GLAnimation glAnimation) {
                AlphaAnimation_hide(view);
            }
        });
        view.startAnimation(animation_show);
    }
    private GLAnimation animation_down1;
    private void TranslateAnimation_down1(GLRectView view) {
        animation_down1 = new GLTranslateAnimation(0f, getHeight()*2-40, 0f);
        animation_down1.setAnimView(view);
        animation_down1.setDuration(1200);
        animation_down1.setOnGLAnimationListener( new GLAnimation.OnGLAnimationListener(){
            @Override
            public void onAnimationStart(GLAnimation glAnimation) {

            }

            @Override
            public void onAnimationEnd(GLAnimation glAnimation) {
                if( !mbPause) {
                    setY(1000);

                    GLTransformation transform = animation_down1.getGlTransformation();
                    if( null != transform){
                        transform.setXYZ(0, 0, 0);
                    }
                    AlphaAnimation_show(DownloadIconView.this);
                    TranslateAnimation_down1(DownloadIconView.this);
                    Logger.printTime();
                }

            }
        });
        view.startAnimation(animation_down1);
    }
//    private GLAnimation animation_down2;
//    private void TranslateAnimation_down2(GLRectView view) {
//        //setY(y+12-20);
//        animation_down2 = new GLTranslateAnimation(0f, getHeight()-20, 0f);
//        animation_down2.setAnimView(view);
//        animation_down2.setDuration(600);
//        animation_down2.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
//            @Override
//            public void onAnimationStart(GLAnimation glAnimation) {
//
//            }
//
//            @Override
//            public void onAnimationEnd(GLAnimation glAnimation) {
//
//            }
//        });
//        view.startAnimation(animation_down2);
//    }
}
