package com.bfmj.viewcore.view;

import com.bfmj.viewcore.render.GLColor;

import android.content.Context;
import android.graphics.Bitmap;

import com.bfmj.viewcore.render.GLConstant;

/**
 * 进度view
 * ClassName: GLProcessView <br/>
 * @author linzanxian    
 * @date: 2015年3月17日 上午9:57:12 <br/>  
 * description:进度view
 */
public class GLProcessView extends GLGroupView {
	private GLConstant.GLProcessMode mGlProcessMode = GLConstant.GLProcessMode.RECTANGLE; //默认长方形
	private GLColor mColorBackgroup = new GLColor(1.0f, 1.0f, 1.0f);
	private GLColor mColorProcess = new GLColor(0.2f, 1.0f, 0.2f);
	private GLImageView mImageView = null;
	private Context mContext;
	private boolean mRotate = false;
	private int mSpeed = 100;
//	private float mProcess = 0;
	private float mAngle = 45;

	public GLProcessView(Context context) {
		super(context);
		
		mContext = context;
		init();
	}

	/**
	 * 实例化
	 * @author linzanxian  @Date 2015年3月17日 上午9:56:16
	 * @param context Context
	 * @param mode GLProcessMode 进度模式
	 * @return void
	 */
	public GLProcessView(Context context, GLConstant.GLProcessMode mode) {
		super(context);
		
		mContext = context;
		mGlProcessMode = mode;
		init();
	}
	
	/**
	 * 初始化
	 * @author linzanxian  @Date 2015年3月17日 上午10:55:41
	 * @return void
	 */
	public void init() {
		if (mGlProcessMode == GLConstant.GLProcessMode.RECTANGLE) {
			super.setBackground(mColorBackgroup);
		}
	}

	/**
	 * 设置外边距
	 * @author linzanxian  @Date 2015年3月17日 上午10:09:16
	 * @param left 左  
	 * @param top 上
	 * @param right 右  
	 * @param bottom 下  
	 * @return void
	 */
	@Override
	public void setMargin(float left, float top, float right, float bottom) {
		//修改view
		setPosition(this, left, top, right, bottom);
		if (mImageView != null) {
			mImageView.setX(getX());
			mImageView.setY(getY());
		}
				
		super.setMargin(left, top, right, bottom);
	}
	
	/**
	 * 设置相对布局的位置
	 * @author linzanxian  @Date 2015年3月12日 上午10:45:07
	 * @param mView GLRectView
	 * @param left 左  
	 * @param top 上
	 * @param right 右  
	 * @param bottom 下   
	 * @return void
	 */
	private void setPosition(GLRectView view, float left, float top, float right, float bottom) {
		if (view != null) {
			if (view.getAlign() == null
					|| view.getAlign().equals(GLConstant.GLAlign.DEFAULT)
					|| view.getAlign().equals(GLConstant.GLAlign.LEFT)
					|| view.getAlign().equals(GLConstant.GLAlign.TOP)) { //left, top
				view.setX(view.getX() + left - this.getMarginLeft());
				view.setY(view.getY() + top - this.getMarginTop());
			} else if(view.getAlign().equals(GLConstant.GLAlign.RIGHT)) { //top, right
				view.setX(view.getX() - right + this.getMarginRight());
				view.setY(view.getY() + top - this.getMarginTop());
			} else if(view.getAlign().equals(GLConstant.GLAlign.BOTTOM)) { //left, bottom
				view.setX(view.getX() + left - this.getMarginLeft());
				view.setY(view.getY() - bottom + this.getMarginBottom());
			} else if(view.getAlign().equals(GLConstant.GLAlign.CENTER_VERTICAL)) {
				view.setX(view.getX() + left - this.getMarginLeft());
			} else if(view.getAlign().equals(GLConstant.GLAlign.CENTER_HORIZONTAL)) {
				view.setY(view.getY() + top - this.getMarginTop());
			}
		}
	}

//	/**
//	 * 设置大小
//	 * @author linzanxian  @Date 2015年3月17日 上午10:12:49
//	 * @param width 宽
//	 * @param height 高
//	 * @return void
//	 */
//	public void setLayoutParams(float width, float height) {
//		setLayoutParams(0, 0, width, height);
//	}

