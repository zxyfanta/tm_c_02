//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_PROCESSOR_H
#define TM_C_01_PROCESSOR_H

#include "../Simulator/Simulator.hpp"
#include "../Utils/DataManagerUtil.hpp"

class Processor {
public:
    void run() {
        Simulator::Initialize(u1, u2);
        DataManagerUtil::OutputData(u1, u2);
        while (Parameter::count * savT < simT) {
            Simulator::CalculatS(S1, S2, u1, u2);
            Simulator::EvaluateU(pu1, pu2, u1, u2, S1, S2, dt / 2);

            Simulator::CalculatS(S1, S2, pu1, pu2);
            Simulator::EvaluateU(u1, u2, u1, u2, S1, S2, dt);

            Time += dt;
            if (Time > savT) {
                Parameter::count += 1;
                Time = 0.0;
                DataManagerUtil::OutputData(u1, u2);
            }
        }
    };
    Processor(){
        MatrixUtil::ini(u1);
        MatrixUtil::ini(u2);
        MatrixUtil::ini(pu1);
        MatrixUtil::ini(pu1);
        MatrixUtil::ini(S1);
        MatrixUtil::ini(S2);
    }

private:
    double u1[L0 + 1][Nr + 1]{};
    double u2[L0 + 1][Nr + 1]{};
    double pu1[L0 + 1][Nr + 1]{};
    double pu2[L0 + 1][Nr + 1]{};
    double S1[L0 + 1][Nr + 1]{};
    double S2[L0 + 1][Nr + 1]{};
};

#endif //TM_C_01_PROCESSOR_H
