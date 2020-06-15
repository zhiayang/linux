/**
 * \file
 *
 * \brief Functions to configure the FPGA9001 Data Chains
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include <stdlib.h>

#include "adi_adrv9001_user.h"
#include "adi_fpga9001_user.h"
#include "adi_fpga9001_datachain.h"
#include "adi_fpga9001_error.h"
#include "adi_fpga9001_hal.h"
#include "adi_fpga9001_ssi.h"
#include "fpga9001_utilities.h"

int32_t adi_fpga9001_DataChain_Tollgate_Configure(adi_fpga9001_Device_t *device,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  adi_fpga9001_TollgateCfg_t *tollGateCfg)
{
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_OK, ADI_COMMON_ACT_NO_ACTION, 0,
        "The function `adi_fpga9001_DataChain_Tollgate_Configure' is deprecated.");
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Tollgate_Inspect(adi_fpga9001_Device_t *device,
                                                adi_common_Port_e port,
                                                adi_common_ChannelNumber_e channel,
                                                adi_fpga9001_TollgateCfg_t *tollGateCfg)
{
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_OK, ADI_COMMON_ACT_NO_ACTION, 0,
        "The function `adi_fpga9001_DataChain_Tollgate_Inspect' is deprecated.");
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_TollgateReset_Set(adi_fpga9001_Device_t *device,
                                                 adi_common_Port_e port,
                                                 adi_common_ChannelNumber_e channel,
                                                 uint8_t resetVal)
{
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_OK, ADI_COMMON_ACT_NO_ACTION, 0,
        "The function `adi_fpga9001_DataChain_TollgateReset_Set' is deprecated.");
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_TollgateReset_Get(adi_fpga9001_Device_t *device,
                                                 adi_common_Port_e port,
                                                 adi_common_ChannelNumber_e channel,
                                                 uint16_t *resetVal)
{
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_OK, ADI_COMMON_ACT_NO_ACTION, 0,
        "The function `adi_fpga9001_DataChain_TollgateReset_Get' is deprecated.");
    ADI_API_RETURN(device);
}

/* trigger select here is re-interpreted as follows; this needs to be fixed at the header. */
/* hardware supports four trigger sources. */
/* 1. CARRIER.SMA.0 (In ZC706, this is J67). */
/* 2. CARRIER.SMA.1 (In ZC706, this is J68). */
/* 3. FMC.SMA (In ADRV9001, this is MCS (this can also be sourced by device)). */
/* 4. DEVICE.GPIO (In ADRV9001, this is any one of the digital GPIO.) */
/* Each source can be either disabled (if all are disabled, it is immediate) */
/* Individually rising edge OR falling edge (combination is allowed). */
/* ADI_FPGA9001_TRIGGER_IMMEDIATE  : all trigger sources are disabled. */
/* ADI_FPGA9001_TRIGGER_EXTERNAL   : CARRIER.SMA.0 is selected, rising edge. */
/* ADI_FPGA9001_TRIGGER_TDD_DP     : Ignored. */
/* ADI_FPGA9001_TRIGGER_TDD_GPIO   : DEVICE.GPIO is selected, rising edge. */
/* Also, setting a trigger source while DMA is in progress will result in UNKNOWN  behavior. */
/* It is certain that data integrity is lost, as data is disabled until trigger occurs. */

