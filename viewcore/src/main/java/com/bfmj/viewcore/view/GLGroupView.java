package com.bfmj.viewcore.view;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import com.baofeng.mojing.input.base.MojingKeyCode;
import com.bfmj.viewcore.render.GLConstant;
import com.bfmj.viewcore.render.GLScreenParams;
import com.bfmj.viewcore.util.GLFocusUtils;

import android.content.Context;

/**
 * Group
 * ClassName: GLGroupView <br/>
 * @author linzanxian
 * @date: 2015年3月9日 上午10:46:33 <br/>  
 * description:
 */
public class GLGroupView extends GLRectView {
	protected ArrayList<GLRectView> mChildView = new ArrayList<GLRectView>();
	private GLFocusUtils mGlFocusUtils;
	protected float mX = 0;
	protected float mY = 0;
	protected static float NOXY = -10000; //默认参数
	private boolean isAutomaticFocus = false;
	private boolean isInitDraw = false;
	protected float mCosV = 0.0f;
	protected float mSinV = 0.0f;
	protected float mAngle = 0.0f;

	public GLGroupView(Context context) {
		super(context);
		init();
	}

	/**
	 * 初始化
	 * @author linzanxian  @Date 2015年3月16日 下午2:31:14
	 * description:初始化
	 * @return void
	 */
	private void init() {
		mGlFocusUtils = new GLFocusUtils();
	}

//	/**
//	 * 设置属性
//	 * @author linzanxian  @Date 2015年3月13日 下午2:55:28
//	 * description:设置属性 （宽、高）
//	 * @param width 宽
//	 * @param height 高
//	 * @return void
//	 */
//	public void setLayoutParams(float width, float height) {
//		setLayoutParams(0, 0, width, height);
//	}

	/**
	 * 添加view
	 * @author linzanxian  @Date 2015年3月10日 下午12:02:34
	 * description:添加view
	 * @param view GLRectView
	 * @return void
	 */
	public void addView(GLRectView view) {
		view.setParent(this);
		if (view.isFocusable()){
			setFocusable(true);
		}

		view.setFixed(isFixed());

		if (isInitDraw) {
			view.initDraw();
		}
		mChildView.add(view);
	}

	/**
	 * 添加view
	 * @author linzanxian  @Date 2015年3月10日 下午12:02:34
	 * description:添加view
	 * @param index 索引
	 * @param view GLRectView
	 * @return void
	 */
	public void addView(int index, GLRectView view) {
		view.setParent(this);
		if (view.isFocusable()){
			setFocusable(true);
		}

		view.setFixed(isFixed());

		if (isInitDraw) {
			view.initDraw();
		}

		if (index > mChildView.size()) {
			index = mChildView.size() - 1;
		}

		if (index < 0) {
			index = 0;
		}

		mChildView.add(index, view);
	}

	/**
	 * 删除view
	 * @author linzanxian  @Date 2015年3月10日 下午12:02:34
	 * description:删除view
	 * @param view GLRectView
	 * @return void
	 */
	public void removeView(GLRectView view) {
		mChildView.remove(view);
		view.release();
		view = null;
	}

	/**
	 * 删除view
	 * @author linzanxian  @Date 2015年3月10日 下午12:02:34
	 * description:删除view
	 * @param index 索引
	 * @return void
	 */
	public void removeView(int index) {
		if (index >= mChildView.size() || index<0) {
			return;
		}
		GLRectView view = mChildView.get(index);
		mChildView.remove(index);
		view.release();
		view = null;

	}

	/**
	 * 删除所有的子view
	 * @author linzanxian  @Date 2015年4月13日 上午11:36:07
	 * description:删除所有的子view
	 * @return void
	 */
	public void removeAllView() {
		if (mChildView != null && mChildView.size()>0) {
			int size = mChildView.size();
			for (int i = 0; i < size; i++) {
				removeView(mChildView.get(0));
			}

			mChildView.clear();
		}
	}

	/**
	 * 获取全部子view
	 * @author linzanxian  @Date 2015年3月17日 下午5:22:34
	 * description:获取全部子view
	 * @return ArrayList<GLRectView>
	 */
	public ArrayList<GLRectView> getView() {
		return mChildView;
	}

	/**
	 * 根据索引获取view
	 * @author linzanxian  @Date 2015年3月17日 下午5:20:40
	 * description:获取view
	 * @param index 索引
	 * @return GLRectView
	 */
	public GLRectView getView(int index) {
		if (mChildView == null || index >= mChildView.size()) {
			return null;
		}

		int size = mChildView.size();
		for (int i=0; i<size; i++) {
			if (index == i) {
				return mChildView.get(i);
			}
		}

		return null;
	}

	/**
	 * 根据Id获取view
	 * @author linzanxian  @Date 2015年3月17日 下午5:39:41
	 * description:根据Id获取view
	 * @param id ID字符串
	 * @return GLRectView
	 */
	public GLRectView getView(String id) {
		if (mChildView == null) {
			return null;
		}

		int size = mChildView.size();
		for (int i=0; i<size; i++) {
			String gId = mChildView.get(i).getId();
			if (gId!=null && gId.equals(id)) {
				return mChildView.get(i);
			}
		}

		return null;
	}

