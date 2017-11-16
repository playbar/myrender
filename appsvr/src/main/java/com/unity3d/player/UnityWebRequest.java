//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.JarURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLConnection;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.channels.Channels;
import java.nio.channels.ReadableByteChannel;
import java.security.SecureRandom;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.KeyManager;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLHandshakeException;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;

class UnityWebRequest implements Runnable {
    private long a;
    private String b;
    private String c;
    private Map d;
    private static final String[] e = new String[]{"TLSv1.2", "TLSv1.1"};
    private static volatile SSLSocketFactory f;

    private static SSLSocketFactory getSSLSocketFactory() {
        if(ClsI.d) {
            return null;
        } else if(f != null) {
            return f;
        } else {
            String[] var0 = e;
            synchronized(e) {
                String[] var1 = e;
                int var2 = e.length;
                int var3 = 0;

                while(true) {
                    if(var3 < var2) {
                        String var4 = var1[var3];

                        SSLSocketFactory var10000;
                        try {
                            SSLContext var5;
                            (var5 = SSLContext.getInstance(var4)).init((KeyManager[])null, (TrustManager[])null, (SecureRandom)null);
                            var10000 = f = var5.getSocketFactory();
                        } catch (Exception var6) {
                            ClsG.Log(5, "UnityWebRequest: No support for " + var4 + " (" + var6.getMessage() + ")");
                            ++var3;
                            continue;
                        }

                        return var10000;
                    }

                    return null;
                }
            }
        }
    }

    UnityWebRequest(long var1, String var3, Map var4, String var5) {
        this.a = var1;
        this.b = var5;
        this.c = var3;
        this.d = var4;
    }

    public void run() {
        URL var1;
        URLConnection var2;
        try {
            SSLSocketFactory var3;
            if((var2 = (var1 = new URL(this.b)).openConnection()) instanceof HttpsURLConnection && (var3 = getSSLSocketFactory()) != null) {
                ((HttpsURLConnection)var2).setSSLSocketFactory(var3);
            }
        } catch (MalformedURLException var14) {
            this.malformattedUrlCallback(var14.toString());
            return;
        } catch (IOException var15) {
            this.errorCallback(var15.toString());
            return;
        }

        if(var1.getProtocol().equalsIgnoreCase("file") && !var1.getHost().isEmpty()) {
            this.malformattedUrlCallback("file:// must use an absolute path");
        } else if(var2 instanceof JarURLConnection) {
            this.badProtocolCallback("A URL Connection to a Java ARchive (JAR) file or an entry in a JAR file is not supported");
        } else {
            if(var2 instanceof HttpURLConnection) {
                try {
                    HttpURLConnection var18;
                    (var18 = (HttpURLConnection)var2).setRequestMethod(this.c);
                    var18.setInstanceFollowRedirects(false);
                } catch (ProtocolException var9) {
                    this.badProtocolCallback(var9.toString());
                    return;
                }
            }

            if(this.d != null) {
                Iterator var19 = this.d.entrySet().iterator();

                while(var19.hasNext()) {
                    Entry var4 = (Entry)var19.next();
                    var2.addRequestProperty((String)var4.getKey(), (String)var4.getValue());
                }
            }

            int var20 = 1428;
            int var21;
            if((var21 = this.uploadCallback((ByteBuffer)null)) > 0) {
                var2.setDoOutput(true);

                try {
                    ByteBuffer var22 = ByteBuffer.allocateDirect(Math.min(var21, 1428));
                    OutputStream var5 = var2.getOutputStream();

                    for(int var6 = this.uploadCallback(var22); var6 > 0; var6 = this.uploadCallback(var22)) {
                        var5.write(var22.array(), var22.arrayOffset(), var6);
                    }
                } catch (Exception var13) {
                    this.errorCallback(var13.toString());
                    return;
                }
            }

            if(var2 instanceof HttpURLConnection) {
                HttpURLConnection var23 = (HttpURLConnection)var2;

                try {
                    int var25 = var23.getResponseCode();
                    this.responseCodeCallback(var25);
                } catch (UnknownHostException var7) {
                    this.unknownHostCallback(var7.toString());
                } catch (IOException var8) {
                    this.errorCallback(var8.toString());
                    return;
                }
            }

            Map var24 = var2.getHeaderFields();
            this.headerCallback(var24);
            if((var24 == null || !var24.containsKey("content-length")) && var2.getContentLength() != -1) {
                this.headerCallback("content-length", String.valueOf(var2.getContentLength()));
            }

            if((var24 == null || !var24.containsKey("content-type")) && var2.getContentType() != null) {
                this.headerCallback("content-type", var2.getContentType());
            }

            if((var21 = var2.getContentLength()) > 0) {
                this.contentLengthCallback(var21);
            }

            if(var1.getProtocol().equalsIgnoreCase("file")) {
                var20 = var21 == 0?32768:Math.min(var21, 32768);
            }

            try {
                InputStream var26 = null;
                if(var2 instanceof HttpURLConnection) {
                    HttpURLConnection var27 = (HttpURLConnection)var2;
                    this.responseCodeCallback(var27.getResponseCode());
                    var26 = var27.getErrorStream();
                }

                if(var26 == null) {
                    var26 = var2.getInputStream();
                }

                ReadableByteChannel var28 = Channels.newChannel(var26);
                ByteBuffer var16 = ByteBuffer.allocateDirect(var20);

                for(int var17 = var28.read(var16); var17 != -1 && this.downloadCallback(var16, var17); var17 = var28.read(var16)) {
                    var16.clear();
                }

                var28.close();
            } catch (UnknownHostException var10) {
                this.unknownHostCallback(var10.toString());
            } catch (SSLHandshakeException var11) {
                this.sslCannotConnectCallback(var11.toString());
            } catch (Exception var12) {
                this.errorCallback(var12.toString());
            }
        }
    }

