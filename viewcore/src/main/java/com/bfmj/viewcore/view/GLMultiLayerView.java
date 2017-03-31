package com.bfmj.viewcore.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;

import com.bfmj.viewcore.entity.LayerInfo;
import com.bfmj.viewcore.entity.TextInfo;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.util.GLFontUtils;
import com.bfmj.viewcore.util.GLThreadPool;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

/**
 * Created by lixianke on 2016/9/12.
 */
public class GLMultiLayerView extends GLRelativeView {
    private ArrayList<LayerInfo> mLayerInfos;

    public GLMultiLayerView(Context context) {
        super(context);
    }

    /**
     * 设置资源列表
     * @param layerInfos
     */
    public void setLayerInfos(ArrayList<LayerInfo> layerInfos){
        if (layerInfos != null){
            mLayerInfos = layerInfos;
            updateUI();
        }
    }

    /**
     * 获取资源列表
     * @return
     */
    public ArrayList<LayerInfo> getLayerInfos(){
        return mLayerInfos;
    }

    /**
     * 更新UI，重新生成Bitmap
     */
    public void updateUI(){
        GLThreadPool.getThreadPool().execute(new Runnable() {
            @Override
            public void run() {
                Bitmap bitmap = createBitmap(mLayerInfos);
                if (bitmap != null){
                    setBackground(bitmap);
                }
            }
        });
    }

    /**
     * 根据layerinfo list生成bitmap
     * @param layerInfos
     * @return
     */
    private Bitmap createBitmap(ArrayList<LayerInfo> layerInfos){
        Bitmap bitmap = null;
        //初始化画布
        int width = (int)getWidth();
        int height = (int)getHeight();

        if (width > 0 && height > 0) {
            bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(bitmap);
            canvas.drawColor(Color.TRANSPARENT);

            for (int i = 0; i < layerInfos.size(); i++) {
                LayerInfo layerInfo = layerInfos.get(i);
                if (layerInfo != null) {
                    drawLayer(canvas, layerInfo);
                }
            }
        }
        return bitmap;
    }

    /**
     * 根据类型绘制
     * @param canvas
     * @param layerInfo
     */
    private void drawLayer(Canvas canvas, LayerInfo layerInfo){
        Rect rect = layerInfo.getRect();
        if (rect == null){
            rect = new Rect(0, 0, (int)getWidth(), (int)getHeight());
        }

        switch (layerInfo.getType()){
            case TYPE_RESOURCE_ID:
                Bitmap rBitmap = createBitmap(layerInfo.getResourceId());
                canvas.drawBitmap(rBitmap, new Rect(0, 0, rBitmap.getWidth(), rBitmap.getHeight()),
                        rect, new Paint());
                rBitmap.recycle();
                break;
            case TYPE_BITMAP:
                Bitmap dBitmap = layerInfo.getBitmap();
                canvas.drawBitmap(dBitmap, new Rect(0, 0, dBitmap.getWidth(), dBitmap.getHeight()),
                        rect, new Paint());
                break;
            case TYPE_COLOR:
                Paint p = new Paint();
                p.setStyle(Paint.Style.FILL);
                GLColor color = layerInfo.getColor();
                p.setARGB((int)(color.getA() * 255.0F), (int)(color.getR() * 255.0F), (int)(color.getG() * 255.0F), (int)(color.getB() * 255.0F));
                canvas.drawRect(rect, p);
                break;
            case TYPE_TEXT:
                TextInfo info = layerInfo.getTextInfo();
                drawText(canvas, info, rect);
                break;
        }
    }

    /**
     * 绘制文字
     * @param canvas
     * @param info
     * @param rect
     */
    private void drawText(Canvas canvas, TextInfo info, Rect rect){
        GLColor tColor = info.getColor();
        TextPaint tp = new TextPaint();
        tp.setARGB((int)(tColor.getA() * 255.0F), (int)(tColor.getR() * 255.0F), (int)(tColor.getG() * 255.0F), (int)(tColor.getB() * 255.0F));
        tp.setTypeface(GLFontUtils.getInstance(this.getContext()).getFontTypeface());
        tp.setTextSize(info.getSize());
        // 先将文字绘制到临时的canvas上
        int w = (rect.right - rect.left);
        int h = (rect.bottom - rect.top);
        if (w > 0 && h > 0){
            Bitmap bm = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
            Canvas cv = new Canvas(bm);
            cv.drawColor(Color.TRANSPARENT);

            float lineHeight = info.getLineHeight() > 0 ? (float)info.getLineHeight() / info.getSize() : 1.2f;
            StaticLayout sl = new StaticLayout(info.getContent(), tp, w, info.getAlignment(), lineHeight, 0.0F, true);
            sl.draw(cv);

            canvas.drawBitmap(bm, new Rect(0, 0, bm.getWidth(), bm.getHeight()),
                    rect, new Paint());

            bm.recycle();
        }
    }

    /**
     * 根据资源id生成Bitmap
     * @param resId
     * @return
     */
    private Bitmap createBitmap(int resId) {
        Bitmap bm = null;
        if(resId != 0) {
            InputStream is = this.getContext().getResources().openRawResource(resId);

            try {
                bm = BitmapFactory.decodeStream(is);
            } finally {
                try {
                    is.close();
                } catch (IOException var10) {
                    var10.printStackTrace();
                }

            }
        }

        return bm;
    }
}
