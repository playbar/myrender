package com.bfmj.viewcore.util;

import com.bfmj.viewcore.render.GLVector2;
import com.bfmj.viewcore.render.GLVector3;

/**
 * Created by mac on 16/10/11.
 */

public class IntersectionTest
{
    private float a;
    private float b;
    private float c;
    private float d;

    private float m_width;
    private float m_height;

    private GLVector3 m_topLeft;
    private GLVector3 m_topRight;
    private GLVector3 m_bottomLeft;

    private GLVector3 m_xAxis;
    private GLVector3 m_yAxis;

    public static  float[] vecMulMatrxi(float[]vec, float[]mat){
        float []ret = new float[4];
        for (int i = 0; i < 4; ++i){
            ret[i] = vec[0]*mat[i * 4] + vec[1] *mat[i*4+1] +
                    vec[2]*mat[i*4+2] + vec[3]*mat[i*4+3];
        }
        return ret;
    }

    public static float Distance(GLVector3 a, GLVector3 b)
    {
        return (float) Math.sqrt((a.getX() - b.getX()) * (a.getX() - b.getX())
                + (a.getY() - b.getY()) * (a.getY() - b.getY())
                + (a.getZ() - b.getZ()) * (a.getZ() -b.getZ()));

    }

    public float Distance(GLVector3 pt){

        return Math.abs((float) (a*pt.x + b*pt.y + c*pt.z + d)) / (float) Math.sqrt(a*a + b*b + c*c);
    }

    public IntersectionTest(GLVector3 topLeft, GLVector3 topRight, GLVector3 bottomLeft)
    {
        a = ((topRight.y - topLeft.y)*(bottomLeft.z - topLeft.z) - (topRight.z - topLeft.z)*(bottomLeft.y - topLeft.y));
        b = ((topRight.z - topLeft.z)*(bottomLeft.x - topLeft.x) - (topRight.x - topLeft.x)*(bottomLeft.z - topLeft.z));
        c = ((topRight.x - topLeft.x)*(bottomLeft.y - topLeft.y) - (topRight.y - topLeft.y)*(bottomLeft.x - topLeft.x));
        d = a*topLeft.x + b*topLeft.y + c*topLeft.z;

        float distance = (float)Math.sqrt(a*a + b*b + c*c);
        a /= distance;
        b /= distance;
        c /= distance;
        d /= distance;

        GLVector3 vec = new GLVector3();
        vec = GLVector3.Sub(topRight, topLeft, vec);
        m_xAxis = GLVector3.Normalize(vec);
        GLVector3 vecy = new GLVector3();
        vecy = GLVector3.Sub(bottomLeft, topLeft, vecy);
        m_yAxis = GLVector3.Normalize(vecy);//normalize(bottomLeft - topLeft);

        m_width = Distance(topLeft, topRight);
        m_height = Distance(topLeft, bottomLeft);

        m_topLeft = topLeft;
        m_topRight = topRight;
        m_bottomLeft = bottomLeft;
    }

    public boolean Intersection(GLVector3 light, GLVector2 position){
        float div = a * light.x + b * light.y + c * light.z;
        if (Math.abs(div) < 1e-6)
        {
            // light is parallel with RECT
            return false;
        }

        float t = d / (a * light.x + b * light.y + c * light.z);
        if (t < 0.0f)
        {
            // light direction is reverse
            return false;
        }

        GLVector3 vecInPlane =  GLVector3.Sub(GLVector3.Mul(t, light ), m_topLeft);

        if (Math.abs(GLVector3.Dot(m_xAxis, m_yAxis)) < 1e-6)
        {
            //rectangle
            float width = GLVector3.Dot(vecInPlane, m_xAxis);
            if ((width < 0) || (width > m_width))
                return false;

            float height = GLVector3.Dot(vecInPlane, m_yAxis);
            if ((height < 0) || (height > m_height))
                return false;

            position.s = width / m_width;
            position.t = height / m_height;
            return true;
        }
        else
        {
            //Parallelogram
            float x1, x2, y1, y2, m, n;
            if ((div = m_xAxis.x * m_yAxis.y - m_yAxis.x * m_xAxis.y) != 0.0f)
            {
                // use x,y
                x1 = m_xAxis.x;
                y1 = m_xAxis.y;
                x2 = m_yAxis.x;
                y2 = m_yAxis.y;
                m = vecInPlane.x;
                n = vecInPlane.y;
            }
            else if ((div = m_xAxis.x * m_yAxis.z - m_yAxis.x * m_xAxis.z) != 0.0f)
            {
                // use x,z
                x1 = m_xAxis.x;
                y1 = m_xAxis.z;
                x2 = m_yAxis.x;
                y2 = m_yAxis.z;
                m = vecInPlane.x;
                n = vecInPlane.z;
            }
            else if ((div = m_xAxis.y * m_yAxis.z - m_yAxis.y * m_xAxis.z) != 0.0f)
            {
                // use y,z
                x1 = m_xAxis.y;
                y1 = m_xAxis.z;
                x2 = m_yAxis.y;
                y2 = m_yAxis.z;
                m = vecInPlane.y;
                n = vecInPlane.z;
            }
            else
            {
                // My Godness, this shall be impossible!
                return false;
            }

            float width = (m * y2 - n * x2) / div;
            if ((width < 0) || (width > m_width))
                return false;

            float height = (n * x1 - m * y1) / div;
            if ((height < 0) || (height > m_height))
                return false;

            position.s = width / m_width;
            position.t = height / m_height;
            return true;
        }
    }
}
