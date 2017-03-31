package com.bfmj.viewcore.adapter;

import android.database.DataSetObserver;

import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLRectView;


public interface GLAdapter {
	void registerDataSetObserver(DataSetObserver observer);
	void unregisterDataSetObserver(DataSetObserver observer);

	/**
	 * 从数据集里添加一项
	 * @param index
	 */
	void addIndex(int index, GLRectView view);
	/**
	 * 从数据集里移动某一项
	 * @param index
	 */
	void removeIndex(int index);
	/**
	 * 得到总数
	 * @return
	 */
	int getCount();
	/**
	 * 得到第 position 项
	 * @param position
	 * @return
	 */
	Object getItem(int position);
	/**
	 * 根据索引得到item唯一标识
	 * @param position
	 * @return
	 */
	long getItemId(int position);
	/**
	 * 得到item视图
	 * @param position
	 * @param convertView
	 * @param parent
	 * @return
	 */
	GLRectView getGLView(int position, GLRectView convertView, GLGroupView parent);
	/**
	 * 是否为空
	 * @return
	 */
	boolean isEmpty();
}
