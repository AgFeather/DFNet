#include "./c_runtime_api.h"
#include "./cpu_device_api.h"
#include "./cuda_device_api.h"
#include "./runtime_base.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <thread>

namespace dlsys {
    namespace runtime {

        class DeviceAPIManager {
        /*
        单一模式，所有函数都为静态，构造函数私有化防止被实例化
        提供对设备的操作

        */
        public:
            static const int kMaxDeviceAPI = 8; // 允许的设备种类的最大值

            static DeviceAPI *Get(DLContext ctx) {
                return Global()->GetAPI(ctx.device_type);
            }

        private:
            std::array<DeviceAPI*, kMaxDeviceAPI> api_; // 用来保存设备的array

            DeviceAPIManager() {
                std::fill(api_.begin(), api_.end(), nullptr); // 创建一个设备列表
                static CPUDeviceAPI cpu_device_api_inst;
                static CUDADeviceAPI gpu_device_api_inst;
                api_[kCPU] = static_cast<DeviceAPI *>(&cpu_device_api_inst);
                api_[kGPU] = static_cast<DeviceAPI *>(&gpu_device_api_inst);
                // 注意，这里api_只有[1] and [2]被初始化，也就意味着其它6位是null，引用会报错
            }

            static DeviceAPIManager *Global() {
                // 返回API Manager
                static DeviceAPIManager inst;
                return &inst;
            }

            DeviceAPI *GetAPI(DLDeviceType type) {
                // 获取对应类型的设备，只有CPU=1和GPU=2两个
                if (api_[type] == nullptr) {
                    std::cerr << "Device API not supported" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return api_[type];
            }
        };

        inline DLArray *DLArrayCreate_() {
            // 创建一个 DLArray并null初始化
            DLArray *arr = new DLArray();
            arr->shape = nullptr;
            arr->ndim = 0;
            arr->data = nullptr;
            return arr;
        }

        inline void DLArrayFree_(DLArray *arr) {
            if (arr != nullptr) {
                // 将一个DLArray释放
                delete[] arr->shape;
                if (arr->data != nullptr) {
                    DeviceAPIManager::Get(arr->ctx)->FreeDataSpace(arr->ctx, arr->data);
                }
            }
            delete arr;
        }

        inline size_t GetDataSize(DLArray *arr) {
            // 返回一个DLArray占用了多少字节
            size_t size = 1;
            for (index_t i = 0; i < arr->ndim; ++i) {
                size *= arr->shape[i];
            }
            // assume 32-bit float
            size *= 4;
            return size;
        }

        inline size_t GetDataAlignment(DLArray *arr) {
            // assume 32-bit float
            return 8;
        }

    } 
} 

using namespace dlsys::runtime;

int DLArrayAlloc(const index_t *shape, index_t ndim, DLContext ctx,
                 DLArrayHandle *out) {
    // 根据输入数据形状初始化一个DLArray
    DLArray *arr = nullptr;
    API_BEGIN() ;
        // 新建一个nullDLArray
        arr = DLArrayCreate_(); 
        // 初始化ndim
        arr->ndim = ndim;
        // 初始化shape
        index_t *shape_copy = new index_t[ndim];
        std::copy(shape, shape + ndim, shape_copy);
        arr->shape = shape_copy;
        // 初始化ctx
        arr->ctx = ctx;
        // 初始化data数据块
        size_t size = GetDataSize(arr);
        size_t alignment = GetDataAlignment(arr);
        arr->data = DeviceAPIManager::Get(ctx)->AllocDataSpace(ctx, size, alignment);
        // 将创建的DLArray赋值给返回
        *out = arr;
    API_END_HANDLE_ERROR(DLArrayFree_(arr));
}

int DLArrayFree(DLArrayHandle handle) {
    API_BEGIN() ;
        DLArray *arr = handle;
        DLArrayFree_(arr);
    API_END();
}

int DLArrayReshape(const DLArrayHandle handle, 
                    const index_t *new_shape, index_t new_dim) {
    // 对一个DLArray reshape
    API_BEGIN() ;
        DLArray *arr = handle;
        index_t *shape_copy = new index_t[new_dim];
        std::copy(new_shape, new_shape + new_dim, shape_copy);
        arr->shape = shape_copy;
        arr->ndim = new_dim;
    API_END();
}

int DLArrayCopyFromTo(DLArrayHandle from, DLArrayHandle to,
                      DLStreamHandle stream) {
    // 将一个DLArray复制到指定DLArray中
    API_BEGIN() ;
        size_t from_size = GetDataSize(from);
        size_t to_size = GetDataSize(to);
        // 两个DLArray的大小必须相同
        assert(from_size == to_size);
        DLContext ctx = from->ctx;
        if (ctx.device_type == kCPU) {
            ctx = to->ctx;
        } else {
            // Can not copy across different ctx types directly
            assert((to->ctx.device_type == kCPU) ||
                   (to->ctx.device_type == from->ctx.device_type));
        }
        DeviceAPIManager::Get(ctx)->CopyDataFromTo(from->data, to->data, from_size,
                                                   from->ctx, to->ctx, stream);
    API_END();
}



