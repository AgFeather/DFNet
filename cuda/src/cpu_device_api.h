#ifndef DLSYS_RUNTIME_CPU_DEVICE_API_H_
#define DLSYS_RUNTIME_CPU_DEVICE_API_H_

#include "c_runtime_api.h"
#include "device_api.h"
#include <assert.h>
#include <string>

namespace dlsys{
    namespace runtime{
        class CPUDeviceAPI:public DeviceAPI{
            // CPU对数据进行操作，对DeviceAPI接口的实现
            public:
                void *AllocDataSpace(DLContext ctx, size_t size, size_t alignment) final;
                void FreeDataSpace(DLContext ctx, void *ptr) final;
                void CopyDataFromTo(const void *from, void *to, size_t size,
                                DLContext ctx_from, DLContext ctx_to, DLStreamHandle stream) final;
                void StreamSync(DLContext ctx, DLStreamHandle stream) final;
        };
    }
}

#endif