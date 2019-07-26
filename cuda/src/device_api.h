#ifndef DLSYS_RUNTIME_DEVICE_API_H_
#define DLSYS_RUNTIME_DEVICE_API_H_

#include "c_runtime_api.h"
#include <assert.h>
#include <string>


namespace dlsys {
    namespace runtime {

        class DeviceAPI{
            /* 对数据结构DLArray进行操作。
             * DeviceAPI是一个接口类，包括CPU实现和GPU实现
             * 
            */
            public:
                virtual ~DeviceAPI(){}
                // 分配指定大小的空间
                virtual void *AllocDataSpace(DLContext ctx, size_t size,
                                         size_t alignment) = 0;
                // 释放空间
                virtual void FreeDataSpace(DLContext ctx, void *ptr) = 0;
                // 将数据从from转移到to
                virtual void CopyDataFromTo(const void *from, void *to, size_t size,
                                        DLContext ctx_from, DLContext ctx_to,
                                        DLStreamHandle stream) = 0;
                // 流同步
                 virtual void StreamSync(DLContext ctx, DLStreamHandle stream) = 0;
        };
    }
}

#endif