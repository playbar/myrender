package com.bfmj.viewcore.view;


import java.util.ArrayList;
import java.util.List;

import android.content.Context;

import com.bfmj.viewcore.render.GLConstant;

/**
 * RelativeLayout
 * ClassName: GLRelativeView <br/>
 * @author linzanxian    
 * @date: 2015年3月9日 上午10:47:38 <br/>  
 * description:相对布局
 */
public class GLRelativeView extends GLGroupView {
	private List<ArrayList<GLRectView>> mRelativeViewList = new ArrayList<ArrayList<GLRectView>>();
	private boolean mIsScale = false;
	
	public GLRelativeView(Context context) {
		super(context);
	}
	
	/**
	 * 设置padding
	 * @author linzanxian  @Date 2015年3月11日 下午5:13:31
	 * @param left 左  
	 * @param top 上
	 * @param right 右  
	 * @param bottom 下  
	 * @return void
	 */
	@Override
	public void setPadding(float left, float top, float right, float bottom) {
		if (mChildView != null) {
			for (GLRectView childView : mChildView) {
				setChildViewPadding(childView, left, top, right, bottom);
			}
		}
		
		super.setPadding(left, top, right, bottom);
	}

	/**
	 * 设置外边距与align互斥
	 * @author linzanxian  @Date 2015年3月10日 下午5:25:39
	 * @param left 左  
	 * @param top 上
	 * @param right 右  
	 * @param bottom 下  
	 * @return void
	 */
	@Override
	public void setMargin(float left, float top, float right, float bottom) {
		//修改view
		setViewMargin(left, top, right, bottom);
		
		super.setMargin(left, top, right, bottom);
	}

	/**
	 * 添加View
	 * @author linzanxian  @Date 2015年3月11日 下午4:53:57
	 * @param view GLRectView
	 * @param align 位置
	 * @return void
	 */
	public void addView(GLRectView view, GLConstant.GLAlign align) {
		if (align == GLConstant.GLAlign.TOP) {
			addViewTop(view);
		} else if (align == GLConstant.GLAlign.BOTTOM) {
			addViewBottom(view);
		} else if (align == GLConstant.GLAlign.LEFT) {
			addViewLeft(view);
		} else if (align == GLConstant.GLAlign.RIGHT) {
			addViewRight(view);
		} else if (align == GLConstant.GLAlign.LEFT_TOP) {
			addViewLeftTop(view);
		} else if (align == GLConstant.GLAlign.RIGHT_TOP) {
			addViewRightTop(view);
		} else if (align == GLConstant.GLAlign.LEFT_BOTTOM) {
			addViewLeftBottom(view);
		} else if (align == GLConstant.GLAlign.RIGHT_BOTTOM) {
			addViewRightBottom(view);
		} else if (align == GLConstant.GLAlign.CENTER) {
			addViewCenter(view);
		} else if (align == GLConstant.GLAlign.CENTER_HORIZONTAL) {
			addViewCenterHorizontal(view);
		} else if (align == GLConstant.GLAlign.CENTER_VERTICAL) {
			addViewCenterVertical(view);
		} else {
			addView(view);
		}
	}
	
