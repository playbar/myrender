//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.Proxy;
import java.util.ArrayList;
import java.util.Iterator;

final class ReflectionHelper {
    protected static boolean LOG = false;
    protected static final boolean LOGV = false;
    private static ReflectionHelper.a[] a = new ReflectionHelper.a[4096];

    ReflectionHelper() {
    }

    private static boolean a(ReflectionHelper.a var0) {
        ReflectionHelper.a var1 = a[var0.hashCode() & a.length - 1];
        if(!var0.equals(var1)) {
            return false;
        } else {
            var0.a = var1.a;
            return true;
        }
    }

    private static void a(ReflectionHelper.a var0, Member var1) {
        var0.a = var1;
        a[var0.hashCode() & a.length - 1] = var0;
    }

    protected static Constructor getConstructorID(Class var0, String var1) {
        Constructor var2 = null;
        ReflectionHelper.a var3;
        if(a(var3 = new ReflectionHelper.a(var0, "", var1))) {
            var2 = (Constructor)var3.a;
        } else {
            Class[] var4 = a(var1);
            float var5 = 0.0F;
            Constructor[] var6;
            int var7 = (var6 = var0.getConstructors()).length;

            for(int var8 = 0; var8 < var7; ++var8) {
                Constructor var9 = var6[var8];
                float var10;
                if((var10 = a(Void.TYPE, var9.getParameterTypes(), var4)) > var5) {
                    var2 = var9;
                    var5 = var10;
                    if(var10 == 1.0F) {
                        break;
                    }
                }
            }

            a((ReflectionHelper.a)var3, (Member)var2);
        }

        if(var2 == null) {
            throw new NoSuchMethodError("<init>" + var1 + " in class " + var0.getName());
        } else {
            return var2;
        }
    }

    protected static Method getMethodID(Class var0, String var1, String var2, boolean var3) {
        Method var4 = null;
        ReflectionHelper.a var5;
        if(a(var5 = new ReflectionHelper.a(var0, var1, var2))) {
            var4 = (Method)var5.a;
        } else {
            Class[] var6 = a(var2);

            for(float var7 = 0.0F; var0 != null; var0 = var0.getSuperclass()) {
                Method[] var8;
                int var9 = (var8 = var0.getDeclaredMethods()).length;

                for(int var10 = 0; var10 < var9; ++var10) {
                    Method var11 = var8[var10];
                    float var12;
                    if(var3 == Modifier.isStatic(var11.getModifiers()) && var11.getName().compareTo(var1) == 0 && (var12 = a(var11.getReturnType(), var11.getParameterTypes(), var6)) > var7) {
                        var4 = var11;
                        var7 = var12;
                        if(var12 == 1.0F) {
                            break;
                        }
                    }
                }

                if(var7 == 1.0F || var0.isPrimitive() || var0.isInterface() || var0.equals(Object.class) || var0.equals(Void.TYPE)) {
                    break;
                }
            }

            a((ReflectionHelper.a)var5, (Member)var4);
        }

        if(var4 == null) {
            throw new NoSuchMethodError(String.format("no %s method with name=\'%s\' signature=\'%s\' in class L%s;", new Object[]{var3?"non-static":"static", var1, var2, var0.getName()}));
        } else {
            return var4;
        }
    }

    protected static Field getFieldID(Class var0, String var1, String var2, boolean var3) {
        Field var4 = null;
        ReflectionHelper.a var5;
        if(a(var5 = new ReflectionHelper.a(var0, var1, var2))) {
            var4 = (Field)var5.a;
        } else {
            Class[] var6 = a(var2);

            for(float var7 = 0.0F; var0 != null; var0 = var0.getSuperclass()) {
                Field[] var8;
                int var9 = (var8 = var0.getDeclaredFields()).length;

                for(int var10 = 0; var10 < var9; ++var10) {
                    Field var11 = var8[var10];
                    float var12;
                    if(var3 == Modifier.isStatic(var11.getModifiers()) && var11.getName().compareTo(var1) == 0 && (var12 = a(var11.getType(), (Class[])null, var6)) > var7) {
                        var4 = var11;
                        var7 = var12;
                        if(var12 == 1.0F) {
                            break;
                        }
                    }
                }

                if(var7 == 1.0F || var0.isPrimitive() || var0.isInterface() || var0.equals(Object.class) || var0.equals(Void.TYPE)) {
                    break;
                }
            }

            a((ReflectionHelper.a)var5, (Member)var4);
        }

        if(var4 == null) {
            throw new NoSuchFieldError(String.format("no %s field with name=\'%s\' signature=\'%s\' in class L%s;", new Object[]{var3?"non-static":"static", var1, var2, var0.getName()}));
        } else {
            return var4;
        }
    }

