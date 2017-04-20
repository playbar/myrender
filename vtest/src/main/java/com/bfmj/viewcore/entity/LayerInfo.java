package com.bfmj.viewcore.entity;

import android.graphics.Bitmap;
import android.graphics.Rect;

import com.bfmj.viewcore.render.GLColor;

import java.io.Serializable;

/**
 * Created by lixianke on 2016/9/12.
 */
public class LayerInfo implements Serializable {
    public enum LayerType {
        TYPE_RESOURCE_ID, TYPE_BITMAP, TYPE_COLOR, TYPE_TEXT
    }

    private LayerType type;
    private int resourceId;
    private Bitmap bitmap;
    private boolean mbRecyle = false;
    private GLColor color;
    private TextInfo textInfo;
    private Rect rect;

    public LayerType getType() {
        return type;
    }

    public void setType(LayerType type) {
        this.type = type;
    }

    public int getResourceId() {
        return resourceId;
    }

    public void setResourceId(int resourceId) {
        this.resourceId = resourceId;
    }

    public Bitmap getBitmap() {
        return bitmap;
    }

    public boolean getBmpRecyle(){
        return mbRecyle;
    }

    public void setBitmap(Bitmap bitmap) {
        this.bitmap = bitmap;
    }

    public void setBitmap(Bitmap bitmap, boolean recyle) {
        this.bitmap = bitmap;
        mbRecyle = recyle;
    }

    public GLColor getColor() {
        return color;
    }

    public void setColor(GLColor color) {
        this.color = color;
    }

    public TextInfo getTextInfo() {
        return textInfo;
    }

    public void setTextInfo(TextInfo textInfo) {
        this.textInfo = textInfo;
    }

    public Rect getRect() {
        return rect;
    }

    public void setRect(Rect rect) {
        this.rect = rect;
    }
}
