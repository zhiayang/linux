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

#ifndef AXI_SYSID_H
#define AXI_SYSID_H

#include "axi_common.h"

// defines

#define AXI_SYSID_VERSION_ADDR              0x000
#define AXI_SYSID_INSTANCE_ID_ADDR          0x004
#define AXI_SYSID_SCRATCH_ADDR              0x008
#define AXI_SYSID_TIMER_ADDR                0x00c
#define AXI_SYSID_GPIO_IN_ADDR              0x040
#define AXI_SYSID_GPIO_OUT_ADDR             0x044
#define AXI_SYSID_GPIO_IN_MASK_ADDR         0x048
#define AXI_SYSID_STATUS_STATE_ADDR         0x080
#define AXI_SYSID_STATUS_EVENT_ADDR         0x084
#define AXI_SYSID_STATUS_EVENT_MASK_ADDR    0x088
#define AXI_SYSID_SYS_VERSION_ADDR          0x100
#define AXI_SYSID_SYS_VERSION_SIZE          0x001
#define AXI_SYSID_SYS_PARAMS_ADDR           0x104
#define AXI_SYSID_SYS_PARAMS_SIZE           0x002
#define AXI_SYSID_SYS_BUILD_ADDR            0x140
#define AXI_SYSID_SYS_BUILD_SIZE            0x002
#define AXI_SYSID_SYS_INFO_0_ADDR           0x200
#define AXI_SYSID_SYS_INFO_0_SIZE           0x008
#define AXI_SYSID_SYS_INFO_1_ADDR           0x300
#define AXI_SYSID_SYS_INFO_1_SIZE           0x008

// axi_sysid functions

uint32_t axi_sysid_instance_id_get(void *device, uint32_t peripheral_id);
void axi_sysid_timer_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_sysid_timer_get(void *device, uint32_t peripheral_id);
void axi_sysid_delay_us(void *device, uint32_t peripheral_id, uint32_t delay);

void axi_sysid_gpio_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_sysid_gpio_get(void *device, uint32_t peripheral_id); 
void axi_sysid_gpio_mask_set(void *device, uint32_t peripheral_id, uint32_t data);

uint32_t axi_sysid_state_get(void *device, uint32_t peripheral_id);
uint32_t axi_sysid_event_get(void *device, uint32_t peripheral_id);
void axi_sysid_event_clear(void *device, uint32_t peripheral_id);
void axi_sysid_event_mask_set(void *device, uint32_t peripheral_id, uint32_t data);

// platform functions

void axi_sysid_sys_read(void *device, uint32_t peripheral_id, uint32_t *data,
  uint32_t addr, uint32_t size);
void axi_sysid_sys_version_get(void *device, uint32_t peripheral_id, uint32_t *data);
void axi_sysid_sys_params_get(void *device, uint32_t peripheral_id, uint32_t *data);
void axi_sysid_sys_build_get(void *device, uint32_t peripheral_id, uint32_t *data);
void axi_sysid_sys_info_0_get(void *device, uint32_t peripheral_id, uint32_t *data);
void axi_sysid_sys_info_1_get(void *device, uint32_t peripheral_id, uint32_t *data);

#endif

// **********************************************************************************
// **********************************************************************************
