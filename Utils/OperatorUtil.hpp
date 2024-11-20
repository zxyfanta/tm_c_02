//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_OPERATORUTIL_H
#define TM_C_01_OPERATORUTIL_H

#include "../Parameter/Parameter.hpp"
using namespace std;

void Difference(double ux[L0 + 1][Nr + 1], double u[L0 + 1][Nr + 1]) {
    for (int l = 0; l <= L0; l++)
        for (int nr = 1; nr < Nr; nr++)
            ux[l][nr] = (u[l][nr + 1] - u[l][nr - 1]) / 2.0 / dr;
}

void Laplace(double Lu[L0 + 1][Nr + 1], double u[L0 + 1][Nr + 1]) {
    for (int l = 0; l <= L0; l++) {
        //内点处中心差分
        for (int nr = 1; nr < Nr; nr++)
            Lu[l][nr] = (u[l][nr + 1] - 2 * u[l][nr] + u[l][nr - 1]) / dr / dr +
                        (u[l][nr + 1] - u[l][nr - 1]) / 2.0 / dr / nr / dr -
                        l * l * mm * mm * u[l][nr] / nr / dr / nr / dr;

        //中心处数值边界条件
        Lu[l][0] = (l == 0) ? Lu[l][1] : 0.0;
        //外边界数值边界条件
        Lu[l][Nr] = (2 * u[l][Nr] - 5 * u[l][Nr - 1] + 4 * u[l][Nr - 2] - u[l][Nr - 3]) / dr / dr +
                    (3 * u[l][Nr] - 4 * u[l][Nr - 1] + u[l][Nr - 2]) / 2 / dr - mm * mm * l * l * u[l][Nr];
    }
}

#endif //TM_C_01_OPERATORUTIL_H
