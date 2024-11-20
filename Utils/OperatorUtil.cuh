//
// Created by Administrator on 2024/7/24.
//

#ifndef TM_C_01_OPERATORUTILCU_CUH
#define TM_C_01_OPERATORUTILCU_CUH

#include "../Parameter/Parameter.hpp"
#include <cuda_runtime.h>
#include <iostream>
#include <device_launch_parameters.h>

// CUDA 错误检查宏
#define CUDA_CHECK_ERROR(call) do {                                \
    cudaError_t err = call;                                        \
    if(err != cudaSuccess) {                                       \
        std::cerr << "CUDA Error: " << cudaGetErrorString(err)     \
                  << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        exit(1);                                                   \
    }                                                              \
} while(0)
// 在cuda中实现 中心有限差分算子的实现

__global__ void KernelDifference(double* ux,double *u,int L0,int Nr,double dr2,int pitch){
    int nr=blockIdx.y*blockDim.y+threadIdx.y;
    int l=blockIdx.x*blockDim.x+threadIdx.x;
    if(l<=L0 && nr>0 &&nr<Nr){
        int idx=l*pitch+nr;
        // ux[idx]=(u[l][nr+1]-u[l][nr-1])/dr2;
        ux[idx]=(u[idx+1]-u[idx-1])/dr2;
    }
}
void Difference(double ux[L0+1][Nr+1],double u[L0+1][Nr+1]){
    double *d_u;
    double *d_ux;
    int size=(L0+1)*(Nr+1)*sizeof(double);
    CUDA_CHECK_ERROR(cudaMalloc(&d_u,size));
    CUDA_CHECK_ERROR(cudaMalloc(&d_ux,size));
    CUDA_CHECK_ERROR(cudaMemcpy(d_u,u,size,cudaMemcpyHostToDevice));
    // CUDA_CHECK_ERROR(cudaMalloc(&d_u,sizeof(double)*(L0+1)*(Nr+1)));
    // CUDA_CHECK_ERROR(cudaMalloc(&d_ux,sizeof(double)*(L0+1)*(Nr+1)));
    // CUDA_CHECK_ERROR(cudaMemcpy(d_u,u,sizeof(double)*(L0+1)*(Nr+1),cudaMemcpyHostToDevice));
    dim3 blockSize(32,32);
    dim3 gridSize((L0+blockSize.x-1)/blockSize.x,(Nr+blockSize.y-1)/blockSize.y);
    KernelDifference<<<gridSize,blockSize>>>(d_ux,d_u,L0,Nr,dr*2,(L0+1));
    // CUDA_CHECK_ERROR(cudaGetLastError());
    // CUDA_CHECK_ERROR(cudaMemcpy(ux,d_ux,sizeof(double)*(L0+1)*(Nr+1),cudaMemcpyDeviceToHost));
    CUDA_CHECK_ERROR(cudaMemcpy(ux,d_ux,size,cudaMemcpyDeviceToHost));
    CUDA_CHECK_ERROR(cudaFree(d_u));
    CUDA_CHECK_ERROR(cudaFree(d_ux));
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


#endif // TM_C_01_OPERATORUTILCU_CUH