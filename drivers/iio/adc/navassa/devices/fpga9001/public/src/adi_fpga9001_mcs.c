/**
* \file
* \brief FPGA9001 Multi-Chip Synchronization (MCS) functions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/
#include "adi_adrv9001_user.h"
#include "adi_fpga9001_mcs.h"
#include "axi_adrv9001.h"

static int32_t adi_fpga9001_Mcs_Start_Validate(adi_fpga9001_Device_t *device, 
                                               uint8_t numberOfPulses, 
                                               uint8_t mcsPeriod,
                                               uint8_t mcsPulseWidth,
                                               adi_fpga9001_McsEdge_e edge)
{
    ADI_RANGE_CHECK(device, numberOfPulses, 1, 16);
    ADI_RANGE_CHECK(device, mcsPulseWidth, 1, 8);
    ADI_RANGE_CHECK(device, edge, ADI_FPGA9001_MCS_EDGE_RISING, ADI_FPGA9001_MCS_EDGE_FALLING);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Mcs_Start(adi_fpga9001_Device_t *device, 
                               uint8_t numberOfPulses, 
                               uint8_t mcsPeriod,
                               uint8_t mcsPulseWidth,
                               adi_fpga9001_McsEdge_e edge)
{
    struct axi_adrv9001_mcs_params mcs_params;
    static const uint32_t timeOut = 1000*100;
    
    ADI_PERFORM_VALIDATION(adi_fpga9001_Mcs_Start_Validate, device, numberOfPulses, mcsPeriod, mcsPulseWidth, edge);

    axi_adrv9001_timer_set((void *)device, AXI_ADRV9001_ID, timeOut);
    while (1)
    {
        if (axi_adrv9001_mcs_status((void *)device, AXI_ADRV9001_ID) == 0) break;
        if (axi_adrv9001_timer_get((void *)device, AXI_ADRV9001_ID) == 0)
        {
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_RESET_FEATURE, 0,
                "Time out waiting for previous MCS sequence to complete.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    mcs_params.period = mcsPeriod - 1;
    mcs_params.width = mcsPulseWidth - 1;
    mcs_params.count = numberOfPulses - 1;
    mcs_params.fall1_rise0 = (edge == ADI_FPGA9001_MCS_EDGE_FALLING) ? 1 : 0;
    axi_adrv9001_mcs_config((void *)device, AXI_ADRV9001_ID, &mcs_params);
    axi_adrv9001_mcs_start((void *)device, AXI_ADRV9001_ID);
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_Mcs_Requested_Validate(adi_fpga9001_Device_t *device, bool *requested)
{
    ADI_NULL_PTR_RETURN(&device->common, requested);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Mcs_Requested(adi_fpga9001_Device_t *device, bool *requested)
{

    ADI_PERFORM_VALIDATION(adi_fpga9001_Mcs_Requested_Validate, device, requested);
    *requested = false;
    if (axi_adrv9001_gpint_status((void *)device, AXI_ADRV9001_ID) == 1) 
    {
        *requested = true;
        axi_adrv9001_gpint_status_clear((void *)device, AXI_ADRV9001_ID);
    }

    ADI_API_RETURN(device);
}

