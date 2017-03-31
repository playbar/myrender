package com.bfmj.viewcore.view;

import com.bfmj.viewcore.render.GLColor;

import android.content.Context;
import android.graphics.Typeface;

import com.bfmj.viewcore.render.GLConstant;

/**
 * 圆形进度控制
 * ClassName: GLProcessCircleView <br/>
 * @author linzanxian    
 * @date: 2015-6-30 上午9:37:22 <br/>  
 * description:强制条件，接收两个半圆，默认逆时针转动(可设置)，左半圆为进度，右半圆为背景
 */
public class GLProcessCircleView extends GLRelativeView {
	private Context mContext;
	private GLConstant.GLDirection mDirection = GLConstant.GLDirection.ANTICLOCKWISE; //默认为逆时针
	
	private int mProcessResId = 0;
	private int mBgResId = 0;
	private float mDiameter = 60; //直径
	private float mAngle = 0;
	
	private int mTextSize = 20;
	private GLColor mTextColor = new GLColor(1.0f, 1.0f, 1.0f);
	private boolean mIsBold = false;
	
	//private GLRelativeView this;;
	
	public GLProcessCircleView(Context context) {
		super(context);
		
		this.setLayoutParams(mDiameter+4, mDiameter+4);
		mContext = context;
	}

	/**
	 * 设置旋转方向
	 * @author linzanxian  @Date 2015-6-30 上午10:12:23
	 * @param direction 旋转方向(顺或逆时针)
	 * @return void
	 */
	public void setDirection(GLConstant.GLDirection direction) {
		mDirection = direction;
	}
	
	/**
	 * 设置属性
	 * @author linzanxian  @Date 2015-6-30 上午10:16:08
	 * @param processResId 进度资源
	 * @param bgResId 背景资源
	 * @param diameter 圆的直径 
	 * @return void
	 */
	public void setAttribute(int processResId, int bgResId, float diameter) {
		mProcessResId = processResId;
		mBgResId = bgResId;
		mDiameter = diameter;
	}
	
	/**
	 * 设置文本属性
	 * @author linzanxian  @Date 2015-6-30 下午2:30:51
	 * @param textSize 文本字体大小 
	 * @param color 文本颜色  
	 * @param isBold 是否加粗  
	 * @return void
	 */
	public void setTextAttribute(int textSize, GLColor color, boolean isBold) {
		mTextSize = textSize;
		if (color != null) {
			mTextColor = color;
		}
		mIsBold = isBold;
	}
	
	/**
	 * 创建进度
	 * @author linzanxian  @Date 2015-6-30 上午10:20:37
	 * @return void
	 */
	public void createView() {
		if (mProcessResId <= 0 || mBgResId <= 0 || mDiameter <= 0) {
			return;
		}
		
		if (mDirection == GLConstant.GLDirection.ANTICLOCKWISE) {
			GLImageView leftImageView = new GLImageView(mContext);
			leftImageView.setLayoutParams( mDiameter, mDiameter);
			leftImageView.setCutting(false);
			leftImageView.setImage(mProcessResId);
			leftImageView.rotate(180, 0, 0, 1);
			leftImageView.setId("1");
			
			GLImageView leftImageView2 = new GLImageView(mContext);
			leftImageView2.setLayoutParams(mDiameter, mDiameter);
			leftImageView2.setCutting(false);
			leftImageView2.setImage(mBgResId);
			leftImageView2.rotate(180, 0, 0, 1);
			leftImageView2.setId("2");
			
			GLImageView rightImageView = new GLImageView(mContext);
			rightImageView.setLayoutParams( mDiameter, mDiameter);
			rightImageView.setCutting(false);
			rightImageView.setImage(mBgResId);
			rightImageView.setId("3");

			
			this.addView(leftImageView2);
			this.addView(leftImageView);
			this.addView(rightImageView);
		} else {
			GLImageView leftImageView = new GLImageView(mContext);
			leftImageView.setLayoutParams(mDiameter, mDiameter);
			leftImageView.setCutting(false);
			leftImageView.setImage(mProcessResId);
			leftImageView.setId("1");
			
			GLImageView leftImageView2 = new GLImageView(mContext);
			leftImageView2.setLayoutParams( mDiameter, mDiameter);
			leftImageView2.setCutting(false);
			leftImageView2.setImage(mBgResId);
			leftImageView2.rotate(180, 0, 0, 1);
			leftImageView2.setId("2");
			
			GLImageView rightImageView = new GLImageView(mContext);
			rightImageView.setLayoutParams( mDiameter, mDiameter);
			rightImageView.setCutting(false);
			rightImageView.setImage(mBgResId);
			rightImageView.setId("3");

			
			this.addView(rightImageView);
			this.addView(leftImageView);
			this.addView(leftImageView2);
		}
		
		//文本
		GLTextView textView = new GLTextView(mContext);
		textView.setLayoutParams(mTextSize*3, mTextSize);
		//textView.setBackground(new GLColor(0.2f,0.2f,0.2f));
		textView.setTextSize(mTextSize);
		textView.setTextColor(mTextColor);
		if (mIsBold) {
			textView.setStyle(Typeface.BOLD);
		}
		textView.setAlignment(GLTextView.ALIGN_CENTER);
		textView.setId("text");
		
		this.addViewCenter(textView);
		this.setDepth(this.getDepth()-0.001f);
	}
	
