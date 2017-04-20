package com.viewcore.test;

import com.bfmj.viewcore.animation.GLAlphaAnimation;
import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLScaleAnimation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.view.GLRectView;

/**
 * Created by yushaochen on 2016/8/4.
 */
public class AnimUtils {

    public static void startScaleAndTranslate(GLRectView view , boolean focuse){
//        if(focuse) {
//            GLScaleAnimation animation2 = new GLScaleAnimation(1.10f, 1.10f);
//            animation2.setDuration(300);
//            animation2.setAnimView(view);
//            view.startAnimation(animation2);
//        } else {
//            GLScaleAnimation animation2 = new GLScaleAnimation(1.0f, 1.0f);
//            animation2.setDuration(300);
//            animation2.setAnimView(view);
//            view.startAnimation(animation2);
//        }
        startTranslate(view,focuse);
//        startAlphaAnimation(view, focuse);
    }

    public static void startAlphaAnimation(final GLRectView view, boolean focuse ) {
        if( focuse){
            GLAnimation animation = new GLAlphaAnimation(1,0);
            animation.setAnimView(view);
            animation.setDuration(300);
            view.startAnimation(animation);
        }else{
            GLAnimation animation = new GLAlphaAnimation(0,1);
            animation.setAnimView(view);
            animation.setDuration(300);
            view.startAnimation(animation);
        }

    }

    public static void startTranslate( GLRectView view , boolean focuse){
        if(focuse) {
            GLAnimation animation1 = new GLTranslateAnimation(-100, 0, -0.5f);
            animation1.setAnimView(view);
            animation1.setDuration(300);
            view.startAnimation(animation1);
        } else {
            GLAnimation animation1 = new GLTranslateAnimation(100, 0, 0.5f);
            animation1.setAnimView(view);
            animation1.setDuration(300);
            view.startAnimation(animation1);
//            animation1.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
//                @Override
//                public void onAnimationStart(GLAnimation glAnimation) {
//
//                }
//
//                @Override
//                public void onAnimationEnd(GLAnimation glAnimation) {
//                    view.setDepth(view.getParent().getDepth());
//                }
//            });
        }
    }

    public static void startScale(GLRectView view , boolean focuse){
        if(focuse) {
            GLScaleAnimation animation2 = new GLScaleAnimation(1.10f, 1.10f);
            animation2.setDuration(300);
            animation2.setAnimView(view);
            view.startAnimation(animation2);
        } else {
            GLScaleAnimation animation2 = new GLScaleAnimation(1.0f, 1.0f);
            animation2.setDuration(300);
            animation2.setAnimView(view);
            view.startAnimation(animation2);
        }
    }
}
