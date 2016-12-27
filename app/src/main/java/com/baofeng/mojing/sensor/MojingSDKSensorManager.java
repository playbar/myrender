package com.baofeng.mojing.sensor;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

import com.baofeng.mojing.MojingSDK;
import com.baofeng.mojing.MojingSDKServiceManager;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;

/**
 * 传感器监听
 * Created by liumansong on 2016/7/20.
 */
public class MojingSDKSensorManager {
    private static final String TAG = "MojingSDKSensorManager";
    private static MojingSDKSensorManager instance;
    //速度传感器
    private Sensor accelerometerSensor;
    //陀螺仪传感器
    private Sensor gyroscopeSensor;
    //地磁传感器
    private Sensor magneticFieldSensor;
    //原始地磁传感器
    private Sensor magneticFieldUncalibratedSensor;
    //传感器数据拼装
    private float[] sensorArr = null;
    //是否有校正后的地磁数据
    boolean hasMagneticField = false;
    //子线程
    private HandlerThread handlerThread;
    //子线程Handler
    private Handler threadHandler;
	//保持当前Context
	private Context m_context = null;
    //是否使用Java层取得的Sensor数据
	//private boolean bUseJavaSensor = false;
	//测试用
    private StringBuilder stringBuilder = new StringBuilder();

    private MojingSDKSensorManager() {
		initHandler();
    }

    /**
     * 获取用户配置
     *
     * @return
     */
    public static boolean useJavaSensor() {
		boolean bUseJavaSensor = false;
        try {
            String json = MojingSDK.GetUserSettings();
			Log.i(TAG, "MojingSDK GetUserSettings = " + json);
            JSONObject jsonObject = new JSONObject(json);
            if (jsonObject.has("SensorDataFromJava")) {
                if (jsonObject.getInt("SensorDataFromJava") == 1) {
                    bUseJavaSensor = true;
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return bUseJavaSensor;
    }

    public static MojingSDKSensorManager getInstance() {
        if (instance == null) {
            instance = new MojingSDKSensorManager();
        }
        return instance;
    }

    
    public static void RegisterSensor(Context context)
    {
        //Log.i(TAG, "RegisterSensor");	
    	MojingSDKSensorManager.getInstance().register(context);
    }

    public static void UnRegisterSensor(Context context)
    {
        //Log.i(TAG, "UnRegisterSensor");		
    	MojingSDKSensorManager.getInstance().unRegister(context);
    }

    /**
     * 初始化子线程
     */
    private void initHandler() {
        handlerThread = new HandlerThread("jerome");
        handlerThread.start();
        threadHandler = new Handler(handlerThread.getLooper()) {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                MojingSDK.SendSensorData((float[]) msg.obj, System.nanoTime() / 1000000000D);
				if(m_context != null && MojingSDKServiceManager.isServiceTracker())
				{
					unRegister(m_context);
					m_context = null;
				}
            }
        };
    }

    private void register(Context context) {
        if (!useJavaSensor()) {
            Log.i(TAG, "useJavaSensor=false");
            return;
        }
        Log.i(TAG, "useJavaSensor=true");
        hasMagneticField = false;
        sensorArr = new float[12];
        SensorManager sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        //list(sensorManager);
//        unRegister(context);
        register(sensorManager);
		m_context = context;
    }

    /**
     * 更新数据
     *
     * @param sensorType
     */
    private int what = 0;

    private void pushData(int sensorType, float[] data) {
        if (sensorArr == null) {
            sensorArr = new float[12];
        }
        switch (sensorType) {
            case Sensor.TYPE_ACCELEROMETER:
                sensorArr[0] = data[0];
                sensorArr[1] = data[1];
                sensorArr[2] = data[2];
                break;
            case Sensor.TYPE_GYROSCOPE:
                stringBuilder = new StringBuilder();
                sensorArr[3] = data[0];
                sensorArr[4] = data[1];
                sensorArr[5] = data[2];
                /*Log.i(TAG, "------");
                int i = 0;
                for (float floats : sensorArr) {
                    stringBuilder.append(i++ + "=" + floats + ",");
                }
                Log.i(TAG, stringBuilder.toString());
                Log.i(TAG, "second d=" + System.nanoTime() / 1000000000D);*/
//                MojingSDK.SendSensorData(sensorArr, System.nanoTime() / 1000000000D);
                if (threadHandler != null && handlerThread.isAlive()) {
                    threadHandler.obtainMessage(what++, sensorArr).sendToTarget();
                }
                break;
            case Sensor.TYPE_MAGNETIC_FIELD:
                hasMagneticField = true;
                sensorArr[6] = data[0] / 100;
                sensorArr[7] = data[1] / 100;
                sensorArr[8] = data[2] / 100;
                break;
            case Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                if (!hasMagneticField) {
                    sensorArr[6] = data[0] / 100;
                    sensorArr[7] = data[1] / 100;
                    sensorArr[8] = data[2] / 100;
                }
                sensorArr[9] = data[3] / 100;
                sensorArr[10] = data[4] / 100;
                sensorArr[11] = data[5] / 100;
                break;
        }
    }

    /**
     * 注册监听
     *
     * @param sensorManager
     */
    private void register(SensorManager sensorManager) {
        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        gyroscopeSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        magneticFieldSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        magneticFieldUncalibratedSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED);
        //注册速度传感器
        sensorManager.registerListener(accelerometerListrner, accelerometerSensor, SensorManager.SENSOR_DELAY_FASTEST);
        //注册陀螺仪传感器
        sensorManager.registerListener(gyroscopeListrner, gyroscopeSensor, SensorManager.SENSOR_DELAY_FASTEST);
        //注册地磁传感器
        sensorManager.registerListener(magneticFieldListrner, magneticFieldSensor, SensorManager.SENSOR_DELAY_FASTEST);
        //注册原始地磁传感器
        sensorManager.registerListener(magneticFieldUncalibratedListrner, magneticFieldUncalibratedSensor, SensorManager.SENSOR_DELAY_FASTEST);
    }

    /**
     * 解绑
     *
     * @param context
     */
    private void unRegister(Context context) {
        if (handlerThread != null) {
            handlerThread.quit();
            handlerThread = null;
            threadHandler = null;
        }
        SensorManager sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);

        if (accelerometerListrner != null) {
            sensorManager.unregisterListener(accelerometerListrner);
            accelerometerListrner = null;
        }

        if (gyroscopeListrner != null) {
            sensorManager.unregisterListener(gyroscopeListrner);
            gyroscopeListrner = null;
        }

        if (magneticFieldListrner != null) {
            sensorManager.unregisterListener(magneticFieldListrner);
            magneticFieldListrner = null;
        }

        if (magneticFieldUncalibratedListrner != null) {
            sensorManager.unregisterListener(magneticFieldUncalibratedListrner);
            magneticFieldUncalibratedListrner = null;
        }
        instance = null;
    }

