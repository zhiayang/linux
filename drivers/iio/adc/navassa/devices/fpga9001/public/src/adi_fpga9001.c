/**
* \file
* \brief Contains FPGA9001 API configuration and run-time type definitions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
 * Copyright 2018 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "adi_common_error.h"

#include "adi_fpga9001_user.h"
#include "adi_fpga9001.h"
#include "adi_fpga9001_error.h"
#include "adi_fpga9001_hal.h"
#include "adi_platform.h"
#include "fpga9001_utilities.h"

int32_t adi_fpga9001_HwOpen(adi_fpga9001_Device_t *device)
{
    ADI_API_ENTRY_EXPECT(device);

    adi_common_LogLevelSet(&device->common, ADI_FPGA9001_LOGGING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adi_common_hal_HwOpen(&device->common);
    
    return (device->common.error.newAction);
}

int32_t adi_fpga9001_HwClose(adi_fpga9001_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    ADI_API_ENTRY_EXPECT(device);

    recoveryAction = adi_common_hal_HwClose(&device->common);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_DEVICEHAL,
            device->common.error.errCode,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            NULL,
            "Param error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_fpga9001_SwitchBin(adi_fpga9001_Device_t *device, adi_fpga9001_Binary_e bin)
{
    // This script takes two positional arguments:
    //      1. Path to the directory containing FPGA binaries
    //      2. The "basename" of the binary to switch to
    // The script will search the directory for the latest semantic version of the file and program the FPGA
    static const char *SCRIPT = "/home/analog/platform/scripts/switch_bin.sh /home/analog/adrv9001_server/resources/Adi.Adrv9001.FpgaBinaries";
    char cmdBuf[256] = { 0 };
    strcat(cmdBuf, SCRIPT);

    /* we are still building device variants as separate binaries based on a define compiler directive. */

#ifdef SI_REV_A0

    if (bin == ADI_FPGA9001_CMOS) {
        strcat(cmdBuf, " adrv9001a_cmos");
        return(system(cmdBuf));
    }

    if (bin == ADI_FPGA9001_LVDS) {
        strcat(cmdBuf, " adrv9001a_lvds");
        return(system(cmdBuf));
    }

#else

    if (bin == ADI_FPGA9001_CMOS) {
        strcat(cmdBuf, " adrv9001_cmos");
        return(system(cmdBuf));
    }

    if (bin == ADI_FPGA9001_LVDS) {
        strcat(cmdBuf, " adrv9001_lvds");
        return(system(cmdBuf));
    }

#endif

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, bin,
        "Invalid fpga bit file parameter,  must be a valid adi_fpga9001_Binary_e enum value");
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_VersionGet(adi_fpga9001_Device_t *device, 
                                adi_fpga9001_Version_t *version)
{
    uint32_t regVal = 0;
    
    axi_sysid_sys_version_get((void *)device, AXI_SYSID_ID, &regVal);
    version->major = (uint8_t)((regVal >> 16) & 0xff);
    version->minor = (uint8_t)((regVal >>  8) & 0xff);
    version->patch = (uint8_t)((regVal >>  0) & 0xff);
    ADI_API_RETURN(device);
}
