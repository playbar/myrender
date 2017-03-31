package com.viewcore.test;

import java.util.ArrayList;

import android.content.Context;

import com.bfmj.viewcore.adapter.GLBaseAdapter;
import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRectView;

public class WangDemoAdpater extends GLBaseAdapter {
	
	private ArrayList<Integer> list;
	private Context context;
	
	
	public WangDemoAdpater(Context c,ArrayList<Integer> dataList){
		list = dataList;		
		
		context = c;
	}

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return list.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return list.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}

	@Override
	public GLRectView getGLView(int position, GLRectView convertView, GLGroupView parent) {
		// TODO Auto-generated method stub
		
		TestRelativeLayout grv = (TestRelativeLayout)convertView;
		GLImageView image=null;
		if(grv == null){
			grv = new TestRelativeLayout(context);			
		}

		image = (GLImageView)grv.getView("timage");
		image.setImage(list.get(position));
		return grv;
	}

	@Override
	public void removeIndex(int index) {
		// TODO Auto-generated method stub
		list.remove(index);
	}

	@Override
	public void addIndex(int index, GLRectView view ) {
		// TODO Auto-generated method stub
//		list.add(index, (GLRelativeView)view);
	}

	
}