	/**
	 * 绘制初始化
	 * @author linzanxian  @Date 2015年3月10日 下午12:04:21
	 * description:绘制初始化
	 * @return void
	 */
	@Override
	public void initDraw() {
		isInitDraw = true;
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).initDraw();
		}

		super.initDraw();
	}

	@Override
	public void onResume(){
		for (GLRectView view : mChildView) {
			view.onResume();
		}

		super.onResume();
	}

	@Override
	public void onPause(){
		for (GLRectView view : mChildView) {
			view.onPause();
		}

		super.onPause();
	}

	@Override
	public void onSurfaceCreated() {

		for (GLRectView view : mChildView) {
			view.onSurfaceCreated();
		}

		super.onSurfaceCreated();
	}

	@Override
	public void onSurfaceChanged(int width, int height) {

		for (GLRectView view : mChildView) {
			view.onSurfaceChanged(width, height);
		}

		super.onSurfaceChanged(width, height);
	}


	/**
	 * 改变同级view
	 * @author linzanxian  @Date 2015年3月11日 下午6:29:54
	 * description:改变同级子元素
	 * @param left 左
	 * @param top 上
	 * @param right 右
	 * @param bottom 下
	 * @return void
	 */
	protected void setViewMargin(float left, float top, float right, float bottom) {
		if (getParent() != null) {
			if (getParent() instanceof GLLinearView) { //线性布局时
				ArrayList<GLRectView> mView = getParent().mChildView;
				boolean isChange = false;

				int size = mView.size();
				for (int i = 0; i < size; i++) {
					GLRectView currView = mView.get(i);
					if (this.equals(currView)) {
						isChange = true;
					}

					//重置受影响的子元素
					if (isChange) {
						setPosition(this, left, top, right, bottom);
						//setChildViewMargin(currView, left, top, right, bottom);
					}
				}
			} else if(getParent() instanceof GLRelativeView
					|| getParent() instanceof GLAbsoluteView) { //相对布局、绝对布局
				//父级是相对布局，只改变当前view, 同级view不用改变
				setPosition(this, left, top, right, bottom);

				//改变子布局或元素
				//setChildViewMargin(this, left, top, right, bottom);
			}
		} else { //最上层
			setPosition(this, left, top, right, bottom);

			//setChildViewMargin(this, left, top, right, bottom);
		}
	}

	/**
	 * 改变子元素
	 * @author linzanxian  @Date 2015年3月11日 下午6:20:43
	 * description:改变子元素, 添加其它布局在此处理
	 * @param mView GLRectView
	 * @param left 左
	 * @param top 上
	 * @param right 右
	 * @param bottom 下
	 * @return void
	 */
	protected void setChildViewMargin(GLRectView mView, float left, float top, float right, float bottom) {
		if (mView != null) {

			GLRelativeView relativeView = null;
			ArrayList<GLRectView> cViews = null;
			if (mView instanceof GLLinearView) { //线性布局
				GLLinearView linearView = (GLLinearView) mView;
				cViews = linearView.mChildView;
			} else if(mView instanceof GLRelativeView) { //相对布局
				relativeView = (GLRelativeView) mView;
				cViews = relativeView.mChildView;
			} else if(mView instanceof GLAbsoluteView) { //绝对布局
				GLAbsoluteView absoluteView = (GLAbsoluteView) mView;
				cViews = absoluteView.mChildView;
			}

			if (cViews != null && cViews.size() > 0) {
				for (GLRectView childView : cViews) {
					boolean isOf = false;
					if (relativeView != null) { //相对布局，处理of情况
						List<ArrayList<GLRectView>> mRelativeViewList = relativeView.getViewOfList();
						for (ArrayList<GLRectView> relativeViews : mRelativeViewList) {
							if (relativeViews.get(0).equals(childView)) {
								isOf = true;
								relativeView.setViewOfPostion(childView, relativeViews.get(1), relativeViews.get(2), relativeViews.get(3), relativeViews.get(4));
								//setChildViewMargin(childView, left, top, right, bottom);
								break;
							}
						}
					}

					if (!isOf) {
						setPosition(childView, left, top, right, bottom);
						//setChildViewMargin(childView, left, top, right, bottom);
					}
				}
			}
		}

	}

	/**
	 * 设置相对布局的位置
	 * @author linzanxian  @Date 2015年3月12日 上午10:45:07
	 * description:设置相对布局的位置
	 * @param mView GLRectView
	 * @param left 左
	 * @param top 上
	 * @param right 右
	 * @param bottom 下
	 * @return void
	 */
	private void setPosition(GLRectView view, float left, float top, float right, float bottom) {
		if (view != null) {
			if (view.getAlign() == null
					|| view.getAlign().equals(GLConstant.GLAlign.DEFAULT)
					|| view.getAlign().equals(GLConstant.GLAlign.LEFT)
					|| view.getAlign().equals(GLConstant.GLAlign.TOP)
					|| view.getAlign().equals(GLConstant.GLAlign.CENTER_HORIZONTAL)
					|| view.getAlign().equals(GLConstant.GLAlign.CENTER_VERTICAL)
					|| view.getAlign().equals(GLConstant.GLAlign.CENTER)) { //left, top
				view.setX(view.getX() + left - this.getMarginLeft());
				view.setY(view.getY() + top - this.getMarginTop());
			} else if(view.getAlign().equals(GLConstant.GLAlign.RIGHT)) { //top, right
				view.setX(view.getX() - right + this.getMarginRight());
				view.setY(view.getY() + top - this.getMarginTop());
			} else if(view.getAlign().equals(GLConstant.GLAlign.BOTTOM)) { //left, bottom
				view.setX(view.getX() + left - this.getMarginLeft());
				view.setY(view.getY() - bottom + this.getMarginBottom());
			} 
			/*
			else if(view.getAlign().equals(GLAlign.CENTER_HORIZONTAL)) {
				view.setX(view.getX() + (left - this.getMarginLeft())/2);
				view.setY(view.getY() + top - this.getMarginTop());
			} else if(view.getAlign().equals(GLAlign.CENTER_VERTICAL)) {
				view.setX(view.getX() + left - this.getMarginLeft());
				view.setY(view.getY() + (top - this.getMarginTop())/2);
			} else if(view.getAlign().equals(GLAlign.CENTER)) {
				view.setX(view.getX() + (left - this.getMarginLeft())/2);
				view.setY(view.getY() + (top - this.getMarginTop())/2);
			}*/
		}
	}

	/**
	 * 重置子元素
	 * @author linzanxian  @Date 2015年3月12日 下午5:25:05
	 * description:重置子元素
	 * @param view GLRectView 视图
	 * @return void
	 */
	protected void resetChildView(GLRectView view) {
		if (view == null) {
			return;
		}

		try {
			if (view instanceof GLLinearView) {
				GLLinearView linearView = (GLLinearView) view;
				ArrayList<GLRectView> childViews = linearView.mChildView;

				float maxX = view.getX() + view.getPaddingLeft();
				float maxY = view.getY() + view.getPaddingTop();
				int size = childViews.size();
				if (size > 0) {
					for (int i = 0; i < size; i++) {
						GLRectView childView = childViews.get(i);
						if (childView != null) {
							if (childView.getAlign() == null
									|| childView.getAlign().equals(GLConstant.GLAlign.DEFAULT)) {
								childView.setX(maxX + childView.getMarginLeft() + childView.getmIncrementX());
								childView.setY(maxY + childView.getMarginTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER)) {
								childView.setX(view.getX() + (view.getWidth() - view.getPaddingLeft() - view.getPaddingRight() - childView.getWidth())/2 + view.getPaddingLeft());
								childView.setY(view.getY() + (view.getHeight() - view.getPaddingTop() - view.getPaddingBottom() - childView.getHeight())/2 + view.getPaddingTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER_HORIZONTAL)) {
								childView.setX(view.getX() + (view.getWidth() - view.getPaddingLeft() - view.getPaddingRight() - childView.getWidth())/2 + view.getPaddingLeft());
								childView.setY(maxY + childView.getMarginTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER_VERTICAL)) {
								childView.setX(maxX + childView.getMarginLeft() - childView.getmIncrementX());
								childView.setY(view.getY() + (view.getHeight() - view.getPaddingTop() - view.getPaddingBottom() - childView.getHeight())/2 + view.getPaddingTop());
							}
						}

						if (linearView.getOrientation().equals(GLConstant.GLOrientation.HORIZONTAL)) {
							if (i == 0) {
								maxX = childView.getX() + childView.getWidth() + childView.getMarginRight() - childView.getmIncrementX();
							} else {
								if (maxX < (childView.getX() + childView.getWidth() + childView.getMarginRight())) {
									maxX = childView.getX() + childView.getWidth() + childView.getMarginRight();
								}
							}
						} else {
							if (i == 0) {
								maxY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
							} else {
								if (maxY < (childView.getY() + childView.getHeight() + childView.getMarginBottom())) {
									maxY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
								}
							}
						}
//						childView.translateX( childView.getmIncrementX());
					}
				}
			} else if(view instanceof GLRelativeView) {
				GLRelativeView relativeView = (GLRelativeView) view;
				ArrayList<GLRectView> childViews = relativeView.mChildView;

				int size = childViews.size();
				if (size > 0) {
					parent : for (int i = 0; i < size; i++) {
						GLRectView childView = childViews.get(i);
						if (childView != null) {
							//相对布局，处理of情况
							List<ArrayList<GLRectView>> mRelativeViewList = relativeView.getViewOfList();
							for (ArrayList<GLRectView> relativeViews : mRelativeViewList) {
								if (relativeViews.get(0).equals(childView)) {
									relativeView.setViewOfPostion(childView, relativeViews.get(1), relativeViews.get(2), relativeViews.get(3), relativeViews.get(4));
									continue parent;
								}
							}

							if (childView.getAlign() == null
									|| childView.getAlign().equals(GLConstant.GLAlign.DEFAULT)
									|| childView.getAlign().equals(GLConstant.GLAlign.LEFT)
									|| childView.getAlign().equals(GLConstant.GLAlign.TOP)) {
								childView.setX(view.getX() + view.getPaddingLeft() + childView.getMarginLeft());
								childView.setY(view.getY() + view.getPaddingTop() + childView.getMarginTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.RIGHT) || childView.getAlign().equals(GLConstant.GLAlign.RIGHT_TOP)) {
								childView.setX(view.getX() + (view.getWidth() - view.getPaddingRight() - childView.getWidth() - childView.getMarginRight()));
								childView.setY(view.getY() + view.getPaddingTop() + childView.getMarginTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.BOTTOM)) {
								childView.setX(view.getX() + view.getPaddingLeft() + childView.getMarginLeft());
								childView.setY(view.getY() + (view.getHeight() - view.getPaddingBottom() - childView.getHeight() - childView.getMarginBottom()));
							} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER_HORIZONTAL)) {
								childView.setX(view.getX() + (view.getWidth() - view.getPaddingLeft() - view.getPaddingRight() - childView.getWidth())/2 + view.getPaddingLeft());
								childView.setY(view.getY() + view.getPaddingTop() + childView.getMarginTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER_VERTICAL)) {
								childView.setX(view.getX() + view.getPaddingLeft() + childView.getMarginLeft());
								childView.setY(view.getY() + (view.getHeight() - view.getPaddingTop() - view.getPaddingBottom() - childView.getHeight())/2 + view.getPaddingTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER)) {
								childView.setX(view.getX() + (view.getWidth() - view.getPaddingLeft() - view.getPaddingRight() - childView.getWidth())/2 + view.getPaddingLeft());
								childView.setY(view.getY() + (view.getHeight() - view.getPaddingTop() - view.getPaddingBottom() - childView.getHeight())/2 + view.getPaddingTop());
							} else if(childView.getAlign().equals(GLConstant.GLAlign.RIGHT_BOTTOM)) {
								childView.setX(view.getX() + (view.getWidth() - view.getPaddingRight() - childView.getWidth() - childView.getMarginRight()));
								childView.setY(view.getY() + (view.getHeight() - view.getPaddingBottom() - childView.getHeight() - childView.getMarginBottom()));
							}

							childView.translateX( childView.getmIncrementX());
							//resetChildView(childView);
						}
					}
				}
			} else if(view instanceof GLAbsoluteView) {
				GLAbsoluteView absuluteView = (GLAbsoluteView) view;
				ArrayList<GLRectView> childViews = absuluteView.mChildView;

				int size = childViews.size();
				if (size > 0) {
					for (int i = 0; i < size; i++) {
						GLRectView childView = childViews.get(i);
						if (childView != null) {
							childView.setX(view.getX() + view.getPaddingLeft() + childView.getMarginLeft());
							childView.setY(view.getY() + view.getPaddingTop() + childView.getMarginTop());
							childView.translateX( childView.getmIncrementX());
						}

						//resetChildView(childView);
					}
				}
			} else if(view instanceof GLListView) {
				GLListView listView = (GLListView) view;
				ArrayList<GLRectView> childViews = listView.mChildView;

				int size = childViews.size();
				if (size > 0) {
					for (int i = 0; i < size; i++) {
						GLRectView childView = childViews.get(i);
						if (childView != null) {
							childView.setX(childView.getX() + view.getX() - listView.mX);
							childView.setY(childView.getY() + view.getY() - listView.mY);
							childView.translateX( childView.getmIncrementX());
						}

						//resetChildView(childView);
					}
				}
			} else if(view instanceof GLGridView) {
				GLGridView gridView = (GLGridView) view;
				ArrayList<GLRectView> childViews = gridView.mChildView;

				int size = childViews.size();
				if (size > 0) {
					for (int i = 0; i < size; i++) {
						GLRectView childView = childViews.get(i);
						if (childView != null) {
							childView.setX(childView.getX() + view.getX() - gridView.mX);
							childView.setY(childView.getY() + view.getY() - gridView.mY);
							childView.translateX( childView.getmIncrementX());
						}

						//resetChildView(childView);
					}
				}
			} else if(view instanceof GLProcessView) {
				GLProcessView listView = (GLProcessView) view;
				ArrayList<GLRectView> childViews = listView.mChildView;

				int size = childViews.size();
				if (size > 0) {
					for (int i = 0; i < size; i++) {
						GLRectView childView = childViews.get(i);
						if (childView != null) {
							childView.setX(view.getX());
							childView.setY(view.getY());
						}

						//resetChildView(childView);
					}
				}
			}
		} catch (Exception e) {

		}
	}

	/**
	 * 设置子view内距
	 * @author linzanxian  @Date 2015年3月13日 下午1:47:06
	 * description:设置子view内距
	 * @param mView GLRectView
	 * @param left 左
	 * @param top 上
	 * @param right 右
	 * @param bottom 下
	 * @return void
	 */
	protected void setChildViewPadding(GLRectView childView, float left, float top, float right, float bottom) {
		if (childView.getAlign() == null
				|| childView.getAlign().equals(GLConstant.GLAlign.DEFAULT)
				|| childView.getAlign().equals(GLConstant.GLAlign.TOP)
				|| childView.getAlign().equals(GLConstant.GLAlign.LEFT)) {
			childView.setX(childView.getX() + left - getPaddingLeft());
			childView.setY(childView.getY() + top - getPaddingTop());
		} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER)) {
			childView.setX(childView.getX() + (left - getPaddingLeft())/2 - (right - getPaddingRight())/2);
			childView.setY(childView.getY() + (top - getPaddingTop())/2 - (bottom - getPaddingBottom())/2);
		} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER_HORIZONTAL)) {
			childView.setX(childView.getX() + (left - getPaddingLeft())/2 - (right - getPaddingRight())/2);
			childView.setY(childView.getY() + top - getPaddingTop());
		} else if(childView.getAlign().equals(GLConstant.GLAlign.CENTER_VERTICAL)) {
			childView.setX(childView.getX() + left - getPaddingLeft());
			childView.setY(childView.getY() + (top - getPaddingTop())/2 - (bottom - getPaddingBottom())/2);
		} else if(childView.getAlign().equals(GLConstant.GLAlign.BOTTOM)) {
			childView.setX(childView.getX() + left - getPaddingLeft());
			childView.setY(childView.getY() + getPaddingBottom() - bottom);
		} else if(childView.getAlign().equals(GLConstant.GLAlign.RIGHT)) {
			childView.setX(childView.getX() + getPaddingRight() - right);
			childView.setY(childView.getY() + top - getPaddingTop());
		}

		//重置子view位置
		//resetChildView(childView);
	}

	/**
	 * 设置X坐标
	 * @author linzanxian  @Date 2015年3月19日 下午4:13:54
	 * description:设置X坐标
	 * @param float x X坐标
	 * @return void
	 */
	@Override
	public void setX(float x) {
		mX = this.getX();
		mY = this.getY();
		super.setX(x);

		resetChildView(this);
		//mX = 0;
	}

	public void setThisX(float x) {
		super.setX(x);
	}

	/**
	 * 设置Y坐标
	 * @author linzanxian  @Date 2015年3月19日 下午4:13:54
	 * description:设置Y坐标
	 * @param float y Y坐标
	 * @return void
	 */
	@Override
	public void setY(float y) {
		mX = this.getX();
		mY = this.getY();

		super.setY(y);

		resetChildView(this);
		//mY = 0;
	}

	public void setThisY(float y) {
		super.setY(y);
	}

	/**
	 * 设置深度
	 * @author linzanxian  @Date 2015年3月19日 下午4:41:16
	 * description:设置深度
	 * @param depth 深度
	 * @return void
	 */
	@Override
	public void setDepth(float depth) {
		if (this.mChildView != null && this.mChildView.size() > 0) {
			int size = this.mChildView.size();
			float changeDepth = depth - this.getDepth();

			for (int i = 0; i < size; i++) {
				GLRectView view = this.mChildView.get(i);
				view.setDepth(view.getDepth() + changeDepth );
//				view.translateZ(changeDepth);
//				view.translate(0, 0, view.getmIncrementDepth());
			}
		}

		super.setDepth(depth);
	}

	/**
	 * 只设置当前view深度
	 * @param depth
	 */
	public void setThisDepth(float depth) {
		super.setDepth(depth);
	}

	/**
	 * 设置子view x,y坐标
	 * @author linzanxian  @Date 2015年3月19日 下午6:48:09
	 * description:设置子view x,y坐标，暂只开放父级为listview处理, 如果同时改变x,y坐标，请使用此函数，默认值NOXY
	 * @param x x坐标
	 * @return void
	 */
	public void setChildXY(float x, float y) {
		ArrayList<GLRectView> childViews = getChildViews(this);
		if (childViews != null && childViews.size() > 0) {
			GLRectView childView;
			for (int i = 0; i < this.mChildView.size(); i++) {
				childView = this.mChildView.get(i);
				childView.setX(childView.getX() + x);
				childView.setY(childView.getY() + y);
			}
		}
	}

	/**
	 * 设置子view x,y坐标
	 * @author linzanxian  @Date 2015年3月19日 下午6:48:09
	 * description:设置子view x,y坐标，暂只开放父级为listview处理, 如果同时改变x,y坐标，请使用此函数，默认值NOXY
	 * @param x x坐标
	 * @return void
	 */
