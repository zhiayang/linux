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

#ifdef ADI_ADRV9001_PLATFORM_LINUX

#include "axi_common.h"

struct uio_device {
  uint32_t id;
  uint32_t base;
  uint32_t size;
  char  name[64];
  char uio[64];
  int32_t f_device;
  uint32_t *p_device;
  int32_t status;
};

struct uio_device uio_devices[NUM_OF_DEVICES] = {
  { .base = 0x44a00000, .size = (1<<12), .uio = "/dev/uio0",
    .id = AXI_MSPI_ID, .name = "axi_mspi"},
  { .base = 0x43000000, .size = (1<<12), .uio = "/dev/uio1",
    .id = AXI_SYSID_ID, .name = "axi_sysid"},
  { .base = 0x43010000, .size = (1<<16), .uio = "/dev/uio2",
    .id = AXI_ADRV9001_ID, .name = "axi_adrv9001"},
  { .base = 0x43020000, .size = (1<<12), .uio = "/dev/uio3",
    .id = AXI_DMA_TX0_ID, .name = "axi_dma_tx0"},
  { .base = 0x43030000, .size = (1<<12), .uio = "/dev/uio4",
    .id = AXI_DMA_TX1_ID, .name = "axi_dma_tx1"},
  { .base = 0x43040000, .size = (1<<12), .uio = "/dev/uio5",
    .id = AXI_DMA_RX0_ID, .name = "axi_dma_rx0"},
  { .base = 0x43050000, .size = (1<<12), .uio = "/dev/uio6",
    .id = AXI_DMA_RX1_ID, .name = "axi_dma_rx1"},
  { .base = 0x43060000, .size = (1<<12), .uio = "/dev/uio7",
    .id = AXI_DMA_ORX0_ID, .name = "axi_dma_orx0"},
  { .base = 0x43070000, .size = (1<<12), .uio = "/dev/uio8",
    .id = AXI_DMA_ORX1_ID, .name = "axi_dma_orx1"},
  { .base = 0x43080000, .size = (1<<12), .uio = "/dev/uio9",
    .id = AXI_DMA_GAIN_ID, .name = "axi_dma_gain"},
  { .base = 0x43100000, .size = (1<<12), .uio = "/dev/uio10",
    .id = AXI_CLKGEN_ID, .name = "axi_clkgen"},
  { .base = 0x80000000, .size = (1<<30), .uio = "/dev/uio11",
    .id = AXI_MEM_ID, .name = "axi_mem"}
};

// **********************************************************************************
// **********************************************************************************

int32_t axi_platform_open(void *device) {

  int32_t i;
  int32_t status;

  status = 0;

  for (i = 0; i < NUM_OF_DEVICES; i++) {
    uio_devices[i].status = 0;

    if (uio_devices[i].id != i) {
      printf("WARNING: id mismatch for device `%s'\n", uio_devices[i].uio);
      uio_devices[i].status = -1;
      status = -1;
    }

    if (uio_devices[i].status == 0) {
      uio_devices[i].f_device = open(uio_devices[i].uio, O_RDWR);
      if (uio_devices[i].f_device < 1) {
        printf("WARNING: can not open device `%s'\n", uio_devices[i].uio);
        uio_devices[i].status = -1;
        status = -1;
      }
    }

    if (uio_devices[i].status == 0) {
      uio_devices[i].p_device = mmap(NULL, uio_devices[i].size,
        (PROT_READ|PROT_WRITE), MAP_SHARED, uio_devices[i].f_device, 0);
      if (uio_devices[i].p_device == NULL) {
        printf("WARNING: can not mmap device `%s'\n", uio_devices[i].uio);
        uio_devices[i].status = -1;
        status = -1;
      }
    }
  }

  return(status);
}

int32_t axi_platform_close(void *device) {

  int32_t i;

  for (i = 0; i < NUM_OF_DEVICES; i++) {
    if (uio_devices[i].status == 0) {
      munmap((void*) uio_devices[i].p_device, uio_devices[i].size);
      close(uio_devices[i].f_device);
      uio_devices[i].status = -1;
    }
  }

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

  if (uio_devices[base].size < offset) {
    axi_platform_log(0, AXI_PLATFORM_ERROR, "can not write at offset `%s'.\n",
      uio_devices[base].name);
    return(-1);
  }
  *(uio_devices[base].p_device + (offset/4)) = data;
  return(0);
}

uint32_t axi_reg_read(void *device, uint32_t base, uint32_t offset) {

  if (uio_devices[base].size < offset) {
    axi_platform_log(0, AXI_PLATFORM_ERROR, "can not read at offset `%s'.\n",
      uio_devices[base].name);
    return((uint32_t) -1);
  }
  return(*(uio_devices[base].p_device + (offset/4)));
}

int32_t axi_mem_write(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  if (uio_devices[base].size < (offset + count)) {
    axi_platform_log(0, AXI_PLATFORM_ERROR, "can not write at offset `%s'.\n",
      uio_devices[base].name);
    return(-1);
  }
  memcpy((uio_devices[base].p_device + (offset/4)), data, (sizeof(uint32_t) * count));
  return(0);
}

void axi_mem_read(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  if (uio_devices[base].size < (offset + count)) {
    axi_platform_log(0, AXI_PLATFORM_ERROR, "can not read at offset `%s'.\n",
      uio_devices[base].name);
    return((uint32_t) -1);
  }
  memcpy(data, (uio_devices[base].p_device + (offset/4)), (sizeof(uint32_t) * count));
}

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************

