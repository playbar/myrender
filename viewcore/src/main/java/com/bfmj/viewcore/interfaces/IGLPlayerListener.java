package com.bfmj.viewcore.interfaces;

import com.bfmj.viewcore.view.GLPlayerView;

/**
 * 
 * ClassName: IGLPlayerListener <br/>
 * @author lixianke    
 * @date: 2015-4-3 上午11:00:23 <br/>  
 * description:
 */
public interface IGLPlayerListener {
	void onPrepared(GLPlayerView player);
	boolean onInfo(GLPlayerView player, int what, Object extra);
	void onBufferingUpdate(GLPlayerView player, int percent);
	void onCompletion(GLPlayerView player);
	void onSeekComplete(GLPlayerView player);
	boolean onError(GLPlayerView player, int what, int extra);
	void onVideoSizeChanged(GLPlayerView player, int width, int height);
	void onTimedText(GLPlayerView player, String text);
}
