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
// Description:       AXI SYSID Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_sysid.h"

// **********************************************************************************
// **********************************************************************************

uint32_t axi_sysid_instance_id_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_SYSID_INSTANCE_ID_ADDR));
}

void axi_sysid_timer_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_SYSID_TIMER_ADDR, data);
}

uint32_t axi_sysid_timer_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_SYSID_TIMER_ADDR));
}

void axi_sysid_delay_us(void *device, uint32_t peripheral_id, uint32_t delay) {

  axi_sysid_timer_set(device, peripheral_id, (delay * 100));
  while (axi_sysid_timer_get(device, peripheral_id) != 0);
}

// **********************************************************************************
// **********************************************************************************

void axi_sysid_gpio_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_SYSID_GPIO_IN_ADDR, data);
}

uint32_t axi_sysid_gpio_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_SYSID_GPIO_OUT_ADDR));
}

void axi_sysid_gpio_mask_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_SYSID_GPIO_IN_MASK_ADDR, data);
}

uint32_t axi_sysid_state_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_SYSID_STATUS_STATE_ADDR));
}

uint32_t axi_sysid_event_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_SYSID_STATUS_EVENT_ADDR));
}

void axi_sysid_event_clear(void *device, uint32_t peripheral_id) {

  axi_reg_write(device, peripheral_id, AXI_SYSID_STATUS_EVENT_ADDR, (uint32_t) -1);
}

void axi_sysid_event_mask_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_SYSID_STATUS_EVENT_MASK_ADDR, data);
}

// **********************************************************************************
// **********************************************************************************

void axi_sysid_sys_read(void *device, uint32_t peripheral_id, uint32_t *data,
  uint32_t addr, uint32_t size) {

  int32_t n;

  for (n = 0; n < size; n++) {
    *(data + n) = axi_reg_read(device, peripheral_id, (addr + (n * 4)));
  }
}

void axi_sysid_sys_version_get(void *device, uint32_t peripheral_id, uint32_t *data) {

  axi_sysid_sys_read(device, peripheral_id, data, AXI_SYSID_SYS_VERSION_ADDR,
    AXI_SYSID_SYS_VERSION_SIZE);
}

void axi_sysid_sys_params_get(void *device, uint32_t peripheral_id, uint32_t *data) {

  axi_sysid_sys_read(device, peripheral_id, data, AXI_SYSID_SYS_PARAMS_ADDR,
    AXI_SYSID_SYS_PARAMS_SIZE);
}

void axi_sysid_sys_build_get(void *device, uint32_t peripheral_id, uint32_t *data) {

  axi_sysid_sys_read(device, peripheral_id, data, AXI_SYSID_SYS_BUILD_ADDR,
    AXI_SYSID_SYS_BUILD_SIZE);
}

void axi_sysid_sys_info_0_get(void *device, uint32_t peripheral_id, uint32_t *data) {

  axi_sysid_sys_read(device, peripheral_id, data, AXI_SYSID_SYS_INFO_0_ADDR,
    AXI_SYSID_SYS_INFO_0_SIZE);
}

void axi_sysid_sys_info_1_get(void *device, uint32_t peripheral_id, uint32_t *data) {

  axi_sysid_sys_read(device, peripheral_id, data, AXI_SYSID_SYS_INFO_1_ADDR,
    AXI_SYSID_SYS_INFO_1_SIZE);
}

// **********************************************************************************
// **********************************************************************************

