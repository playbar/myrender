//------------------------------------------------------------------------------
// Copyright 2016 Baofeng Mojing Inc. All rights reserved.
//------------------------------------------------------------------------------

using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System;
using System.IO;
public class FPS : MonoBehaviour {

    private float fps = 60;
    public Text text1;
    private int count = 60;
    int battery_capacity;
    string current_electric;
    string gpu_frequency;
    string cpu_frequency;
    string gpu_temperature;
    string cpu_temperature;

    void Update()
    {
        count++;
        float interp = Time.deltaTime / (0.5f + Time.deltaTime);
        float currentFPS = 1.0f / Time.deltaTime;
        if (count >= 10)
        {
            fps = Mathf.Lerp(fps, currentFPS, interp);
            battery_capacity = GetBatteryCapacity();
            current_electric = GetElectricCurrent();
            gpu_frequency = GetGPUFreq();
            cpu_frequency = GetCPUFreq();
            gpu_temperature = GetGPUTemperature();
            cpu_temperature = GetCPUTemperature();
            text1.text = "Fps:" + Mathf.RoundToInt(fps)
                + ", " + "Time:" + Mathf.RoundToInt(Time.time)
                + "\n" + "Cap:" + battery_capacity
                + ", " + "Current：" + current_electric
                + "\n" + "GPUF：" + gpu_frequency
                + ", " + "GPUT：" + gpu_temperature
                + "\n" + "CPUF：" + cpu_frequency
                + "\n" + "CPUT：" + cpu_temperature;
            //MojingLog.LogTrace("帧率：" + Mathf.RoundToInt(fps)
            //    + ",时长：" + Mathf.RoundToInt(Time.time)
            //    + ",电量：" + battery_capacity
            //    + ",电流：" + current_electric
            //    + ",GPU频率：" + gpu_frequency
            //    + ",GPU温度：" + gpu_temperature
            //    + ",CPU频率：" + cpu_frequency
            //    + ",CPU温度：" + cpu_temperature);
            count = 0;
        }
    }

    int GetBatteryCapacity()
    {
        try
        {
            string ValueString = File.ReadAllText("/sys/class/power_supply/battery/capacity");
            return int.Parse(ValueString);
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read battery power; " + e.Message);
        }
        return -1;
    }

    string GetElectricCurrent()
    {
        try
        {
            string ValueString = File.ReadAllText("/sys/class/power_supply/battery/current_now");
            return ValueString.Trim();
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read battery temperature; " + e.Message);
        }
        return "Unknwon";
    }
    int GetBatteryTemperature()
    {
        try
        {
            string ValueString = File.ReadAllText("/sys/class/power_supply/battery/temp");
            return int.Parse(ValueString);
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read battery temperature; " + e.Message);
        }
        return -1;
    }

    string GetBatteryStatus()
    {
        try
        {
            return File.ReadAllText("/sys/class/power_supply/battery/status").Trim();
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read battery status; " + e.Message);
        }
        return "";
    }

    string GetCPUPresent()
    {
        try
        {
            return File.ReadAllText("/sys/devices/system/cpu/present").Trim();
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read cpu present; " + e.Message);
        }
        return "";
    }

    string GetGPUFreq()
    {
        try
        {
            string strRet = ("");
            string ValueString = File.ReadAllText("/sys/class/kgsl/kgsl-3d0/gpuclk");
            double dFreq = Math.Round(int.Parse(ValueString) / 1e6f, 2);
            strRet += dFreq;
            return strRet;
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read GPU freq; " + e.Message);
        }
        return "Unknown";
    }
    string GetGPUTemperature()
    {
        try
        {
            //string ValueString0 = System.IO.File.ReadAllText("/sys/class/thermal/thermal_zone0/temp");
            string ValueString0 = File.ReadAllText("/sys/devices/virtual/thermal/thermal_zone16/temp");
            return ValueString0.Trim();
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read cpu temperature; " + e.Message);
        }
        return "Unknown";
    }

    string GetCPUFreq()
    {
        try
        {
            string strRet = ("");
            for (int n = 0; n < 4; n++)
            {
                string ValueString = File.ReadAllText("/sys/devices/system/cpu/cpu" + n + "/cpufreq/scaling_cur_freq");
                double dFreq = Math.Round(int.Parse(ValueString) / 1e6f, 2);

                if (strRet.Length > 0)
                    strRet += " | ";

                strRet += dFreq;
            }

            return strRet;
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read cpu freq; " + e.Message);
        }
        return "Unknown";
    }

    string GetCPUTemperature()
    {
        try
        {
            //string ValueString0 = System.IO.File.ReadAllText("/sys/class/thermal/thermal_zone0/temp");
            string ValueString0 = File.ReadAllText("/sys/devices/virtual/thermal/thermal_zone3/temp");
            string ValueString1 = File.ReadAllText("/sys/devices/virtual/thermal/thermal_zone9/temp");
            return ValueString0.Trim() + " | " + ValueString1.Trim();
        }
        catch (Exception e)
        {
            Debug.Log("Failed to read cpu temperature; " + e.Message);
        }
        return "Unknown";
    }
}
