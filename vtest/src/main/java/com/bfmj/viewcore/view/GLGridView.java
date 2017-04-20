package com.bfmj.viewcore.view;

import java.util.ArrayList;

import com.baofeng.mojing.input.base.MojingKeyCode;
import com.bfmj.viewcore.animation.GLAlphaAnimation;
import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.render.GLConstant;
import com.bfmj.viewcore.adapter.GLListAdapter;

import android.content.Context;

public class GLGridView extends GLAdapterView<GLListAdapter> {

	private GLConstant.GLOrientation mOrientation = GLConstant.GLOrientation.HORIZONTAL; //默认模向

	private int mNumColumns=0; //列数
	private int mNumRows=0;//行数
	private ArrayList<GLView> mList;//数据集
	public int mStartIndex=0;//当前开始的索引
	private int mSelectedIndex=0;//当前选中的索引
	private float mVerticalSpacing=0.0f;//垂直间距离
	private float mHorizontalSpacing=0.0f;//水平间距离
	private GLListAdapter mGLAdapter;
	private int mNumOneScreen=1;
	private int mTotalCount = 0;
//	private GLRectView convertView;
	private OnItemSelectedListener mOnItemSelectedListener;
	private OnItemClickListener mOnItemClickListener;
	private int mPrevIndex = 0;
	private GLRectView mFirstView;
	protected ArrayList<GLRectView> mItemViews = new ArrayList<GLRectView>();
	protected ArrayList<GLRectView> mItemViews2 = new ArrayList<GLRectView>();
	protected ArrayList<GLRectView> mItemViewCur = mItemViews;
	protected ArrayList<GLRectView> mItemViewBack = mItemViews2;
	protected boolean mbpageChange = true;

	public enum EMoveDirection {
		MOVENONE,
		MOVELTOR,
		MOVERTOL
	}
	protected EMoveDirection mMoveDirection = EMoveDirection.MOVENONE;

	public void SetMoveDirection(EMoveDirection dir){
		mMoveDirection = dir;
	}



	/**
	 * 设置列数
	 * @param columns 列数
	 */
	public void setNumColumns(int columns){
		this.mNumColumns = columns;
	}
	
	/**
	 * 设置行数
	 * @param rows 行数
	 */
	public void setNumRows(int rows){
		this.mNumRows = rows;
	}
	
	/**
	 * 设置垂直间距
	 * @param verticalSpacing 垂直间距
	 */
	public void setVerticalSpacing(float verticalSpacing){
		this.mVerticalSpacing = verticalSpacing;
	}
	
	/**
	 * 设置水平间矩
	 * @param horizontalSpacing 水平间矩
	 */
	public void setHorizontalSpacing(float horizontalSpacing){
		this.mHorizontalSpacing = horizontalSpacing;
	}	
	
	/**
	 * 设置一屏显示几条数据
	 * @param oneScreen
	 */
	public void setNumOneScreen(int oneScreen){
		this.mNumOneScreen = oneScreen;
	}

	public int getNumOneScreen(){
		if( mNumOneScreen == 0 ) {
			mNumOneScreen = 1;
		}
		return mNumOneScreen;
	}

	/**
	 * GLGridView构造函数
	 * @param context
	 */
	public GLGridView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	/**
	 * 设置当前第一个显示的索引
	 * @param index 开始的索引，从0开始算
	 */
	public void setStartIndex(int index){
		this.mStartIndex = index;
	}
	
	/**
	 * 获取当前的索引
	 * @return
	 */
	public int getStartIndex(){
		return this.mStartIndex;
	}	
	
	/**
	 * GLGridView构造函数
	 * @param context
	 * @param rows 行数
	 * @param columns 列数
	 */
	public GLGridView(Context context, int rows, int columns){
		super(context);
		this.mNumColumns = columns;
		this.mNumRows = rows;
	}

	@Override
	public GLListAdapter getGLAdapter() {
		// TODO Auto-generated method stub
		if(mGLAdapter != null) {
			return mGLAdapter;
		}
		return null;
	}

