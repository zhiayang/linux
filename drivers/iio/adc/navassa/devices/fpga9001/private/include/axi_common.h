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
// Description:       ADRV9001 Platform
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_COMMON_H
#define AXI_COMMON_H

// **********************************************************************************
// **********************************************************************************

#ifdef ADI_ADRV9001_PLATFORM_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <netdb.h> 
#include <libgen.h>
#include <netinet/in.h> 
#include <sys/mman.h> 
#include <sys/socket.h> 
#include <sys/reboot.h> 
#include <sys/time.h> 

#define AXI_MSPI_ID       0x0
#define AXI_SYSID_ID      0x1
#define AXI_ADRV9001_ID   0x2
#define AXI_DMA_TX0_ID    0x3
#define AXI_DMA_TX1_ID    0x4
#define AXI_DMA_RX0_ID    0x5
#define AXI_DMA_RX1_ID    0x6
#define AXI_DMA_ORX0_ID   0x7
#define AXI_DMA_ORX1_ID   0x8
#define AXI_DMA_GAIN_ID   0x9
#define AXI_CLKGEN_ID     0xa
#define AXI_MEM_ID        0xb
#define NUM_OF_DEVICES    0xc

#endif

// **********************************************************************************
// **********************************************************************************

#ifdef ADI_ADRV9001_PLATFORM_BARE_METAL

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xil_io.h>

#define AXI_MSPI_ID       0x44a00000
#define AXI_SYSID_ID      0x43000000
#define AXI_ADRV9001_ID   0x43010000
#define AXI_DMA_TX0_ID    0x43020000
#define AXI_DMA_TX1_ID    0x43030000
#define AXI_DMA_RX0_ID    0x43040000
#define AXI_DMA_RX1_ID    0x43050000
#define AXI_DMA_ORX0_ID   0x43060000
#define AXI_DMA_ORX1_ID   0x43070000
#define AXI_DMA_GAIN_ID   0x43080000
#define AXI_CLKGEN_ID     0x43100000
#define AXI_MEM_ID        0x80000000
#define NUM_OF_DEVICES    0xffffffff

#endif

// **********************************************************************************
// **********************************************************************************

#ifdef ADI_ADRV9001_PLATFORM_API_DLL

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define AXI_MSPI_ID       0x44a00000
#define AXI_SYSID_ID      0x43000000
#define AXI_ADRV9001_ID   0x43010000
#define AXI_DMA_TX0_ID    0x43020000
#define AXI_DMA_TX1_ID    0x43030000
#define AXI_DMA_RX0_ID    0x43040000
#define AXI_DMA_RX1_ID    0x43050000
#define AXI_DMA_ORX0_ID   0x43060000
#define AXI_DMA_ORX1_ID   0x43070000
#define AXI_DMA_GAIN_ID   0x43080000
#define AXI_CLKGEN_ID     0x43100000
#define AXI_MEM_ID        0x80000000
#define NUM_OF_DEVICES    0xffffffff

#endif

// **********************************************************************************
// **********************************************************************************

#ifdef ADI_ADRV9001_PLATFORM_SIMULATION

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define AXI_MSPI_ID       0x44a00000
#define AXI_SYSID_ID      0x43000000
#define AXI_ADRV9001_ID   0x43010000
#define AXI_DMA_TX0_ID    0x43020000
#define AXI_DMA_TX1_ID    0x43030000
#define AXI_DMA_RX0_ID    0x43040000
#define AXI_DMA_RX1_ID    0x43050000
#define AXI_DMA_ORX0_ID   0x43060000
#define AXI_DMA_ORX1_ID   0x43070000
#define AXI_DMA_GAIN_ID   0x43080000
#define AXI_CLKGEN_ID     0x43100000
#define AXI_MEM_ID        0x80000000
#define NUM_OF_DEVICES    0xffffffff

#endif

// **********************************************************************************
// **********************************************************************************

#define AXI_PLATFORM_NOTE     0x0
#define AXI_PLATFORM_INFO     0x1
#define AXI_PLATFORM_WARNING  0x2
#define AXI_PLATFORM_ERROR    0x3
#define AXI_PLATFORM_FATAL    0x4

// **********************************************************************************
// **********************************************************************************

int32_t axi_platform_open(void *device);
int32_t axi_platform_close(void *device);
void axi_platform_log(void *device, uint32_t level, const char *format, ...);
int32_t axi_platform_plconfig(void *device, uint32_t ssi_type);

// **********************************************************************************
// **********************************************************************************

int32_t axi_reg_write(void *device, uint32_t base, uint32_t offset, uint32_t data);
uint32_t axi_reg_read(void *device, uint32_t base, uint32_t offset);
int32_t axi_mem_write(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count);
void axi_mem_read(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count);

#endif

// **********************************************************************************
// **********************************************************************************
