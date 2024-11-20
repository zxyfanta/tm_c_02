//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_MATRIXUTIL_H
#define TM_C_01_MATRIXUTIL_H

#include "iostream"

using namespace std;

class MatrixUtil {
public:
    static void ini(double data[L0 + 1][Nr + 1]) {
        for (int l = 0; l <= L0; l++)
            for (int nr = 0; nr <= Nr; nr++)
                data[l][nr] = 0.0;
    };

    //Fourier系数共轭，当l<0时由对称性得到（实对称）
    static double Complex1(double u[L0 + 1][Nr + 1], int l, int nr) {
        return u[abs(l)][nr];
    }


    //Fourier系数共轭，当l<0时由对称性得到（反对称）

    static double Complex2(double u[L0 + 1][Nr + 1], int l, int nr) {
        return ((l >= 0) ? 1 : -1) * u[abs(l)][nr];
    }
};

#endif //TM_C_01_MATRIXUTIL_H