	@Override
	public void setAdapter(GLListAdapter adapter) {
		// TODO Auto-generated method stub
		if( adapter == null ){
			return;
		}

		if( mGLAdapter != null &&  mDataSetObserver != null ){
			mGLAdapter.unregisterDataSetObserver( mDataSetObserver);
		}

		for(GLRectView var : mItemViewCur){
			removeView(var);
		}
		mItemViewCur.clear();

		for( GLRectView var : mItemViewBack){
			removeView(var);
		}
		mItemViewBack.clear();


		//清除以前的数据
		mGLAdapter = adapter;
		this.mTotalCount = adapter.getCount();
		mOnItemSelectedListener = getOnItemSelectedListener();
		mOnItemClickListener = getOnItemClickListener();
		mDataSetObserver = new GLAdapterDataSetObserver();
		mGLAdapter.registerDataSetObserver( mDataSetObserver );

		requestLayout();
	}

	public void showHItem( int cIndex ){
		int tempIndex=cIndex;
		int index = 0;
		for(int rows=0; rows<this.mNumRows; rows++)
		{
			for(int col=0;col<this.mNumColumns;col++)
			{
				//如果大于设置的一屏显示数则不再添加

				GLRectView item = null;
				if( mItemViewCur.size() == mNumOneScreen) {
					item = mItemViewCur.get(index);
					index++;
				}

				if( tempIndex > this.mNumOneScreen + cIndex || tempIndex > this.mTotalCount-1){
					if( item != null )
					{
						item.setVisible(false);
					}
					continue;
				}

				GLRectView view = this.mGLAdapter.getGLView(tempIndex, item, this);
				if (col == 0 && rows == 0) {
					mFirstView = view;
				}
				if( view == null ){
					continue;
				}
				view.setAlpha(0.01f);
				view.setVisible(false);

				float fx = getX() + getPaddingLeft() + getMarginLeft() + view.getWidth() * col + this.mHorizontalSpacing * col;
				if( EMoveDirection.MOVERTOL == mMoveDirection){
					fx += this.getWidth();
				}else if( EMoveDirection.MOVELTOR == mMoveDirection){
					fx -= this.getWidth();
				}else{
					view.setAlpha(1.0f);
				}
				view.setX(fx);
				view.setY(getY() + getPaddingTop() + view.getHeight() * rows + this.mVerticalSpacing * rows);

				view.setId("gridview_" + tempIndex);

				final int position = tempIndex;

				view.setFocusListener(new GLViewFocusListener() {

					@Override
					public void onFocusChange(GLRectView view, boolean focused) {
						if (mOnItemSelectedListener == null) {
							return;
						}
						if (focused) {
							mOnItemSelectedListener.onItemSelected(null, view, position, position);
						} else {
							mOnItemSelectedListener.onNothingSelected(null, view, position, position);
						}
					}
				});
				view.setOnKeyListener(new GLOnKeyListener() {

					@Override
					public boolean onKeyUp(GLRectView view, int keycode) {
						return false;
					}

					@Override
					public boolean onKeyLongPress(GLRectView view, int keycode) {
						return false;
					}

					@Override
					public boolean onKeyDown(GLRectView view, int keycode) {
						if (mOnItemClickListener == null) {
							return false;
						}

						if (keycode == MojingKeyCode.KEYCODE_ENTER) {
							mOnItemClickListener.onItemClick(null, view, position, position);
						}
						mPrevIndex = position;
						return false;
					}
				});
				if( view.getDepth() > this.getDepth() ){
					view.setDepth( this.getDepth() );
				}
				if( item == null) {
					addView(view);
					mItemViewCur.add(view);
					view.setVisible(true);
				}else {
					if(  EMoveDirection.MOVENONE == mMoveDirection){
						item.setAlpha(1.0f);
					}else {
						item.setAlpha(0.01f);
					}
					item.setVisible(true);
				}
				tempIndex++;
			}
		}
	}

