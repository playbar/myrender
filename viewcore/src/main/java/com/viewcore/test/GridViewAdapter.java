package com.viewcore.test;

import android.content.Context;

import com.bfmj.viewcore.adapter.GLBaseAdapter;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRectView;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.display.RoundedBitmapDisplayer;

import java.util.List;
import java.util.Map;

public class GridViewAdapter extends GLBaseAdapter {

	private List<? extends Map<String, ?>> list;
	private Context context;
	private DisplayImageOptions options;        // DisplayImageOptions是用于设置图片显示的类
	private ImageLoader imageLoader = ImageLoader.getInstance();

	//////////////

	GLViewFocusListener listener = new GLViewFocusListener() {

		@Override
		public void onFocusChange(GLRectView view, boolean focused) {
			if (focused){
				view.setBackground(new GLColor(0, 0, 1));
			} else {
				view.setBackground(new GLColor(1, 1, 1));
			}
		}
	};

	public GridViewAdapter(List<? extends Map<String, ?>> listData, Context c) {
		context = c;
		list = listData;

		imageLoader.init(ImageLoaderConfiguration.createDefault(c));

		// 使用DisplayImageOptions.Builder()创建DisplayImageOptions  
        options = new DisplayImageOptions.Builder()  
            .showStubImage(R.drawable.a1)          // 设置图片下载期间显示的图片
            .showImageForEmptyUri(R.drawable.a1)  // 设置图片Uri为空或是错误的时候显示的图片  
            .showImageOnFail(R.drawable.a1)       // 设置图片加载或解码过程中发生错误显示的图片      
            .cacheInMemory(true)                        // 设置下载的图片是否缓存在内存中  
            .cacheOnDisc(true)                          // 设置下载的图片是否缓存在SD卡中  
            .displayer(new RoundedBitmapDisplayer(20))  // 设置成圆角图片  
            .build();                                   // 创建配置过得DisplayImageOption对象  
	}

	@Override
	public void addIndex(int index, GLRectView view) {
		// TODO Auto-generated method stub

	}

	@Override
	public void removeIndex(int index) {
		// TODO Auto-generated method stub
		
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
		return 0;
	}

	@Override
	public GLRectView getGLView(int position, GLRectView convertView,
			GLGroupView parent) {
		// TODO Auto-generated method stub
		
//		TestRelativeLayout grv = (TestRelativeLayout)convertView;
//
//		if(grv == null){
//			grv = new TestRelativeLayout(context);
//		}

		GLImageView image = (GLImageView)convertView;
		if( image == null ){
			image = new GLImageView( context );
		}

		Map data = list.get( position );
		int imgid = (int)data.get("image");

		image.setImage(imgid);
		image.setWidth(300.0f);
		image.setHeight(300.0f);

		//image.setFocusListener( listener );

		
		/** 
         * 显示图片 
         * 参数1：图片url 
         * 参数2：显示图片的控件 
         * 参数3：显示图片的设置 
         * 参数4：监听器 
         */  
//		imageLoader.displayImage(list.get(position), iv, options, new ImageLoadingListener() {
//			
//			@Override
//			public void onLoadingStarted(String arg0, View arg1) {
//				// TODO Auto-generated method stub
//				
//			}
//			
//			@Override
//			public void onLoadingFailed(String arg0, View arg1, FailReason arg2) {
//				// TODO Auto-generated method stub
//				
//			}
//			
//			@Override
//			public void onLoadingComplete(String arg0, View arg1, Bitmap bitmap) {
//				// TODO Auto-generated method stub	
//				
//			}
//			
//			@Override
//			public void onLoadingCancelled(String arg0, View arg1) {
//				// TODO Auto-generated method stub
//				
//			}
//		});


		return image;

	}

	public boolean isEmpty() {return getCount() == 0;}

	public boolean areAllItemsEnabled() {
		return true;
	}

	public boolean isEnabled(int position) {
		return true;
	}


}