	/**
	 * 添加View
	 * @author linzanxian  @Date 2015年3月11日 下午4:53:57
	 * @param view GLRectView
	 * @return void
	 */
	@Override
	public void addView(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.DEFAULT);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());

		//resetChildView(view);
		super.addView(view);
	}
	
	@Override
	public void addView(int index, GLRectView view) {
		view.setAlign(GLConstant.GLAlign.DEFAULT);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());
		
		super.addView(index, view);
	}

	/**
	 * 添加View到top
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewTop(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.TOP);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到Bottom
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewBottom(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.BOTTOM);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getHeight() - getPaddingBottom() - view.getHeight() - view.getMarginBottom());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到Left
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewLeft(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.LEFT);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());
		
		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到Right
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewRight(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.RIGHT);
		view.setX(getX() + getWidth() - getPaddingRight() - view.getWidth() - view.getMarginRight());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());
		
		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加view到左上角
	 * @author linzanxian  @Date 2015年3月13日 下午4:14:37
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewLeftTop(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.LEFT_TOP);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加view到右上角
	 * @author linzanxian  @Date 2015年3月13日 下午4:14:37
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewRightTop(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.RIGHT_TOP);
		view.setX(getX() + getWidth() - getPaddingRight() - view.getWidth() - view.getMarginRight());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加view到左下角
	 * @author linzanxian  @Date 2015年3月13日 下午4:14:37
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewLeftBottom(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.LEFT_BOTTOM);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + getHeight() - getPaddingBottom() - view.getHeight() - view.getMarginBottom());

		//resetChildView(view);
		super.addView(view);
	}

	/**
	 * 添加view到右下角
	 * @author linzanxian  @Date 2015年3月13日 下午4:14:37
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewRightBottom(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.RIGHT_BOTTOM);
		view.setX(getX() + getWidth() - getPaddingRight() - view.getWidth() - view.getMarginRight());
		view.setY(getY() + getHeight() - getPaddingBottom() - view.getHeight() - view.getMarginBottom());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到Center
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewCenter(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.CENTER);
		view.setX(getX() + (getWidth() - view.getWidth() - getPaddingLeft() - getPaddingRight())/2 + getPaddingLeft());
		view.setY(getY() + (getHeight() - view.getHeight() - getPaddingTop() - getPaddingBottom())/2 + getPaddingTop());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到CenterHorizontal
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewCenterHorizontal(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.CENTER_HORIZONTAL);
		view.setX(getX() + (getWidth() - view.getWidth() - getPaddingLeft() - getPaddingRight())/2 + getPaddingLeft());
		view.setY(getY() + getPaddingTop() + view.getMarginTop());
		
		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到CenterVertical
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewCenterVertical(GLRectView view) {
		view.setAlign(GLConstant.GLAlign.CENTER_VERTICAL);
		view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		view.setY(getY() + (getHeight() - view.getHeight() - getPaddingTop() - getPaddingBottom())/2 + getPaddingTop());
		
		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加view到相对位置
	 * @author linzanxian  @Date 2015年3月13日 下午4:11:27
	 * @param view GLRectView 要添加的view
	 * @param leftView GLRectView 相对左边view
	 * @param topView GLRectView 相对上面view
	 * @param rightView GLRectView 相对右边view
	 * @param bottomView GLRectView 相对下面iew
	 * @return void
	 */
	public void addViewOf(GLRectView view, GLRectView leftView, GLRectView topView, GLRectView rightView, GLRectView bottomView) {
		if (view == null) {
			return;
		}
		
		ArrayList<GLRectView> rectViews = new ArrayList<GLRectView>();
		rectViews.add(view);
		rectViews.add(leftView);
		rectViews.add(topView);
		rectViews.add(rightView);
		rectViews.add(bottomView);
		
		mRelativeViewList.add(rectViews);
		
		//设置坐标
		setViewOfPostion(view, leftView, topView, rightView, bottomView);
		
		//重置子元素
		//resetChildView(view);
		
		super.addView(view);
	}
	
	/**
	 * 
	 * @author linzanxian  @Date 2015年3月13日 下午5:06:24
	 * description:{这里用一句话描述这个方法的作用}
	 * @param {引入参数名} {引入参数说明}  
	 * @return {返回值说明}
	 */
	public void setViewOfPostion(GLRectView view, GLRectView leftView, GLRectView topView, GLRectView rightView, GLRectView bottomView) {
		if (view == null) {
			return;
		}
		float x = getX() + getPaddingLeft() + view.getMarginLeft();
		float y = getY() + getPaddingTop() + view.getMarginTop();
		
		if (mIsScale) {
			x = x - view.getWidth()/2*(1-view.getScaleX()) + getWidth()/2*(1-getScaleX());
			y = y - view.getHeight()/2*(1-view.getScaleY()) + getHeight()/2*(1-getScaleY());
		}
		view.setX(x);
		view.setY(y);
		
		//处理位置
		if (leftView != null) {
			rightView = null;
			x = leftView.getX() - view.getMarginRight() - view.getWidth();
			if (mIsScale) {
				x = x + leftView.getWidth()/2*(1-leftView.getScaleX());
			}
			
			view.setX(x);
		}
		if (topView != null) {
			bottomView = null;
			y = topView.getY() - view.getMarginBottom() - view.getHeight();
			if (mIsScale) {
				y = y + topView.getHeight()/2*(1-topView.getScaleY());
			}
			
			view.setY(y);
		}
		if (rightView != null) {
			x = rightView.getX() + rightView.getWidth() + rightView.getMarginRight() + view.getMarginLeft();
			
			if (mIsScale) {
				x = x - rightView.getWidth()/2*(1-rightView.getScaleX()) - view.getWidth()/2*(1-view.getScaleX());
				x = x - rightView.getMarginRight()*(1-rightView.getScaleX()) - view.getMarginLeft()*(1-view.getScaleX());
			}
			view.setX(x);
		}
		if (bottomView != null) {
			y = bottomView.getY() + bottomView.getHeight() + bottomView.getMarginBottom() + view.getMarginTop();
			
			if (mIsScale) {
				y = y - bottomView.getHeight()/2*(1-bottomView.getScaleY()) - view.getHeight()/2*(1-view.getScaleY());
				y = y - bottomView.getMarginBottom()*(1-bottomView.getScaleY()) - view.getMarginTop()*(1-view.getScaleY());
			}
			view.setY(y);
		}
	}
	
	/**
	 * 返回view列表
	 * @author linzanxian  @Date 2015年3月13日 下午4:44:56
	 * @return List<ArrayList<GLRectView>>
	 */
	public List<ArrayList<GLRectView>> getViewOfList() {
		return mRelativeViewList;
	}
	
	/**
	 * 设置是否缩放处理
	 * @author linzanxian  @Date 2015-7-3 上午9:54:43
	 * @param scale 是否缩放  
	 * @return void
	 */
	public void setIsScacle(boolean scale) {
		mIsScale = scale;
	}
}

