// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Author:            Rejeesh Kutty
// Description:       AXI DMA Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_DMA_H
#define AXI_DMA_H

#include "axi_common.h"

// defines

#define AXI_DMA_TRIGGER_DISABLE 0x000
#define AXI_DMA_TRIGGER_RISING_EDGE 0x001
#define AXI_DMA_TRIGGER_FALLING_EDGE 0x002
#define AXI_DMA_TRIGGER_HIGH 0x003
#define AXI_DMA_VERSION_ADDR 0x000
#define AXI_DMA_INSTANCE_ID_ADDR 0x004
#define AXI_DMA_SCRATCH_ADDR 0x008
#define AXI_DMA_TIMER_ADDR 0x00c
#define AXI_DMA_DATA_WIDTH_ADDR 0x010
#define AXI_DMA_SWRESET_ADDR 0x020
#define AXI_DMA_CONTROL_ADDR 0x024
#define AXI_DMA_START 0x001
#define AXI_DMA_STOP 0x002
#define AXI_DMA_CYCLIC(n) ((n & 0x1) << 2)
#define AXI_DMA_QTHRESHOLD(n) ((n & 0xf) << 4)
#define AXI_DMA_BUSY_ADDR 0x028
#define AXI_DMA_STATUS_ADDR 0x02c
#define AXI_DMA_STATUS(n) ((n >> 0) & 0x1)
#define AXI_DMA_OVERFLOW(n) ((n >> 1) & 0x1)
#define AXI_DMA_LENGTH_ERROR(n) ((n >> 2) & 0x1)
#define AXI_DMA_TLAST_ERROR(n) ((n >> 3) & 0x1)
#define AXI_DMA_ADDRDEC_ERROR(n) ((n >> 4) & 0x1)
#define AXI_DMA_SLAVE_ERROR(n) ((n >> 5) & 0x1)
#define AXI_DMA_START_ADDR_LOW_ADDR 0x030
#define AXI_DMA_START_ADDR_HIGH_ADDR 0x034
#define AXI_DMA_LENGTH_ADDR 0x040
#define AXI_DMA_TRIGGER_MODE_ADDR 0x050
#define AXI_DMA_TRIGGER_SET(n, m) ((m & 0x3) << (n * 4))
#define AXI_DMA_TRIGGER_GET(d, n) ((d >> (n * 4)) & 0x3)

// axi_dma parameters

struct axi_dma_params {
  uint32_t start_address;
  uint32_t length;
  uint32_t qthreshold;
  uint32_t cyclic;
};

// **********************************************************************************
// **********************************************************************************
// axi_dma functions

uint32_t axi_dma_instance_id_get(void *device, uint32_t peripheral_id);
void axi_dma_timer_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_dma_timer_get(void *device, uint32_t peripheral_id);
void axi_dma_delay_us(void *device, uint32_t peripheral_id, uint32_t delay);

void axi_dma_reset(void *device, uint32_t peripheral_id);
void axi_dma_start(void *device, uint32_t peripheral_id);
void axi_dma_stop(void *device, uint32_t peripheral_id);
uint32_t axi_dma_busy(void *device, uint32_t peripheral_id);
uint32_t axi_dma_status(void *device, uint32_t peripheral_id);
void axi_dma_status_clear(void *device, uint32_t peripheral_id);

void axi_dma_trigger_mode_set(void *device, uint32_t peripheral_id, uint32_t *mode);
void axi_dma_trigger_mode_get(void *device, uint32_t peripheral_id, uint32_t *mode);
void axi_dma_config(void *device, uint32_t peripheral_id, struct axi_dma_params *param);
int32_t axi_dma_run(void *device, uint32_t peripheral_id, struct axi_dma_params *param,
  uint32_t *mode, uint32_t timeout_us);

#endif

// **********************************************************************************
// **********************************************************************************
