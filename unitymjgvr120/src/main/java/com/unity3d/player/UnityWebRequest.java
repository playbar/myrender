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
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLHandshakeException;
import javax.net.ssl.SSLSocketFactory;

class UnityWebRequest implements Runnable {
    private long a;
    private String b;
    private String c;
    private Map d;

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
            if((var2 = (var1 = new URL(this.b)).openConnection()) instanceof HttpsURLConnection && (var3 = Acls.a()) != null) {
                ((HttpsURLConnection)var2).setSSLSocketFactory(var3);
            }
        } catch (MalformedURLException var12) {
            this.malformattedUrlCallback(var12.toString());
            return;
        } catch (IOException var13) {
            this.errorCallback(var13.toString());
            return;
        }

        if(var1.getProtocol().equalsIgnoreCase("file") && !var1.getHost().isEmpty()) {
            this.malformattedUrlCallback("file:// must use an absolute path");
        } else if(var2 instanceof JarURLConnection) {
            this.badProtocolCallback("A URL Connection to a Java ARchive (JAR) file or an entry in a JAR file is not supported");
        } else {
            if(var2 instanceof HttpURLConnection) {
                try {
                    HttpURLConnection var20;
                    (var20 = (HttpURLConnection)var2).setRequestMethod(this.c);
                    var20.setInstanceFollowRedirects(false);
                } catch (ProtocolException var7) {
                    this.badProtocolCallback(var7.toString());
                    return;
                }
            }

            if(this.d != null) {
                Iterator var21 = this.d.entrySet().iterator();

                while(var21.hasNext()) {
                    Entry var4 = (Entry)var21.next();
                    var2.addRequestProperty((String)var4.getKey(), (String)var4.getValue());
                }
            }

            ByteBuffer var23 = ByteBuffer.allocateDirect(131072);
            int var22;
            if(this.uploadCallback((ByteBuffer)null) > 0) {
                var2.setDoOutput(true);

                try {
                    OutputStream var14 = var2.getOutputStream();

                    for(var22 = this.uploadCallback(var23); var22 > 0; var22 = this.uploadCallback(var23)) {
                        var14.write(var23.array(), var23.arrayOffset(), var22);
                    }
                } catch (Exception var11) {
                    this.errorCallback(var11.toString());
                    return;
                }
            }

            HttpURLConnection var15;
            if(var2 instanceof HttpURLConnection) {
                var15 = (HttpURLConnection)var2;

                try {
                    var22 = var15.getResponseCode();
                    this.responseCodeCallback(var22);
                } catch (UnknownHostException var5) {
                    this.unknownHostCallback(var5.toString());
                } catch (IOException var6) {
                    this.errorCallback(var6.toString());
                    return;
                }
            }

            Map var16 = var2.getHeaderFields();
            this.headerCallback(var16);
            if((var16 == null || !var16.containsKey("content-length")) && var2.getContentLength() != -1) {
                this.headerCallback("content-length", String.valueOf(var2.getContentLength()));
            }

            if((var16 == null || !var16.containsKey("content-type")) && var2.getContentType() != null) {
                this.headerCallback("content-type", var2.getContentType());
            }

            int var17;
            if((var17 = var2.getContentLength()) > 0) {
                this.contentLengthCallback(var17);
            }

            try {
                InputStream var24 = null;
                if(var2 instanceof HttpURLConnection) {
                    var15 = (HttpURLConnection)var2;
                    this.responseCodeCallback(var15.getResponseCode());
                    var24 = var15.getErrorStream();
                }

                if(var24 == null) {
                    var24 = var2.getInputStream();
                }

                ReadableByteChannel var19;
                for(int var18 = (var19 = Channels.newChannel(var24)).read(var23); var18 != -1 && this.downloadCallback(var23, var18); var18 = var19.read(var23)) {
                    var23.clear();
                }

                var19.close();
            } catch (UnknownHostException var8) {
                this.unknownHostCallback(var8.toString());
            } catch (SSLHandshakeException var9) {
                this.sslCannotConnectCallback(var9.toString());
            } catch (Exception var10) {
                this.errorCallback(var10.toString());
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
