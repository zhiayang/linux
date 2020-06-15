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
// Description:       AXI SPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_MSPI_H
#define AXI_MSPI_H

#include "axi_common.h"

#define AXI_MSPI_VERSION_ADDR 0x000
#define AXI_MSPI_INSTANCE_ID_ADDR 0x004
#define AXI_MSPI_SCRATCH_ADDR 0x008
#define AXI_MSPI_TIMER_ADDR 0x00c
#define AXI_MSPI_PERIOD_ADDR 0x010
#define AXI_MSPI_COUNT_ADDR 0x014
#define AXI_MSPI_SCLK_HIGH_COUNT_SET(n) ((n & 0xff) << 0)
#define AXI_MSPI_SCLK_LOW_COUNT_SET(n) ((n & 0xff) << 8)
#define AXI_MSPI_MOSI_COUNT_SET(n) ((n & 0xff) << 16)
#define AXI_MSPI_MISO_COUNT_SET(n) ((n & 0xff) << 24)
#define AXI_MSPI_SCLK_HIGH_COUNT_GET(n) ((n >> 0) & 0xff)
#define AXI_MSPI_SCLK_LOW_COUNT_GET(n) ((n >> 8) & 0xff)
#define AXI_MSPI_MOSI_COUNT_GET(n) ((n >> 16) & 0xff)
#define AXI_MSPI_MISO_COUNT_GET(n) ((n >> 24) & 0xff)
#define AXI_MSPI_CONTROL_ADDR 0x018
#define AXI_MSPI_SDIO_COUNT_SET(n) ((n & 0xff) << 0)
#define AXI_MSPI_SDIO_MODE_SET(n) ((n & 0x1) << 8)
#define AXI_MSPI_SDIO_COUNT_GET(n) ((n >> 0) & 0xff)
#define AXI_MSPI_SDIO_MODE_GET(n) ((n >> 8) & 0x1)
#define AXI_MSPI_MOSI_DATA_ADDR 0x020
#define AXI_MSPI_MOSI_STATUS_ADDR 0x024
#define AXI_MSPI_MOSI_FIFO_FULL_GET(n) ((n >> 0) & 0x1)
#define AXI_MSPI_MOSI_FIFO_EMPTY_GET(n) ((n >> 1) & 0x1)
#define AXI_MSPI_MISO_DATA_ADDR 0x030
#define AXI_MSPI_MISO_STATUS_ADDR 0x034
#define AXI_MSPI_MISO_FIFO_FULL_GET(n) ((n >> 0) & 0x1)
#define AXI_MSPI_MISO_FIFO_EMPTY_GET(n) ((n >> 1) & 0x1)
#define AXI_MSPI_CSN_ADDR 0x040
#define AXI_MSPI_CSN_SET(n, m) ((m & 0x1) << n)
#define AXI_MSPI_CSN_GET(d, n) ((d >> n) & 0x1)
#define AXI_MSPI_ENABLE_ADDR 0x050
#define AXI_MSPI_BUSY_ADDR 0x054

// axi_mspi parameters

struct axi_mspi_params {
  uint32_t clk_period_count;
  uint32_t clk_high_count;
  uint32_t clk_low_count;
  uint32_t mosi_count;
  uint32_t miso_count;
  uint32_t sdio_count;
  uint32_t sdio_mode;
};

// axi_mspi functions

uint32_t axi_mspi_instance_id_get(void *device, uint32_t peripheral_id);
void axi_mspi_timer_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_mspi_timer_get(void *device, uint32_t peripheral_id);
void axi_mspi_delay_us(void *device, uint32_t peripheral_id, uint32_t delay);

void axi_mspi_enable_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_mspi_busy(void *device, uint32_t peripheral_id);
int32_t axi_mspi_config(void *device, uint32_t peripheral_id,
  struct axi_mspi_params *param);

void axi_mspi_chip_select_set(void *device, uint32_t peripheral_id,
  uint32_t index, uint32_t data);
int32_t axi_mspi_xfer_all(void *device, uint32_t peripheral_id,
  uint8_t *mosi_data, uint8_t *miso_data, uint32_t count);
int32_t axi_mspi_xfer(void *device, uint32_t peripheral_id,
  uint32_t *mosi_data, uint32_t mosi_count,
  uint32_t *miso_data, uint32_t miso_count);
    
#endif

// **********************************************************************************
// **********************************************************************************
