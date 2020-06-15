/**
 * \file
 * \brief Contains top level fpga9001 MMCM related functions
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
 * Copyright 2019 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_common_error.h"
#include "adi_common_macros.h"
#include "adi_fpga9001_mmcm.h"
#include "fpga9001_utilities.h"

static int32_t adi_fpga9001_MmcmRegBf_Get(adi_fpga9001_Device_t *device,
                                          uint8_t drpRegAddr,
                                          uint16_t *bfValue)
{
    axi_adrv9001_clk_reset_set((void *)device, AXI_ADRV9001_ID, 0x1);
    *bfValue = axi_adrv9001_mmcm_read((void *)device, AXI_ADRV9001_ID, drpRegAddr);
    axi_adrv9001_clk_reset_set((void *)device, AXI_ADRV9001_ID, 0x0);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Mmcm_Inspect(adi_fpga9001_Device_t *device, 
                                  uint32_t adrv9001DeviceClockIn_kHz,
                                  adi_fpga9001_Mmcm_ClockOutput_Divisor_e adrv9001DeviceClockOutDivisor,
                                  adi_fpga9001_MmcmCfg_t *actualMmcmCfg)
{
    uint8_t mVal = 0;
    uint8_t lowCount = 0;
    uint8_t highCount = 0;
    uint8_t clkOut = 0;
    uint8_t i = 0;
    uint8_t drpRegAddr[] = { 0x08, 0x0A, 0x0C };

    static const uint8_t CLK_FB_OUT_REG1_ADDR = 0x14;
    static const uint8_t MAX_USED_MMCM_CLKOUT = 3;

    uint16_t clkReg1 = 0;

    /* Temporarily store MCS, GPIO and TDD CLK OUT in this order in the array below */
    uint32_t clock_kHz[] = { 0, 0, 0 };
    uint32_t adrv9001DeviceClockOut_kHz = 0;


    ADI_API_ENTRY_PTR_EXPECT(device, actualMmcmCfg);

    /* Device clock divisor value should be between 0 and 6 */
    ADI_RANGE_CHECK(device, adrv9001DeviceClockOutDivisor, ADI_FPGA9001_MMCM_CLKDIV_BYPASS, ADI_FPGA9001_MMCM_CLKDIV_64);

    adrv9001DeviceClockOut_kHz = adrv9001DeviceClockIn_kHz >> adrv9001DeviceClockOutDivisor;

    ADI_EXPECT(adi_fpga9001_MmcmRegBf_Get, device, CLK_FB_OUT_REG1_ADDR, &clkReg1);
    lowCount = (uint8_t)(clkReg1 & 0x3F);
    highCount = (uint8_t)((clkReg1 >> 6) & 0x3F);

    mVal = highCount + lowCount;
    actualMmcmCfg->fpgaVcoClockFreq_kHz = mVal * adrv9001DeviceClockOut_kHz;

    for (i = 0; i < MAX_USED_MMCM_CLKOUT; i++)
    {
        ADI_EXPECT(adi_fpga9001_MmcmRegBf_Get, device, drpRegAddr[i], &clkReg1);

        lowCount = (uint8_t)(clkReg1 & 0x3F);
        highCount = (uint8_t)((clkReg1 >> 6) & 0x3F);

        clkOut = highCount + lowCount;
        clock_kHz[i] = (uint32_t)(actualMmcmCfg->fpgaVcoClockFreq_kHz / clkOut);
    }

    actualMmcmCfg->mcsClock_kHz = clock_kHz[0];
    actualMmcmCfg->observationGpiosClock_kHz = clock_kHz[1];
    actualMmcmCfg->tddClock_kHz = clock_kHz[2];

    ADI_API_RETURN(device);
}