	/**
	 * 设置进度颜色
	 * @author linzanxian  @Date 2015年3月17日 下午2:26:08
	 * @param resId 资源Id
	 * @return void
	 */
	public void setProcessColor(int resId) {
		if( null == mImageView) {
			mImageView = new GLImageView(mContext);
		}
		mImageView.setBackground(resId);

		setLayoutParams();
	}
	
	/**
	 * 设置进度颜色
	 * @author linzanxian  @Date 2015年3月17日 下午2:26:08
	 * @param bitmap Bitmap
	 * @return void
	 */
	public void setProcessColor(Bitmap bitmap) {
		if( null == mImageView) {
			mImageView = new GLImageView(mContext);
		}
		mImageView.setBackground(bitmap);
		
		setLayoutParams();
	}

	/**
	 * 设置进度颜色
	 * @author linzanxian  @Date 2015年3月17日 下午2:26:08
	 * @param color 颜色
	 * @return void
	 */
	public void setProcessColor(GLColor color) {
		if( null == mImageView ) {
			mImageView = new GLImageView(mContext);
		}
		mImageView.setBackground(color);

		setLayoutParams();
	}
	
	/**
	 * 更新进度
	 * @author linzanxian  @Date 2015年3月17日 下午2:25:38
	 * @param process 进度 0 - 100
	 * @return void
	 */
	public void setProcess(float process) {
		if (process < 0 || process > 100) {
			return;
		}
		if (mImageView == null) {
			setProcessColor(mColorProcess);
		}

		mImageView.setX(getX()+getPaddingLeft());
		mImageView.setY(getY()+getPaddingTop());

		mImageView.setLayoutParams((getWidth() - getPaddingLeft() - getPaddingRight()) / 100 * process,
				getHeight() - getPaddingTop() - getPaddingBottom());

		return;
	}
	
	public void setProcess(int process) {
		setProcess((float)process);
	}
	
	/**
	 * 绘制初始化
	 * @author linzanxian  @Date 2015年3月10日 下午12:04:21
	 * @return void
	 */
	@Override
	public void initDraw() {
		super.initDraw();
	}

	@Override
	public void setLayoutParams(float width, float height){
		super.setLayoutParams(width, height);
		if( null == mImageView ) {
			mImageView = new GLImageView(mContext);
		}
		mImageView.setImgWH(width, height);
	}
	/**
	 * 设置属性
	 * @author linzanxian  @Date 2015年3月17日 下午2:35:36
	 * @return void
	 */
	private void setLayoutParams() {
		if (mImageView == null) {
			return;
		}

		this.addView(mImageView);
		
		if (mGlProcessMode == GLConstant.GLProcessMode.CIRCLE) {
			mImageView.setX(getX());
			mImageView.setY(getY());
			mImageView.setLayoutParams(getWidth(), getHeight());
			if (!mRotate) {
				mRotate = true;
				rotate();
			}
		}

	}
	
	/**
	 * 旋转
	 * @author linzanxian  @Date 2015年3月18日 下午1:58:09
	 * @return void
	 */
	private void rotate() {
		
		new Thread(new Runnable() {  
            
            @Override  
            public void run() {  
            	while (mRotate) {
            		try {  
                        Thread.sleep(mSpeed);  
                    } catch (InterruptedException e) {  
                    	e.printStackTrace();
                    }  
            		
            		mImageView.rotate(-mAngle, 0f, 0f, 1f);
				}
            }  
        }).start();
	}
	
	/**
	 * 设置转动速度.默认100，越小越快
	 * @author linzanxian  @Date 2015年3月18日 下午1:59:04
	 * @param speed 速度
	 * @return void
	 */
	public void setSpeed(int speed) {
		mSpeed = speed;
	}
	
//	public float getProcess() {
//		return mProcess;
//	}
	
	public void removeRotate() {
		mRotate = false;
	}

	public float getAngle() {
		return mAngle;
	}
	
	/**
	 * 每帧选中的角度
	 * @author lixianke  @Date 2015-7-6 下午5:40:18
	 * @param mAngle 角度 
	 * @return
	 */
	public void setAngle(float angle) {
		this.mAngle = angle;
	}
}