//	public void setAbsolutChildX(float x) {
//		ArrayList<GLRectView> childViews = getChildViews(this);
//		if (childViews != null && childViews.size() > 0) {
//			for (GLRectView childView : childViews) {
//				childView.setX(x);
//			}
//		}
//	}

	/**
	 * 设置子view x,y坐标
	 * @author linzanxian  @Date 2015年3月19日 下午6:48:09
	 * description:设置子view x,y坐标，暂只开放父级为listview处理, 如果同时改变x,y坐标，请使用此函数，默认值NOXY
	 * @param x x坐标
	 * @return void
	 */
//	public void setAbsolutChildY(float y) {
//		ArrayList<GLRectView> childViews = getChildViews(this);
//		if (childViews != null && childViews.size() > 0) {
//			for (GLRectView childView : childViews) {
//				childView.setY(y);
//			}
//		}
//	}

	/**
	 * 设置子view x,y坐标
	 * @author linzanxian  @Date 2015年3月19日 下午6:48:09
	 * description:设置子view x,y坐标，暂只开放父级为listview处理, 如果同时改变x,y坐标，请使用此函数，默认值NOXY
	 * @param x x坐标
	 * @return void
	 */
//	public void setAbsolutChildDepth(float depth) {
//		ArrayList<GLRectView> childViews = getChildViews(this);
//		if (childViews != null && childViews.size() > 0) {
//			for (GLRectView childView : childViews) {
//				childView.setDepth(depth);
//			}
//		}
//	}

	/**
	 * 设置子view x坐标
	 * @author linzanxian  @Date 2015年3月19日 下午6:48:09
	 * description:设置子view x坐标，暂只开放父级为listview处理
	 * @param x x坐标
	 * @return void
	 */
	public void setChildX(float x) {
		setChildXY(x, NOXY);
	}

	/**
	 * 设置子view y坐标
	 * @author linzanxian  @Date 2015年3月19日 下午6:48:09
	 * description:设置子view y坐标，暂只开放父级为listview处理
	 * @param y y坐标
	 * @return void
	 */
	public void setChildY(float y) {
		setChildXY(NOXY, y);
	}

	/**
	 * 缩放
	 * @author linzanxian  @Date 2015年3月20日 上午11:04:09
	 * description:缩放, 每次都在1的基础上进行缩放
	 * @param scale 倍数
	 * @return void
	 */
	@Override
	public void scale(float sx, float sy) {
		if (!isSetOriginal()) {
			super.scale(sx, sy);

			ArrayList<GLRectView> childViews = getChildViews(this);
			if (childViews != null && childViews.size() > 0) {
				for (GLRectView childView : childViews) {
					childView.scale(sx, sy);

					scaleCorrect(this, childView, sx, sy);
				}

				//resetChildView(this);
			}
		} else {
			scaleChild(this, this, sx, sy);
		}
	}

	private void scaleChild(GLRectView view, GLRectView parentView, float sx, float sy) {
		ArrayList<GLRectView> childViews = getChildViews(parentView);
		if (childViews != null && childViews.size() > 0) {
			for (GLRectView childView : childViews) {
				scaleCorrect(view, childView, sx, sy);

				scaleChild(view, childView, sx, sy);
			}
		}

		if (parentView instanceof GLGroupView) {
			((GLGroupView) parentView).scaleSuper(sx, sy);
		} else {
			parentView.scale(sx, sy);
		}
	}

	public void rotateonly(float angle, float rx, float ry, float rz)
	{
		float radian = (float) (Math.PI / 180 * (-angle));
		float cosv = (float) Math.cos(radian);
		float sinv = (float) Math.sin(radian);
		mCosV = cosv;
		mSinV = sinv;
		mAngle = angle;
		super.rotate(angle, rx, ry, rz);
	}

	private void scaleSuper(float sx, float sy) {
		super.scale(sx, sy);
	}

	/**
	 * 设置旋转
	 * @author linzanxian  @Date 2015年3月20日 上午11:02:42
	 * description:设置旋转
	 * @param angle 角度
	 * @param rx x坐标
	 * @param ry y坐标
	 * @param rz z坐标
	 * @return void
	 */
	@Override
	public void rotate(float angle, float rx, float ry, float rz) {
		ArrayList<GLRectView> childViews = getChildViews(this);

		Float[] translateArray = null;
		float radian = (float) (Math.PI / 180 * (-angle));
		float cosv = (float) Math.cos(radian);
		float sinv = (float) Math.sin(radian);
		if(childViews != null && childViews.size() > 0) {
			for (GLRectView childView : childViews) {
				translateArray = rotateTranslate(this, childView);
				float depth = GLScreenParams.getScreenWidth() / 2400 * translateArray[0] * sinv;
				GLRectView pview = childView.getParent();
				if(pview != null )
				{
					depth = depth + pview.getmIncrementDepth();
				}
				depth=(float)(Math.floor(depth*1000000)/1000000);
				float tx = translateArray[0]*(1-cosv);
				childView.translatesuper(tx, 0, depth);
				childView.rotate(angle, rx, ry, rz);
			}
		}
		mCosV = cosv;
		mSinV = sinv;
		mAngle = angle;
		super.rotate(angle, rx, ry, rz);
	}

	private Float[] rotateTranslate(GLRectView parentView, GLRectView currentView) {
		if( parentView == null )
			return null;
		float parentCenterX = parentView.getWidth()/2;
		float parentCenterY = parentView.getHeight()/2;
		float currentCenterX = currentView.getWidth()/2;
		float currentCenterY = currentView.getHeight()/2;
		float parentX = parentView.getX() - parentView.getmIncrementX();
		float curentX = currentView.getX() - currentView.getmIncrementX();

		float translateX = parentCenterX - currentCenterX - (curentX -parentX);
		float translateY = parentCenterY - currentCenterY;
		float translateZ = parentView.getDepth() - currentView.getDepth();


//		currentView.translate(translateX, 0, translateZ);

		Float[] translateArray = new Float[3];
		translateArray[0] = translateX;
		translateArray[1] = translateY;
		translateArray[2] = translateZ;

		return translateArray;
	}

	/**
	 * 旋转修正
	 * @author linzanxian  @Date 2015年4月2日 上午9:57:40
	 * description:旋转修正
	 * @param pView 父view
	 * @param cView 子view
	 * @return void
	 */
	private void rotateCorrect(GLRectView pView, GLRectView cView, float angle, float rx, float ry, float rz) {
		float radian = (float) (2*Math.PI/360*Math.abs(angle)); //把角度换算成狐度
		if (ry > 0) { //y轴旋转
			float pCenterX = pView.getWidth()/2;
			float cCenterX = cView.getWidth()/2;
			float pDepth = pView.getDepth();
			float cDepth = cView.getDepth();

			float CenterDistance = (pCenterX - cCenterX)/ GLScreenParams.getXDpi();
			float tz = (float) (CenterDistance*Math.sin(radian));
			float tx = CenterDistance - (float) (CenterDistance*Math.cos(radian));

			//Log.d("test2", "tz:"+ tz +"---CenterDistance:"+CenterDistance+"---"+pDepth+"---"+cDepth +"---"+tx);
			//cView.setDepth(3f);
			cView.translate(tx, 0, -tz);
		}
	}

	/**
	 * 设置透明度
	 * @author linzanxian  @Date 2015年3月20日 上午10:52:37
	 * description:设置透明度
	 * @param alpha 透明度
	 * @return void
	 */
	@Override
	public void setAlpha(float alpha) {
		ArrayList<GLRectView> childViews = getChildViews(this);
		Iterator<GLRectView> iter = childViews.iterator();
		while (iter.hasNext() ){
			GLRectView var = iter.next();
			var.setAlpha(alpha);
		}
//		if (childViews != null && childViews.size() > 0) {
//			for (GLRectView childView : childViews) {
//				childView.setAlpha(alpha);
//			}
//		}

		super.setAlpha(alpha);
	}

	/**
	 * 可见
	 * @author linzanxian  @Date 2015年3月27日 下午3:44:24
	 * description:移动
	 * @param tx x坐标
	 * @param ty y坐标
	 * @param tz z坐标
	 * @return void
	 */
	@Override
	public void setVisible(boolean visible) {
		/*
		ArrayList<GLRectView> childViews = getChildViews(this);
		if (childViews != null && childViews.size() > 0) {
			for (GLRectView childView : childViews) {
				childView.setVisible(visible);
//Log.d("test", "childView");				
			}
		}*/

		super.setVisible(visible);
	}

	/**
	 * 移动
	 * @author linzanxian  @Date 2015年3月27日 下午3:44:24
	 * description:移动
	 * @param tx x坐标
	 * @param ty y坐标
	 * @param tz z坐标
	 * @return void
	 */
	@Override
	public void translate(float tx, float ty, float tz) {
		ArrayList<GLRectView> childViews = getChildViews(this);
		if (childViews != null && childViews.size() > 0) {
			for (GLRectView childView : childViews) {
				childView.translate(tx, ty, tz);
			}
		}

		super.translate(tx, ty, tz);
	}

	/**
	 * 平移子view
	 * @author linzanxian  @Date 2015年4月1日 上午9:51:43
	 * description:平移子view
	 * @param tx x坐标
	 * @param ty y坐标
	 * @param tz z坐标
	 * @return void
	 */
	public void translateChild(float tx, float ty, float tz) {
		ArrayList<GLRectView> childViews = getChildViews(this);
		if (childViews != null && childViews.size() > 0) {
			for (GLRectView childView : childViews) {
				childView.translate(tx, ty, tz);
			}
		}
	}

	/**
	 * 根据类型获取子views
	 * @author linzanxian  @Date 2015年3月20日 上午10:30:34
	 * description:根据类型获取子views
	 * @param view GLRectView
	 * @return ArrayList<GLRectView>
	 */
	private ArrayList<GLRectView> getChildViews(GLRectView view) {
		ArrayList<GLRectView> childViews = new ArrayList<GLRectView>();
		if (view instanceof GLGroupView) {
			childViews = ((GLGroupView) view).mChildView;
		}

		return childViews;
	}

	public GLRectView getFocusedView(){
		for (int i = 0; i < mChildView.size(); i++) {
			GLRectView view = mChildView.get(i);
			if (view.isFocused()){
				return view;
			}
		}
		return null;
	}

	@Override
	public boolean onKeyDown(int keycode) {

		GLRectView view = getFocusedView();
		boolean flag = false;
		if (view != null && view.isVisible()){
			flag = view.onKeyDown(keycode);
		}

		if (!flag){
			flag = super.onKeyDown(keycode);
		}

		if (!flag && !isAutomaticFocus){
			switch (keycode) {
				case MojingKeyCode.KEYCODE_DPAD_LEFT:
					flag = mGlFocusUtils.handleFocused(GLFocusUtils.TO_LEFT, view, mChildView);
					break;
				case MojingKeyCode.KEYCODE_DPAD_RIGHT:
					flag = mGlFocusUtils.handleFocused(GLFocusUtils.TO_RIGHT, view, mChildView);
					break;
				case MojingKeyCode.KEYCODE_DPAD_UP:
					flag = mGlFocusUtils.handleFocused(GLFocusUtils.TO_UP, view, mChildView);
					break;
				case MojingKeyCode.KEYCODE_DPAD_DOWN:
					flag = mGlFocusUtils.handleFocused(GLFocusUtils.TO_DOWN, view, mChildView);
					break;
				default:
					break;
			}
		}

		return flag;
	}

	@Override
	public boolean onKeyUp(int keycode) {
		GLRectView view = getFocusedView();
		boolean flag = false;
		if (view != null && view.isVisible()){
			flag = view.onKeyUp(keycode);
		}
		return flag ? true : super.onKeyUp(keycode);
	}

	@Override
	public boolean onKeyLongPress(int keycode) {
		GLRectView view = getFocusedView();
		boolean flag = false;
		if (view != null){
			flag = view.onKeyLongPress(keycode);
		}
		return flag ? true : super.onKeyLongPress(keycode);
	}

	public GLRectView getFocusedChild(){
		return getFocusedView();
	}

	public void setFocusedChild(GLRectView child){
		mGlFocusUtils.setFousedView(child);
	}

	public void onFocusChild(){
		if (getFocusedView() != null){
			getFocusedView().onFocusChange(GLFocusUtils.TO_UNKNOWN, false);
			mGlFocusUtils.setFousedView(null);
		}
	}

	@Override
	public void onFocusChange(int direction, boolean isFocused) {
		if (isFocused() == isFocused){
			return;
		}

		if (!isFocused && getFocusedView() != null){
			getFocusedView().onFocusChange(direction, false);
			mGlFocusUtils.setFousedView(null);
		}

		super.onFocusChange(direction, isFocused);

		if (isFocused && direction != GLFocusUtils.TO_UNKNOWN){
			mGlFocusUtils.handleFocused(direction, null, mChildView);
		}
	}

	public void lostParentFocus(){
		if (getParent() != null && getParent().isFocused()){
			getParent().onFocusChange(GLFocusUtils.TO_UNKNOWN, false);
			getParent().lostParentFocus();
		}
	}

	/**
	 * 缩放修正
	 * @author linzanxian  @Date 2015年4月2日 下午6:59:20
	 * description:缩放修正
	 * @param pView 父view
	 * @param cView 子view
	 * @return void
	 */
	private void scaleCorrect(GLRectView pView, GLRectView cView, float sx, float sy) {
		if (!isSetOriginal()) {
			float pCenterX = pView.getWidth()/2 + pView.getX();
			float pCenterY = pView.getHeight()/2 + pView.getY();

			float cCenterX = 0;
			if (cView.getWidth() > pView.getWidth()) {
				cCenterX = pView.getWidth()/2 + cView.getX();
			} else {
				cCenterX = cView.getWidth()/2 + cView.getX();
			}

			float cCenterY = 0;
			if (cView.getHeight() > pView.getHeight()) {
				cCenterY = pView.getHeight()/2 + cView.getY();
			} else {
				cCenterY = cView.getHeight()/2 + cView.getY();
			}


			float ty = 0;
			float tx = 0;

			if (pCenterX == cCenterX) {
				tx = 0;
			} else {
				tx = (sx - 1) * -(pCenterX - cCenterX);
			}
			if (pCenterY == cCenterY) {
				ty = 0;
			} else {
				ty = (sy - 1) * -(pCenterY - cCenterY);
			}

			//Log.d("test2", sx +"===="+ sy +"===="+ cView.getTranslateX() +"===="+ tx +"===="+ cView.getTranslateY() +"===="+ ty +"===="+ cView.getX() +"===="+ cView.getY());
//			cView.setX(cView.getX() + tx - cView.getTranslateX());
//			cView.setY(cView.getY() + ty - cView.getTranslateY());
//
//			cView.setTranslateX(tx);
//			cView.setTranslateY(ty);
		} else { //兼容旧方案
			float pCenterX = pView.getWidth()/2 + pView.getOriginalX();
			float pCenterY = pView.getHeight()/2 + pView.getOriginalY();
			float cCenterX = cView.getWidth()/2 + cView.getOriginalX();
			float cCenterY = cView.getHeight()/2 + cView.getOriginalY();

			float ty = 0;
			float tx = 0;
			if (pCenterX == cCenterX) {
				tx = 0;
			} else {
				tx = (sx - 1) * -(pCenterX - cCenterX);
			}
			if (pCenterY == cCenterY) {
				ty = 0;
			} else {
				ty = (sy - 1) * -(pCenterY - cCenterY);
			}

			if (cView instanceof GLGroupView) {
				((GLGroupView) cView).setThisX(cView.getOriginalX() + tx);
				((GLGroupView) cView).setThisY(cView.getOriginalY() + ty);
			} else {
				cView.setX(cView.getOriginalX() + tx);
				cView.setY(cView.getOriginalY() + ty);
			}
		}
	}

	public boolean isAutomaticFocus() {
		return isAutomaticFocus;
	}

	public void setAutomaticFocus(boolean isAutomaticFocus) {
		this.isAutomaticFocus = isAutomaticFocus;
	}

	public ArrayList<GLRectView> getChildView() {
		return mChildView;
	}

	@Override
	public void setOriginal() {
		ArrayList<GLRectView> childViews = getChildViews(this);
		if (childViews != null && childViews.size() > 0) {
			for (GLRectView childView : childViews) {
				childView.setOriginal();
			}
		}

		super.setOriginal();
	}

	@Override
	public void release() {
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).release();
		}
		super.release();
	}

	@Override
	public void setLookAngle(float lookAngle) {
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).setLookAngle(lookAngle);
		}
		super.setLookAngle(lookAngle);
	}

	@Override
	public void setLookTranslateZ(float mLookTranslateZ) {
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).setLookTranslateZ(mLookTranslateZ);
		}
		super.setLookTranslateZ(mLookTranslateZ);
	}

	@Override
	public void setMask(float mask) {
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).setMask(mask);
		}
		super.setMask(mask);
	}

	@Override
	public void setZIndex(int index) {
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).setZIndex(index);
		}
		super.setZIndex(index);
	}

	@Override
	public void setFixed(boolean fixed) {
		for (int i = 0; i < mChildView.size(); i++) {
			mChildView.get(i).setFixed(fixed);
		}
		super.setFixed(fixed);
	}
}