//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_PARAMETER_HPP
#define TM_C_01_PARAMETER_HPP
//Fourier截止项
// #define L0 1
const int L0=1;
//网格节点数
// #define Nr 1000
const int Nr=1000;

const double mm = 1.0;//极向模数
const double nn = 1.0;//环向模数
const double R0 = 4.4364;//大半径
const double Eta0 = 1e-6;//中心处电阻率
const double dr = 1.0 / Nr;//径向步长

const double rs = 0.3;//初始有理面位置
const double q0 = 0.9;//初始中心安全因子
const double Ew = 2 * Eta0 / q0;//边界电场

const double simT = 500;//模拟总时间
const double savT = 50;//数据保存时间
const double dt = 0.01;//模拟步长
namespace Parameter {
    int count;//储存文件编号
}
double Time;//判断是否保存的时间
double r0;//电流通道宽度
double r[Nr + 1];//节点位置
double eta[Nr + 1];//电阻率

#endif //TM_C_01_PARAMETER_H
