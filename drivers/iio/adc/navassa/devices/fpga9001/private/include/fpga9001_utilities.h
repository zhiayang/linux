/**
 * \file
 * \brief Contains FPGA9001 utility functions like common math routines.
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the FPGA9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _FPGA9001_UTILITIES_H_
#define _FPGA9001_UTILITIES_H_

#include <stdbool.h>
#include "adi_fpga9001_types.h"
#include "adi_common_error.h"
#include "adi_common_macros.h"
#include "axi_common.h"
#include "axi_dma.h"
#include "axi_sysid.h"
#include "axi_adrv9001.h"
#include "axi_adrv9001_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief This utility function calculates the LCM of two unsigned integers
*
* \param[in]  fpga9001       Context variable - Pointer to FPGA9001 device data structure
* \param[in]  number1        First number of LCM function
* \param[in]  number2        Second number of LCM function
* \param[out] lcm            The least common multiple of number1 and number2
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t fpga9001_Utilities_CalculateLcm(adi_fpga9001_Device_t *fpga9001,
                                        uint32_t number1,
                                        uint32_t number2,
                                        uint32_t *lcm);

uint32_t fpga9001_SsiIdGet(adi_fpga9001_Device_t *device,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel);

uint32_t fpga9001_TddIdGet(adi_fpga9001_Device_t *device,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel);
    
uint32_t fpga9001_TddDmaIdGet(adi_fpga9001_Device_t *device,
                              adi_common_Port_e port,
                              adi_common_ChannelNumber_e channel);

uint32_t fpga9001_DMAIdGet(adi_fpga9001_Device_t *device,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel);

uint32_t fpga9001_BufBaseAddrGet(adi_common_Port_e port,
                                 adi_common_ChannelNumber_e channel);

#ifdef __cplusplus
}
#endif

#endif /* _FPGA9001_UTILITIES_H_ */
