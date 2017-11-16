package com.bfmj.viewcore.view;

import com.bfmj.viewcore.adapter.GLAdapter;

import android.content.Context;
import android.view.View;
import android.view.ViewDebug;
import android.database.DataSetObserver;

public abstract class GLAdapterView<T extends GLAdapter> extends GLGroupView {

	/**
     * The number of items in the current adapter.
     */
    @ViewDebug.ExportedProperty(category = "list")
    int mFirstPosition = 0;
    int mItemCount;
    int mOldItemCount;
    OnItemClickListener mOnItemClickListener;
    OnItemSelectedListener mOnItemSelectedListener;
    protected OnListViewFocusChange mOnListViewFocusChange;
    GLAdapterDataSetObserver mDataSetObserver;
    
    public static final int INVALID_POSITION = -1;

    /**
     * Represents an empty or invalid row id
     */
    public static final long INVALID_ROW_ID = Long.MIN_VALUE;

    /**
     * The last selected position we used when notifying
     */
    int mOldSelectedPosition = INVALID_POSITION;
    
    /**
     * The id of the last selected position we used when notifying
     */
    long mOldSelectedRowId = INVALID_ROW_ID;
    
    
    
    int mNextSelectedPosition = INVALID_POSITION;

    long mNextSelectedRowId = INVALID_ROW_ID;

    @ViewDebug.ExportedProperty(category = "list")
    int mSelectedPosition = INVALID_POSITION;

    long mSelectedRowId = INVALID_ROW_ID;

    /**
     * View to show if there are no items to show.
     */
    private View mEmptyView;
	
	public GLAdapterView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	public interface OnItemClickListener {
        void onItemClick(GLAdapterView<?> glparent, GLView glview, int position, long id);
    }
	
	public void setOnItemClickListener(OnItemClickListener listener) {
        mOnItemClickListener = listener;
    }
	
	public final OnItemClickListener getOnItemClickListener() {
        return mOnItemClickListener;
    }
	
	public interface OnItemLongClickListener {
		boolean onItemLongClick(GLAdapterView<?> glparent, GLView glview, int position, long id);
	}
	
	public interface OnItemSelectedListener {        
        void onItemSelected(GLAdapterView<?> glparent, GLView glview, int position, long id);

        void onNothingSelected(GLAdapterView<?> glparent, GLView glview, int position, long id);
        
        void onNoItemData();
    }
	
	public void setOnItemSelectedListener(OnItemSelectedListener listener) {
        mOnItemSelectedListener = listener;
    }

    public OnItemSelectedListener getOnItemSelectedListener() {
        return mOnItemSelectedListener;
    }
    

	public interface OnListViewFocusChange{
		void focusChange(GLRectView view, boolean isFocus);
	}
	
	public void setOnListViewFocusChange(OnListViewFocusChange listener) {
		mOnListViewFocusChange = listener;
    }

    public OnListViewFocusChange getOnListViewFocusChange() {
        return mOnListViewFocusChange;
    }
    
    
    
    
    public abstract T getGLAdapter();

    
    public abstract void setAdapter(T adapter);

    
    public void addView(GLRectView child, int index) {
        //throw new UnsupportedOperationException("addView(View, int) is not supported in AdapterView");
        super.addView(index, child );
    }
    
    public int getSelectedItemPosition() {
        return mNextSelectedPosition;
    }
    
    public long getSelectedItemId() {
        return mNextSelectedRowId;
    }
    
    @Override
    public void removeView(GLRectView view) {
    	// TODO Auto-generated method stub
    	super.removeView(view);
    }
    
    @Override
    public void addView(GLRectView view) {
    	// TODO Auto-generated method stub
    	super.addView(view);
    }
    
    public void removeGLViewAt(int index) {
        //throw new UnsupportedOperationException("removeViewAt(int) is not supported in AdapterView");
    	super.removeView(index);
    }
    
    public void removeAllGLViews() {
//        throw new UnsupportedOperationException("removeAllViews() is not supported in AdapterView");
    	for(int i=0;i<mChildView.size();i++)
    		super.removeView(i);
    	
    	mChildView.clear();
    	
    }
    
    public abstract GLView getSelectedGLView();
    
    public Object getSelectedItem() {
        T gladapter = getGLAdapter();
        int selection = getSelectedItemPosition();
        if (gladapter != null && gladapter.getCount() > 0 && selection >= 0) {
            return gladapter.getItem(selection);
        } else {
            return null;
        }
    }
    
    public int getCount() {
        return mItemCount;
    }
    protected RemoveListener mRemoveListener;
    public void setRemoveListener(RemoveListener listener){
    	mRemoveListener = listener;
    }
    public interface RemoveListener{
    	/**
    	 * 删除后回调
    	 * @param position  当前索引
    	 * @param local  当前数据索引
    	 */
    	void back(int position, int local);
    }

    class GLAdapterDataSetObserver extends DataSetObserver{

        @Override
        public void onChanged(){
            requestLayout();
        }

        @Override
        public void onInvalidated(){

        }

    }
    
}