	public void showVItem( int cIndex ){
		int tempIndex=cIndex;
		int index = 0;
		for(int col=0; col<this.mNumColumns; col++)
		{
			for(int row=0; row<this.mNumRows; row++)
			{
				//如果大于设置的一屏显示数则不再添加
				GLRectView item = null;
				if(mItemViewCur.size() == mNumOneScreen ){
					item = mItemViewCur.get(index);
				}
				index++;
				if( tempIndex > this.mNumOneScreen + cIndex || tempIndex > this.mTotalCount-1){
					if( null != item ){
						item.setVisible(false);
					}
					continue;
				}
				GLRectView view = this.mGLAdapter.getGLView(tempIndex, item, this);
				if (col == 0 && row == 0) {
					mFirstView = view;
				}
				if(view == null ){
					continue;
				}
				view.setAlpha(0.01f);
				view.setVisible(false);

				float fx = getX() + getPaddingLeft() + getMarginLeft() + view.getWidth() * col + this.mHorizontalSpacing * col;
				if( EMoveDirection.MOVERTOL == mMoveDirection){
					fx += this.getWidth();
				}else if( EMoveDirection.MOVELTOR == mMoveDirection){
					fx -= this.getWidth();
				}else{
					view.setAlpha(1.0f);
				}
				view.setX(fx);
				view.setY(getY() + getPaddingTop() + getMarginTop() + view.getHeight() * row + this.mVerticalSpacing * row);

				view.setId("gridview_" + tempIndex);

				final int position = tempIndex;
				view.setFocusListener(new GLViewFocusListener() {

					@Override
					public void onFocusChange(GLRectView view, boolean focused) {
						if (mOnItemSelectedListener == null) {
							return;
						}
						if (focused) {
							mOnItemSelectedListener.onItemSelected(null, view, position, position);
						} else {
							mOnItemSelectedListener.onNothingSelected(null, view, position, position);
						}
					}
				});
				view.setOnKeyListener(new GLOnKeyListener() {

					@Override
					public boolean onKeyUp(GLRectView view, int keycode) {
						return false;
					}

					@Override
					public boolean onKeyLongPress(GLRectView view, int keycode) {
						return false;
					}

					@Override
					public boolean onKeyDown(GLRectView view, int keycode) {
						if (mOnItemClickListener == null) {
							return false;
						}

						if (keycode == MojingKeyCode.KEYCODE_ENTER) {
							mOnItemClickListener.onItemClick(null, view, position, position);
						}
						mPrevIndex = position;
						return false;
					}
				});
				if( view.getDepth() > this.getDepth() ){
					view.setDepth( this.getDepth() );
				}
				if( item == null) {
					addView(view);
					mItemViewCur.add(view);
					view.setVisible(true);
				}else {
					if(EMoveDirection.MOVENONE == mMoveDirection){
						item.setAlpha(1.0f);
					}else {
						item.setAlpha(0.01f);
					}
					item.setVisible(true);
				}
				tempIndex++;
			}
		}
	}


	public void showVRItem( int cIndex ){
		int tempIndex=cIndex;
		int index = 0;
		for(int col=this.mNumColumns -1; col >= 0; --col )
		{
			for(int row=0; row<this.mNumRows; row++)
			{
				//如果大于设置的一屏显示数则不再添加
				GLRectView item = null;
				if( mItemViewCur.size() == mNumOneScreen ){
					item = mItemViewCur.get(index);
				}
				index++;
				if( tempIndex > this.mNumOneScreen + cIndex || tempIndex > this.mTotalCount-1){
					if( null != item ){
						item.setVisible(false);
					}
					continue;
				}
				GLRectView view = this.mGLAdapter.getGLView(tempIndex, item, this);
				if (col == 0 && row == 0) {
					mFirstView = view;
				}
				if( view == null ){
					continue;
				}
				view.setAlpha(0.01f);
				view.setVisible(false);

//				float fx = getX() + getPaddingLeft() + getMarginLeft() + view.getWidth() * col + this.mHorizontalSpacing * col;
				float fx = (getX() + getWidth()) - getPaddingLeft() - getMarginRight() - (view.getWidth() + this.mHorizontalSpacing) * (mNumColumns - col);
				if( EMoveDirection.MOVERTOL == mMoveDirection){
					fx += this.getWidth();
				}else if( EMoveDirection.MOVELTOR == mMoveDirection){
					fx -= this.getWidth();
				}else{
					view.setAlpha(1.0f);
				}
				view.setX(fx);

//				view.setX((getX() + getWidth()) - getPaddingLeft() - getMarginRight() - (view.getWidth() + this.mHorizontalSpacing) * (mNumColumns - col) );
				view.setY(getY() + getPaddingTop() + getMarginTop() + view.getHeight() * row + this.mVerticalSpacing * row);

				view.setId("gridview_" + tempIndex);

				//final int position = mNumColumns*row+col;
				final int position = tempIndex;
				view.setFocusListener(new GLViewFocusListener() {

					@Override
					public void onFocusChange(GLRectView view, boolean focused) {
						if (mOnItemSelectedListener == null) {
							return;
						}
						if (focused) {
							mOnItemSelectedListener.onItemSelected(null, view, position, position);
						} else {
							mOnItemSelectedListener.onNothingSelected(null, view, position, position);
						}
					}
				});
				view.setOnKeyListener(new GLOnKeyListener() {

					@Override
					public boolean onKeyUp(GLRectView view, int keycode) {
						return false;
					}

					@Override
					public boolean onKeyLongPress(GLRectView view, int keycode) {
						return false;
					}

					@Override
					public boolean onKeyDown(GLRectView view, int keycode) {
						if (mOnItemClickListener == null) {
							return false;
						}

						if (keycode == MojingKeyCode.KEYCODE_ENTER) {
							mOnItemClickListener.onItemClick(null, view, position, position);
						}
						mPrevIndex = position;
						return false;
					}
				});
				if( view.getDepth() > this.getDepth() ){
					view.setDepth( this.getDepth() );
				}
				if( item == null) {
					addView(view);
					mItemViewCur.add(view);
					view.setVisible(true);
				}else {
					if(EMoveDirection.MOVENONE == mMoveDirection){
						item.setAlpha(1.0f);
					}else {
						item.setAlpha(0.01f);
					}
					item.setVisible(true);
				}
				tempIndex++;
			}
		}
	}

