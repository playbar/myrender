//
// Source code recreated from ma .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.os.Build.VERSION;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;

import javax.net.ssl.HandshakeCompletedEvent;
import javax.net.ssl.HandshakeCompletedListener;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLPeerUnverifiedException;
import javax.net.ssl.SSLSession;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;

public final class Acls extends SSLSocketFactory {
    private final SSLSocketFactory ma;
    private final Acls.aa b;
    private static volatile SSLSocketFactory c;
    private static final Object[] d = new Object[0];
    private static final boolean e;

    private Acls() {
        SSLContext var1 = null;
        try {
            (var1 = SSLContext.getInstance("TLS")).init(null, null, null);
        }catch (Exception ex){
            ex.printStackTrace();
        }
        this.ma = var1.getSocketFactory();
        this.b = new Acls.aa();
    }

    public static SSLSocketFactory a() {
        Object[] var0 = d;
        synchronized(d) {
            if(c != null) {
                return c;
            } else {
                SSLSocketFactory var10000;
                try {
                    var10000 = c = new Acls();
                } catch (Exception var2) {
                    Ecls.Log(5, "CustomSSLSocketFactory: Failed to create SSLSocketFactory (" + var2.getMessage() + ")");
                    return null;
                }

                return var10000;
            }
        }
    }

    public final Socket createSocket(Socket var1, String var2, int var3, boolean var4) throws IOException{
        return a(this.ma.createSocket(var1, var2, var3, var4));
    }

    public final Socket createSocket(String var1, int var2) throws IOException  {
        return a(this.ma.createSocket(var1, var2));
    }

    public final Socket createSocket(InetAddress var1, int var2) throws IOException {
        return a(this.ma.createSocket(var1, var2));
    }

    public final Socket createSocket(InetAddress var1, int var2, InetAddress var3, int var4) throws IOException {
        return a(this.ma.createSocket(var1, var2, var3, var4));
    }

    public final Socket createSocket(String var1, int var2, InetAddress var3, int var4) throws IOException {
        return a(this.ma.createSocket(var1, var2, var3, var4));
    }

    public final Socket createSocket() throws IOException {
        return a(this.ma.createSocket());
    }

    private static Socket a(Socket var0) {
        if(var0 != null && var0 instanceof SSLSocket && e) {
            ((SSLSocket)var0).setEnabledProtocols(((SSLSocket)var0).getSupportedProtocols());
        }

        return var0;
    }

    public final String[] getDefaultCipherSuites() {
        return this.ma.getDefaultCipherSuites();
    }

    public final String[] getSupportedCipherSuites() {
        return this.ma.getSupportedCipherSuites();
    }

    static {
        e = VERSION.SDK_INT >= 16 && VERSION.SDK_INT < 20;
    }

    class aa implements HandshakeCompletedListener {
        aa() {
        }

        public final void handshakeCompleted(HandshakeCompletedEvent var1) {
            SSLSession var3;
            (var3 = var1.getSession()).getCipherSuite();
            var3.getProtocol();

            try {
                var3.getPeerPrincipal().getName();
            } catch (SSLPeerUnverifiedException var2) {
                ;
            }
        }
    }
}
