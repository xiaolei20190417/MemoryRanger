// Copyright (c) 2015-2018, Satoshi Tanda. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.

/// @file
/// Declares interfaces to VMM functions.

#ifndef HYPERPLATFORM_VMM_H_
#define HYPERPLATFORM_VMM_H_

#include <fltKernel.h> //#include <ntddk.h>

////////////////////////////////////////////////////////////////////////////////
//
// macro utilities
//

////////////////////////////////////////////////////////////////////////////////
//
// constants and macros
//

////////////////////////////////////////////////////////////////////////////////
//
// types
//

/// Represents VMM related data shared across all processors
struct SharedProcessorData {
  volatile long reference_count;  //!< Number of processors sharing this data
  void* msr_bitmap;               //!< Bitmap to activate MSR I/O VM-exit
  void* io_bitmap_a;              //!< Bitmap to activate IO VM-exit (~ 0x7FFF)
  void* io_bitmap_b;              //!< Bitmap to activate IO VM-exit (~ 0xffff)
  struct SharedShadowHookData* shared_sh_data;  //!< Shared shadow hook data
};

/// Represents VMM related data associated with each processor
struct ProcessorData {
  SharedProcessorData* shared_data;         //!< Shared data
  void* vmm_stack_limit;                    //!< A head of VA for VMM stack
  struct VmControlStructure* vmxon_region;  //!< VA of a VMXON region
  struct VmControlStructure* vmcs_region;   //!< VA of a VMCS region
  struct EptData* ept_data;                 //!< A pointer to EPT related data
  
  struct EptData* ept_data_default_enclave;          //!< A pointer to EPT related data
  struct EptData* ept_data_token;                    //!< A pointer to EPT related data

  //struct EptData* ept_data_monitor;         //!< A pointer to EPT related data
  struct EptData* ept_data_protected_driver;         //!< A pointer to EPT related data

  struct RweData* rwe_data;                 //!< TBD
  void* xsave_area;                         //!< VA to store state components
  ULONG64 xsave_inst_mask;                  //!< A mask to save state components
  UCHAR fxsave_area[512 + 16];              //!< for fxsave (+16 for alignment)
  struct ShadowHookData* sh_data;           //!< Per-processor shadow hook data
};

/// nt!_KTRAP_FRAME on x86
struct KtrapFrameX86 {
  ULONG reserved1[26];
  ULONG ip;  //!< Called EIP in _KTRAP_FRAME
  ULONG reserved2[2];
  ULONG sp;  //!< Called HardwareEsp in _KTRAP_FRAME
  ULONG reserved3[5];
};
static_assert(sizeof(KtrapFrameX86) == 0x8c, "structure size mismatch");
static_assert(FIELD_OFFSET(KtrapFrameX86, ip) == 0x68, "structure size mismatch");
static_assert(FIELD_OFFSET(KtrapFrameX86, sp) == 0x74, "structure size mismatch");

/// nt!_KTRAP_FRAME on x64
struct KtrapFrameX64 {
  ULONG64 reserved1[45];
  ULONG64 ip;  //!< Called EIP in _KTRAP_FRAME
  ULONG64 reserved2[2];
  ULONG64 sp;  //!< Called Rsp in _KTRAP_FRAME
  ULONG64 reserved3;
};
static_assert(sizeof(KtrapFrameX64) == 0x190, "structure size mismatch");
static_assert(FIELD_OFFSET(KtrapFrameX64, ip) == 0x168, "structure size mismatch");
static_assert(FIELD_OFFSET(KtrapFrameX64, sp) == 0x180, "structure size mismatch");

/// See: Stack Usage on Transfers to Interrupt and Exception-Handling Routines
struct MachineFrame {
  ULONG_PTR ip;
  ULONG_PTR cs;
  ULONG_PTR flags;
  ULONG_PTR sp;
  ULONG_PTR ss;
};

#if defined(_AMD64_)
using KtrapFrame = KtrapFrameX64;
#else
using KtrapFrame = KtrapFrameX86;
#endif

////////////////////////////////////////////////////////////////////////////////
//
// prototypes
//

////////////////////////////////////////////////////////////////////////////////
//
// variables
//

////////////////////////////////////////////////////////////////////////////////
//
// implementations
//

#endif  // HYPERPLATFORM_VMM_H_
