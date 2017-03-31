package com.viewcore.test;

import android.text.TextUtils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class TimeFormat {

	/**
	 * 视频时长格式化
	 * @param time 时长（秒数）
	 */
	private static String format(int time, boolean isShowHour,
			boolean isShowZero, String[] spaces) {

		String strTime = "";

		long second = time % 60;

		strTime = (second < 10 ? "0" : "") + second + spaces[2];

		time = time / 60;

		if (time >= 1) {
			long minute = time % 60;
			strTime = (minute < 10 && isShowZero ? "0" : "") + minute
					+ spaces[1] + strTime;
		} else {
			strTime = (isShowZero ? "00" + spaces[1] : "") + strTime;
		}

		if (isShowHour) {
			time = time / 60;

			if (time >= 1) {
				strTime = (time < 10 && isShowZero ? "0" : "") + time
						+ spaces[0] + strTime;
			} else {
				strTime = (isShowZero ? "00" + spaces[0] : "") + strTime;
			}

		}

		return strTime;
	}

	public static String format(int time) {
		return format(time, true, true, new String[] { ":", ":", "" });
	}
	public static String format(int time,boolean showZero) {
		return format(time, true, showZero, new String[] { ":", ":", "" });
	}
	public static String formatHideHour(int time) {
		return format(time, false, true, new String[] { ":", ":", "" });
	}

	public static String formatHideHourAndShowCN(int time) {
		return format(time, false, true, new String[] { "时", "分", "秒" });
	}

	public static String formatCH(int time) {
		return format(time, true, false, new String[] { "时", "分", "秒" });
	}

	public static String formatCH(int time, boolean isshowzero) {
		return format(time, true, isshowzero, new String[] { "时", "分", "秒" });
	}

	/**
	 * 根据秒数获取天数
	 * @param time 秒
	 * @return
	 */
	public static String formatDay(int time) {
		if (time <= 0)
			return "";
		if (time < 24 * 60 * 60) {
			return "";
		}
		int day = time / 60 / 60 / 24;
		return day + "天";
	}

	/**
	 * 把long型的时间格式化
	 * @param time
	 * @param fromate yyyy-MM-dd HH:mm:ss 定义格式类型
	 * @return
	 */
	public static String getFormatDate(long time, String fromate) {
		SimpleDateFormat format = new SimpleDateFormat(fromate);
		Date date = new Date(time);
		String formattime = format.format(date);
		return formattime;
	}

	/**
	 * 根据年月日获取毫秒级时间
	 * @param year
	 * @param month
	 * @param day
	 * @param hour
	 * @param minute
	 * @param second
	 * @return
	 */
	public static long getLongTime(int year, int month, int day, int hour,
			int minute, int second) {
		Calendar c = Calendar.getInstance();
		c.set(Calendar.YEAR, year);
		c.set(Calendar.MONTH, month);
		c.set(Calendar.DAY_OF_MONTH, day);
		c.set(Calendar.HOUR_OF_DAY, hour);
		c.set(Calendar.MINUTE, minute);
		c.set(Calendar.SECOND, second);
		return c.getTimeInMillis();
	}

	/**
	 * 根据日期格式的时间获取毫秒级时间
	 * @param formatdate yyyy-MM-dd HH:mm:ss 格式时间
	 * @return
	 */
	public static long getLongTime(String formatdate) {
		if (TextUtils.isEmpty(formatdate)) {
			return 0;
		}
		String[] datetime = formatdate.split(" ");
		String[] date = datetime[0].split("-");
		String[] time = datetime[1].split(":");
		int year = Integer.parseInt(date[0]);
		int month = Integer.parseInt(date[1]);
		int day = Integer.parseInt(date[2]);
		int hour = Integer.parseInt(time[0]);
		int minute = Integer.parseInt(time[1]);
		int second = Integer.parseInt(time[2]);
		return getLongTime(year, month, day, hour, minute, second);
	}

	public static long getLongFormat(String strTime) {
		SimpleDateFormat simpleDateFormat = new SimpleDateFormat(
				"yyyy-MM-dd HH:mm:ss");

		long time = 0;
		try {
			time = simpleDateFormat.parse(strTime).getTime();
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return time;
	}

	public static String getLongFormatClipSec(String strTime) {
		SimpleDateFormat simpleDateFormat = new SimpleDateFormat(
				"yyyy-MM-dd HH:mm:ss");
		SimpleDateFormat simpleDateFormatClipSec = new SimpleDateFormat(
				"yyyy-MM-dd HH:mm");

		String shortTime = strTime;
		try {
			Date time = simpleDateFormat.parse(strTime);

			shortTime = simpleDateFormatClipSec.format(time);

		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return shortTime;
	}

}