    /**
     * 查看手机支持传感器类型
     *
     * @param sensorManager
     */
    private void list(SensorManager sensorManager) {
        // 获取全部传感器列表
        List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ALL);

        // 打印每个传感器信息
        StringBuilder strLog = new StringBuilder();
        int iIndex = 1;
        for (Sensor item : sensors) {
            strLog.append(iIndex + ".");
            strLog.append("	Sensor Type - " + item.getType() + "\r\n");
            strLog.append("	Sensor Name - " + item.getName() + "\r\n");
            strLog.append("	Sensor Version - " + item.getVersion() + "\r\n");
            strLog.append("	Sensor Vendor - " + item.getVendor() + "\r\n");
            strLog.append("	Maximum Range - " + item.getMaximumRange() + "\r\n");
            strLog.append("	Minimum Delay - " + item.getMinDelay() + "\r\n");
            strLog.append("	Power - " + item.getPower() + "\r\n");
            strLog.append("	Resolution - " + item.getResolution() + "\r\n");
            strLog.append("\r\n");
            iIndex++;
        }
        Log.i("adjustSensor", strLog.toString());

    }

    /**
     * 速度传感器监听
     */
    private SensorEventListener accelerometerListrner = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            pushData(Sensor.TYPE_ACCELEROMETER, event.values);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            Log.i("adjustSensor", "accelerometerListrner " + sensor.toString() + ",accuracy=" + accuracy);

        }
    };

    /**
     * 陀螺仪传感器监听
     */
    private SensorEventListener gyroscopeListrner = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            pushData(Sensor.TYPE_GYROSCOPE, event.values);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            Log.i("adjustSensor", "gyroscopeListrner " + sensor.toString() + ",accuracy=" + accuracy);

        }
    };

    /**
     * 地磁传感器监听
     */
    private SensorEventListener magneticFieldListrner = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            pushData(Sensor.TYPE_MAGNETIC_FIELD, event.values);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            Log.i("adjustSensor", "magneticFieldListrner " + sensor.toString() + ",accuracy=" + accuracy);

        }
    };

    /**
     * 原始地磁传感器监听
     */
    private SensorEventListener magneticFieldUncalibratedListrner = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            pushData(Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED, event.values);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            Log.i("adjustSensor", "magneticFieldUncalibratedListrner " + sensor.toString() + ",accuracy=" + accuracy);
        }
    };

}
