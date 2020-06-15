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
// Description:       AXI MSPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_mspi.h"

// **********************************************************************************
// **********************************************************************************

uint32_t axi_mspi_instance_id_get(void *device, uint32_t peripheral_id) {

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-MSPI: "
    "function axi_mspi_instance_id_get() is NOT available in this core.\n",
    __FILE__, __LINE__);
  return((uint32_t) -1);
}

void axi_mspi_timer_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-MSPI: "
    "function axi_mspi_timer_set() is NOT available in this core.\n",
    __FILE__, __LINE__);
}

uint32_t axi_mspi_timer_get(void *device, uint32_t peripheral_id) {

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-MSPI: "
    "function axi_mspi_timer_get() is NOT available in this core.\n",
    __FILE__, __LINE__);
  return((uint32_t) -1);
}

void axi_mspi_delay_us(void *device, uint32_t peripheral_id, uint32_t delay) {

  axi_mspi_timer_set(device, peripheral_id, (delay * 100));
  while (axi_mspi_timer_get(device, peripheral_id) != 0);
}

// **********************************************************************************
// **********************************************************************************

void axi_mspi_enable_set(void *device, uint32_t peripheral_id, uint32_t data) {

}

uint32_t axi_mspi_busy(void *device, uint32_t peripheral_id) {

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-MSPI: "
    "function axi_mspi_busy() is NOT available in this core.\n",
    __FILE__, __LINE__);
  return((uint32_t) -1);
}

int32_t axi_mspi_config(void *device, uint32_t peripheral_id,
  struct axi_mspi_params *param) {

  axi_reg_write(device, peripheral_id, 0x60, 0x86);
  axi_reg_write(device, peripheral_id, 0x80, 0x31031);
  return(0);
}

void axi_mspi_chip_select_set(void *device, uint32_t peripheral_id,
  uint32_t index, uint32_t data) {

  axi_reg_write(device, peripheral_id, 0x70, data);
}

int32_t axi_mspi_xfer_all(void *device, uint32_t peripheral_id,
  uint8_t *mosi_data, uint8_t *miso_data, uint32_t count) {

  int32_t i;

  for (i = 0; i < count; i++) {
    axi_reg_write(device, peripheral_id, 0x68, *(mosi_data + i));
    while ((axi_reg_read(device, peripheral_id, 0x64) & 0x1) == 0x1);
    *(miso_data + i) = axi_reg_read(device, peripheral_id, 0x6c) & 0xff;
  }

  return(0);
}

int32_t axi_mspi_xfer(void *device, uint32_t peripheral_id,
  uint32_t *mosi_data, uint32_t mosi_count,
  uint32_t *miso_data, uint32_t miso_count) {

  int32_t i;

  for (i = 0; i < mosi_count; i++) {
    axi_reg_write(device, peripheral_id, 0x68, *(mosi_data + i));
    while ((axi_reg_read(device, peripheral_id, 0x64) & 0x1) == 0x1);
    axi_reg_read(device, peripheral_id, 0x6c);
  }

  for (i = 0; i < miso_count; i++) {
    axi_reg_write(device, peripheral_id, 0x68, 0x00);
    while ((axi_reg_read(device, peripheral_id, 0x64) & 0x1) == 0x1);
    *(miso_data + i) = axi_reg_read(device, peripheral_id, 0x6c) & 0xff;
  }

  return(0);
}
    
// **********************************************************************************
// **********************************************************************************
