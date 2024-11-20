//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_DATAMANAGERUTIL_H
#define TM_C_01_DATAMANAGERUTIL_H

#include "../Parameter/Parameter.hpp"
#include <iostream>

using namespace std;

class DataManagerUtil {
public:
    static void initialize(double);


    //从断点导入数据，重新开始模拟
    void InputData(int count, double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1]) {
        char filename[64];
        sprintf(filename, "%05d.txt", count);
        FILE *fp = fopen(filename, "r");
        //跳过前两行
        fscanf(fp, "%*[^\n]%*c");
        fscanf(fp, "%*[^\n]%*c");

        //从文件中读取数据
        for (int nr = 0; nr <= Nr; nr++) {
            for (int l = 0; l <= L0; l++)
                fscanf(fp, "%lf", &u1[l][nr]);

            for (int l = 0; l <= L0; l++)
                fscanf(fp, "%lf", &u2[l][nr]);
        }

        //关闭文件
        fclose(fp);
    }


    //导出数据
    static void OutputData(double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1]) {
        char filename[64];
        sprintf(filename, "data_%05d.txt", Parameter::count); //文件不超过1e6
        FILE *fp = fopen(filename, "w");
        fprintf(fp, "time=%lf\n", Parameter::count * savT);
        // fprintf(fp, "u1[0] u1[1] ... u2[0] u2[1] ...\n");
        fprintf(fp, "u1[0] u1[1] u2[0] u2[1]\n");

        for (int nr = 0; nr <= Nr; nr++) {
            for (int l = 0; l <= L0; l++)
                fprintf(fp, "%e ", u1[l][nr]);

            for (int l = 0; l <= L0; l++)
                fprintf(fp, "%e ", u2[l][nr]);

            fprintf(fp, "\n");
        }

        fclose(fp);
    }


};


#endif //TM_C_01_DATAMANAGERUTIL_H
