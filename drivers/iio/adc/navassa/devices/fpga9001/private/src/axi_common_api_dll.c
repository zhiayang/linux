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
// Description:       Platform Common
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifdef ADI_ADRV9001_PLATFORM_API_DLL

#include "axi_common.h"
#include "adi_fpga9001_hal.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_platform_open(void *device) {

  return(0);
}

int32_t axi_platform_close(void *device) {

  return(0);
}

void axi_platform_log(void *device, uint32_t level, const char *format, ...) {

  adi_fpga9001_Device_t *m_device;
  char message[1024];

  va_list arglist;

  va_start(arglist, format);
  vsprintf(message, format, arglist);
  va_end(arglist);

  m_device = (adi_fpga9001_Device_t *)device;
  message[1023] = 0;

  if ((level == AXI_PLATFORM_FATAL) || (level == AXI_PLATFORM_ERROR)) {
    ADI_ERROR_REPORT(&m_device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
      ADI_COMMON_ACT_NO_ACTION, 0, message);
  }
}

int32_t axi_platform_plconfig(void *device, uint32_t ssi_type) {

  return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_reg_write(void *device, uint32_t base, uint32_t offset, uint32_t data) {

  return(adi_fpga9001_hal_Register_Write((adi_fpga9001_Device_t *)device,
    (base + offset), data));
}

uint32_t axi_reg_read(void *device, uint32_t base, uint32_t offset) {

  int32_t status;
  uint32_t data;

  status = adi_fpga9001_hal_Register_Read((adi_fpga9001_Device_t *)device,
    (base + offset), &data);
  if (status != 0) return(-1);
  return (data);
}

int32_t axi_mem_write(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  return(adi_fpga9001_hal_Ram_Write((adi_fpga9001_Device_t *)device, 
    (base + offset), data, count));
}

void axi_mem_read(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  adi_fpga9001_hal_Ram_Read((adi_fpga9001_Device_t *)device, 
    (base + offset), data, count);
}

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************

