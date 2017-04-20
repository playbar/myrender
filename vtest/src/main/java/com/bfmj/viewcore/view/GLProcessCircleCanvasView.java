package com.bfmj.viewcore.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.RectF;

import com.bfmj.viewcore.render.GLConstant;

public class GLProcessCircleCanvasView extends GLImageView {
	private Context mContext;
	private float mWidth = 60;
	private float mHeight = 60;
	
	private int mProcessColor = Color.rgb(51, 154, 255);
	private int mBgColor = Color.WHITE;
	private float mStrokeWidth = 6; //进度圆环宽度
	
	//text
	private int mTextSize = 0;
	private int mTextColor = Color.rgb(255, 255, 255);
	private int mTextStrokeWidth = 6;
	
	
	private RectF mRectF;
	private Paint mPaint;
	private float mSweepAngle = -360;//默认为逆时针
	
	private boolean isShowText = true;
	

	public GLProcessCircleCanvasView(Context context) {
		super(context);
		mContext = context;
		
		mRectF = new RectF();
		mPaint = new Paint();
		
		this.setLayoutParams(mWidth, mHeight);
	}
	
	public void setLayoutParams(float width, float height) {
		mWidth = width;
		mHeight = height;
		super.setLayoutParams(width, height);
	}

	/**
	 * 设置旋转方向
	 * @author linzanxian  @Date 2015-6-30 上午10:12:23
	 * @param direction 旋转方向(顺或逆时针)
	 * @return void
	 */
	public void setDirection(GLConstant.GLDirection direction) {
		if (direction == GLConstant.GLDirection.ANTICLOCKWISE) {
			mSweepAngle = -360;
		} else {
			mSweepAngle = 360;
		}
	}
	
	/**
	 * 设置进度颜色
	 * @author linzanxian  @Date 2015-7-13 上午10:34:33
	 * @param mProcessColor 进度颜色
	 * @param bgColor 背景颜色
	 * @return void
	 */
	public void setAttribute(int processColor, int bgColor, float strokeWidth) {
		if (processColor != 0) {
			mProcessColor = processColor;
		}
		if (bgColor > -1) {
			mBgColor = bgColor;
		}
		if (strokeWidth != 0) {
			mStrokeWidth = strokeWidth;
		}
	}

	
	/**
	 * 设置文本属性
	 * @author linzanxian  @Date 2015-6-30 下午2:30:51
	 * @param textSize 文本字体大小 
	 * @param color 文本颜色  
	 * @param isBold 是否加粗  
	 * @return void
	 */
	public void setTextAttribute(int textSize, int color, int textStrokeWidth) {
		if (textSize != 0) {
			mTextSize = textSize;
		}
		if (color != 0) {
			mTextColor = color;
		}
		if (textStrokeWidth != 0) {
			mTextStrokeWidth = textStrokeWidth;
		}
	}
	
	/**
	 * 创建view
	 * @author linzanxian  @Date 2015-7-13 上午10:44:55
	 * @param process 初始化进度  
	 * @return void
	 */
	public void createView(float process) {
		setProcess(process);
	}
	
	/**
	 * 创建进度
	 * @author linzanxian  @Date 2015-7-13 上午11:36:38
	 * @param process 进度  
	 * @return void
	 */
	public void setProcess(int process) {
		setProcess((float)process);
	}
	
	public void setProcess(float process) {
		if (mWidth != mHeight) {
			float min = Math.min(mWidth, mHeight);
			mWidth = min;
			mHeight = min;
		}
	
		Bitmap bitmap = Bitmap.createBitmap((int)mWidth, (int)mHeight, Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap);
		
		mPaint.setAntiAlias(true); // 设置画笔为抗锯齿
		mPaint.setColor(mBgColor); // 设置画笔颜色
		canvas.drawColor(Color.TRANSPARENT); // 白色背景
		mPaint.setStrokeWidth(mStrokeWidth); //线宽
		mPaint.setStyle(Style.STROKE);
 
		mRectF.left = mStrokeWidth / 2 + 2; // 左上角x
		mRectF.top = mStrokeWidth / 2 + 2; // 左上角y
		mRectF.right = mWidth - mStrokeWidth / 2 - 2; // 左下角x
		mRectF.bottom = mHeight - mStrokeWidth / 2 - 2; // 右下角y
 
		canvas.drawArc(mRectF, -90, mSweepAngle, false, mPaint); // 绘制白色圆圈，即进度条背景
		mPaint.setColor(mProcessColor);
		canvas.drawArc(mRectF, -90, ((float) process / 100) * mSweepAngle, false, mPaint); // 绘制进度圆弧，这里是蓝色
		
		if (isShowText) {
			mPaint.setColor(mTextColor);
			mPaint.setStrokeWidth(mTextStrokeWidth);
			//String text = (int)process + "%";
			String text = (int)process+"";
			float textHeight = mTextSize==0 ? mHeight / 4 : mTextSize;
			mPaint.setTextSize(textHeight);
			float textWidth = (int) mPaint.measureText(text, 0, text.length());
			mPaint.setStyle(Style.FILL);
			canvas.drawText(text, mWidth / 2 - textWidth / 2, mHeight / 2 + textHeight/2-5, mPaint);
		}
		
		this.setImage(bitmap);
		bitmap = null;
	}

	public boolean isShowText() {
		return isShowText;
	}

	public void setShowText(boolean isShowText) {
		this.isShowText = isShowText;
	}
}