	public static void setTranslateAnimation(GLRectView view, float x, float y,float z){
		if(view == null)
			return;
		GLAnimation animation = new GLTranslateAnimation(x, y, z);
		animation.setAnimView(view);
		animation.setDuration(500);
		view.startAnimation(animation);
	}

	public static void startGLAlphaAnimation(GLRectView view, float startAlpha, float endAlpha) {
		if (view == null)
			return;
		GLAnimation animation = new GLAlphaAnimation(startAlpha,endAlpha);
		animation.setAnimView(view);
		animation.setDuration(500);
		view.startAnimation(animation);
	}

	public void pageChange(){
		mbpageChange = !mbpageChange;
		if(mbpageChange){
			mItemViewCur = mItemViews;
			mItemViewBack = mItemViews2;
		}else{
			mItemViewCur = mItemViews2;
			mItemViewBack = mItemViews;
		}
		mNumOneScreen =  mNumRows * mNumColumns;
		if(this.mGLAdapter ==null)
			return;
		float fx = 0.0f;
		if(EMoveDirection.MOVERTOL == mMoveDirection ){
			fx = -this.getWidth();
		}else if( EMoveDirection.MOVELTOR == mMoveDirection){
			fx = this.getWidth();
		}
		int count = mItemViewBack.size();
		for (int i = 0; i < count; ++i){
			setTranslateAnimation(mItemViewBack.get(i), fx, 0, 0);
			startGLAlphaAnimation(mItemViewBack.get(i), 1.0f, 0.0f);
		}
		this.mTotalCount = mGLAdapter.getCount();
		if( mStartIndex >= mTotalCount ){
			mStartIndex = 0;
		}
		if( mOrientation.equals( GLConstant.GLOrientation.HORIZONTAL )){
			showHItem(mStartIndex);
		}else if( mOrientation.equals(GLConstant.GLOrientation.VERTICAL )){
			showVItem(mStartIndex);
		}else {
			showVRItem( mStartIndex );
		}
		count = mItemViewCur.size();
		for (int i = 0; i < count; ++i){
			setTranslateAnimation(mItemViewCur.get(i), fx, 0, 0);
			startGLAlphaAnimation(mItemViewCur.get(i), 0.0f, 1.0f);
		}
		return;
	}

	@Override
	public void requestLayout(){
		mNumOneScreen =  mNumRows * mNumColumns;
		if(this.mGLAdapter ==null)
			return;
		this.mTotalCount = mGLAdapter.getCount();
		if( mStartIndex >= mTotalCount ){
			mStartIndex = 0;
		}
		if( mOrientation.equals( GLConstant.GLOrientation.HORIZONTAL )){
			showHItem(mStartIndex);
		}else if( mOrientation.equals(GLConstant.GLOrientation.VERTICAL )){
			showVItem(mStartIndex);
		}else {
			showVRItem( mStartIndex );
		}
		return;

	}

	@Override
	public GLView getSelectedGLView() {
		// TODO Auto-generated method stub
		return null;
	}
	
	/**
	 * 返回索引为index的项
	 * @param index
	 * @return
	 */
	public GLRectView getChatAtChild(int index){
		if(this.mGLAdapter !=null){
//			return this.mGLAdapter.getItem(index);
		}
		return null;
	}
	
	/**
	 * 得到总个数
	 * @return
	 */
	public int getTotalNum(){
		if(this.mGLAdapter !=null){
			return this.mGLAdapter.getCount();
		}
		
		return 0;
	}
	
