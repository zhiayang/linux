/**
 * \file
 * \brief Functions for FPGA9001 Synchronous Serial Interface (SSI) configuration
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adrv9001_user.h"
#include "adi_common_error.h"
#include "adi_common_macros.h"
#include "adi_common_types.h"
#include "adrv9001_reg_addr_macros.h"
#include "adi_fpga9001_ssi.h"
#include "adi_fpga9001_hal.h"
#include "axi_adrv9001.h"
#include "axi_adrv9001_regs.h"
#include "fpga9001_utilities.h"

static uint32_t fpga9001_SsiFormat2NumOfBits(adi_fpga9001_Device_t *device, adi_fpga9001_SsiFormat_e format)
{
    if (format == ADI_FPGA9001_SSI_FORMAT_2_BIT_SYMBOL_DATA) return(AXI_ADRV9001_SSI_NUM_OF_BITS_02);
    if (format == ADI_FPGA9001_SSI_FORMAT_8_BIT_SYMBOL_DATA) return(AXI_ADRV9001_SSI_NUM_OF_BITS_08);
    if (format == ADI_FPGA9001_SSI_FORMAT_16_BIT_SYMBOL_DATA) return(AXI_ADRV9001_SSI_NUM_OF_BITS_16);
    if (format == ADI_FPGA9001_SSI_FORMAT_12_BIT_I_Q_DATA) return(AXI_ADRV9001_SSI_NUM_OF_BITS_24);
    if (format == ADI_FPGA9001_SSI_FORMAT_16_BIT_I_Q_DATA) return(AXI_ADRV9001_SSI_NUM_OF_BITS_32);

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
        ADI_COMMON_ACT_ERR_CHECK_PARAM, format, "Invalid parameter value, format.");
    return((uint32_t) -1);
}

static uint32_t fpga9001_NumOfBits2SsiFormat(adi_fpga9001_Device_t *device, uint32_t regVal)
{
    if (regVal == AXI_ADRV9001_SSI_NUM_OF_BITS_02) return(ADI_FPGA9001_SSI_FORMAT_2_BIT_SYMBOL_DATA);
    if (regVal == AXI_ADRV9001_SSI_NUM_OF_BITS_08) return(ADI_FPGA9001_SSI_FORMAT_8_BIT_SYMBOL_DATA);
    if (regVal == AXI_ADRV9001_SSI_NUM_OF_BITS_16) return(ADI_FPGA9001_SSI_FORMAT_16_BIT_SYMBOL_DATA);
    if (regVal == AXI_ADRV9001_SSI_NUM_OF_BITS_24) return(ADI_FPGA9001_SSI_FORMAT_12_BIT_I_Q_DATA);
    if (regVal == AXI_ADRV9001_SSI_NUM_OF_BITS_32) return(ADI_FPGA9001_SSI_FORMAT_16_BIT_I_Q_DATA);

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
        ADI_COMMON_ACT_ERR_CHECK_PARAM, regVal, "Invalid register settings, SSI number of bits.");
    return((uint32_t) -1);
}

static int32_t adi_fpga9001_ssi_Mode_Set_Validate(adi_fpga9001_Device_t *device,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  bool sdrEnable,
                                                  bool multiLane,
                                                  adi_fpga9001_SsiFormat_e format)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, format, ADI_FPGA9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_FPGA9001_SSI_FORMAT_16_BIT_I_Q_DATA);
    ADI_API_RETURN(device);
}

/* this is a disaster function, there aren't enough information to call driver directly */
/* for now, override driver and set register directly. */

