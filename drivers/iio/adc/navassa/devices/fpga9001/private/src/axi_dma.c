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

#include "axi_dma.h"

// **********************************************************************************
// **********************************************************************************

uint32_t axi_dma_instance_id_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_DMA_INSTANCE_ID_ADDR));
}

void axi_dma_timer_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_DMA_TIMER_ADDR, data);
}

uint32_t axi_dma_timer_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_DMA_TIMER_ADDR));
}

void axi_dma_delay_us(void *device, uint32_t peripheral_id, uint32_t delay) {

  axi_dma_timer_set(device, peripheral_id, (delay * 100));
  while (axi_dma_timer_get(device, peripheral_id) != 0);
}

// **********************************************************************************
// **********************************************************************************

void axi_dma_reset(void *device, uint32_t peripheral_id) {

  axi_reg_write(device, peripheral_id, AXI_DMA_SWRESET_ADDR, 0x1);
  while (axi_reg_read(device, peripheral_id, AXI_DMA_SWRESET_ADDR) == 1);
}

void axi_dma_start(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_DMA_CONTROL_ADDR);
  axi_reg_write(device, peripheral_id, AXI_DMA_CONTROL_ADDR, (data | AXI_DMA_START));
}

void axi_dma_stop(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_DMA_CONTROL_ADDR);
  if ((data & AXI_DMA_START) == AXI_DMA_START) {
    axi_reg_write(device, peripheral_id, AXI_DMA_CONTROL_ADDR, (data | AXI_DMA_STOP));
  }
}

uint32_t axi_dma_busy(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_DMA_BUSY_ADDR));
}

uint32_t axi_dma_status(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_DMA_STATUS_ADDR) ^ 0x1);
}

void axi_dma_status_clear(void *device, uint32_t peripheral_id) {

  axi_reg_write(device, peripheral_id, AXI_DMA_STATUS_ADDR, (uint32_t) -1);
}

// **********************************************************************************
// **********************************************************************************

void axi_dma_trigger_mode_set(void *device, uint32_t peripheral_id, uint32_t *mode) {

  axi_reg_write(device, peripheral_id, AXI_DMA_TRIGGER_MODE_ADDR,
    (AXI_DMA_TRIGGER_SET(0, *(mode + 0)) |
     AXI_DMA_TRIGGER_SET(1, *(mode + 1)) |
     AXI_DMA_TRIGGER_SET(2, *(mode + 2)) |
     AXI_DMA_TRIGGER_SET(3, *(mode + 3))));
}

void axi_dma_trigger_mode_get(void *device, uint32_t peripheral_id, uint32_t *mode) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_DMA_TRIGGER_MODE_ADDR);

  *(mode + 0) = AXI_DMA_TRIGGER_GET(0, data);
  *(mode + 1) = AXI_DMA_TRIGGER_GET(1, data);
  *(mode + 2) = AXI_DMA_TRIGGER_GET(2, data);
  *(mode + 3) = AXI_DMA_TRIGGER_GET(3, data);
}

void axi_dma_config(void *device, uint32_t peripheral_id, struct axi_dma_params *param) {

  axi_reg_write(device, peripheral_id, AXI_DMA_START_ADDR_HIGH_ADDR, 0x0);
  axi_reg_write(device, peripheral_id, AXI_DMA_START_ADDR_LOW_ADDR, param->start_address);
  axi_reg_write(device, peripheral_id, AXI_DMA_LENGTH_ADDR, param->length);
  axi_reg_write(device, peripheral_id, AXI_DMA_CONTROL_ADDR,
    (AXI_DMA_QTHRESHOLD(param->qthreshold) |
     AXI_DMA_CYCLIC(param->cyclic)));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_dma_run(void *device, uint32_t peripheral_id, struct axi_dma_params *param,
  uint32_t *mode, uint32_t timeout_us) {

  uint32_t status;

  if ((param->length & 0x7fffff) == 0x0) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-DMA-ID[0x%x]: "
      "dma length[0x%x] must be [1, 0x7fffff].\n", __FILE__, __LINE__,
      peripheral_id, param->length);
    return(-1);
  }

  if ((axi_dma_busy(device, peripheral_id) != 0) ||
    (axi_dma_status(device, peripheral_id) != 0)) {
    axi_dma_reset(device, peripheral_id);
  }

  axi_dma_status_clear(device, peripheral_id);
  axi_dma_config(device, peripheral_id, param);
  axi_dma_trigger_mode_set(device, peripheral_id, mode);
  axi_dma_start(device, peripheral_id);
  axi_dma_delay_us(device, peripheral_id, 1);
  axi_dma_timer_set(device, peripheral_id, timeout_us);
  while (axi_dma_busy(device, peripheral_id) == 1) {
    if (axi_dma_timer_get(device, peripheral_id) == 0) {
      axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-DMA-ID[0x%x]: "
        "function axi_dma_run() has timed out.\n", __FILE__, __LINE__,
        peripheral_id);
      return(-1);
    }
  }

  status = axi_dma_status(device, peripheral_id);
  if (status == 0) {
    return(0);
  }

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-DMA-ID[0x%x]: "
    "overflow(%d), invalid-length(%d), missing-tlast(%d), invalid-address(%d) "
    "slave-error(%d).\n", __FILE__, __LINE__, peripheral_id, AXI_DMA_OVERFLOW(status),
    AXI_DMA_LENGTH_ERROR(status), AXI_DMA_TLAST_ERROR(status),
    AXI_DMA_ADDRDEC_ERROR(status), AXI_DMA_SLAVE_ERROR(status));
  return(-1);
}

// **********************************************************************************
// **********************************************************************************
