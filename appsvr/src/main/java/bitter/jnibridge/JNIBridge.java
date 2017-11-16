//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package bitter.jnibridge;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

public class JNIBridge {
    public JNIBridge() {
    }

    static native Object invoke(long var0, Class var2, Method var3, Object[] var4);

    static native void delete(long var0);

    static Object newInterfaceProxy(long var0, Class[] var2) {
        return Proxy.newProxyInstance(JNIBridge.class.getClassLoader(), var2, new JNIBridge.a(var0));
    }

    static void disableInterfaceProxy(Object var0) {
        ((JNIBridge.a)Proxy.getInvocationHandler(var0)).a();
    }

    private static class a implements InvocationHandler {
        private Object a = new Object[0];
        private long b;

        public a(long var1) {
            this.b = var1;
        }

        public final Object invoke(Object var1, Method var2, Object[] var3) {
            var1 = this.a;
            synchronized(this.a) {
                return this.b == 0L?null:JNIBridge.invoke(this.b, var2.getDeclaringClass(), var2, var3);
            }
        }

        public final void finalize() {
            Object var1 = this.a;
            synchronized(this.a) {
                if(this.b != 0L) {
                    JNIBridge.delete(this.b);
                }
            }
        }

        public final void a() {
            Object var1 = this.a;
            synchronized(this.a) {
                this.b = 0L;
            }
        }
    }
}
