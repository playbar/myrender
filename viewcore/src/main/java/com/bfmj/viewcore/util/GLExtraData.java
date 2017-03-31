package com.bfmj.viewcore.util;

import java.util.HashMap;

/**
 * 
 * ClassName: GLIntent <br/>
 * @author lixianke    
 * @date: 2015-3-24 上午11:09:54 <br/>  
 * description:
 */
public class GLExtraData {
	private HashMap<String, Object> mHashMap = new HashMap<String, Object>();
	
	public GLExtraData(){
	}
	
	public void putExtraString(String key, String value){
		mHashMap.put(key, value);
	}
	
	public void putExtraInt(String key, int value){
		mHashMap.put(key, value);
	}
	
	public void putExtraLong(String key, Long value){
		mHashMap.put(key, value);
	}
	
	public void putExtraBoolean(String key, Boolean value){
		mHashMap.put(key, value);
	}
	
	public void putExtraObject(String key, Object value){
		mHashMap.put(key, value);
	}
	
	public void putExtraHashMap(String key, HashMap<String, Object> value){
		mHashMap.put(key, value);
	}
	
	public String getExtraString(String key){
		return getExtraString(key, "");
	}
	
	public String getExtraString(String key, String defaultValue){
		return mHashMap.get(key) != null ? mHashMap.get(key).toString() : defaultValue;
	}
	
	public int getExtraInt(String key){
		return getExtraInt(key, 0);
	}
	
	public int getExtraInt(String key, int defaultValue){
		return mHashMap.get(key) != null ? Integer.parseInt(mHashMap.get(key).toString()) : defaultValue;
	}
	
	public Long getExtraLong(String key){
		return getExtraLong(key, 0L);
	}
	
	public Long getExtraLong(String key, Long defaultValue){
		return mHashMap.get(key) != null ? Long.parseLong(mHashMap.get(key).toString()) : defaultValue;
	}
	
	public boolean getExtraBoolean(String key){
		return getExtraBoolean(key, false);
	}
	
	public boolean getExtraBoolean(String key, boolean defaultValue){
		return mHashMap.get(key) != null ? Boolean.parseBoolean(mHashMap.get(key).toString()) : defaultValue;
	}
	
	public Object getExtraObject(String key){
		return mHashMap.get(key);
	}
	
	@SuppressWarnings("unchecked")
	public HashMap<String, Object> getExtraHashMap(String key){
		return (HashMap<String, Object>)mHashMap.get(key);
	}
}
