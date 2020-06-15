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
// Description:       AXI Clock Gen
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_CLKGEN_H
#define AXI_CLKGEN_H

#include "axi_common.h"

#define AXI_CLKGEN_REF_CLK_FREQ_HZ 200000000
#define AXI_CLKGEN_STATUS_ADDR 0x004
#define AXI_CLKGEN_VCO_ADDR 0x200
#define AXI_CLKGEN_VCO_DIV_SET(d) ((d & 0xff) << 0)
#define AXI_CLKGEN_VCO_DIV_GET(d) ((d >> 0) & 0xff)
#define AXI_CLKGEN_VCO_MUL_INT_SET(d) ((d & 0xff) << 8)
#define AXI_CLKGEN_VCO_MUL_INT_GET(d) ((d >> 8) & 0xff)
#define AXI_CLKGEN_VCO_MUL_FRAC_SET(d) ((d & 0x3ff) << 16)
#define AXI_CLKGEN_VCO_MUL_FRAC_GET(d) ((d >> 16) & 0x3ff)
#define AXI_CLKGEN_OUT_ADDR 0x208
#define AXI_CLKGEN_OUT_DIV_INT_SET(d) ((d & 0xff) << 0)
#define AXI_CLKGEN_OUT_DIV_INT_GET(d) ((d >> 0) & 0xff)
#define AXI_CLKGEN_OUT_DIV_FRAC_SET(d) ((d & 0x3ff) << 8)
#define AXI_CLKGEN_OUT_DIV_FRAC_GET(d) ((d >> 8) & 0x3ff)
#define AXI_CLKGEN_CTL_ADDR 0x25c

// **********************************************************************************
// **********************************************************************************

uint32_t axi_clkgen_status_get(void *device, uint32_t peripheral_id);
int32_t axi_clkgen_configure(void *device, uint32_t peripheral_id,
  uint32_t clk_freq_hz, uint32_t *locked);

#endif

// **********************************************************************************
// **********************************************************************************