int32_t adi_fpga9001_ssi_Mode_Set(adi_fpga9001_Device_t *device,
                                  adi_common_Port_e port,
                                  adi_common_ChannelNumber_e channel,
                                  bool sdrEnable,
                                  bool multiLane,
                                  adi_fpga9001_SsiFormat_e format)
{
    uint32_t ssi_id;
    uint32_t read_data;
    uint32_t num_of_bits;
    adi_common_Port_e port_hack;

    ADI_PERFORM_VALIDATION(adi_fpga9001_ssi_Mode_Set_Validate, device, port, channel, sdrEnable, multiLane, format);

    /* hack to keep some tests to happy. */

    port_hack = port;

    if ((port == ADI_ORX) || (port == ADI_ILB) || (port == ADI_ELB)) {
        port_hack = ADI_RX;
    }

    ssi_id = fpga9001_SsiIdGet(device, port_hack, channel);
    if (ssi_id == (uint32_t) -1) return(-1);

    num_of_bits = fpga9001_SsiFormat2NumOfBits(device, format);
    if (num_of_bits == (uint32_t) -1) return(-1);

    axi_reg_write((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR),
        (AXI_ADRV9001_SSI_CLK_RATE_SET(0) |
        AXI_ADRV9001_SSI_NUM_OF_BITS_SET(num_of_bits) |
        AXI_ADRV9001_SSI_LSTRB1_PSTRB0_SET(0) |
        AXI_ADRV9001_SSI_MLANE1_SLANE0_SET(((multiLane) ? 1 : 0)) |
        AXI_ADRV9001_SSI_SDR1_DDR0_SET(((sdrEnable) ? 1 : 0)) |
        AXI_ADRV9001_SSI_LSB1_MSB0_SET(0) |
        AXI_ADRV9001_SSI_Q1_I0_SET(0) |
        AXI_ADRV9001_SSI_FALL1_RISE0_SET(0)));

    axi_reg_write((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_INIT_ADDR), 0x1);
    axi_reg_write((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_TDD_SWRESET_ADDR, 0x1);
    read_data = axi_reg_read((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_IO_CONTROL_ADDR));
    axi_reg_write((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_IO_CONTROL_ADDR),
        (read_data | AXI_ADRV9001_SSI_BUF_ENABLE_SET(0x1)));
    ADI_API_RETURN(device);
}


static int32_t adi_fpga9001_ssi_Mode_Get_Validate(adi_fpga9001_Device_t *device,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  bool *sdrEnabled,
                                                  bool *multiLane,
                                                  adi_fpga9001_SsiFormat_e *format)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, sdrEnabled);
    ADI_NULL_PTR_RETURN(&device->common, multiLane);
    ADI_NULL_PTR_RETURN(&device->common, format);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_ssi_Mode_Get(adi_fpga9001_Device_t *device,
                                  adi_common_Port_e port,
                                  adi_common_ChannelNumber_e channel,
                                  bool *sdrEnabled,
                                  bool *multiLane,
                                  adi_fpga9001_SsiFormat_e *format)
{
    uint32_t ssi_id;
    uint32_t read_data;
    adi_common_Port_e port_hack;

    ADI_PERFORM_VALIDATION(adi_fpga9001_ssi_Mode_Get_Validate, device, port, channel, sdrEnabled, multiLane, format);

    /* hack to keep some tests to happy. */

    port_hack = port;

    if ((port == ADI_ORX) || (port == ADI_ILB) || (port == ADI_ELB)) {
        port_hack = ADI_RX;
    }

    ssi_id = fpga9001_SsiIdGet(device, port_hack, channel);
    if (ssi_id == (uint32_t) -1) return(-1);

    read_data = axi_reg_read((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR));
    *multiLane = (AXI_ADRV9001_SSI_MLANE1_SLANE0_GET(read_data) == 1) ? true : false;
    *sdrEnabled = (AXI_ADRV9001_SSI_SDR1_DDR0_GET(read_data) == 1) ? true : false;
    *format = fpga9001_NumOfBits2SsiFormat(device, AXI_ADRV9001_SSI_NUM_OF_BITS_GET(read_data));
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_ssi_BytesPerSample_GetValidate(adi_fpga9001_Device_t *device, uint8_t *bytesPerSample)
{
    ADI_NULL_PTR_RETURN(&device->common, bytesPerSample);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_ssi_BytesPerSample_Get(adi_fpga9001_Device_t *device,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint8_t *bytesPerSample)
{
    bool sdrEnabled = false;
    bool multiLane = false;
    adi_fpga9001_SsiFormat_e format = ADI_FPGA9001_SSI_FORMAT_2_BIT_SYMBOL_DATA;

    ADI_PERFORM_VALIDATION(adi_fpga9001_ssi_BytesPerSample_GetValidate, device, bytesPerSample);

    ADI_EXPECT(adi_fpga9001_ssi_Mode_Get, device, port, channel, &sdrEnabled, &multiLane, &format);
    
    switch (format)
    {
    case ADI_FPGA9001_SSI_FORMAT_2_BIT_SYMBOL_DATA:     /* Falls through */
    case ADI_FPGA9001_SSI_FORMAT_8_BIT_SYMBOL_DATA:
        *bytesPerSample = 1;
        break;
    case ADI_FPGA9001_SSI_FORMAT_16_BIT_SYMBOL_DATA:
        *bytesPerSample = 2;
        break;
    case ADI_FPGA9001_SSI_FORMAT_12_BIT_I_Q_DATA:
        *bytesPerSample = 3;
        break;
    case ADI_FPGA9001_SSI_FORMAT_16_BIT_I_Q_DATA:
        *bytesPerSample = 4;
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_ssi_Reset(adi_fpga9001_Device_t *device)
{
    axi_reg_write((void *)device, AXI_ADRV9001_ID, (AXI_ADRV9001_SSI_RX0_ID + AXI_ADRV9001_SSI_INIT_ADDR), 0x1);
    axi_reg_write((void *)device, AXI_ADRV9001_ID, (AXI_ADRV9001_SSI_RX1_ID + AXI_ADRV9001_SSI_INIT_ADDR), 0x1);
    axi_reg_write((void *)device, AXI_ADRV9001_ID, (AXI_ADRV9001_SSI_TX0_ID + AXI_ADRV9001_SSI_INIT_ADDR), 0x1);
    axi_reg_write((void *)device, AXI_ADRV9001_ID, (AXI_ADRV9001_SSI_TX1_ID + AXI_ADRV9001_SSI_INIT_ADDR), 0x1);
    ADI_API_RETURN(device);
}

static uint32_t fpga9001_SsiDataSelGet(adi_fpga9001_Device_t *device,
                                       adi_fpga9001_SsiTestModeData_e sel)
{
    if (sel == ADI_FPGA9001_SSI_TESTMODE_DATA_NORMAL) return(AXI_ADRV9001_SSI_DATA_SEL_DMA);
    if (sel == ADI_FPGA9001_SSI_TESTMODE_DATA_FIXED_PATTERN) return(AXI_ADRV9001_SSI_DATA_SEL_PATTERN);
    if (sel == ADI_FPGA9001_SSI_TESTMODE_DATA_RAMP_NIBBLE) return(AXI_ADRV9001_SSI_DATA_SEL_NIBBLE_RAMP);
    if (sel == ADI_FPGA9001_SSI_TESTMODE_DATA_RAMP_16_BIT) return(AXI_ADRV9001_SSI_DATA_SEL_RAMP);
    if (sel == ADI_FPGA9001_SSI_TESTMODE_DATA_PRBS15) return(AXI_ADRV9001_SSI_DATA_SEL_PRBS15);
    if (sel == ADI_FPGA9001_SSI_TESTMODE_DATA_PRBS7) return(AXI_ADRV9001_SSI_DATA_SEL_PRBS7);

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, 0, 0, sel, "Invalid SSI data select.");
    return((uint32_t) -1);
}

static int32_t adi_fpga9001_ssi_Rx_TestMode_Configure_Validate(adi_fpga9001_Device_t *device,
                                                               adi_common_ChannelNumber_e channel,
                                                               adi_fpga9001_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
    ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_FPGA9001_SSI_TESTMODE_DATA_NORMAL, ADI_FPGA9001_SSI_TESTMODE_DATA_PRBS7);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_ssi_Rx_TestMode_Configure(adi_fpga9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga9001_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
    uint32_t ssi_id;
    uint32_t pattern[2];

    ADI_PERFORM_VALIDATION(adi_fpga9001_ssi_Rx_TestMode_Configure_Validate, device, channel, ssiTestModeConfig);

    ssi_id = fpga9001_SsiIdGet(device, ADI_RX, channel);
    if (ssi_id == (uint32_t) -1) return(-1);

    pattern[0] = ssiTestModeConfig->fixedDataPatternToCheck;
    pattern[1] = 0;

    axi_adrv9001_ssi_data_pattern_set((void *)device, AXI_ADRV9001_ID, ssi_id, &pattern[0]);
    axi_adrv9001_ssi_data_sel_set((void *)device, AXI_ADRV9001_ID, ssi_id, fpga9001_SsiDataSelGet(device, ssiTestModeConfig->testData));
    axi_adrv9001_delay_us((void *)device, AXI_ADRV9001_ID, 1);
    axi_adrv9001_ssi_status_clear((void *)device, AXI_ADRV9001_ID, ssi_id);
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_ssi_Rx_TestMode_Inspect_Validate(adi_fpga9001_Device_t *device,
                                                             adi_common_ChannelNumber_e channel,
                                                             bool *dataError)
{
    ADI_NULL_PTR_RETURN(&device->common, dataError);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_ssi_Rx_TestMode_Inspect(adi_fpga9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             bool *dataError)
{
    uint32_t ssi_id;

    ADI_PERFORM_VALIDATION(adi_fpga9001_ssi_Rx_TestMode_Inspect_Validate, device, channel, dataError);

    ssi_id = fpga9001_SsiIdGet(device, ADI_RX, channel);
    if (ssi_id == (uint32_t) -1) return(-1);

    *dataError = (axi_adrv9001_ssi_status_get((void *)device, AXI_ADRV9001_ID, ssi_id) == 0) ? false : true;
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_ssi_Tx_TestMode_Configure_Validate(adi_fpga9001_Device_t *device,
                                                               adi_common_ChannelNumber_e channel,
                                                               adi_fpga9001_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
    ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_ssi_Tx_TestMode_Configure(adi_fpga9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga9001_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
    uint32_t ssi_id;
    uint32_t pattern[2];

    ADI_PERFORM_VALIDATION(adi_fpga9001_ssi_Tx_TestMode_Configure_Validate, device, channel, ssiTestModeConfig);

    ssi_id = fpga9001_SsiIdGet(device, ADI_RX, channel);
    if (ssi_id == (uint32_t) -1) return(-1);

    pattern[0] = ssiTestModeConfig->fixedDataPatternToTransmit;
    pattern[1] = 0;

    axi_adrv9001_ssi_data_pattern_set((void *)device, AXI_ADRV9001_ID, ssi_id, &pattern[0]);
    axi_adrv9001_ssi_data_sel_set((void *)device, AXI_ADRV9001_ID, ssi_id, fpga9001_SsiDataSelGet(device, ssiTestModeConfig->testData));
    axi_adrv9001_delay_us((void *)device, AXI_ADRV9001_ID, 1);
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_Ssi_Delay_Configure_Validate(adi_fpga9001_Device_t *device,
                                                         adi_fpga9001_SsiType_e ssiType,
                                                         adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{
    static const uint8_t MAX_DELAY = 31;

    ADI_RANGE_CHECK(device, ssiType, ADI_FPGA9001_SSI_TYPE_CMOS, ADI_FPGA9001_SSI_TYPE_LVDS);
    ADI_NULL_PTR_RETURN(&device->common, ssiCalibration);

    ADI_RANGE_CHECK(device, ssiCalibration->rxStrobeDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxStrobeDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxIDataDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxIDataDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxQDataDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxQDataDelay[1], 0, MAX_DELAY);
    
    ADI_RANGE_CHECK(device, ssiCalibration->txClkDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txClkDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txStrobeDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txStrobeDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txIDataDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txIDataDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txQDataDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txQDataDelay[1], 0, MAX_DELAY);

    ADI_API_RETURN(device);
}

/* we have a very badly thought out structure here, parameters must be grouped within interface */
/* if we grouped them by interface, all the things below could have been just a single function */

static void fpga9001_SsiDelaySet_1(adi_fpga9001_Device_t *device,
                                      adi_common_Port_e port,
                                      adi_common_ChannelNumber_e channel,
                                      uint32_t *data)
{
    int32_t i;

    for (i = 0; i < 6; i++) {
        axi_reg_write((void*)device, AXI_ADRV9001_ID, (fpga9001_SsiIdGet(device, port, channel) +
            AXI_ADRV9001_SSI_DELAY_CONTROL_ADDR + (i * 0x04)), *(data + i));
    }
}

static void fpga9001_SsiDelaySet(adi_fpga9001_Device_t *device,
                                 adi_fpga9001_SsiType_e ssiType,
                                 adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{
    uint32_t ssiDelay[6];

    ssiDelay[0] = ssiCalibration->rxIDataDelay[0];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->rxQDataDelay[0];
    else
        ssiDelay[1] = ssiCalibration->rxIDataDelay[0];
    ssiDelay[2] = ssiCalibration->rxQDataDelay[0];
    ssiDelay[3] = ssiCalibration->rxQDataDelay[0];
    ssiDelay[4] = ssiCalibration->rxStrobeDelay[0];
    ssiDelay[5] = 0;
    fpga9001_SsiDelaySet_1(device, ADI_RX, ADI_CHANNEL_1, &ssiDelay[0]);

    ssiDelay[0] = ssiCalibration->rxIDataDelay[1];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->rxQDataDelay[1];
    else
        ssiDelay[1] = ssiCalibration->rxIDataDelay[1];
    ssiDelay[2] = ssiCalibration->rxQDataDelay[1];
    ssiDelay[3] = ssiCalibration->rxQDataDelay[1];
    ssiDelay[4] = ssiCalibration->rxStrobeDelay[1];
    ssiDelay[5] = 0;
    fpga9001_SsiDelaySet_1(device, ADI_RX, ADI_CHANNEL_2, &ssiDelay[0]);

    ssiDelay[0] = ssiCalibration->txIDataDelay[0];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->txQDataDelay[0];
    else
        ssiDelay[1] = ssiCalibration->txIDataDelay[0];
    ssiDelay[2] = ssiCalibration->txQDataDelay[0];
    ssiDelay[3] = ssiCalibration->txQDataDelay[0];
    ssiDelay[4] = ssiCalibration->txStrobeDelay[0];
    ssiDelay[5] = ssiCalibration->txClkDelay[0];
    fpga9001_SsiDelaySet_1(device, ADI_TX, ADI_CHANNEL_1, &ssiDelay[0]);

    ssiDelay[0] = ssiCalibration->txIDataDelay[1];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->txQDataDelay[1];
    else
        ssiDelay[1] = ssiCalibration->txIDataDelay[1];
    ssiDelay[2] = ssiCalibration->txQDataDelay[1];
    ssiDelay[3] = ssiCalibration->txQDataDelay[1];
    ssiDelay[4] = ssiCalibration->txStrobeDelay[1];
    ssiDelay[5] = ssiCalibration->txClkDelay[1];
    fpga9001_SsiDelaySet_1(device, ADI_TX, ADI_CHANNEL_2, &ssiDelay[0]);
}

int32_t adi_fpga9001_Ssi_Delay_Configure(adi_fpga9001_Device_t *device,
                                         adi_fpga9001_SsiType_e ssiType,
                                         adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_PERFORM_VALIDATION(adi_fpga9001_Ssi_Delay_Configure_Validate, device, ssiType, ssiCalibration);
    fpga9001_SsiDelaySet(device, ssiType, ssiCalibration);
    ADI_API_RETURN(device);
}

static void fpga9001_SsiDelayGet_1(adi_fpga9001_Device_t *device,
                                      adi_common_Port_e port,
                                      adi_common_ChannelNumber_e channel,
                                      uint32_t *data)
{
    int32_t i;

    for (i = 0; i < 6; i++) {
        *(data + i) = axi_reg_read((void*)device, AXI_ADRV9001_ID, (fpga9001_SsiIdGet(device, port, channel) +
            AXI_ADRV9001_SSI_DELAY_CONTROL_ADDR + (i * 0x04))) & 0xff;
    }
}

static void fpga9001_SsiDelayGet(adi_fpga9001_Device_t *device,
                                 adi_fpga9001_SsiType_e ssiType,
                                 adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{
    uint32_t ssiDelay[6];

    fpga9001_SsiDelayGet_1(device, ADI_RX, ADI_CHANNEL_1, &ssiDelay[0]);
    ssiCalibration->rxIDataDelay[0] = ssiDelay[0];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiCalibration->rxQDataDelay[0] = ssiDelay[1];
    else
        ssiCalibration->rxQDataDelay[0] = ssiDelay[2];
    ssiCalibration->rxStrobeDelay[0] = ssiDelay[4];

    fpga9001_SsiDelayGet_1(device, ADI_RX, ADI_CHANNEL_2, &ssiDelay[0]);
    ssiCalibration->rxIDataDelay[1] = ssiDelay[0];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiCalibration->rxQDataDelay[1] = ssiDelay[1];
    else
        ssiCalibration->rxQDataDelay[1] = ssiDelay[2];
    ssiCalibration->rxStrobeDelay[1] = ssiDelay[4];

    fpga9001_SsiDelayGet_1(device, ADI_TX, ADI_CHANNEL_1, &ssiDelay[0]);
    ssiCalibration->txIDataDelay[0] = ssiDelay[0];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiCalibration->txQDataDelay[0] = ssiDelay[1];
    else
        ssiCalibration->txQDataDelay[0] = ssiDelay[2];
    ssiCalibration->txStrobeDelay[0] = ssiDelay[4];
    ssiCalibration->txClkDelay[0] = ssiDelay[5];

    fpga9001_SsiDelayGet_1(device, ADI_TX, ADI_CHANNEL_2, &ssiDelay[0]);
    ssiCalibration->txIDataDelay[1] = ssiDelay[0];
    if (ssiType == ADI_FPGA9001_SSI_TYPE_LVDS) 
        ssiCalibration->txQDataDelay[1] = ssiDelay[1];
    else
        ssiCalibration->txQDataDelay[1] = ssiDelay[2];
    ssiCalibration->txStrobeDelay[1] = ssiDelay[4];
    ssiCalibration->txClkDelay[1] = ssiDelay[5];
}

static int32_t adi_fpga9001_Ssi_Delay_Inspect_Validate(adi_fpga9001_Device_t *device,
                                                       adi_fpga9001_SsiType_e ssiType,
                                                       adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_RANGE_CHECK(device, ssiType, ADI_FPGA9001_SSI_TYPE_CMOS, ADI_FPGA9001_SSI_TYPE_LVDS);
    ADI_NULL_PTR_RETURN(&device->common, ssiCalibration);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Ssi_Delay_Inspect(adi_fpga9001_Device_t *device,
                                       adi_fpga9001_SsiType_e ssiType,
                                       adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_PERFORM_VALIDATION(adi_fpga9001_Ssi_Delay_Inspect_Validate, device, ssiType, ssiCalibration);
    fpga9001_SsiDelayGet(device, ssiType, ssiCalibration);
    ADI_API_RETURN(device);
}
