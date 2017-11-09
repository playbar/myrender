//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import java.lang.reflect.Method;
import java.util.HashMap;

final class Lcls {
    private HashMap a = new HashMap();
    private Class b = null;
    private Object c = null;

    public Lcls(Class var1, Object var2) {
        this.b = var1;
        this.c = var2;
    }

    public final void a(String var1, Class[] var2) {
        this.a.put(var1, new Lcls.a(var2));
    }

    private void a(String var1, Lcls.a var2) {
        try {
            var2.b = this.b.getMethod(var1, var2.a);
        } catch (Exception var4) {
            Ecls.Log(6, "Exception while trying to get method " + var1 + ". " + var4.getLocalizedMessage());
            var2.b = null;
        }
    }

    public final Object a(String var1, Object... var2) {
        if(!this.a.containsKey(var1)) {
            Ecls.Log(6, "No definition for method " + var1 + " can be found");
            return null;
        } else {
            Lcls.a var3;
            if((var3 = (Lcls.a)this.a.get(var1)).b == null) {
                this.a(var1, var3);
            }

            if(var3.b == null) {
                Ecls.Log(6, "Unable to create method: " + var1);
                return null;
            } else {
                Object var5;
                try {
                    if(var2.length == 0) {
                        var5 = var3.b.invoke(this.c, new Object[0]);
                    } else {
                        var5 = var3.b.invoke(this.c, var2);
                    }
                } catch (Exception var4) {
                    Ecls.Log(6, "Error trying to call delegated method " + var1 + ". " + var4.getLocalizedMessage());
                    var5 = null;
                }

                return var5;
            }
        }
    }

    class a {
        public Class[] a;
        public Method b;

        public a(Class[] var2) {
            this.a = var2;
            this.b = null;
        }
    }
}