	/**
	 * 删除一个View
	 * @param index 要删除的索引
	 */
	public void removeView(int index){
		if(this.mGLAdapter == null) return;
		//如果索引大于长度则不处理
		if(index >= this.mTotalCount) return;
		
//		super.removeView(this.mGLAdapter.getItem(index));
		//总数量减去1
		this.mTotalCount--;
		//从数据集里移除
		this.mGLAdapter.removeIndex(index);
		//计算行数
		this.mNumRows = (int)Math.ceil((float)this.mTotalCount/(float)this.mNumColumns);
		
		//行数
		int _rows = (int)Math.ceil((float)(index+1)/(float)this.mNumColumns)-1;
		//列数
		int _col = (index+1)%this.mNumColumns-1;
		int tempIndex = index;
		for(int rows=_rows; rows<this.mNumRows; rows++){
			if(rows > _rows) _col = 0;
			for(int col=_col;col<this.mNumColumns;col++){
				//如果大于设置的一屏显示数则不再添加
				if((tempIndex > this.mNumOneScreen && this.mNumOneScreen != -1) || tempIndex > this.mTotalCount-1){
					break;
				}
				GLRectView view = this.getChatAtChild(tempIndex);
				view.setX(view.getWidth() * col + this.mHorizontalSpacing * col);
				view.setY(view.getHeight() * rows + this.mVerticalSpacing * rows);
				tempIndex++;
			}
		}
	}
	
	/**
	 * 添加一个控件到最后
	 * @param view
	 */
	public void addChildView(GLRectView view){
		if(this.mGLAdapter == null) return;
		
		this.mTotalCount++;
		//添加到数据集里
		this.mGLAdapter.addIndex(this.mGLAdapter.getCount()-1, view);
		//计算行数
		this.mNumRows = (int)Math.ceil((double)this.mTotalCount/(double)this.mNumColumns) - 1;
		
		//列数
		int _col = (this.mTotalCount-1) %this.mNumColumns;		
				
		view.setX(view.getWidth() * _col + this.mHorizontalSpacing * _col);
		view.setY(view.getHeight() * this.mNumRows + this.mVerticalSpacing * this.mNumRows);
		view.setId("gridview_" + this.mTotalCount);
		this.addView(view);
	}
	
	/**
	 * 添加一个控件到指定的索引处
	 * @param view
	 * @param index
	 */
	public void addChildView(GLRectView view, int index){
		if(this.mGLAdapter == null) return;
		
		this.mTotalCount++;
		//添加到数据集里
		this.mGLAdapter.addIndex(index, view);
		
		//总行数
		this.mNumRows = (int)Math.ceil((double)this.mTotalCount/(double)this.mNumColumns);
		//计算行数
		
		int _rows = (int)Math.ceil((double)index/(double)this.mNumColumns) - 1;
		
		//要插入的列数
		int _col = index % this.mNumColumns;
		
		int tempIndex = index;
		
		for(int rows=_rows; rows<this.mNumRows; rows++){
			if(rows > _rows) _col = 0;
			for(int col=_col;col<this.mNumColumns;col++){
				//如果大于设置的一屏显示数则不再添加
				if((tempIndex > this.mNumOneScreen && this.mNumOneScreen != -1) || tempIndex > this.mTotalCount-1){
					break;
				}
				
				GLRectView _view = this.getChatAtChild(tempIndex);
				_view.setX(view.getWidth() * col + this.mHorizontalSpacing * col);
				_view.setY(view.getHeight() * rows + this.mVerticalSpacing * rows);
				tempIndex++;
			}
		}
		
		this.addView(view, index);
	}

	public int getPrevIndex() {
		return mPrevIndex;
	}
	
	public void setChildFocus() {
		if (mFirstView != null) {
			mFirstView.requestFocus();
		}
	}
	
	/**
	 * 获取列数
	 * @author linzanxian  @Date 2015-7-9 下午3:09:13
	 * @return int
	 */
	public int getNumColumns() {
		return mNumColumns;
	}

	public void setOrientation(GLConstant.GLOrientation orientation) {
		mOrientation = orientation;
	}

	/**
	 * 获取布局方向
	 * @author linzanxian  @Date 2015年3月10日 下午5:36:47
	 * @return String
	 */
	public GLConstant.GLOrientation getOrientation() {
		return mOrientation;
	}

}
