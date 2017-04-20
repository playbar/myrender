package com.bfmj.viewcore.adapter;

import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLRectView;


public interface GLSpinnerAdapter extends GLAdapter {
	public GLRectView getDropDownView(int position, GLRectView convertView, GLGroupView parent);
}
