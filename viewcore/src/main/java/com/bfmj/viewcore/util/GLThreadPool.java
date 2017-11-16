package com.bfmj.viewcore.util;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * Created by mac on 16/8/25.
 */
public class GLThreadPool {

    private static ThreadPoolExecutor mExecutor = new ThreadPoolExecutor(1, 1, 200,
            TimeUnit.MILLISECONDS,
            new ArrayBlockingQueue<Runnable>(10000),
            new ThreadPoolExecutor.DiscardOldestPolicy());

    public static ThreadPoolExecutor getThreadPool(){
        return mExecutor;
    }

}
