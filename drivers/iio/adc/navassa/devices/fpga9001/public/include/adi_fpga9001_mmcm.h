/**
 * \file
 * \brief Contains top level fpga9001 MMCM related function prototypes
 *
 * Refer to https://www.xilinx.com/support/documentation/application_notes/xapp888_7Series_DynamicRecon.pdf
 * for more info on MMCM DRP registers
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */
/**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef _ADI_FPGA9001_MMCM_H_
#define _ADI_FPGA9001_MMCM_H_

#include "adi_fpga9001_types.h"
#include "adi_fpga9001_mmcm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Inspect the configuration to the FPGA MMCM clock tipe via the Dynamic Reconfiguration Port (DRP)
 *
 * \param[in]  fpga9001                         Context variable - Pointer to FPGA9001 device data structure
 * \param[in]  adrv9001DeviceClockIn_kHz        ADRV9001 device clock input
 * \param[in]  adrv9001DeviceClockOutDivisor    ADRV9001 device clock output divisor; An enum type ranging from 0 to 6
 * \param[out] actualMmcmCfg                    The current (or actual) MMCM configuration read back from ADRV9001
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga9001_Mmcm_Inspect(adi_fpga9001_Device_t *fpga9001,
                                  uint32_t adrv9001DeviceClockIn_kHz,
                                  adi_fpga9001_Mmcm_ClockOutput_Divisor_e adrv9001DeviceClockOutDivisor,
                                  adi_fpga9001_MmcmCfg_t *actualMmcmCfg);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA9001_MMCM_H_ */