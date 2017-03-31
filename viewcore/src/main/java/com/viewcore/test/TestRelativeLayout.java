package com.viewcore.test;

import android.content.Context;

import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRelativeView;

public class TestRelativeLayout extends GLRelativeView {

	public TestRelativeLayout(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		
		this.setWidth(150);
		this.setHeight(200);
		
		GLImageView image = new GLImageView(context);
		image.setId("timage");
		image.setWidth(150.0f);
		image.setHeight(200.0f);
		this.addView(image);
	}

}
