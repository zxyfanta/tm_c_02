#ifndef TM_C_01_PARAMETER_HPP
#define TM_C_01_PARAMETER_HPP

#include <nlohmann/json.hpp>

const int Nr = 1000; // 网格节点数

class Parameters {
public:
    static int L0;
    static double mm;
    static double nn;
    static double R0;
    static double Eta0;
    static double dr;
    static double rs;
    static double q0;
    static double Ew;
    static double simT;
    static double savT;
    static double dt;
    static int count;
    static double Time;
    static double r0;
    static double r[Nr + 1]; // 节点位置
    static double eta[Nr + 1]; // 电阻率

    static void load(const std::string& jsonFilePath);
    static void save(const std::string& jsonFilePath);
};

#endif // TM_C_01_PARAMETER_HPP