	/**
	 * 设置进度
	 * @author linzanxian  @Date 2015-6-30 下午2:51:08
	 * @param process 进度  
	 * @return void
	 */
	public void setProcess(int process) {
		setProcess((float)process);
	}
	
	public void setProcess(float process) {
		if(this == null) {
			return;
		}
		GLImageView imageView1 = (GLImageView) this.getView("1");
		if(imageView1 == null) {
			return;
		}
		GLTextView textView = (GLTextView) this.getView("text");
		textView.setText((int)process+"%");
		
		if (mDirection == GLConstant.GLDirection.ANTICLOCKWISE) {
			if (process <= 50) {
				float angle = process/100*360;
				imageView1.rotate(angle-mAngle, 0, 0, 1);
				mAngle = angle;
			} else {
				GLImageView imageView2 = (GLImageView) this.getView("2");
				if(imageView2 == null) {
					return;
				}
				
				GLImageView imageView3 = (GLImageView) this.getView("3");
				if(imageView3 == null) {
					return;
				}
				
				//初始化大小process>50
				if (mAngle == 0) {
					mAngle = 180;
					imageView1.rotate(180, 0, 0, 1);
				}
				
				if (mAngle <= 180) {
					imageView1.rotate(180-mAngle, 0, 0, 1);
					mAngle = 180;
					
					imageView3.setDepth(imageView1.getDepth()+0.0005f);
					
					imageView2.setImage(mProcessResId);
					imageView2.rotate(180, 0, 0, 1);
				}
				
				
				float angle = process/100*360;
				imageView2.rotate(angle-mAngle, 0, 0, 1);
				mAngle = angle;
			}
			
		} else {
			if (process <= 50) {
				float angle = process/100*360;
				imageView1.rotate(-(angle-mAngle), 0, 0, 1);
				mAngle = angle;
			} else {
				GLImageView imageView2 = (GLImageView) this.getView("2");
				if(imageView2 == null) {
					return;
				}
				
				GLImageView imageView3 = (GLImageView) this.getView("3");
				if(imageView3 == null) {
					return;
				}
				
				//初始化大小process>50
				if (mAngle == 0) {
					mAngle = 180;
					imageView1.rotate(180, 0, 0, 1);
				}
				
				if (mAngle <= 180) {
					imageView1.rotate(-(180-mAngle), 0, 0, 1);
					mAngle = 180;
					
					imageView2.setDepth(imageView1.getDepth()+0.0005f);
					
					imageView3.setImage(mProcessResId);
					imageView3.rotate(180, 0, 0, 1);
				}
				
				
				float angle = process/100*360;
				imageView3.rotate(-(angle-mAngle), 0, 0, 1);
				mAngle = angle;
			}
		}
		
	}
}
