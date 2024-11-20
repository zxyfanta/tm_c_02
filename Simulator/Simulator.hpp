//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_SIMULATOR_H
#define TM_C_01_SIMULATOR_H

#include "../Parameter/Parameter.hpp"
#include <iostream>
#include "../Utils/DataManagerUtil.hpp"
#include "../Utils/MatrixUtil.hpp"
#include "cmath"
#include "../Utils/OperatorUtil.hpp"

using namespace std;

class Simulator {

public:
//模拟初始化，模拟开始时计算一次
    static void Initialize(double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1]) {
        //时间统计归零
        Time = 0;
        Parameter::count = 0;
        //计算电流通道宽度
        r0 = (rs * sqrt(nn * q0 / (mm - nn * q0)));
        MatrixUtil::ini(u1);
        MatrixUtil::ini(u2);

        for (int nr = 0; nr <= Nr; nr++) {
            //节点计算
            r[nr] = nr * dr;
            //电阻率计算
            eta[nr] = Eta0 * (1.0 + r[nr] * r[nr] / r0 / r0) * (1.0 + r[nr] * r[nr] / r0 / r0);
            //平衡磁通
            u1[0][nr] = r0 * r0 / 2.0 / q0 * (log(1.0 + r[nr] * r[nr] / r0 / r0) - log(1.0 + 1.0 / r0 / r0)) -
                        (nn / mm / 2.0 * r[nr] * r[nr] - nn / mm / 2.0);
            //扰动磁通
            u1[1][nr] = 1e-9 * r[nr] * r[nr] * (1 - r[nr] * r[nr]) * (1 - r[nr] * r[nr]);
        }

        //count=1000;InputData(count,u1,u2);//从断点开始重新计算
    }

    //计算源项
    static void CalculatS(double S1[L0 + 1][Nr + 1], double S2[L0 + 1][Nr + 1], double u1[L0 + 1][Nr + 1],
                          double u2[L0 + 1][Nr + 1]) {
        //初始化返回值
        MatrixUtil::ini(S1);
        MatrixUtil::ini(S2);
        //计算Laplace算子的值
        double f1[L0 + 1][Nr + 1], f2[L0 + 1][Nr + 1];
        Laplace(f1, u1);
        Laplace(f2, u2);
        //计算一阶差分算子的值
        double u1x[L0 + 1][Nr + 1], u2x[L0 + 1][Nr + 1], f1x[L0 + 1][Nr + 1], f2x[L0 + 1][Nr + 1];
        Difference(u1x, u1);
        Difference(u2x, u2);
        Difference(f1x, f1);
        Difference(f2x, f2);

        for (int nr = 1; nr < Nr; nr++) {
            //当l=0时，只需要计算S1，因为此时S2等于零
            S1[0][nr] = 2 * eta[nr] * nn / mm + eta[nr] * f1[0][nr] - Ew;

            //计算卷积项，当l<0时由共轭条件取值
            for (int k = -L0; k <= L0; k++)
                S1[0][nr] -=
                        1 / R0 / r[nr] * mm * k * (MatrixUtil::Complex2(u2, k, nr) * MatrixUtil::Complex1(u1x, -k, nr) -
                                                   MatrixUtil::Complex1(u1, k, nr) * MatrixUtil::Complex2(u2x, -k, nr));
        }

        for (int l = 1; l <= L0; l++)
            for (int nr = 1; nr < Nr; nr++) {
                //计算非卷积项
                S1[l][nr] = eta[nr] * f1[l][nr];

                //计算卷积项，当l<0时由共轭条件取值
                for (int k = l - L0; k <= L0; k++) {
                    S1[l][nr] -= 1 / R0 / r[nr] * mm * k *
                                 (MatrixUtil::Complex2(u2, k, nr) * MatrixUtil::Complex1(u1x, l - k, nr) -
                                  MatrixUtil::Complex1(u1, k, nr) * MatrixUtil::Complex2(u2x, l - k, nr));
                    S2[l][nr] += 1 / R0 / r[nr] * mm * k *
                                 (MatrixUtil::Complex1(f1, k, nr) * MatrixUtil::Complex1(u1x, l - k, nr) -
                                  MatrixUtil::Complex1(u1, k, nr) * MatrixUtil::Complex1(f1x, l - k, nr));
                    S2[l][nr] += 1 / R0 / r[nr] * mm * k *
                                 (MatrixUtil::Complex2(f2, k, nr) * MatrixUtil::Complex2(u2x, l - k, nr) -
                                  MatrixUtil::Complex2(u2, k, nr) * MatrixUtil::Complex2(f2x, l - k, nr));
                }
            }
    }


    //时间推进
    static void EvaluateU(double nu1[L0 + 1][Nr + 1],
                          double nu2[L0 + 1][Nr + 1],
                          double u1[L0 + 1][Nr + 1],
                          double u2[L0 + 1][Nr + 1],
                          double S1[L0 + 1][Nr + 1],
                          double S2[L0 + 1][Nr + 1],
                          double Dt) {
        //u1推进，推进为nu1
        for (int l = 0; l <= L0; l++) {
            for (int nr = 1; nr < Nr; nr++)
                nu1[l][nr] = u1[l][nr] + Dt * S1[l][nr];

            //中心处数值边界条件
            nu1[l][0] = (l == 0) ? nu1[l][1] : 0;
            //外边界数值边界条件
            nu1[l][Nr] = 0.0;
        }

        //u2推进，追赶法推进为nu2
        double du2[L0 + 1][Nr + 1];
        MatrixUtil::ini(du2);

        for (int l = 1; l <= L0; l++) {
            //追赶法的系数矩阵
            double x[Nr + 1];
            double L[Nr + 1], D[Nr + 1], R[Nr + 1];

            for (int nr = 1; nr < Nr; nr++) {
                L[nr] = 1.0 - 0.5 / nr;
                D[nr] = -2.0 - mm * mm * l * l / nr / nr;
                R[nr] = 1.0 + 0.5 / nr;
            }

            //求已知向量
            for (int nr = 1; nr < Nr; nr++)
                x[nr] = Dt * dr * dr * S2[l][nr];

            //追赶法求未知向量
            double u[Nr], v[Nr], w[Nr];
            u[1] = D[1];
            v[1] = R[1] / u[1];
            w[1] = x[1] / u[1];

            //追
            for (int nr = 2; nr < Nr; nr++) {
                u[nr] = D[nr] - L[nr] * v[nr - 1];
                v[nr] = R[nr] / u[nr];
                w[nr] = (x[nr] - L[nr] * w[nr - 1]) / u[nr];
            }

            //赶
            du2[l][Nr - 1] = w[Nr - 1];

            for (int nr = Nr - 2; nr >= 1; nr--)
                du2[l][nr] = w[nr] - v[nr] * du2[l][nr + 1];
        }

        for (int l = 1; l <= L0; l++) {
            for (int nr = 1; nr < Nr; nr++)
                nu2[l][nr] = u2[l][nr] + du2[l][nr];

            //中心处数值边界条件
            nu2[l][0] = 0.0;
            //外边界数值边界条件
            nu2[l][Nr] = 0.0;
        }
    }
};

#endif //TM_C_01_SIMULATOR_H