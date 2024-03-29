#ifndef DLSYS_RUNTIME_C_RUNTIME_API_H_
#define DLSYS_RUNTIME_C_RUNTIME_API_H_

#ifdef __cplusplus
#define DLSYS_EXTERN_C extern "C"
#else
#define DLSYS_EXTERN_C
#endif

#include "dlarray.h"
#include <stddef.h>
#include <stdint.h>

DLSYS_EXTERN_C {
    typedef int64_t index_t;
    typedef DLArray *DLArrayHandle;
    typedef void *DLStreamHandle;

    int DLArrayAlloc(const index_t *shape, index_t ndim, DLContext ctx,
                 DLArrayHandle *out);
    int DLArrayFree(DLArrayHandle handle);
    int DLArrayCopyFromTo(DLArrayHandle from, DLArrayHandle to,
                      DLStreamHandle stream);
    int DLArrayReshape(const DLArrayHandle handle, const index_t *new_shape, index_t new_dim);

    // GPU上的数组初始化
    int DLGpuArraySet(DLArrayHandle arr, float value);
    // GPU广播机制
    int DLGpuBroadcastTo(const DLArrayHandle input, DLArrayHandle output);
    // 对第一个维度进行累加
    int DLGpuReduceSumAxisZero(const DLArrayHandle input, DLArrayHandle output);
    // 矩阵对应元素相加
    int DLGpuMatrixElementwiseAdd(const DLArrayHandle matA,
                              const DLArrayHandle matB, DLArrayHandle output);
    int DLGpuMatrixElementwiseAddByConst(const DLArrayHandle input, float val,
                                     DLArrayHandle output);


    int DLGpuMatrixElementwiseSubtract(const DLArrayHandle matA,
                                   const DLArrayHandle matB, DLArrayHandle output);

    int DLGpuMatrixElementwiseSubtractByConst(const DLArrayHandle input, float val,
                                          DLArrayHandle output);
    int DLGpuMatrixElementwiseMultiply(
        const DLArrayHandle matA, const DLArrayHandle matB, DLArrayHandle output);
    int DLGpuMatrixMultiplyByConst(const DLArrayHandle input, float val,
                               DLArrayHandle output);

    int DLGpuMatrixElementwiseDivByConst(const DLArrayHandle matA, float val,
                                     DLArrayHandle output);

    // 
    int DLGpuMatrixMultiply(const DLArrayHandle matA, bool transposeA,
                        const DLArrayHandle matB, bool transposeB,
                        DLArrayHandle matC);
    // GPU ReLU
    int DLGpuRelu(const DLArrayHandle input, DLArrayHandle output);
    // GPU 上对ReLU求导
    int DLGpuReluGradient(const DLArrayHandle input, const DLArrayHandle in_grad,
                      DLArrayHandle output);
    // GPU softmax
    int DLGpuSoftmax(const DLArrayHandle input, DLArrayHandle output);
    // GPU softmax交叉熵
    int DLGpuSoftmaxCrossEntropy(const DLArrayHandle input_a,
                             const DLArrayHandle input_b,
                             DLArrayHandle output);

    int DLGpuMatrixElementwiseSqrt(const DLArrayHandle input_a, DLArrayHandle output);
}

#endif
