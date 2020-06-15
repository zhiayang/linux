/**
* \file
* \brief Private core board functions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001ee01.h"

#include "adrv9001ee01.h"

#include "adi_adrv9001_rx.h"

#include "adi_fpga9001.h"
#include "adi_fpga9001_hal.h"
#include "adi_fpga9001_datachain.h"

#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"

#include "adi_fpga9001_ssi_types.h"
#include "fpga9001_utilities.h"

int32_t adi_adrv9001Ee01_SelectFpgaBin(adi_adrv9001Ee01_Board_t *adrv9001Ee01, adi_adrv9001_Init_t *adrv9001Init)
{
    adi_adrv9001_SsiType_e ssiType = adrv9001Init->rx.rxChannelCfg[0].profile.rxSsiConfig.ssiType |
        adrv9001Init->rx.rxChannelCfg[1].profile.rxSsiConfig.ssiType | 
        adrv9001Init->tx.txProfile[0].txSsiConfig.ssiType |
        adrv9001Init->tx.txProfile[1].txSsiConfig.ssiType;
    
    if (ssiType == ADI_ADRV9001_SSI_TYPE_CMOS)
    {
        ADI_EXPECT(adi_fpga9001_SwitchBin, adrv9001Ee01->fpga9001Device, ADI_FPGA9001_CMOS);
    }
    else if (ssiType == ADI_ADRV9001_SSI_TYPE_LVDS)
    {
        ADI_EXPECT(adi_fpga9001_SwitchBin, adrv9001Ee01->fpga9001Device, ADI_FPGA9001_LVDS);
    }
    else
    {
        ADI_ERROR_REPORT(&adrv9001Ee01->fpga9001Device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         adrv9001Init,
                         "Invalid SSI type. Must be LVDS or CMOS. All initialized channels must use the same SSI type");
    }

    ADI_EXPECT(adi_adrv9001_HwClose, adrv9001Ee01->adrv9001Device);

    adi_adrv9001_SpiSettings_t spiSettings = {
        .msbFirst = 1,
        .enSpiStreaming = 0,
        .autoIncAddrUp = 1,
        .fourWireMode = 1,
        .cmosPadDrvStrength = ADI_ADRV9001_CMOSPAD_DRV_STRONG,
    };

    ADI_EXPECT(adi_adrv9001_HwOpen, adrv9001Ee01->adrv9001Device, &spiSettings);

    ADI_API_RETURN(adrv9001Ee01->fpga9001Device);
}

/* The hard-coded values are specific to the FPGA9001/Zc706 platform. */
static void ssiDelayConfigInitDefault(adi_fpga9001_SsiCalibrationCfg_t *ssiCalibration)
{

    ssiCalibration->rxIDataDelay[0] = 0x13;
    ssiCalibration->rxIDataDelay[1] = 0x13;
    ssiCalibration->rxQDataDelay[0] = 0x13;
    ssiCalibration->rxQDataDelay[1] = 0x13;
    ssiCalibration->rxStrobeDelay[0] = 0x13;
    ssiCalibration->rxStrobeDelay[1] = 0x13;

    ssiCalibration->txIDataDelay[0] = 0;
    ssiCalibration->txIDataDelay[1] = 0;
    ssiCalibration->txQDataDelay[0] = 0;
    ssiCalibration->txQDataDelay[1] = 0;
    ssiCalibration->txStrobeDelay[0] = 0;
    ssiCalibration->txStrobeDelay[1] = 0;
    ssiCalibration->txClkDelay[0] = 0xF;
    ssiCalibration->txClkDelay[1] = 0xF;
}

