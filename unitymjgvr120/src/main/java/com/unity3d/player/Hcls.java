//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Dialog;
import android.content.Context;
import android.graphics.drawable.ColorDrawable;
import android.text.Editable;
import android.text.Selection;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.ViewGroup.LayoutParams;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import com.unity3d.player.UnityPlayer;

public final class Hcls extends Dialog implements TextWatcher, OnClickListener {
    private Context a = null;
    private UnityPlayer b = null;
    private static int c = 1627389952;
    private static int d = -1;

    public Hcls(Context var1, UnityPlayer var2, String var3, int var4, boolean var5, boolean var6, boolean var7, String var8) {
        super(var1);
        this.a = var1;
        this.b = var2;
        this.getWindow().setGravity(80);
        this.getWindow().requestFeature(1);
        this.getWindow().setBackgroundDrawable(new ColorDrawable(0));
        this.setContentView(this.createSoftInputView());
        this.getWindow().setLayout(-1, -2);
        this.getWindow().clearFlags(2);
        EditText var9 = (EditText)this.findViewById(1057292289);
        Button var10 = (Button)this.findViewById(1057292290);
        this.a(var9, var3, var4, var5, var6, var7, var8);
        var10.setOnClickListener(this);
        var9.setOnFocusChangeListener(new OnFocusChangeListener() {
            public final void onFocusChange(View var1, boolean var2) {
                if(var2) {
                    Hcls.this.getWindow().setSoftInputMode(5);
                }

            }
        });
    }

    private void a(EditText var1, String var2, int var3, boolean var4, boolean var5, boolean var6, String var7) {
        var1.setImeOptions(6);
        var1.setText(var2);
        var1.setHint(var7);
        var1.setHintTextColor(c);
        var1.setInputType(a(var3, var4, var5, var6));
        var1.addTextChangedListener(this);
        var1.setClickable(true);
        if(!var5) {
            var1.selectAll();
        }

    }

    public final void afterTextChanged(Editable var1) {
        this.b.reportSoftInputStr(var1.toString(), 0, false);
    }

    public final void beforeTextChanged(CharSequence var1, int var2, int var3, int var4) {
    }

    public final void onTextChanged(CharSequence var1, int var2, int var3, int var4) {
    }

    private static int a(int var0, boolean var1, boolean var2, boolean var3) {
        int var4 = (var1?'耀':0) | (var2?131072:0) | (var3?128:0);
        if(var0 >= 0 && var0 <= 7) {
            int[] var5 = new int[]{1, 16385, 12290, 17, 2, 3, 97, 33};
            return var4 | var5[var0];
        } else {
            return var4;
        }
    }

    private void a(String var1, boolean var2) {
        Selection.removeSelection(((EditText)this.findViewById(1057292289)).getEditableText());
        this.b.reportSoftInputStr(var1, 1, var2);
    }

    public final void onClick(View var1) {
        this.a(this.a(), false);
    }

    public final void onBackPressed() {
        this.a(this.a(), true);
    }

    protected final View createSoftInputView() {
        RelativeLayout var1;
        (var1 = new RelativeLayout(this.a)).setLayoutParams(new LayoutParams(-1, -1));
        var1.setBackgroundColor(d);
        EditText var3 = new EditText(this.a) {
            public final boolean onKeyPreIme(int var1, KeyEvent var2) {
                if(var1 == 4) {
                    Hcls.this.a(Hcls.this.a(), true);
                    return true;
                } else {
                    return var1 == 84?true:super.onKeyPreIme(var1, var2);
                }
            }

            public final void onWindowFocusChanged(boolean var1) {
                super.onWindowFocusChanged(var1);
                if(var1) {
                    ((InputMethodManager)Hcls.this.a.getSystemService("input_method")).showSoftInput(this, 0);
                }

            }
        };
        android.widget.RelativeLayout.LayoutParams var2;
        (var2 = new android.widget.RelativeLayout.LayoutParams(-1, -2)).addRule(15);
        var2.addRule(0, 1057292290);
        var3.setLayoutParams(var2);
        var3.setId(1057292289);
        var1.addView(var3);
        Button var4;
        (var4 = new Button(this.a)).setText(this.a.getResources().getIdentifier("ok", "string", "android"));
        (var2 = new android.widget.RelativeLayout.LayoutParams(-2, -2)).addRule(15);
        var2.addRule(11);
        var4.setLayoutParams(var2);
        var4.setId(1057292290);
        var4.setBackgroundColor(0);
        var1.addView(var4);
        ((EditText)var1.findViewById(1057292289)).setOnEditorActionListener(new OnEditorActionListener() {
            public final boolean onEditorAction(TextView var1, int var2, KeyEvent var3) {
                if(var2 == 6) {
                    Hcls.this.a(Hcls.this.a(), false);
                }

                return false;
            }
        });
        var1.setPadding(16, 16, 16, 16);
        return var1;
    }

    private String a() {
        EditText var1;
        return (var1 = (EditText)this.findViewById(1057292289)) == null?null:var1.getText().toString().trim();
    }

    public final void a(String var1) {
        EditText var2;
        if((var2 = (EditText)this.findViewById(1057292289)) != null) {
            var2.setText(var1);
            var2.setSelection(var1.length());
        }

    }
}
