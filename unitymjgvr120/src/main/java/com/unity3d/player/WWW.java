//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLSocketFactory;

class WWW extends Thread {
    private int a;
    private int b;
    private String c;
    private byte[] d;
    private Map e;

    WWW(int var1, String var2, byte[] var3, Map var4) {
        this.b = var1;
        this.c = var2;
        this.d = var3;
        this.e = var4;
        this.a = 0;
    }

    public void run() {
        try {
            this.runSafe();
        } catch (Throwable var2) {
            errorCallback(this.b, "Error: " + var2.toString());
        }
    }

    public void runSafe() {
        if(++this.a > 5) {
            errorCallback(this.b, "Too many redirects");
        } else {
            URL var1;
            URLConnection var2;
            try {
                SSLSocketFactory var3;
                if((var2 = (var1 = new URL(this.c)).openConnection()) instanceof HttpsURLConnection && (var3 = Acls.a()) != null) {
                    ((HttpsURLConnection)var2).setSSLSocketFactory(var3);
                }
            } catch (MalformedURLException var14) {
                errorCallback(this.b, var14.toString());
                return;
            } catch (IOException var15) {
                errorCallback(this.b, var15.toString());
                return;
            }

            if(var1.getProtocol().equalsIgnoreCase("file") && var1.getHost() != null && var1.getHost().length() != 0) {
                errorCallback(this.b, var1.getHost() + var1.getFile() + " is not an absolute path!");
            } else {
                if(this.e != null) {
                    Iterator var17 = this.e.entrySet().iterator();

                    while(var17.hasNext()) {
                        Entry var4 = (Entry)var17.next();
                        var2.addRequestProperty((String)var4.getKey(), (String)var4.getValue());
                    }
                }

                int var18 = 1428;
                int var5;
                if(this.d != null) {
                    var2.setDoOutput(true);

                    try {
                        OutputStream var19 = var2.getOutputStream();
                        var5 = 0;

                        while(var5 < this.d.length) {
                            int var6 = Math.min(1428, this.d.length - var5);
                            var19.write(this.d, var5, var6);
                            this.progressCallback(var5 += var6, this.d.length, 0, 0, 0L, 0L);
                        }
                    } catch (Exception var13) {
                        errorCallback(this.b, var13.toString());
                        return;
                    }
                }

                if(var2 instanceof HttpURLConnection) {
                    HttpURLConnection var20 = (HttpURLConnection)var2;

                    try {
                        var5 = var20.getResponseCode();
                    } catch (IOException var11) {
                        errorCallback(this.b, var11.toString());
                        return;
                    }

                    List var8;
                    Map var25;
                    if((var25 = var20.getHeaderFields()) != null && (var5 == 301 || var5 == 302) && (var8 = (List)var25.get("Location")) != null && !var8.isEmpty()) {
                        var20.disconnect();
                        this.c = (String)var8.get(0);
                        this.run();
                        return;
                    }
                }

                Map var22 = var2.getHeaderFields();
                boolean var24 = this.headerCallback(var22);
                if((var22 == null || !var22.containsKey("content-length")) && var2.getContentLength() != -1) {
                    var24 = var24 || this.headerCallback("content-length", String.valueOf(var2.getContentLength()));
                }

                if((var22 == null || !var22.containsKey("content-type")) && var2.getContentType() != null) {
                    var24 = var24 || this.headerCallback("content-type", var2.getContentType());
                }

                if(var24) {
                    errorCallback(this.b, this.c + " aborted");
                } else {
                    int var23 = var2.getContentLength() > 0?var2.getContentLength():0;
                    if(var1.getProtocol().equalsIgnoreCase("file") || var1.getProtocol().equalsIgnoreCase("jar")) {
                        var18 = var23 == 0?'耀':Math.min(var23, '耀');
                    }

                    var5 = 0;

                    try {
                        long var26 = System.currentTimeMillis();
                        byte[] var27 = new byte[var18];
                        InputStream var16 = null;
                        boolean var21 = true;
                        String var9 = "";
                        if(var2 instanceof HttpURLConnection) {
                            HttpURLConnection var10;
                            var16 = (var10 = (HttpURLConnection)var2).getErrorStream();
                            var9 = var10.getResponseCode() + ": " + var10.getResponseMessage();
                        }

                        if(var16 == null) {
                            var16 = var2.getInputStream();
                            var21 = false;
                        }

                        for(int var28 = 0; var28 != -1; var28 = var16.read(var27)) {
                            if(this.readCallback(var27, var28)) {
                                errorCallback(this.b, this.c + " aborted");
                                return;
                            }

                            if(!var21) {
                                var5 += var28;
                                this.progressCallback(0, 0, var5, var23, System.currentTimeMillis(), var26);
                            }
                        }

                        if(var21) {
                            errorCallback(this.b, var9);
                        }
                    } catch (Exception var12) {
                        errorCallback(this.b, var12.toString());
                        return;
                    }

                    this.progressCallback(0, 0, var5, var5, 0L, 0L);
                    doneCallback(this.b);
                }
            }
        }
    }

    private static native boolean headerCallback(int var0, String var1);

    protected boolean headerCallback(Map var1) {
        if(var1 != null && var1.size() != 0) {
            StringBuilder var2 = new StringBuilder();
            Iterator var6 = var1.entrySet().iterator();

            while(true) {
                Entry var3;
                Iterator var4;
                String var5;
                do {
                    if(!var6.hasNext()) {
                        return headerCallback(this.b, var2.toString());
                    }

                    var4 = ((List)(var3 = (Entry)var6.next()).getValue()).iterator();

                    while(var4.hasNext()) {
                        var5 = (String)var4.next();
                        var2.append((String)var3.getKey());
                        var2.append(": ");
                        var2.append(var5);
                        var2.append("\r\n");
                    }
                } while(var3.getKey() != null);

                var4 = ((List)var3.getValue()).iterator();

                while(var4.hasNext()) {
                    var5 = (String)var4.next();
                    var2.append("Status: ");
                    var2.append(var5);
                    var2.append("\r\n");
                }
            }
        } else {
            return false;
        }
    }

    protected boolean headerCallback(String var1, String var2) {
        StringBuilder var3;
        (var3 = new StringBuilder()).append(var1);
        var3.append(": ");
        var3.append(var2);
        var3.append("\n\r");
        return headerCallback(this.b, var3.toString());
    }

    private static native boolean readCallback(int var0, byte[] var1, int var2);

    protected boolean readCallback(byte[] var1, int var2) {
        return readCallback(this.b, var1, var2);
    }

    private static native void progressCallback(int var0, float var1, float var2, double var3, int var5);

    protected void progressCallback(int var1, int var2, int var3, int var4, long var5, long var7) {
        float var9;
        double var10;
        float var14;
        if(var4 > 0) {
            var9 = (float)var3 / (float)var4;
            var14 = 1.0F;
            var2 = Math.max(var4 - var3, 0);
            double var12 = 1000.0D * (double)var3 / Math.max((double)(var5 - var7), 0.1D);
            if(Double.isInfinite(var10 = (double)var2 / var12) || Double.isNaN(var10)) {
                var10 = 0.0D;
            }
        } else {
            if(var2 <= 0) {
                return;
            }

            var9 = 0.0F;
            var14 = (float)(var1 / var2);
            var10 = 0.0D;
        }

        progressCallback(this.b, var14, var9, var10, var4);
    }

    private static native void errorCallback(int var0, String var1);

    private static native void doneCallback(int var0);
}
