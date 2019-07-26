#include "./cuda_device_api.h"
#include <cassert>
#include <cuda_runtime.h>
#include <iostream>

// 定义CUDA报错宏
#define CUDA_CALL(func)                                                        \
  {                                                                            \
    cudaError_t e = (func);                                                    \
    assert((e == cudaSuccess) || (e == cudaErrorCudartUnloading));             \
  }

namespace dlsys {
    namespace runtime {

        static void GPUCopy(const void *from, void *to, size_t size,
                            cudaMemcpyKind kind, cudaStream_t stream) {
            if (stream != 0) {
                CUDA_CALL(cudaMemcpyAsync(to, from, size, kind, stream));
            } else {
                CUDA_CALL(cudaMemcpy(to, from, size, kind));
            }
        }

        void *CUDADeviceAPI::AllocDataSpace(DLContext ctx, size_t size,
                                            size_t alignment) {
            // 在CUDA上申请内存
            CUDA_CALL(cudaSetDevice(ctx.device_id));
            assert((256 % alignment) == 0U); // << "CUDA space is aligned at 256 bytes";
            void *ret;
            CUDA_CALL(cudaMalloc(&ret, size));
            return ret;
        }

        void CUDADeviceAPI::FreeDataSpace(DLContext ctx, void *ptr) {
            // 释放CUDA内存
            CUDA_CALL(cudaSetDevice(ctx.device_id));
            CUDA_CALL(cudaFree(ptr));
        }

        void CUDADeviceAPI::CopyDataFromTo(const void *from, void *to, size_t size,
                                           DLContext ctx_from, DLContext ctx_to, DLStreamHandle stream) {
            // 数据复制
            cudaStream_t cu_stream = static_cast<cudaStream_t>(stream);
            if (ctx_from.device_type == kGPU && ctx_to.device_type == kGPU) {
                // from 和 to 都是GPU上的内存时，直接调用CUDA的复制函数
                CUDA_CALL(cudaSetDevice(ctx_from.device_id));
                if (ctx_from.device_id == ctx_to.device_id) {
                    GPUCopy(from, to, size, cudaMemcpyDeviceToDevice, cu_stream);
                } else {
                    cudaMemcpyPeerAsync(to, ctx_to.device_id, from, ctx_from.device_id,
                                        size, cu_stream);
                }
            } else if (ctx_from.device_type == kGPU && ctx_to.device_type == kCPU) {
                // 数据从GPU复制到CPU上
                CUDA_CALL(cudaSetDevice(ctx_from.device_id));
                GPUCopy(from, to, size, cudaMemcpyDeviceToHost, cu_stream);
            } else if (ctx_from.device_type == kCPU && ctx_to.device_type == kGPU) {
                // 数据从CPU复制到GPU上
                CUDA_CALL(cudaSetDevice(ctx_to.device_id));
                GPUCopy(from, to, size, cudaMemcpyHostToDevice, cu_stream);
            } else {
                std::cerr << "expect copy from/to GPU or between GPU" << std::endl;
            }
        }

        void CUDADeviceAPI::StreamSync(DLContext ctx, DLStreamHandle stream) {
            CUDA_CALL(cudaSetDevice(ctx.device_id));
            CUDA_CALL(cudaStreamSynchronize(static_cast<cudaStream_t>(stream)));
        }

    } // namespace runtime
} // namespace dlsys