    private static native void headerCallback(long var0, String var2, String var3);

    protected void headerCallback(String var1, String var2) {
        headerCallback(this.a, var1, var2);
    }

    protected void headerCallback(Map var1) {
        if(var1 != null && var1.size() != 0) {
            Iterator var5 = var1.entrySet().iterator();

            while(var5.hasNext()) {
                Entry var2;
                String var3;
                if((var3 = (String)(var2 = (Entry)var5.next()).getKey()) == null) {
                    var3 = "Status";
                }

                Iterator var6 = ((List)var2.getValue()).iterator();

                while(var6.hasNext()) {
                    String var4 = (String)var6.next();
                    this.headerCallback(var3, var4);
                }
            }

        }
    }

    private static native int uploadCallback(long var0, ByteBuffer var2);

    protected int uploadCallback(ByteBuffer var1) {
        return uploadCallback(this.a, var1);
    }

    private static native void contentLengthCallback(long var0, int var2);

    protected void contentLengthCallback(int var1) {
        contentLengthCallback(this.a, var1);
    }

    private static native boolean downloadCallback(long var0, ByteBuffer var2, int var3);

    protected boolean downloadCallback(ByteBuffer var1, int var2) {
        return downloadCallback(this.a, var1, var2);
    }

    protected void responseCodeCallback(int var1) {
        responseCodeCallback(this.a, var1);
    }

    private static native void responseCodeCallback(long var0, int var2);

    protected void unknownHostCallback(String var1) {
        errorCallback(this.a, 7, var1);
    }

    protected void badProtocolCallback(String var1) {
        errorCallback(this.a, 4, var1);
    }

    protected void malformattedUrlCallback(String var1) {
        errorCallback(this.a, 5, var1);
    }

    protected void sslCannotConnectCallback(String var1) {
        errorCallback(this.a, 16, var1);
    }

    protected void errorCallback(String var1) {
        errorCallback(this.a, 2, var1);
    }

    private static native void errorCallback(long var0, int var2, String var3);
}