int32_t adi_fpga9001_Initialize(adi_fpga9001_Device_t *fpga9001Device,
                                adi_adrv9001_Init_t *init,
                                adi_fpga9001_Mmcm_ClockOutput_Divisor_e adrv9001DeviceClockDivisor)
{
    uint8_t i, k;
    bool ssi_ddr[ADI_ADRV9001_NUM_TXRX_CHANNELS] = { 0 };
    uint32_t version;
    uint32_t locked;
    uint32_t ssi_id[ADI_ADRV9001_NUM_TXRX_CHANNELS] = { 0 };
    uint32_t ssi_clk_freq[ADI_ADRV9001_NUM_TXRX_CHANNELS] = { 0 };
    uint32_t ssi_clk_ratio[ADI_ADRV9001_NUM_TXRX_CHANNELS] = { 0 };
    uint32_t ssi_sample_freq[ADI_ADRV9001_NUM_TXRX_CHANNELS] = { 0 } ;
    uint32_t axi_adrv9001_ssi_mode[ADI_ADRV9001_NUM_TXRX_CHANNELS] = { SSI_MODE_UNDEFINED, SSI_MODE_UNDEFINED, SSI_MODE_UNDEFINED, SSI_MODE_UNDEFINED };
    struct axi_adrv9001_clk_params clk_params = { 0 };
    struct axi_adrv9001_ssi_params ssi_params = { 0 };
    adi_adrv9001_SsiType_e ssi_type[ADI_ADRV9001_NUM_TXRX_CHANNELS];
    adi_adrv9001_SsiDataFormat_e ssi_num_of_bits[ADI_ADRV9001_NUM_TXRX_CHANNELS];
    adi_adrv9001_SsiNumLane_e ssi_num_of_lanes[ADI_ADRV9001_NUM_TXRX_CHANNELS];
    adi_adrv9001_SsiStrobeType_e ssi_strobe_type[ADI_ADRV9001_NUM_TXRX_CHANNELS];

    uint32_t RX_CHANNELS[] = {
        ADI_ADRV9001_RX1,
        ADI_ADRV9001_RX2,
        ADI_ADRV9001_ORX1,
        ADI_ADRV9001_ORX2,
        ADI_ADRV9001_ILB1,
        ADI_ADRV9001_ILB2,
        ADI_ADRV9001_ELB1,
        ADI_ADRV9001_ELB2
    };

    bool ssiDelayConfigured = false;

    /* Check device info is valid */
    ADI_API_ENTRY_EXPECT(fpga9001Device);

    adi_fpga9001_SsiCalibrationCfg_t ssiCalibration = { { 0 } };
    ssiDelayConfigInitDefault(&ssiCalibration);

    for (i = 0; ((i < ADI_ADRV9001_MAX_RXCHANNELS) && (ssiDelayConfigured == false)); i++)
    {
        if (ADRV9001_BF_EQUAL(init->rx.rxInitChannelMask, RX_CHANNELS[i]))
        {
            ADI_EXPECT(adi_fpga9001_Ssi_Delay_Configure,
                fpga9001Device,
                (adi_fpga9001_SsiType_e)(init->rx.rxChannelCfg[i].profile.rxSsiConfig.ssiType),
                &ssiCalibration);

            ssiDelayConfigured = true;
        }
    }

    /* Set FPGA Tx delay to 0x1A for SSI DDR */
    if (true == init->tx.txProfile[0].txSsiConfig.cmosDdrEn)
    {
        adi_hal_BbicRegisterWrite(fpga9001Device->common.devHalInfo, 0x430160A4, 0x1A);
    }

    /* ideally you want application to report versions, this is meant to be a */
    /* developer friendly api, and users are free to use any version at their own risk */
    /* do not block, simply inform -- better to change it to a NOTE. */

    axi_sysid_sys_version_get((void *)fpga9001Device, AXI_SYSID_ID, &version);
    ADI_ERROR_REPORT(&fpga9001Device->common, ADI_COMMON_ERRSRC_API,
        0, 0, version, "INFO: Platform FPGA version.");
    
    /* deassert reset to the device */

    axi_adrv9001_reset_set((void *)fpga9001Device, AXI_ADRV9001_ID, 0x1);

    /* get the ssi based info for Rx channels from the Rx profiles */
    for (i = 0; i < ADI_ADRV9001_NUM_RX_CHANNELS; i++) 
    {
        if (i == 0) 
            ssi_id[i] = AXI_ADRV9001_SSI_RX0_ID;
        else 
            ssi_id[i] = AXI_ADRV9001_SSI_RX1_ID;
        ssi_sample_freq[i] = init->rx.rxChannelCfg[i].profile.rxInterfaceSampleRate_Hz;
        ssi_type[i] = init->rx.rxChannelCfg[i].profile.rxSsiConfig.ssiType;
        ssi_num_of_bits[i] = init->rx.rxChannelCfg[i].profile.rxSsiConfig.ssiDataFormatSel;
        ssi_num_of_lanes[i] = init->rx.rxChannelCfg[i].profile.rxSsiConfig.numLaneSel;
        ssi_strobe_type[i] = init->rx.rxChannelCfg[i].profile.rxSsiConfig.strobeType;
        ssi_ddr[i] = (init->rx.rxChannelCfg[i].profile.rxSsiConfig.ssiType == ADI_ADRV9001_SSI_TYPE_LVDS) ? true :
            init->rx.rxChannelCfg[i].profile.rxSsiConfig.cmosDdrEn;
        if (0 != init->rx.rxChannelCfg[i].profile.rxOutputRate_Hz &&
            0 != init->rx.rxChannelCfg[i].profile.rxInterfaceSampleRate_Hz)
        {
            ssi_clk_ratio[i] = init->rx.rxChannelCfg[i].profile.rxOutputRate_Hz /
                init->rx.rxChannelCfg[i].profile.rxInterfaceSampleRate_Hz;
        }
        else
        {
            ssi_clk_ratio[i] = 1;
        }
    }

    /* get the ssi based info for Tx channels from the Tx profiles */
    for (i = 0; i < ADI_ADRV9001_NUM_TX_CHANNELS; i++) {
        k = i + ADI_ADRV9001_NUM_RX_CHANNELS;
        if (i == 0) 
            ssi_id[k] = AXI_ADRV9001_SSI_TX0_ID;
        else
            ssi_id[k] = AXI_ADRV9001_SSI_TX1_ID;
        ssi_sample_freq[k] = init->tx.txProfile[i].txInterfaceSampleRate_Hz;
        ssi_type[k] = init->tx.txProfile[i].txSsiConfig.ssiType;
        ssi_num_of_bits[k] = init->tx.txProfile[i].txSsiConfig.ssiDataFormatSel;
        ssi_num_of_lanes[k] = init->tx.txProfile[i].txSsiConfig.numLaneSel;
        ssi_strobe_type[k] = init->tx.txProfile[i].txSsiConfig.strobeType;
        ssi_ddr[k] = (init->tx.txProfile[i].txSsiConfig.ssiType == ADI_ADRV9001_SSI_TYPE_LVDS) ? true :
            init->tx.txProfile[i].txSsiConfig.cmosDdrEn;
        ssi_clk_ratio[k] = 1;
    }
    
    /* get the ssi mode for each of the channels */  
    for (i = 0; i < ADI_ADRV9001_NUM_TXRX_CHANNELS; i++) 
    {
        if (ADI_ADRV9001_SSI_TYPE_DISABLE == ssi_type[i])
        {
            continue;
        }
        
        axi_adrv9001_ssi_mode[i] = SSI_MODE_UNDEFINED;

        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_LONG_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_LS_32X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*1)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_16X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*16*1)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_8_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_08X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*8*1)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_02X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*2*1)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_32X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*1)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_LONG_STROBE) &&
            (ssi_clk_ratio[i] == 2)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_LS_32X2;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*2)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_LONG_STROBE) &&
            (ssi_clk_ratio[i] == 4)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_LS_32X4;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*4)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_LONG_STROBE) &&
            (ssi_clk_ratio[i] == 8)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_LS_32X8;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*8)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 2)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_16X2;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*16*2)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 4)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_16X4;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*16*4)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 8)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_16X8;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*16*8)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_8_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 2)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_08X2;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*8*2)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_8_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 4)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_08X4;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*8*4)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_8_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 8)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_08X8;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*8*8)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 2)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_02X2;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*2*2)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 4)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_02X4;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*2*4)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 8)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_02X8;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*2*8)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 2)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_32X2;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*2)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 4)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_32X4;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*4)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_1_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 8)) {
            axi_adrv9001_ssi_mode[i] = CMOS_1L_PS_32X8;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*8)/(1*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_CMOS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_4_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = CMOS_4L_PS_32X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*1)/(4*1*4);
        }
        if ((ssi_type[i] == ADI_ADRV9001_SSI_TYPE_LVDS) &&
            (ssi_num_of_bits[i] == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
            (ssi_num_of_lanes[i] == ADI_ADRV9001_SSI_2_LANE) &&
            (ssi_strobe_type[i] == ADI_ADRV9001_SSI_SHORT_STROBE) &&
            (ssi_clk_ratio[i] == 1)) {
            axi_adrv9001_ssi_mode[i] = LVDS_2L_PS_32X1;
            ssi_clk_freq[i] = (ssi_sample_freq[i]*32*1)/(2*1*4);
        }

        if (axi_adrv9001_ssi_mode[i] == SSI_MODE_UNDEFINED) {
            ADI_ERROR_REPORT(&fpga9001Device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
                0,
                "Unsupported interface mode");
            ADI_API_RETURN(fpga9001Device);
        }

        axi_adrv9001_ssi_init_param((void *)fpga9001Device, AXI_ADRV9001_ID, &ssi_params, axi_adrv9001_ssi_mode[i]);
        ssi_params.sdr1_ddr0 = (ssi_ddr[i] == true) ? 0 : 1;
        if (ssi_ddr[i] == true) ssi_clk_freq[i] = ssi_clk_freq[i]/2;
        if (axi_adrv9001_ssi_config((void *)fpga9001Device, AXI_ADRV9001_ID, ssi_id[i], &ssi_params) != 0) {			
            ADI_ERROR_REPORT(&fpga9001Device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
                0,
                "function axi_adrv9001_ssi_config has failed");
            ADI_API_RETURN(fpga9001Device);
        }
    }

    clk_params.ref_clk_freq_hz = (1000*init->clocks.deviceClock_kHz) >> adrv9001DeviceClockDivisor;
    clk_params.dev_clk_freq_hz = (1000*init->clocks.deviceClock_kHz);
    clk_params.ssi_rx0_clk_freq_hz = ssi_clk_freq[0];
    clk_params.ssi_rx1_clk_freq_hz = ssi_clk_freq[1];
    clk_params.ssi_tx0_clk_freq_hz = ssi_clk_freq[2];
    clk_params.ssi_tx1_clk_freq_hz = ssi_clk_freq[3];

    if (axi_adrv9001_clk_config((void *)fpga9001Device, AXI_ADRV9001_ID, &clk_params, &locked) != 0) {
        ADI_ERROR_REPORT(&fpga9001Device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
            0,
            "function axi_adrv9001_clk_config has failed");
        ADI_API_RETURN(fpga9001Device);
    }

    if (locked != 1) {
        ADI_ERROR_REPORT(&fpga9001Device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
            0,
            "platform VCO clock NOT locked");
        ADI_API_RETURN(fpga9001Device);
    }
    
    axi_adrv9001_trig_out_enable_set((void *)fpga9001Device, AXI_ADRV9001_ID, 0x3);

    ADI_API_RETURN(fpga9001Device);
}

