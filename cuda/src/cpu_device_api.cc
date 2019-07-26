#include "./cpu_device_api.h"

#include <cstdlib>
#include <cstring>
#include <iostream>


namespace dlsys{
    namespace runtime{
        void *CPUDeviceAPI::AllocDataSpace(DLContext ctx, size_t size,
                                           size_t alignment){
            void *ptr;
            int ret = posix_memalign(&ptr, alignment, size); 
            // 返回size字节的动态内存，类似于malloc，内存的地址时alignment的倍数
            if(ret != 0)
                throw std::bad_alloc();
            return ptr;
        }
        void CPUDeviceAPI::FreeDataSpace(DLContext ctx, void *ptr) { 
            free(ptr); 
        }

        void CPUDeviceAPI::CopyDataFromTo(const void *from, void *to, size_t size,
                                          DLContext ctx_from, DLContext ctx_to,
                                          DLStreamHandle stream) {
            memcpy(to, from, size);
        }

        void CPUDeviceAPI::StreamSync(DLContext ctx, DLStreamHandle stream) {
            // CPU不需要同步
        }
    }

}