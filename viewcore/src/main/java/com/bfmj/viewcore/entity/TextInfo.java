package com.bfmj.viewcore.entity;

import android.text.Layout;

import com.bfmj.viewcore.render.GLColor;

import java.io.Serializable;

/**
 * Created by lixianke on 2016/9/13.
 */
public class TextInfo implements Serializable {
    public static final Layout.Alignment ALIGN_LEFT  = Layout.Alignment.ALIGN_NORMAL;
    public static final Layout.Alignment ALIGN_CENTER  = Layout.Alignment.ALIGN_CENTER;
    public static final Layout.Alignment ALIGN_RIGHT  = Layout.Alignment.ALIGN_OPPOSITE;

    private String content;
    private int size = 32;
    private GLColor color = new GLColor(0xffffff);
    private int lineHeight;
    private Layout.Alignment alignment = ALIGN_LEFT;

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public int getSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public GLColor getColor() {
        return color;
    }

    public void setColor(GLColor color) {
        this.color = color;
    }

    public int getLineHeight() {
        return lineHeight;
    }

    public void setLineHeight(int lineHeight) {
        this.lineHeight = lineHeight;
    }

    public Layout.Alignment getAlignment() {
        return alignment;
    }

    public void setAlignment(Layout.Alignment alignment) {
        this.alignment = alignment;
    }
}
