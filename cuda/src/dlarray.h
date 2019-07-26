#ifndef DLSYS_H_
#define DLSYS_H_


#ifdef __cplusplus
#define DLSYS_EXTERN_C extern "C"
#else

#define DLSYS_EXTERN_C
#endif

#include <stddef.h>
#include <stdint.h>

DLSYS_EXTERN_C {

    // 定义设备类型，包括CPU和GPU两个版本
    typedef enum{
        kCPU = 1,
        kGPU = 2,
    }DLDeviceType;

    // 定义设备的上下文，包括设备id和设备类型
    typedef struct{
        int device_id;
        DLDeviceType device_type;
    }DLContext;

    // 定义数据结构DLArray，包含数据指针data，运行的设备上下文ctx，维度ndim和形状shape
    typedef struct{
        void *data;
        DLContext ctx;
        int ndim;
        int64_t *shape;
    }DLArray;
}
#endif