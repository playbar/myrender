package com.bfmj.viewcore.interfaces;

import com.bfmj.viewcore.view.GLRectView;

public interface GLOnKeyListener {
	boolean onKeyDown(GLRectView view, int keycode);
	boolean onKeyUp(GLRectView view, int keycode);
	boolean onKeyLongPress(GLRectView view, int keycode);
}
