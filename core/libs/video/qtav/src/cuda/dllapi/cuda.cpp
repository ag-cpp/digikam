/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dllapi_p.h"
#include "dllapi.h"

// include nv_inc.h headers later to avoid build error. have not find out why it happens

#define __CUVID_INTERNAL //avoid replaced bt 64 api
#define __CUDA_API_VERSION_INTERNAL

#include "nv_inc.h"

using namespace dllapi;

namespace dllapi
{

namespace cuda
{

namespace
{
// the macro define class dll. so a namespace wrapper is required

//DEFINE_DLL_INSTANCE_N("cuda", "nvcuda", NULL) //now may crash for vc

static char* cuda_names[] = { "nvcuda", NULL };

DEFINE_DLL_INSTANCE_V("cuda", cuda_names)

}

DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuInit, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuDriverGetVersion, int *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuDeviceGet, CUdevice *, int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuDeviceGetCount, int *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuDeviceGetName, char *, int, CUdevice)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuDeviceComputeCapability, int *, int *, CUdevice)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuDeviceTotalMem, int *, CUdevice)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuDeviceTotalMem_v2, int *, CUdevice)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuDeviceGetProperties, CUdevprop *, CUdevice)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuDeviceGetAttribute, int *, CUdevice_attribute, CUdevice)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuCtxCreate, CUcontext *, unsigned int, CUdevice)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuCtxCreate_v2, CUcontext *, unsigned int, CUdevice)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxDestroy, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxDestroy_v2, CUcontext)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuCtxAttach, CUcontext *, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxDetach, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxPushCurrent, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxPushCurrent_v2, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxPopCurrent, CUcontext *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxPopCurrent_v2, CUcontext *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxSetCurrent, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxGetCurrent, CUcontext *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxGetDevice, CUdevice *)
DEFINE_DLLAPI_M_ARG(0, CUresult, CUDAAPI, cuCtxSynchronize)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuCtxSetLimit, CUlimit, int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuCtxGetLimit, int *, CUlimit)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxGetCacheConfig, CUfunc_cache *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxSetCacheConfig, CUfunc_cache)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxGetSharedMemConfig, CUsharedconfig *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxSetSharedMemConfig, CUsharedconfig)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuCtxGetApiVersion, CUcontext, unsigned int *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuModuleLoad, CUmodule *, const char *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuModuleLoadData, CUmodule *, const void *)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuModuleLoadDataEx, CUmodule *, const void *, unsigned int, CUjit_option *, void **)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuModuleLoadFatBinary, CUmodule *, const void *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuModuleUnload, CUmodule)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuModuleGetFunction, CUfunction *, CUmodule, const char *)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuModuleGetGlobal, CUdeviceptr *, int *, CUmodule, const char *)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuModuleGetGlobal_v2, CUdeviceptr *, int *, CUmodule, const char *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuModuleGetTexRef, CUtexref *, CUmodule, const char *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuModuleGetSurfRef, CUsurfref *, CUmodule, const char *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemGetInfo, int *, int *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemGetInfo_v2, int *, int *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemAlloc, CUdeviceptr *, int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemAlloc_v2, CUdeviceptr *, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemAllocPitch, CUdeviceptr *, int *, int, int, unsigned int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemAllocPitch_v2, CUdeviceptr *, int *, int, int, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemFree, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemFree_v2, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemGetAddressRange, CUdeviceptr *, int *, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemGetAddressRange_v2, CUdeviceptr *, int *, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemAllocHost, void **, size_t)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemAllocHost_v2, void **, size_t)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemFreeHost, void *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemHostAlloc, void **, int, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemHostGetDevicePointer, CUdeviceptr *, void *, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemHostGetDevicePointer_v2, CUdeviceptr *, void *, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemHostGetFlags, unsigned int *, void *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuDeviceGetByPCIBusId, CUdevice *, char *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuDeviceGetPCIBusId, char *, int, CUdevice)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuIpcGetEventHandle, CUipcEventHandle *, CUevent)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuIpcOpenEventHandle, CUevent *, CUipcEventHandle)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuIpcGetMemHandle, CUipcMemHandle *, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuIpcOpenMemHandle, CUdeviceptr *, CUipcMemHandle, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuIpcCloseMemHandle, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemHostRegister, void *, int, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemHostUnregister, void *)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpy, CUdeviceptr, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyPeer, CUdeviceptr, CUcontext, CUdeviceptr, CUcontext, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpyHtoD, CUdeviceptr, const void *, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpyHtoD_v2, CUdeviceptr, const void *, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpyDtoH, void *, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpyDtoH_v2, void *, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpyDtoD, CUdeviceptr, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemcpyDtoD_v2, CUdeviceptr, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoA, CUarray, int, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoA_v2, CUarray, int, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyAtoD, CUdeviceptr, CUarray, int, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyAtoD_v2, CUdeviceptr, CUarray, int, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyHtoA, CUarray, int, const void *, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyHtoA_v2, CUarray, int, const void *, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyAtoH, void *, CUarray, int, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyAtoH_v2, void *, CUarray, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyAtoA, CUarray, int, CUarray, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyAtoA_v2, CUarray, int, CUarray, int, int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy2D, const CUDA_MEMCPY2D *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy2D_v2, const CUDA_MEMCPY2D *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy2DUnaligned, const CUDA_MEMCPY2D *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy2DUnaligned_v2, const CUDA_MEMCPY2D *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy3D, const CUDA_MEMCPY3D *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy3D_v2, const CUDA_MEMCPY3D *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuMemcpy3DPeer, const CUDA_MEMCPY3D_PEER *)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyAsync, CUdeviceptr, CUdeviceptr, int, CUstream)
DEFINE_DLLAPI_M_ARG(6, CUresult, CUDAAPI, cuMemcpyPeerAsync, CUdeviceptr, CUcontext, CUdeviceptr, CUcontext, int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyHtoDAsync, CUdeviceptr, const void *, int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyHtoDAsync_v2, CUdeviceptr, const void *, int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoHAsync, void *, CUdeviceptr, size_t, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoHAsync_v2, void *, CUdeviceptr, size_t, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoDAsync, CUdeviceptr, CUdeviceptr, int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoDAsync_v2, CUdeviceptr, CUdeviceptr, int, CUstream)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyHtoAAsync, CUarray, int, const void *, int, CUstream)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyHtoAAsync_v2, CUarray, int, const void *, int, CUstream)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyAtoHAsync, void *, CUarray, int, int, CUstream)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemcpyAtoHAsync_v2, void *, CUarray, int, int, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemcpy2DAsync, const CUDA_MEMCPY2D *, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemcpy2DAsync_v2, const CUDA_MEMCPY2D *, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemcpy3DAsync, const CUDA_MEMCPY3D *, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemcpy3DAsync_v2, const CUDA_MEMCPY3D *, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemcpy3DPeerAsync, const CUDA_MEMCPY3D_PEER *, CUstream)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemsetD8, CUdeviceptr, unsigned char, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemsetD8_v2, CUdeviceptr, unsigned char, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemsetD16, CUdeviceptr, unsigned short, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemsetD16_v2, CUdeviceptr, unsigned short, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemsetD32, CUdeviceptr, unsigned int, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuMemsetD32_v2, CUdeviceptr, unsigned int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemsetD2D8, CUdeviceptr, int, unsigned char, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemsetD2D8_v2, CUdeviceptr, int, unsigned char, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemsetD2D16, CUdeviceptr, int, unsigned short, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemsetD2D16_v2, CUdeviceptr, int, unsigned short, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemsetD2D32, CUdeviceptr, int, unsigned int, int, int)
DEFINE_DLLAPI_M_ARG(5, CUresult, CUDAAPI, cuMemsetD2D32_v2, CUdeviceptr, int, unsigned int, int, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemsetD8Async, CUdeviceptr, unsigned char, int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemsetD16Async, CUdeviceptr, unsigned short, int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemsetD32Async, CUdeviceptr, unsigned int, int, CUstream)
DEFINE_DLLAPI_M_ARG(6, CUresult, CUDAAPI, cuMemsetD2D8Async, CUdeviceptr, int, unsigned char, int, int, CUstream)
DEFINE_DLLAPI_M_ARG(6, CUresult, CUDAAPI, cuMemsetD2D16Async, CUdeviceptr, int, unsigned short, int, int, CUstream)
DEFINE_DLLAPI_M_ARG(6, CUresult, CUDAAPI, cuMemsetD2D32Async, CUdeviceptr, int, unsigned int, int, int, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArrayCreate, CUarray *, const CUDA_ARRAY_DESCRIPTOR *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArrayCreate_v2, CUarray *, const CUDA_ARRAY_DESCRIPTOR *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArrayGetDescriptor, CUDA_ARRAY_DESCRIPTOR *, CUarray)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArrayGetDescriptor_v2, CUDA_ARRAY_DESCRIPTOR *, CUarray)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuArrayDestroy, CUarray)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArray3DCreate, CUarray *, const CUDA_ARRAY3D_DESCRIPTOR *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArray3DCreate_v2, CUarray *, const CUDA_ARRAY3D_DESCRIPTOR *)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArray3DGetDescriptor, CUDA_ARRAY3D_DESCRIPTOR *, CUarray)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuArray3DGetDescriptor_v2, CUDA_ARRAY3D_DESCRIPTOR *, CUarray)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuPointerGetAttribute, void *, CUpointer_attribute, CUdeviceptr)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuStreamCreate, CUstream *, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuStreamWaitEvent, CUstream, CUevent, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuStreamQuery, CUstream)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuStreamSynchronize, CUstream)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuStreamDestroy, CUstream)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuStreamDestroy_v2, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuEventCreate, CUevent *, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuEventRecord, CUevent, CUstream)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuEventQuery, CUevent)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuEventSynchronize, CUevent)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuEventDestroy, CUevent)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuEventDestroy_v2, CUevent)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuEventElapsedTime, float *, CUevent, CUevent)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuFuncGetAttribute, int *, CUfunction_attribute, CUfunction)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuFuncSetCacheConfig, CUfunction, CUfunc_cache)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuFuncSetSharedMemConfig, CUfunction, CUsharedconfig)
DEFINE_DLLAPI_M_ARG(11, CUresult, CUDAAPI, cuLaunchKernel, CUfunction, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, CUstream, void **, void **)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuFuncSetBlockShape, CUfunction, int, int, int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuFuncSetSharedSize, CUfunction, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuParamSetSize, CUfunction, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuParamSeti, CUfunction, int, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuParamSetf, CUfunction, int, float)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuParamSetv, CUfunction, int, void *, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuLaunch, CUfunction)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuLaunchGrid, CUfunction, int, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuLaunchGridAsync, CUfunction, int, int, CUstream)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuParamSetTexRef, CUfunction, int, CUtexref)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuTexRefSetArray, CUtexref, CUarray, unsigned int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuTexRefSetAddress, int *, CUtexref, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuTexRefSetAddress_v2, int *, CUtexref, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuTexRefSetAddress2D, CUtexref, const CUDA_ARRAY_DESCRIPTOR *, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuTexRefSetAddress2D_v3, CUtexref, const CUDA_ARRAY_DESCRIPTOR *, CUdeviceptr, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuTexRefSetFormat, CUtexref, CUarray_format, int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuTexRefSetAddressMode, CUtexref, int, CUaddress_mode)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefSetFilterMode, CUtexref, CUfilter_mode)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefSetFlags, CUtexref, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefGetAddress, CUdeviceptr *, CUtexref)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefGetAddress_v2, CUdeviceptr *, CUtexref)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefGetArray, CUarray *, CUtexref)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuTexRefGetAddressMode, CUaddress_mode *, CUtexref, int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefGetFilterMode, CUfilter_mode *, CUtexref)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuTexRefGetFormat, CUarray_format *, int *, CUtexref)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuTexRefGetFlags, unsigned int *, CUtexref)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuTexRefCreate, CUtexref *)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuTexRefDestroy, CUtexref)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuSurfRefSetArray, CUsurfref, CUarray, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuSurfRefGetArray, CUarray *, CUsurfref)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuDeviceCanAccessPeer, int *, CUdevice, CUdevice)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuCtxEnablePeerAccess, CUcontext, unsigned int)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuCtxDisablePeerAccess, CUcontext)
DEFINE_DLLAPI_M_ARG(1, CUresult, CUDAAPI, cuGraphicsUnregisterResource, CUgraphicsResource)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuGraphicsSubResourceGetMappedArray, CUarray *, CUgraphicsResource, unsigned int, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuGraphicsResourceGetMappedPointer, CUdeviceptr *, int *, CUgraphicsResource)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuGraphicsResourceGetMappedPointer_v2, CUdeviceptr *, int *, CUgraphicsResource)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuGraphicsResourceSetMapFlags, CUgraphicsResource, unsigned int)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuGraphicsMapResources, unsigned int, CUgraphicsResource *, CUstream)
DEFINE_DLLAPI_M_ARG(3, CUresult, CUDAAPI, cuGraphicsUnmapResources, unsigned int, CUgraphicsResource *, CUstream)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuGetExportTable, const void **, const CUuuid *)

#if defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)

DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemAllocHost, void **, unsigned int)
DEFINE_DLLAPI_M_ARG(2, CUresult, CUDAAPI, cuMemAllocHost_v2, void **, unsigned int)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoHAsync, void *, CUdeviceptr, unsigned int, CUstream)
DEFINE_DLLAPI_M_ARG(4, CUresult, CUDAAPI, cuMemcpyDtoHAsync_v2, void *, CUdeviceptr, unsigned int, CUstream)

#endif

} // namespace cuda

} // namespace dllapi
