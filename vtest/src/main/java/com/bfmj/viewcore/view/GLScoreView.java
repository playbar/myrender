package com.bfmj.viewcore.view;

import android.content.Context;

/**
 * 评分图形化控件
 * ClassName: GLStarView <br/>
 * @author linzanxian    
 * @date: 2015-7-16 下午2:33:14 <br/>  
 * description:按满分10分设计
 */
public class GLScoreView extends GLLinearView {
	private Context mContext;
	private float mSpacing = 2f; //间距
	private float mSingleWidth = 30;
	private float mSingleHeight = 30;
	
	private int mStar1 = 0;
	private int mStar2 = 0;
	private int mStar3 = 0;
	
	public GLScoreView(Context context) {
		super(context);
		
		mContext = context;
	}
	
	/**
	 * 设置单个大小
	 * @author linzanxian  @Date 2015-7-16 下午2:23:08
	 * @param width 宽度
	 * @param height 高度
	 * @return void
	 */
	public void setSize(float width, float height) {
		mSingleWidth = width;
		mSingleHeight = height;
	}
	
	/**
	 * 设置间距
	 * @author linzanxian  @Date 2015-7-16 下午2:32:08
	 * @param spacing 间距  
	 * @return void
	 */
	public void setSpacing(float spacing) {
		mSpacing = spacing;
	}

	/**
	 * 设置图形
	 * @author linzanxian  @Date 2015-7-16 下午2:25:06
	 * @param star1 实心
	 * @param star2 半实心,如没有半实心则赋值star3
	 * @param star3 空心
	 * @return void
	 */
	public void setStar(int star1, int star2, int star3) {
		mStar1 = star1;
		mStar2 = star2;
		mStar3 = star3;
	}
	
	/**
	 * 设置评分
	 * @author linzanxian  @Date 2015-7-16 下午2:29:10
	 * @param Score 评分  
	 * @return void
	 */
	public void setScore(float Score) {
		if (mStar1 == 0 || mStar2 == 0 || mStar3 == 0) {
			return;
		}
		
		this.setLayoutParams(mSingleWidth*5+mSpacing*4, mSingleHeight);
		
		int num = 0;
		int full = (int) (Score/2);
		if (full > 5) {
			full = 5;
		}
		for (int i = 1; i <= full; i++) {
			GLImageView fullView = new GLImageView(mContext);
			fullView.setLayoutParams( mSingleWidth, mSingleHeight);
			fullView.setImage(mStar1);
			if (i != 1) {
				fullView.setMargin(mSpacing, 0, 0, 0);
			}
			
			this.addView(fullView);
			++num;
		}
		
		float leave = Score%2;
		if (leave >= 0.5f) {
			GLImageView halfView = new GLImageView(mContext);
			halfView.setLayoutParams(mSingleWidth, mSingleHeight);
			halfView.setImage(mStar2);
			halfView.setMargin(mSpacing, 0, 0, 0);
			
			this.addView(halfView);
			++num;
		}
		
		if (num < 5) {
			int total = 5 - num;
			for (int i = 0; i < total; i++) {
				GLImageView emptyView = new GLImageView(mContext);
				emptyView.setLayoutParams(mSingleWidth, mSingleHeight);
				emptyView.setImage(mStar3);
				emptyView.setMargin(mSpacing, 0, 0, 0);
				
				this.addView(emptyView);
			}
		}
	}
}