    private static float a(Class var0, Class var1) {
        if(var0.equals(var1)) {
            return 1.0F;
        } else {
            if(!var0.isPrimitive() && !var1.isPrimitive()) {
                try {
                    if(var0.asSubclass(var1) != null) {
                        return 0.5F;
                    }
                } catch (ClassCastException var3) {
                    ;
                }

                try {
                    if(var1.asSubclass(var0) != null) {
                        return 0.1F;
                    }
                } catch (ClassCastException var2) {
                    ;
                }
            }

            return 0.0F;
        }
    }

    private static float a(Class var0, Class[] var1, Class[] var2) {
        if(var2.length == 0) {
            return 0.1F;
        } else if((var1 == null?0:var1.length) + 1 != var2.length) {
            return 0.0F;
        } else {
            float var3 = 1.0F;
            int var4 = 0;
            if(var1 != null) {
                int var5 = (var1 = var1).length;

                for(int var6 = 0; var6 < var5; ++var6) {
                    Class var7 = var1[var6];
                    var3 *= a(var7, var2[var4++]);
                }
            }

            return var3 * a(var0, var2[var2.length - 1]);
        }
    }

    private static Class[] a(String var0) {
        int[] var1 = new int[]{0};
        ArrayList var2 = new ArrayList();

        Class var3;
        while(var1[0] < var0.length() && (var3 = a(var0, var1)) != null) {
            var2.add(var3);
        }

        int var7 = 0;
        Class[] var4 = new Class[var2.size()];

        Class var6;
        for(Iterator var5 = var2.iterator(); var5.hasNext(); var4[var7++] = var6) {
            var6 = (Class)var5.next();
        }

        return var4;
    }

    private static Class a(String var0, int[] var1) {
        while(true) {
            if(var1[0] < var0.length()) {
                int var10004 = var1[0];
                int var10001 = var1[0];
                var1[0] = var10004 + 1;
                char var2;
                if((var2 = var0.charAt(var10001)) == 40 || var2 == 41) {
                    continue;
                }

                if(var2 == 76) {
                    int var5;
                    if((var5 = var0.indexOf(59, var1[0])) != -1) {
                        var0 = var0.substring(var1[0], var5);
                        var1[0] = var5 + 1;
                        var0 = var0.replace('/', '.');

                        try {
                            Class var4 = Class.forName(var0);
                            return var4;
                        } catch (ClassNotFoundException var3) {
                            ;
                        }
                    }
                } else {
                    if(var2 == 90) {
                        return Boolean.TYPE;
                    }

                    if(var2 == 73) {
                        return Integer.TYPE;
                    }

                    if(var2 == 70) {
                        return Float.TYPE;
                    }

                    if(var2 == 86) {
                        return Void.TYPE;
                    }

                    if(var2 == 66) {
                        return Byte.TYPE;
                    }

                    if(var2 == 83) {
                        return Short.TYPE;
                    }

                    if(var2 == 74) {
                        return Long.TYPE;
                    }

                    if(var2 == 68) {
                        return Double.TYPE;
                    }

                    if(var2 == 91) {
                        return Array.newInstance(a(var0, var1), 0).getClass();
                    }

                    ClsG.Log(5, "! parseType; " + var2 + " is not known!");
                }
            }

            return null;
        }
    }

    private static native Object nativeProxyInvoke(int var0, String var1, Object[] var2);

    private static native void nativeProxyFinalize(int var0);

    protected static Object newProxyInstance(int var0, Class var1) {
        return newProxyInstance(var0, new Class[]{var1});
    }

    protected static Object newProxyInstance(final int var0, final Class[] var1) {
        return Proxy.newProxyInstance(ReflectionHelper.class.getClassLoader(), var1, new InvocationHandler() {
            public final Object invoke(Object var1x, Method var2, Object[] var3) {
                return ReflectionHelper.nativeProxyInvoke(var0, var2.getName(), var3);
            }

            protected final void finalize() {
                try {
                    ReflectionHelper.nativeProxyFinalize(var0);
                } finally {
                    try {
                        super.finalize();
                    }catch (Throwable e){}
                }

            }
        });
    }

    private static class a {
        private final Class b;
        private final String c;
        private final String d;
        private final int e;
        public volatile Member a;

        a(Class var1, String var2, String var3) {
            this.b = var1;
            this.c = var2;
            this.d = var3;
            int var4 = 527 + this.b.hashCode();
            var4 = 31 * var4 + this.c.hashCode();
            var4 = 31 * var4 + this.d.hashCode();
            this.e = var4;
        }

        public final int hashCode() {
            return this.e;
        }

        public final boolean equals(Object var1) {
            if(var1 == this) {
                return true;
            } else if(var1 instanceof ReflectionHelper.a) {
                ReflectionHelper.a var2 = (ReflectionHelper.a)var1;
                return this.e == var2.e && this.d.equals(var2.d) && this.c.equals(var2.c) && this.b.equals(var2.b);
            } else {
                return false;
            }
        }
    }
}
