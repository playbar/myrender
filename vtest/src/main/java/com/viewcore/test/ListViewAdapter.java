package com.viewcore.test;

import java.util.List;

import com.bfmj.viewcore.adapter.GLBaseAdapter;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLTextView;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.display.RoundedBitmapDisplayer;

import android.content.Context;
import android.util.Log;

public class ListViewAdapter extends GLBaseAdapter {

	private List<Integer> list;
	private Context context;
	private DisplayImageOptions options;        // DisplayImageOptions是用于设置图片显示的类
	private ImageLoader imageLoader = ImageLoader.getInstance();  
//	private ImageView iv;
	
	public ListViewAdapter(List<Integer> listData, Context c) {
		context = c;
		list = listData;
//		mAq = aq;
		
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

		ChannelRightItemView grv = (ChannelRightItemView)convertView;
		
		if(grv == null){
			grv = new ChannelRightItemView(context);
		}

		final GLImageView image = (GLImageView)grv.getView("leftIcon");
		image.setImage(list.get(position));
		image.setFocusListener(new GLViewFocusListener() {
			@Override
			public void onFocusChange(GLRectView view, boolean focused) {
				Log.e("ListViewAdapter", "focuseed is " + focused );
			}
		});

		GLTextView title = (GLTextView)grv.getView("title");
		if( title != null) {
			title.setText("test");
		}
		GLTextView subtitle = (GLTextView)grv.getView("subtitle");
		if( subtitle != null) {
			subtitle.setText("testtest");
		}
//		text.setText("111");
//		image.setWidth( 200 );
//		image.setHeight( 200 );


		return grv;

//		grv.setFocusListener(new GLViewFocusListener() {
//			@Override
//			public void onFocusChange(GLRectView view, boolean focused) {
//				Log.e("ListViewAdapter", "focuseed is " + focused );
//			}
//		});
//
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
		//ImageView iv = new ImageView(context);
//		mAq.id(iv).image(list.get(position), true, true, 0, 0, new BitmapAjaxCallback(){
//
//	        @Override
//	        public void callback(String url, ImageView iv, Bitmap bm,  AjaxStatus status){
//
//	        	Log.d("iiii",url);
//
//				final Bitmap _bitmap = bm;
//				(((BaseViewActivity)context).getRootView()).queueEvent(new Runnable() {
//					@Override
//					public void run() {
//						image.setImage(_bitmap);
////						_bitmap.recycle();
//					}
//				});
//
//	 	       }
//
//		});
		
		
//		mAq.im
		
//		image.setImage(list.get(position));
//		Log.d("bbbbb",list.get(position) + "");
//		return grv;
		
	}
	
	

}