int32_t adi_fpga9001_DataChain_TollgateTrigger_Set(adi_fpga9001_Device_t *device,
                                                   adi_common_Port_e port,
                                                   adi_common_ChannelNumber_e channel,
                                                   uint32_t triggerSelect)
{
    uint32_t dma_id;
    uint32_t trigger_mode[4];

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if ((triggerSelect & ADI_FPGA9001_TRIGGER_TDD_DP) != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_OK, ADI_COMMON_ACT_NO_ACTION, 0,
            "Trigger mode `ADI_FPGA9001_TRIGGER_TDD_DP' is deprecated.");
    }
    
    trigger_mode[0] = (((triggerSelect & ADI_FPGA9001_TRIGGER_EXTERNAL) == 0) ||
        ((triggerSelect & ADI_FPGA9001_TRIGGER_IMMEDIATE) != 0)) ?
        AXI_DMA_TRIGGER_DISABLE : AXI_DMA_TRIGGER_RISING_EDGE;
    trigger_mode[1] = AXI_DMA_TRIGGER_DISABLE;
    trigger_mode[2] = AXI_DMA_TRIGGER_DISABLE;
    trigger_mode[3] = (((triggerSelect & ADI_FPGA9001_TRIGGER_TDD_GPIO) == 0) ||
        ((triggerSelect & ADI_FPGA9001_TRIGGER_IMMEDIATE) != 0)) ?
        AXI_DMA_TRIGGER_DISABLE : AXI_DMA_TRIGGER_RISING_EDGE;

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    axi_dma_trigger_mode_set((void *)device, dma_id, &trigger_mode[0]);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_TollgateTrigger_Get(adi_fpga9001_Device_t *device,
                                                   adi_common_Port_e port,
                                                   adi_common_ChannelNumber_e channel,
                                                   uint32_t *triggerSelect)
{
    uint32_t dma_id;
    uint32_t trigger_data;
    uint32_t trigger_mode[4];

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, triggerSelect);

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    axi_dma_trigger_mode_get((void *)device, dma_id, &trigger_mode[0]);

    trigger_data = ADI_FPGA9001_TRIGGER_IMMEDIATE;
    if (trigger_mode[0] == AXI_DMA_TRIGGER_RISING_EDGE)
        trigger_data = (trigger_data & ~ADI_FPGA9001_TRIGGER_IMMEDIATE) | ADI_FPGA9001_TRIGGER_EXTERNAL;
    if (trigger_mode[3] == AXI_DMA_TRIGGER_RISING_EDGE)
        trigger_data = (trigger_data & ~ADI_FPGA9001_TRIGGER_IMMEDIATE) | ADI_FPGA9001_TRIGGER_TDD_GPIO;

    *triggerSelect = trigger_data;
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Dma_Configure(adi_fpga9001_Device_t *device,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_fpga9001_DmaCfg_t *dmaCfg)
{
    uint32_t dma_id;
    uint32_t dma_max_size;
    struct axi_dma_params dma_params;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, dmaCfg);

    if (dmaCfg->simpleStartAddr % 64 != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, dmaCfg->simpleStartAddr,
            "Invalid parameter value, dmaCfg->simpleStartAddr must be a multiple of 64.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dmaCfg->length % 64 != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, dmaCfg->length,
            "Invalid parameter value, dmaCfg->length must be a multiple of 64.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dma_max_size = (port == ADI_TX) ? TX_DMA_SIZE : RX_DMA_SIZE;
    if (dmaCfg->length > dma_max_size)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, dmaCfg->length,
            "Invalid parameter value, dmaCfg->length can not exceed certain size.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    dma_params.start_address = dmaCfg->simpleStartAddr;
    dma_params.length = dmaCfg->length;
    dma_params.qthreshold = 0x2;
    dma_params.cyclic = dmaCfg->continuous;
    axi_dma_config((void *)device, dma_id, &dma_params);

    if (dmaCfg->runStop == 1)
        axi_dma_start((void *)device, dma_id);
    else
        axi_dma_stop((void *)device, dma_id);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Dma_Inspect(adi_fpga9001_Device_t *device,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_fpga9001_DmaCfg_t *dmaCfg)
{
    uint32_t dma_id;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, dmaCfg);

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    dmaCfg->simpleStartAddr = axi_reg_read((void *)device, dma_id, AXI_DMA_START_ADDR_LOW_ADDR);
    dmaCfg->length = axi_reg_read((void *)device, dma_id, AXI_DMA_LENGTH_ADDR);
    dmaCfg->continuous = AXI_DMA_CYCLIC(axi_reg_read((void *)device, dma_id, AXI_DMA_CONTROL_ADDR));
    dmaCfg->overUnderflow = AXI_DMA_OVERFLOW(axi_reg_read((void *)device, dma_id, AXI_DMA_STATUS_ADDR));
    dmaCfg->active = axi_dma_busy((void *)device, dma_id);
    dmaCfg->complete = ~dmaCfg->active;
    dmaCfg->haltComplete = ~dmaCfg->active;
    dmaCfg->runStop = dmaCfg->active;
    dmaCfg->enableEnhancedMode = 0;
    dmaCfg->enableSg = 0;
    dmaCfg->sgFirstDescriptor = 0;
    dmaCfg->sgLastDescriptor = 0;
    dmaCfg->validDataWidthStream = ADI_FPGA9001_STREAM_512_BITS;
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_DmaReset_Set(adi_fpga9001_Device_t *device,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint8_t reset)
{
    uint32_t dma_id;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    axi_dma_reset((void *)device, dma_id);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_DmaRun_Set(adi_fpga9001_Device_t *device,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              bool run)
{
    uint32_t dma_id;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    if (run)
        axi_dma_start((void *)device, dma_id);
    else
        axi_dma_stop((void *)device, dma_id);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_DmaLength_Set(adi_fpga9001_Device_t *device,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             uint32_t numBytes)
{
    uint32_t dma_id;
    uint32_t dma_max_size;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (numBytes % 64 != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, numBytes,
            "Invalid parameter value, numBytes must be a multiple of 64.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dma_max_size = (port == ADI_TX) ? TX_DMA_SIZE : RX_DMA_SIZE;
    if (numBytes > dma_max_size)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, numBytes,
            "Invalid parameter value, numBytes can not exceed certain size.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    axi_reg_write((void *)device, dma_id, AXI_DMA_LENGTH_ADDR, numBytes);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Configure(adi_fpga9001_Device_t *device,
                                         adi_common_Port_e port,
                                         adi_common_ChannelNumber_e channel,
                                         adi_fpga9001_TollgateCfg_t *tollgateCfg,
                                         adi_fpga9001_DmaCfg_t *dmaCfg)
{
    return(adi_fpga9001_DataChain_Dma_Configure(device, port, channel, dmaCfg));
}

int32_t adi_fpga9001_DataChain_Reset(adi_fpga9001_Device_t *device)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    adi_fpga9001_DataChain_DmaReset_Set(device, ADI_RX, ADI_CHANNEL_1, 1);
    adi_fpga9001_DataChain_DmaReset_Set(device, ADI_RX, ADI_CHANNEL_2, 1);
    adi_fpga9001_DataChain_DmaReset_Set(device, ADI_TX, ADI_CHANNEL_1, 1);
    adi_fpga9001_DataChain_DmaReset_Set(device, ADI_TX, ADI_CHANNEL_2, 1);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_TxData_Stop(adi_fpga9001_Device_t *device)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    axi_dma_stop((void *)device, AXI_DMA_TX0_ID);
    axi_dma_stop((void *)device, AXI_DMA_TX1_ID);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Capture_Wait(adi_fpga9001_Device_t *device,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint32_t timeout_ms)
{
    uint32_t dma_id;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    dma_id = fpga9001_DMAIdGet(device, port, channel);
    axi_dma_timer_set((void *)device, dma_id, (timeout_ms * 100000));
    while (axi_dma_timer_get((void *)device, dma_id) != 0) {
        if (axi_dma_busy((void *)device, dma_id) == 0)
            ADI_API_RETURN(device);
    }
    
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_FPGA9001_ERR_RX_DATA_MOVER_WAIT_TIMEOUT,
        ADI_COMMON_ACT_ERR_RESET_FEATURE, 0, "The function `adi_fpga9001_DataChain_Capture_Wait' has timed out!");
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_PerformCapture(adi_fpga9001_Device_t *device,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              uint32_t timeout_ms,
                                              uint32_t numBytes,
                                              adi_fpga9001_TollgateTriggerSources_e trigger)
{
    uint32_t trigger_select;
    adi_fpga9001_DmaCfg_t dmaCfg;

    ADI_API_ENTRY_EXPECT(device);
    dmaCfg.simpleStartAddr = ADI_FPGA9001_RAM_START_ADDR + fpga9001_BufBaseAddrGet(port, channel);
    dmaCfg.continuous = (port == ADI_TX) ? 1 : 0;
    dmaCfg.length = numBytes;
    dmaCfg.runStop = 1;
    trigger_select = (uint32_t) trigger;

    adi_fpga9001_DataChain_TollgateTrigger_Set(device, port, channel, trigger_select);
    adi_fpga9001_DataChain_Configure(device, port, channel, 0, &dmaCfg);
    adi_fpga9001_DataChain_Capture_Wait(device, port, channel, timeout_ms);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_PerformTx(adi_fpga9001_Device_t *device,
                                         adi_common_ChannelNumber_e channel,
                                         adi_fpga9001_TollgateTriggerSources_e trigger)
{
    uint32_t dma_id;
    uint32_t trigger_select;
    adi_fpga9001_DmaCfg_t dmaCfg;

    ADI_API_ENTRY_EXPECT(device);
    dma_id = fpga9001_DMAIdGet(device, ADI_TX, channel);
    dmaCfg.simpleStartAddr = ADI_FPGA9001_RAM_START_ADDR + fpga9001_BufBaseAddrGet(ADI_TX, channel);
    dmaCfg.length = axi_reg_read((void *)device, dma_id, AXI_DMA_LENGTH_ADDR);
    dmaCfg.continuous = 1;
    dmaCfg.runStop = 1;
    trigger_select = (uint32_t) trigger;

    adi_fpga9001_DataChain_TollgateTrigger_Set(device, ADI_TX, channel, trigger_select);
    adi_fpga9001_DataChain_Configure(device, ADI_TX, channel, 0, &dmaCfg);
    ADI_API_RETURN(device);
}

/** Deduplicate Rx data samples that occur when using LVDS on Navassa A0.
*
*   \param[in,out]  rdBuff          Input raw captured data, length * deduplicateBy samples, each sample is
*                                   16I16Q packed into a 32-bit word. Output deduplicated data, length samples.
*   \param[in]      length          Number of samples after deduplication.
*   \param[in]      deduplicateBy   Decimation factor.
*/
static int32_t adi_fpga9001_Deduplicate(adi_fpga9001_Device_t *device,
                                        uint32_t *rdBuff,
                                        uint32_t length,
                                        uint32_t deduplicateBy)
{
    /* LVDS on A0 repeats blocks of 16 samples, so this function only makes sense when length is a multiple of 16. */
    if (length % 16 != 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         length,
                         "Invalid parameter value. length must be a multiple of 16.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Duplicated samples follow a complex pattern that recurs every 'blockSize' */
    uint32_t blockSize = deduplicateBy * 16;
    uint32_t rawLength = deduplicateBy * length;

    /* Search for n where sample n != sample n+16.  Then samples n..n+15 are good. */
    uint32_t n = 0;
    while ((n < rawLength - 16) && (rdBuff[n] == rdBuff[n + 16]))
    {
        n += 1;
    }

    /* And so are the samples 'blockSize' after AND BEFORE that.
       If necessary, wind n back to somewhere in the first 'blockSize'. */
    n = n % blockSize;

    /* Note that if there's no match, then every start point is valid. */

    /* Copy the required data to the beginning of rdBuff.  Conceptually, what this does is,
       for each i, if (i is in the range n..n+15, modulo blockSize) then keep sample i, else discard it. */
    uint32_t dest = 0u;

    /* Data comes in groups of 16 samples.  If the input data has only part of the
       first group, then those samples are already in the right memory locations, but
       there are less than 16 of them. */
    if (n + 16u > blockSize)
    {
        dest = (n + 16u) - blockSize;
    }

    /* Copy 16 samples from each remaining block. */
    while (n < rawLength - 16u)
    {
        uint32_t i;
        for (i = 0u; i < 16u; i++)
        {
            rdBuff[dest++] = rdBuff[n + i];
        }
        n += blockSize;
    }

    /* If we have a partial block at the end, copy it. */
    while (n < rawLength)
    {
        rdBuff[dest++] = rdBuff[n++];
    }

    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_DataChain_Data_Get_16I16QValidate(adi_fpga9001_Device_t *device,
                                                              adi_common_Port_e port,
                                                              adi_common_ChannelNumber_e channel,
                                                              uint16_t *iData,
                                                              uint16_t *qData,
                                                              uint32_t length,
                                                              uint32_t deduplicateBy)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, deduplicateBy, 1u, 32u);

    ADI_NULL_PTR_RETURN(&device->common, iData);
    ADI_NULL_PTR_RETURN(&device->common, qData);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_16I16Q_Deduplicate(adi_fpga9001_Device_t *device,
                                                           adi_common_Port_e port,
                                                           adi_common_ChannelNumber_e channel,
                                                           uint16_t *iData,
                                                           uint16_t *qData,
                                                           uint32_t length,
                                                           uint32_t deduplicateBy)
{
    uint32_t i = 0;
    uint32_t *rdBuff = NULL;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_Data_Get_16I16QValidate, device, port, channel, iData, qData, length, deduplicateBy);

    rdBuff = calloc(length * deduplicateBy, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    recoveryAction = adi_fpga9001_hal_Ram_Read(device, ramAddress, rdBuff, length * deduplicateBy);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    if (deduplicateBy > 1u)
    {
        ADI_EXPECT(adi_fpga9001_Deduplicate, device, rdBuff, length, deduplicateBy);
    }

    for (i = 0; i < length; i++)
    {
        iData[i] = (uint16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        qData[i] = (uint16_t)(rdBuff[i]  & 0x0000FFFF);
    }

    free(rdBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_16I16Q(adi_fpga9001_Device_t *device,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               uint16_t *iData,
                                               uint16_t *qData,
                                               uint32_t length)
{
    return adi_fpga9001_DataChain_Data_Get_16I16Q_Deduplicate(device, port, channel, iData, qData, length, 1u);
}

static int32_t adi_fpga9001_DataChain_Data_Get_16IQInterleavedValidate(adi_fpga9001_Device_t *device,
                                                                       adi_common_Port_e port,
                                                                       adi_common_ChannelNumber_e channel,
                                                                       uint16_t *iqData,
                                                                       uint32_t length,
                                                                       uint32_t deduplicateBy)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, deduplicateBy, 1u, 32u);

    ADI_NULL_PTR_RETURN(&device->common, iqData);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_16IQInterleaved_Deduplicate(adi_fpga9001_Device_t *device,
                                                                    adi_common_Port_e port,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    uint16_t *iqData,
                                                                    uint32_t length,
                                                                    uint32_t deduplicateBy)
{
    uint32_t i = 0;
    uint32_t *rdBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_Data_Get_16IQInterleavedValidate, device, port, channel, iqData, length, deduplicateBy);

    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    if (adi_fpga9001_hal_Ram_Read(device, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    if (deduplicateBy > 1u)
    {
        ADI_EXPECT(adi_fpga9001_Deduplicate, device, rdBuff, length, deduplicateBy);
    }

    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i]     = (uint16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        iqData[2 * i + 1] = (uint16_t)(rdBuff[i]  & 0x0000FFFF);
    }

    free(rdBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_16IQInterleaved(adi_fpga9001_Device_t *device,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint16_t *iqData,
                                                        uint32_t length)
{
    return adi_fpga9001_DataChain_Data_Get_16IQInterleaved_Deduplicate(device, port, channel, iqData, length, 1u);
}

static int32_t adi_fpga9001_DataChain_Data_Get_16IValidate(adi_fpga9001_Device_t *device,
                                                           adi_common_Port_e port,
                                                           adi_common_ChannelNumber_e channel,
                                                           uint16_t *iData,
                                                           uint32_t length)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, iData);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_16I(adi_fpga9001_Device_t *device,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint16_t *iData,
                                            uint32_t length)
{
    uint32_t i = 0;
    uint32_t *rdBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_Data_Get_16IValidate, device, port, channel, iData, length);

    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    if (adi_fpga9001_hal_Ram_Read(device, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 2; i++)
    {
        iData[2 * i]     = (uint16_t)(rdBuff[i]  & 0x0000FFFF);
        iData[2 * i + 1] = (uint16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
    }

    free(rdBuff);

    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_DataGet_SValidate(adi_fpga9001_Device_t *device,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              uint8_t *symbols,
                                              uint32_t length)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, symbols);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_8S(adi_fpga9001_Device_t *device,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t *rdBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataGet_SValidate, device, port, channel, symbols, length);

    rdBuff = calloc(length / 4, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    if (adi_fpga9001_hal_Ram_Read(device, ramAddress, rdBuff, length / 4) != ADI_COMMON_ACT_NO_ACTION)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 4; i++)
    {
        symbols[4 * i]     = (uint8_t)(rdBuff[i]  & 0x000000FF);
        symbols[4 * i + 1] = (uint8_t)((rdBuff[i] & 0x0000FF00) >> 8);
        symbols[4 * i + 2] = (uint8_t)((rdBuff[i] & 0x00FF0000) >> 16);
        symbols[4 * i + 3] = (uint8_t)((rdBuff[i] & 0xFF000000) >> 24);
    }

    free(rdBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Get_2S(adi_fpga9001_Device_t *device,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t *rdBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataGet_SValidate, device, port, channel, symbols, length);

    rdBuff = calloc(length / 4, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    if (adi_fpga9001_hal_Ram_Read(device, ramAddress, rdBuff, length / 4) != ADI_COMMON_ACT_NO_ACTION)
    {
        free(rdBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 4; i++)
    {
        symbols[4 * i]     = (uint8_t)(rdBuff[i]  & 0x00000003);
        symbols[4 * i + 1] = (uint8_t)((rdBuff[i] & 0x00000300) >> 8);
        symbols[4 * i + 2] = (uint8_t)((rdBuff[i] & 0x00030000) >> 16);
        symbols[4 * i + 3] = (uint8_t)((rdBuff[i] & 0x03000000) >> 24);
    }

    free(rdBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Set_16I16Q(adi_fpga9001_Device_t *device,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               uint16_t *iData,
                                               uint16_t *qData,
                                               uint32_t length)
{
    uint32_t i = 0;
    uint32_t *wrBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_Data_Get_16I16QValidate, device, port, channel, iData, qData, length, 1u);

    ADI_EXPECT(adi_fpga9001_DataChain_DmaLength_Set, device, port, channel, length * 4);

    wrBuff = calloc(length, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(wrBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length; i++)
    {
        wrBuff[i] = (uint32_t)(iData[i] << 16) | (uint32_t)qData[i];
    }

    adi_fpga9001_hal_Ram_Write(device, ramAddress, wrBuff, length);

    free(wrBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Set_16IQInterleaved(adi_fpga9001_Device_t *device,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint16_t *iqData,
                                                        uint32_t length)
{
    uint32_t i = 0;
    uint32_t *wrBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_Data_Get_16IQInterleavedValidate, device, port, channel, iqData, length, 1u);

    ADI_EXPECT(adi_fpga9001_DataChain_DmaLength_Set, device, port, channel, length * 2);

    wrBuff = calloc(length / 2, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(wrBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 2; i++)
    {
        wrBuff[i] = (uint32_t)(iqData[2 * i] << 16) | (uint32_t)iqData[2 * i + 1];
        iqData[2 * i]     = (uint16_t)((wrBuff[i] & 0xFFFF0000) >> 16);
        iqData[2 * i + 1] = (uint16_t)(wrBuff[i]  & 0x0000FFFF);
    }

    adi_fpga9001_hal_Ram_Write(device, ramAddress, wrBuff, length / 2);

    free(wrBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Set_16I(adi_fpga9001_Device_t *device,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint16_t *iData,
                                            uint32_t length)
{
    uint32_t i = 0;
    uint32_t *wrBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_Data_Get_16IValidate, device, port, channel, iData, length);

    ADI_EXPECT(adi_fpga9001_DataChain_DmaLength_Set, device, port, channel, length * 2);

    wrBuff = calloc(length / 2, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(wrBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 2; i++)
    {
        wrBuff[i] = (uint32_t)(iData[2 * i + 1] << 16) | (uint32_t)iData[2 * i];
    }

    adi_fpga9001_hal_Ram_Write(device, ramAddress, wrBuff, length / 2);

    free(wrBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Set_8S(adi_fpga9001_Device_t *device,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t *wrBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataGet_SValidate, device, port, channel, symbols, length);

    ADI_EXPECT(adi_fpga9001_DataChain_DmaLength_Set, device, port, channel, length);

    wrBuff = calloc(length / 4, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(wrBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 4; i++)
    {
        wrBuff[i] = (uint32_t)(symbols[4 * i + 3] << 24) |
                    (uint32_t)(symbols[4 * i + 2] << 16) |
                    (uint32_t)(symbols[4 * i + 1] << 8)  |
                    (uint32_t)symbols[4 * i];
    }

    adi_fpga9001_hal_Ram_Write(device, ramAddress, wrBuff, length / 4);

    free(wrBuff);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_Data_Set_2S(adi_fpga9001_Device_t *device,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t *wrBuff = NULL;
    uint32_t ramAddress = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataGet_SValidate, device, port, channel, symbols, length);

    ADI_EXPECT(adi_fpga9001_DataChain_DmaLength_Set, device, port, channel, length);

    wrBuff = calloc(length / 4, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(device);
    }

    ramAddress = fpga9001_BufBaseAddrGet(port, channel);
    if (ramAddress == (uint32_t) -1)
    {
        free(wrBuff);
        ADI_API_RETURN(device);
    }

    for (i = 0; i < length / 4; i++)
    {
        wrBuff[i] = (uint32_t)((symbols[4 * i + 3] & 0x03) << 24) |
                    (uint32_t)((symbols[4 * i + 2] & 0x03) << 16) |
                    (uint32_t)((symbols[4 * i + 1] & 0x03) << 8) |
                    (uint32_t)(symbols[4 * i] & 0x03);
    }

    adi_fpga9001_hal_Ram_Write(device, ramAddress, wrBuff, length / 4);

    free(wrBuff);

    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_DataChain_RxClkRate_Set_Validate(adi_fpga9001_Device_t *device, 
                                                             adi_common_ChannelNumber_e channel)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_RxClkRate_Set(adi_fpga9001_Device_t *device, 
                                             adi_common_ChannelNumber_e channel, 
                                             uint8_t rxClkRate_Hz)
{
    uint32_t ssi_id;
    uint32_t data;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_RxClkRate_Set_Validate, device, channel);
    
    ssi_id = fpga9001_SsiIdGet(device, ADI_RX, channel);
    data = axi_reg_read((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR));
    data = data & ~AXI_ADRV9001_SSI_CLK_RATE_SET((uint32_t) -1);
    data = data | AXI_ADRV9001_SSI_CLK_RATE_SET(rxClkRate_Hz);
    axi_reg_write((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR), data);
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_DataChain_RxClkRate_Get_Validate(adi_fpga9001_Device_t *device, 
                                                             adi_common_ChannelNumber_e channel, 
                                                             uint8_t *rxClkRate_Hz)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, rxClkRate_Hz);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_DataChain_RxClkRate_Get(adi_fpga9001_Device_t *device, 
                                             adi_common_ChannelNumber_e channel, 
                                             uint8_t *rxClkRate_Hz)
{
    uint32_t ssi_id;
    uint32_t data;

    ADI_PERFORM_VALIDATION(adi_fpga9001_DataChain_RxClkRate_Get_Validate, device, channel, rxClkRate_Hz);
    
    ssi_id = fpga9001_SsiIdGet(device, ADI_RX, channel);
    data = axi_reg_read((void *)device, AXI_ADRV9001_ID, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR));
    *rxClkRate_Hz = AXI_ADRV9001_SSI_CLK_RATE_GET(data);
    ADI_API_RETURN(device);
}
