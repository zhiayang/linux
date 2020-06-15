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

#ifdef ADI_ADRV9001_PLATFORM_BARE_METAL

#include "axi_common.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_platform_open(void *device) {

  return(0);
}

int32_t axi_platform_close(void *device) {

  return(0);
}

void axi_platform_log(void *device, uint32_t level, const char *format, ...) {

  va_list arglist;

  if (level == AXI_PLATFORM_FATAL) {
    printf("FATAL: ");
  } else if (level == AXI_PLATFORM_ERROR) {
    printf("ERROR: ");
  } else if (level == AXI_PLATFORM_WARNING) {
    printf("WARNING: ");
  } else if (level == AXI_PLATFORM_INFO) {
    printf("INFO: ");
  }

  va_start(arglist, format);
  vprintf(format, arglist);
  va_end(arglist);
}

int32_t axi_platform_plconfig(void *device, uint32_t ssi_type) {

  return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_reg_write(void *device, uint32_t base, uint32_t offset, uint32_t data) {

  Xil_Out32((base + offset), data);
  return(0);
}

uint32_t axi_reg_read(void *device, uint32_t base, uint32_t offset) {

  return(Xil_In32(base + offset));
}

int32_t axi_mem_write(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  memcpy((void *)(base + offset), data, (sizeof(uint32_t) * count));
  return(0);
}

void axi_mem_read(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  memcpy(data, (void *)(base + offset), (sizeof(uint32_t) * count));
}

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************
