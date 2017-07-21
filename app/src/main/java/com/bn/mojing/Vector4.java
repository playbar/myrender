package com.bn.mojing;

import android.graphics.Matrix;

/**
 * Created by houguoli on 2017/7/21.
 */

public class Vector4 {
    float x;
    float y;
    float z;
    float w;

    public Vector4(){
        x = y = z = w = 0;
    }

    static float[] matMulVec(float []mat44, float []vec4 ){
        float []re = new float[4];
//        int index = 0;
//        re[index] = mat44[4*index + 0] * vec4[0] + mat44[4*index + 1] * vec4[1] + mat44[4*index +2] *vec4[2] + mat44[4*index +3] * vec4[3];
//        index++;
//        re[index] = mat44[4*index + 0] * vec4[0] + mat44[4*index + 1] * vec4[1] + mat44[4*index +2] *vec4[2] + mat44[4*index +3] * vec4[3];
//        index++;
//        re[index] = mat44[4*index + 0] * vec4[0] + mat44[4*index + 1] * vec4[1] + mat44[4*index +2] *vec4[2] + mat44[4*index +3] * vec4[3];
//        index++;
//        re[index] = mat44[4*index + 0] * vec4[0] + mat44[4*index + 1] * vec4[1] + mat44[4*index +2] *vec4[2] + mat44[4*index +3] * vec4[3];

        int index = 0;
        re[index] = mat44[index] * vec4[0] + mat44[4 +index ] * vec4[1] + mat44[ 8 + index] *vec4[2] + mat44[12 + index] * vec4[3];
        index++;
        re[index] = mat44[index] * vec4[0] + mat44[4 +index ] * vec4[1] + mat44[ 8 + index] *vec4[2] + mat44[12 + index] * vec4[3];
        index++;
        re[index] = mat44[index] * vec4[0] + mat44[4 +index ] * vec4[1] + mat44[ 8 + index] *vec4[2] + mat44[12 + index] * vec4[3];
        index++;
        re[index] = mat44[index] * vec4[0] + mat44[4 +index ] * vec4[1] + mat44[ 8 + index] *vec4[2] + mat44[12 + index] * vec4[3];

        return re;
    }

}
