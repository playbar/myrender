package com.bfmj.viewcore.adapter;

import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLRectView;

import android.database.DataSetObservable;
import android.database.DataSetObserver;


public abstract class GLBaseAdapter implements GLListAdapter, GLSpinnerAdapter {
	private final DataSetObservable mDataSetObservable = new DataSetObservable();

    public boolean hasStableIds() {
        return false;
    }
    
    public void registerDataSetObserver(DataSetObserver observer) {    	
        mDataSetObservable.registerObserver(observer);
    }

    public void unregisterDataSetObserver(DataSetObserver observer) {
        mDataSetObservable.unregisterObserver(observer);
    }
    
    public void notifyDataSetChanged() {
        mDataSetObservable.notifyChanged();
    }
    
    public void notifyDataSetInvalidated() {
        mDataSetObservable.notifyInvalidated();
    }
    
    public boolean areAllItemsEnabled() {
        return true;
    }

    public boolean isEnabled(int position) {
        return true;
    }

    public GLRectView getDropDownView(int position, GLRectView convertView, GLGroupView parent) {
        return getGLView(position, convertView, parent);
    }

    public int getItemViewType(int position) {
        return 0;
    }

    public int getViewTypeCount() {
        return 1;
    }
    
    public boolean isEmpty() {
        return getCount() == 0;
    }    
	
}
