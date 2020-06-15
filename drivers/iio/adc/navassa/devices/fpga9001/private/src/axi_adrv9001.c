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
// Description:       AXI ADRV9001 Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_adrv9001.h"

// **********************************************************************************
// **********************************************************************************

uint32_t axi_adrv9001_instance_id_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_INSTANCE_ID_ADDR));
}

void axi_adrv9001_timer_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TIMER_ADDR, data);
}

uint32_t axi_adrv9001_timer_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_TIMER_ADDR));
}

void axi_adrv9001_delay_us(void *device, uint32_t peripheral_id, uint32_t delay) {

  axi_adrv9001_timer_set(device, peripheral_id, (delay * 100));
  while (axi_adrv9001_timer_get(device, peripheral_id) != 0);
}

uint32_t axi_adrv9001_clk_freq_get(void *device, uint32_t peripheral_id, uint32_t addr) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, addr);
  if ((data >> 21) == 0) {
    return(data * 1526);
  }
  return((uint32_t) -1);
}

// **********************************************************************************
// **********************************************************************************
// reset, mode and interrupts

void axi_adrv9001_reset_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_RESETB_ADDR, data);
}

uint32_t axi_adrv9001_reset_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_RESETB_ADDR));
}

void axi_adrv9001_mode_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MODE_ADDR, data);
}

uint32_t axi_adrv9001_mode_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_MODE_ADDR));
}

uint32_t axi_adrv9001_gpint_status(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_GPINT_ADDR));
}

void axi_adrv9001_gpint_status_clear(void *device, uint32_t peripheral_id) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_GPINT_ADDR, 0x1);
}

void axi_adrv9001_gpint_mode_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_GPINT_MODE_ADDR, data);
}

// **********************************************************************************
// **********************************************************************************
// GPIO functions

void axi_adrv9001_usr_gpio_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_USR_GPIO_OUT_ADDR, data);
}

uint32_t axi_adrv9001_usr_gpio_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_USR_GPIO_IN_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// MCS functions

void axi_adrv9001_mcs_config(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_mcs_params *param) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MCS_CONTROL_ADDR,
    (AXI_ADRV9001_MCS_PERIOD_SET(param->period) |
    AXI_ADRV9001_MCS_WIDTH_SET(param->width) |
    AXI_ADRV9001_MCS_NUM_SET(param->count) |
    AXI_ADRV9001_MCS_EDGE_FALL1_RISE0_SET(param->fall1_rise0)));
}

void axi_adrv9001_mcs_start(void *device, uint32_t peripheral_id) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MCS_REQUEST_ADDR, 0x1);
}

uint32_t axi_adrv9001_mcs_status(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_MCS_REQUEST_ADDR));
}

void axi_adrv9001_mcs_select_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MCS_SELECT_ADDR, data);
}

uint32_t axi_adrv9001_mcs_select_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_MCS_SELECT_ADDR));
}

uint32_t axi_adrv9001_mcs_clk_freq_get(void *device, uint32_t peripheral_id) {

  return(axi_adrv9001_clk_freq_get(device, peripheral_id,
    AXI_ADRV9001_MCS_CLK_MON_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// MMCM functions

uint32_t axi_adrv9001_mmcm_busy(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_MMCM_STATUS_ADDR);
  return(AXI_ADRV9001_MMCM_BUSY_GET(data));
}

void axi_adrv9001_mmcm_write(void *device, uint32_t peripheral_id,
  uint32_t addr, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MMCM_ADDR_ADDR, addr);
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MMCM_WRDATA_ADDR, data);
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MMCM_ACCESS_ADDR, 0x1);
  while (axi_adrv9001_mmcm_busy(device, peripheral_id) == 0x1);
}

uint32_t axi_adrv9001_mmcm_read(void *device, uint32_t peripheral_id,
  uint32_t addr) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MMCM_ADDR_ADDR, addr);
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MMCM_ACCESS_ADDR, 0x3);
  while (axi_adrv9001_mmcm_busy(device, peripheral_id) == 0x1);
  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_MMCM_RDDATA_ADDR));
}

void axi_adrv9001_mmcm_read_modify_write(void *device, uint32_t peripheral_id,
  uint32_t addr, uint32_t mask, uint32_t data) {

  uint32_t m_data;

  m_data = axi_adrv9001_mmcm_read(device, peripheral_id, addr);
  m_data = (m_data & mask) | data;
  axi_adrv9001_mmcm_write(device, peripheral_id, addr, m_data);
}

void axi_adrv9001_mmcm_set_div(void *device, uint32_t peripheral_id,
  uint32_t addr, uint32_t div) {

  uint32_t high_time;
  uint32_t low_time;
  uint32_t edge;

  high_time = (uint32_t) (div/2);
  low_time = div - high_time;
  edge = low_time - high_time;

  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (addr + 0), 0xf000, ((high_time << 6) | low_time));
  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (addr + 1), 0xff7f, (edge << 7));
}

void axi_adrv9001_mmcm_set_lock(void *device, uint32_t peripheral_id, uint32_t div) {

  uint32_t unlock_cnt;
  uint32_t lock_high;
  uint32_t lock_cnt;
  uint32_t lock_delay;

  unlock_cnt = 1;
  lock_high = 0x3e9;

  if (div == 0) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "mmcm divide can not be 0.\n", __FILE__, __LINE__, peripheral_id);
    return;
  }

  if (div >=  1) lock_cnt = 0x3e8;
  if (div >= 11) lock_cnt = 0x384;
  if (div >= 12) lock_cnt = 0x339;
  if (div >= 13) lock_cnt = 0x2ee;
  if (div >= 14) lock_cnt = 0x2bc;
  if (div >= 15) lock_cnt = 0x28a;
  if (div >= 16) lock_cnt = 0x271;
  if (div >= 17) lock_cnt = 0x23f;
  if (div >= 18) lock_cnt = 0x226;
  if (div >= 19) lock_cnt = 0x20d;
  if (div >= 20) lock_cnt = 0x1f4;
  if (div >= 21) lock_cnt = 0x1db;
  if (div >= 22) lock_cnt = 0x1c2;
  if (div >= 23) lock_cnt = 0x1a9;
  if (div >= 24) lock_cnt = 0x190;
  if (div >= 26) lock_cnt = 0x177;
  if (div >= 27) lock_cnt = 0x15e;
  if (div >= 29) lock_cnt = 0x145;
  if (div >= 31) lock_cnt = 0x12c;
  if (div >= 34) lock_cnt = 0x113;
  if (div >= 37) lock_cnt = 0x0fa;

  if (div >=  1) lock_delay = 0x06;
  if (div >=  3) lock_delay = 0x08;
  if (div >=  4) lock_delay = 0x0b;
  if (div >=  5) lock_delay = 0x0e;
  if (div >=  6) lock_delay = 0x11;
  if (div >=  7) lock_delay = 0x13;
  if (div >=  8) lock_delay = 0x16;
  if (div >=  9) lock_delay = 0x19;
  if (div >= 10) lock_delay = 0x1c;
  if (div >= 11) lock_delay = 0x1f;

  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (AXI_ADRV9001_MMCM_LOCK_ID + 0), 0xfc00, lock_cnt);
  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (AXI_ADRV9001_MMCM_LOCK_ID + 1), 0x8000, ((lock_delay << 10) | unlock_cnt));
  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (AXI_ADRV9001_MMCM_LOCK_ID + 2), 0x8000, ((lock_delay << 10) | lock_high));
}

void axi_adrv9001_mmcm_set_filter(void *device, uint32_t peripheral_id, uint32_t div) {

  uint32_t filter_cp;
  uint32_t filter_res_lfhf;

  filter_cp = 0x0800;

  if (div == 0) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "mmcm divide can not be 0.\n", __FILE__, __LINE__, peripheral_id);
    return;
  }

  if (div >=  1) filter_res_lfhf = 0x9900;
  if (div >=  5) filter_res_lfhf = 0x1900;
  if (div >=  6) filter_res_lfhf = 0x8900;
  if (div >=  7) filter_res_lfhf = 0x9100;
  if (div >=  8) filter_res_lfhf = 0x0900;
  if (div >=  9) filter_res_lfhf = 0x1100;
  if (div >= 11) filter_res_lfhf = 0x8100;
  if (div >= 12) filter_res_lfhf = 0x9800;
  if (div >= 16) filter_res_lfhf = 0x0100;
  if (div >= 19) filter_res_lfhf = 0x1800;
  if (div >= 26) filter_res_lfhf = 0x8800;
  if (div >= 31) filter_res_lfhf = 0x9000;
  if (div >= 48) filter_res_lfhf = 0x0800;

  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (AXI_ADRV9001_MMCM_FILTER_ID + 0), 0x66ff, filter_cp);
  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    (AXI_ADRV9001_MMCM_FILTER_ID + 1), 0x666f, filter_res_lfhf);
}

void axi_adrv9001_clk_reset_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_MMCM_RESET_ADDR, data);
}

uint32_t axi_adrv9001_clk_status(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_MMCM_STATUS_ADDR);
  return(AXI_ADRV9001_MMCM_LOCKED_GET(data));
}

int32_t axi_adrv9001_clk_config(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_clk_params *param, uint32_t *locked) {

  uint32_t data;
  uint32_t out_freq[4];
  uint32_t vco_freq_diff;
  uint32_t vco_out_diff;
  uint32_t vco_freq_alt;
  uint32_t vco_out_div;
  uint32_t dev_freq;
  uint32_t vco_freq;
  uint32_t out_div;
  int32_t i;

  // locked status

  *locked = 0;

  // reference clock range (maximum is pd limit)

  if ((param->ref_clk_freq_hz < 10000000) || (param->ref_clk_freq_hz > 500000000)) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "invalid mmcm reference clock frequency[%dHz], must be [10MHz, 500MHz].\n",
      __FILE__, __LINE__, peripheral_id, param->ref_clk_freq_hz);
    return(-1);
  }

  // device clock range (low automatically takes care of 128 limit)
 
  dev_freq = (param->dev_clk_freq_hz == 0) ? param->ref_clk_freq_hz : param->dev_clk_freq_hz;

  if (dev_freq < 4690000) {
    dev_freq = (uint32_t) (dev_freq/1000);
    out_div = (uint32_t) (4690000/dev_freq) + 500;
    out_div = (uint32_t) (out_div/1000);
    dev_freq = out_div * dev_freq;
  }

  if (dev_freq > 250000000) {
    out_div = (uint32_t) (dev_freq/250000000) + 1;
    dev_freq = dev_freq/out_div;
  }

  if (dev_freq != param->dev_clk_freq_hz) {
    axi_platform_log(device, AXI_PLATFORM_WARNING, "device clock frequency "
      "%dHz (requested %dHz).\n", dev_freq, param->dev_clk_freq_hz);
  }

  // tdd must be a common multiple of the ssi clocks

  out_freq[0] = param->ssi_rx0_clk_freq_hz;
  out_freq[1] = param->ssi_rx1_clk_freq_hz;
  out_freq[2] = param->ssi_tx0_clk_freq_hz;
  out_freq[3] = param->ssi_tx1_clk_freq_hz;

  data = 0;
  for (i = 0; i < 4; i++) {
    if (out_freq[i] < 24000) {
      out_freq[i] = 24000;
    }
    if (out_freq[i] > data) {
      data = out_freq[i];
    }
  }

  out_div = ((uint32_t) ((4690000 * 2) / data)) + 1;
  param->gen_tdd_clk_freq_hz = 0;
  while (param->gen_tdd_clk_freq_hz == 0) {
    out_div = out_div + 1;
    param->gen_tdd_clk_freq_hz = out_div * data;
    if (param->gen_tdd_clk_freq_hz > 250000000) {
      param->gen_tdd_clk_freq_hz = 0;
      break;
    }
    for (i = 0; i < 4; i++) {
      if ((param->gen_tdd_clk_freq_hz % out_freq[i]) != 0) {
        param->gen_tdd_clk_freq_hz = 0;
        break;
      }
    }
  }

  if (param->gen_tdd_clk_freq_hz == 0) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "unable to find a suitable TDD clock frequency based on given parameters.\n",
      __FILE__, __LINE__, peripheral_id);
    return(-3);
  }

  param->gen_gpio_clk_freq_hz = param->gen_tdd_clk_freq_hz;

  // device, tdd, gpio and ssi clocks
 
  out_freq[0] = dev_freq;
  out_freq[1] = param->gen_tdd_clk_freq_hz;
  out_freq[2] = param->gen_gpio_clk_freq_hz;

  // pick a vco frequency between 600Mhz and 1440MHz, divisor less than 64

  vco_freq = 0;
  vco_freq_alt = 0;
  vco_freq_diff = (uint32_t) -1;
  vco_out_div = (uint32_t) (600000000/param->ref_clk_freq_hz);
  while (vco_freq == 0) {
    vco_out_div = vco_out_div + 1;
    vco_freq = vco_out_div * param->ref_clk_freq_hz;
    if ((vco_freq > 1440000000) || (vco_out_div > 64)) {
      vco_freq = 0;
      break;
    }
    vco_out_diff = 0;
    for (i = 0; i < 3; i++) {
      vco_out_diff = vco_out_diff + (vco_freq % out_freq[i]);
    }
    if (vco_freq_diff > vco_out_diff) {
      vco_freq_diff = vco_out_diff;
      vco_freq_alt = vco_freq;
    }
    if (vco_out_diff != 0) {
      vco_freq = 0;
    }
  }

  if (vco_freq == 0) {
    vco_freq = vco_freq_alt;
    for (i = 0; i < 3; i++) {
      out_div = (uint32_t) (vco_freq_alt / out_freq[i]);
      out_div = (uint32_t) (vco_freq_alt / out_div);
      if (out_div != out_freq[i]) {
        axi_platform_log(device, AXI_PLATFORM_WARNING, "%s clock frequency "
          "%dHz (requested %dHz).\n", ((i == 0) ? "device" :
          ((i == 1) ? "TDD" : "GPIO")), out_div, out_freq[i]);
        out_freq[i] = out_div;
      }
    }
  }

  axi_adrv9001_clk_reset_set(device, peripheral_id, 0x1);
  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    AXI_ADRV9001_MMCM_POWER_ID, 0x0000, 0xffff);
  axi_adrv9001_mmcm_read_modify_write(device, peripheral_id,
    AXI_ADRV9001_MMCM_REF_DIV_ID, 0xfeff, 0x0100);
  axi_adrv9001_mmcm_set_div(device, peripheral_id,
    AXI_ADRV9001_MMCM_VCO_DIV_ID, vco_out_div);
  axi_adrv9001_mmcm_set_lock(device, peripheral_id, vco_out_div);
  axi_adrv9001_mmcm_set_filter(device, peripheral_id, vco_out_div);
  for (i = 0; i < 3; i++) {
    out_div = (uint32_t) (vco_freq/out_freq[i]);
    if (out_div > 128) out_div = 128;
    axi_adrv9001_mmcm_set_div(device, peripheral_id,
      (AXI_ADRV9001_MMCM_OUT_DIV_ID + (i * 2)), out_div);
  }
  axi_adrv9001_clk_reset_set(device, peripheral_id, 0x0);

  *locked = 0;
  axi_adrv9001_timer_set(device, peripheral_id, (uint32_t) -1);
  while (axi_adrv9001_clk_status(device, peripheral_id) == 0) {
    if (axi_adrv9001_timer_get(device, peripheral_id) == 0) {
      axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
        "MMCM is NOT locked.\n", __FILE__, __LINE__, peripheral_id);
      return(-5);
    }
  }

  *locked = 1;
  return(0);
}
 
uint32_t axi_adrv9001_mmcm_ref_clk_freq_get(void *device, uint32_t peripheral_id) {

  return(axi_adrv9001_clk_freq_get(device, peripheral_id,
    AXI_ADRV9001_MMCM_REF_CLK_MON_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// External trigger functions

void axi_adrv9001_trig_out_enable_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TRIG_EXT_CTL_ADDR, data);
}

uint32_t axi_adrv9001_trig_out_enable_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_TRIG_EXT_CTL_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// GPIO functions

void axi_adrv9001_gpio_timer_set(void *device, uint32_t peripheral_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_GPIO_TIMER_ADDR, data);
}

uint32_t axi_adrv9001_gpio_timer_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_GPIO_TIMER_ADDR));
}

void axi_adrv9001_gpio_delay_us(void *device, uint32_t peripheral_id, uint32_t delay) {

  axi_adrv9001_gpio_timer_set(device, peripheral_id, (delay * 100));
  while (axi_adrv9001_gpio_timer_get(device, peripheral_id) != 0);
}

uint32_t axi_adrv9001_gpio_group_base_get(void *device, uint32_t peripheral_id, uint32_t group) {

  if (group == AXI_ADRV9001_GPIO_PIO_ID) {
    return(AXI_ADRV9001_GPIO_PIO_DATA_ADDR);
  }

  if (group == AXI_ADRV9001_GPIO_RX0_PIO_ID) {
    return(AXI_ADRV9001_GPIO_RX0_PIO_DATA_ADDR);
  }

  if (group == AXI_ADRV9001_GPIO_RX1_PIO_ID) {
    return(AXI_ADRV9001_GPIO_RX1_PIO_DATA_ADDR);
  }

  if (group == AXI_ADRV9001_GPIO_TX0_PIO_ID) {
    return(AXI_ADRV9001_GPIO_TX0_PIO_DATA_ADDR);
  }

  if (group == AXI_ADRV9001_GPIO_TX1_PIO_ID) {
    return(AXI_ADRV9001_GPIO_TX1_PIO_DATA_ADDR);
  }

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
    "invalid GPIO group[0x%x].\n", __FILE__, __LINE__, peripheral_id, group);
  return((uint32_t) -1);
}

void axi_adrv9001_gpio_pin_set(void *device, uint32_t peripheral_id, uint32_t offset,
  uint32_t group, uint32_t pin, uint32_t data) {

  uint32_t base;
  uint32_t reg_data;

  base = axi_adrv9001_gpio_group_base_get(device, peripheral_id, group);
  reg_data = axi_reg_read(device, peripheral_id, (base + offset));
  reg_data = reg_data & ~(AXI_ADRV9001_GPIO_DATA_SET(0x1, pin));
  reg_data = reg_data | AXI_ADRV9001_GPIO_DATA_SET(data, pin);
  axi_reg_write(device, peripheral_id, (base + offset), reg_data);
}

uint32_t axi_adrv9001_gpio_pin_get(void *device, uint32_t peripheral_id, uint32_t offset,
  uint32_t group, uint32_t pin) {

  uint32_t base;
  uint32_t reg_data;

  base = axi_adrv9001_gpio_group_base_get(device, peripheral_id, group);
  reg_data = axi_reg_read(device, peripheral_id, (base + offset));
  return(AXI_ADRV9001_GPIO_DATA_GET(reg_data, pin));
}

void axi_adrv9001_gpio_pin_mode_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t mode) {

  axi_adrv9001_gpio_pin_set(device, peripheral_id, AXI_ADRV9001_GPIO_DATA_OUT_MODE_OFFSET,
    group, pin, mode);
}

uint32_t axi_adrv9001_gpio_pin_mode_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin) {

  return(axi_adrv9001_gpio_pin_get(device, peripheral_id,
    AXI_ADRV9001_GPIO_DATA_OUT_MODE_OFFSET, group, pin));
}

void axi_adrv9001_gpio_pin_direction_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t direction) {

  axi_adrv9001_gpio_pin_set(device, peripheral_id, AXI_ADRV9001_GPIO_DATA_OUT_ENABLE_OFFSET,
    group, pin, direction);
}

uint32_t axi_adrv9001_gpio_pin_direction_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin) {

  return(axi_adrv9001_gpio_pin_get(device, peripheral_id,
    AXI_ADRV9001_GPIO_DATA_OUT_ENABLE_OFFSET, group, pin));
}

void axi_adrv9001_gpio_group_data_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t data) {

  uint32_t base;

  base = axi_adrv9001_gpio_group_base_get(device, peripheral_id, group);
  axi_reg_write(device, peripheral_id, (base + AXI_ADRV9001_GPIO_DATA_OUT_OFFSET), data);
}

uint32_t axi_adrv9001_gpio_group_data_get(void *device, uint32_t peripheral_id, uint32_t group) {

  uint32_t base;

  base = axi_adrv9001_gpio_group_base_get(device, peripheral_id, group);
  return(axi_reg_read(device, peripheral_id, (base + AXI_ADRV9001_GPIO_DATA_IN_OFFSET)));
}

void axi_adrv9001_gpio_pin_data_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t data) {

  axi_adrv9001_gpio_pin_set(device, peripheral_id, AXI_ADRV9001_GPIO_DATA_OUT_OFFSET,
    group, pin, data);
}

uint32_t axi_adrv9001_gpio_pin_data_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin) {

  return(axi_adrv9001_gpio_pin_get(device, peripheral_id,
    AXI_ADRV9001_GPIO_DATA_IN_OFFSET, group, pin));
}

void axi_adrv9001_gpio_pin_source_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t source) {

  uint32_t base;

  base = axi_adrv9001_gpio_group_base_get(device, peripheral_id, group);
  axi_reg_write(device, peripheral_id, (base + AXI_ADRV9001_GPIO_DATA_OUT_SEL_OFFSET + (pin * 4)), source);
}

uint32_t axi_adrv9001_gpio_pin_source_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin) {

  uint32_t base;

  base = axi_adrv9001_gpio_group_base_get(device, peripheral_id, group);
  return(axi_reg_read(device, peripheral_id, (base + AXI_ADRV9001_GPIO_DATA_OUT_SEL_OFFSET + (pin * 4))));
}

void axi_adrv9001_gpio_trig_sel(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_GPIO_TRIG_SRC_GROUP_ADDR, group);
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_GPIO_TRIG_SRC_PIN_ADDR, pin);
}

void axi_adrv9001_gpio_gainindex_sel(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t* pins) {

  int32_t i;

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_GPIO_GAININDEX_SRC_GROUP_ADDR, group);
  for (i = 0; i < 8; i++) {
    axi_reg_write(device, peripheral_id, (AXI_ADRV9001_GPIO_GAININDEX_SRC_PIN_ADDR +
      (i * 4)), *(pins + i));
  }
}

uint32_t axi_adrv9001_gpio_clk_freq_get(void *device, uint32_t peripheral_id) {

  return(axi_adrv9001_clk_freq_get(device, peripheral_id,
    AXI_ADRV9001_GPIO_CLK_MON_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// SSI mode functions

uint32_t axi_adrv9001_ssi_type_get(void *device, uint32_t peripheral_id, uint32_t ssi_id) {

  return(axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_CMOS_LVDS_ID_ADDR)));
}

int32_t axi_adrv9001_ssi_type_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t ssi_type) {

  if (axi_adrv9001_ssi_type_get(device, peripheral_id, ssi_id) == ssi_type) {
    return(0);
  }

  axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
    "SSI-ID[0x%x]: run-time configuration of SSI is NOT supported.\n",
    __FILE__, __LINE__, peripheral_id, ssi_id);
  return(-1);
}

int32_t axi_adrv9001_ssi_init_param(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_ssi_params *param, uint32_t ssi_mode) {

  if (ssi_mode >= SSI_MODE_UNDEFINED) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "ssi mode [0x%x] is NOT supported.\n", __FILE__, __LINE__, peripheral_id, ssi_mode);
    return(-1);
  }

  param->gpio1_ssi0 = 0;
  param->tx_clk_sel = 0;
  param->tx_clk_msb = 0;
  param->tx_buf_enable = 1;
  param->sdr1_ddr0 = 0;
  param->lsb1_msb0 = 0;
  param->q1_i0 = 0;
  param->fall1_rise0 = 0;
  param->rx_dec_ratio = 0;
  param->orx_dec_ratio = 0;

  param->cmos1_lvds0 = 1;
  if (ssi_mode == LVDS_1L_PS_24X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_1L_LS_24X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_1L_PS_32X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_1L_LS_32X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_1L_PS_64X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_1L_LS_64X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_2L_PS_24X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_2L_PS_32X1) param->cmos1_lvds0 = 0;
  if (ssi_mode == LVDS_2L_PS_64X1) param->cmos1_lvds0 = 0;

  param->lstrobe1_pstrobe0 = 0;
  if (ssi_mode == CMOS_1L_LS_32X1) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_32X2) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_32X4) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_32X8) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_64X1) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_64X2) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_64X4) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == CMOS_1L_LS_64X8) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == LVDS_1L_LS_24X1) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == LVDS_1L_LS_32X1) param->lstrobe1_pstrobe0 = 1;
  if (ssi_mode == LVDS_1L_LS_64X1) param->lstrobe1_pstrobe0 = 1;

  param->mlane1_slane0 = 0;
  if (ssi_mode == CMOS_4L_PS_32X1) param->mlane1_slane0 = 1;
  if (ssi_mode == CMOS_4L_PS_64X1) param->mlane1_slane0 = 1;
  if (ssi_mode == LVDS_2L_PS_24X1) param->mlane1_slane0 = 1;
  if (ssi_mode == LVDS_2L_PS_32X1) param->mlane1_slane0 = 1;
  if (ssi_mode == LVDS_2L_PS_64X1) param->mlane1_slane0 = 1;

  param->rx_clk_rate = 0;
  if (ssi_mode == CMOS_1L_PS_02X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_PS_08X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_PS_16X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_PS_32X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_LS_32X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_PS_64X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_LS_64X2) param->rx_clk_rate = 1;
  if (ssi_mode == CMOS_1L_PS_02X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_PS_08X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_PS_16X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_PS_32X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_LS_32X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_PS_64X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_LS_64X4) param->rx_clk_rate = 2;
  if (ssi_mode == CMOS_1L_PS_02X8) param->rx_clk_rate = 3;
  if (ssi_mode == CMOS_1L_PS_08X8) param->rx_clk_rate = 3;
  if (ssi_mode == CMOS_1L_PS_16X8) param->rx_clk_rate = 3;
  if (ssi_mode == CMOS_1L_PS_32X8) param->rx_clk_rate = 3;
  if (ssi_mode == CMOS_1L_LS_32X8) param->rx_clk_rate = 3;
  if (ssi_mode == CMOS_1L_PS_64X8) param->rx_clk_rate = 3;
  if (ssi_mode == CMOS_1L_LS_64X8) param->rx_clk_rate = 3;

  param->num_of_bits = 4;
  if (ssi_mode == CMOS_1L_PS_02X1) param->num_of_bits = 0;
  if (ssi_mode == CMOS_1L_PS_02X2) param->num_of_bits = 0;
  if (ssi_mode == CMOS_1L_PS_02X4) param->num_of_bits = 0;
  if (ssi_mode == CMOS_1L_PS_02X8) param->num_of_bits = 0;
  if (ssi_mode == CMOS_1L_PS_08X1) param->num_of_bits = 1;
  if (ssi_mode == CMOS_1L_PS_08X2) param->num_of_bits = 1;
  if (ssi_mode == CMOS_1L_PS_08X4) param->num_of_bits = 1;
  if (ssi_mode == CMOS_1L_PS_08X8) param->num_of_bits = 1;
  if (ssi_mode == CMOS_1L_PS_16X1) param->num_of_bits = 2;
  if (ssi_mode == CMOS_1L_PS_16X2) param->num_of_bits = 2;
  if (ssi_mode == CMOS_1L_PS_16X4) param->num_of_bits = 2;
  if (ssi_mode == CMOS_1L_PS_16X8) param->num_of_bits = 2;
  if (ssi_mode == LVDS_1L_PS_24X1) param->num_of_bits = 3;
  if (ssi_mode == LVDS_1L_LS_24X1) param->num_of_bits = 3;
  if (ssi_mode == LVDS_2L_PS_24X1) param->num_of_bits = 3;
  if (ssi_mode == CMOS_1L_PS_64X1) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_PS_64X2) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_PS_64X4) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_PS_64X8) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_LS_64X1) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_LS_64X2) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_LS_64X4) param->num_of_bits = 5;
  if (ssi_mode == CMOS_1L_LS_64X8) param->num_of_bits = 5;
  if (ssi_mode == CMOS_4L_PS_64X1) param->num_of_bits = 5;
  if (ssi_mode == LVDS_1L_PS_64X1) param->num_of_bits = 5;
  if (ssi_mode == LVDS_1L_LS_64X1) param->num_of_bits = 5;
  if (ssi_mode == LVDS_2L_PS_64X1) param->num_of_bits = 5;

  return(0);
}

int32_t axi_adrv9001_ssi_config(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, struct axi_adrv9001_ssi_params *param) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR));
  if ((axi_adrv9001_ssi_type_get(device, peripheral_id, ssi_id) == AXI_ADRV9001_SSI_GPIO) ||
    (AXI_ADRV9001_SSI_CMOS1_LVDS0_GET(data) != param->cmos1_lvds0)) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "SSI-ID[0x%x]: incompatible ssi type, hardware reconfiguration is required.\n",
      __FILE__, __LINE__, peripheral_id, ssi_id);
    return(-1);
  }

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_CONTROL_ADDR),
    (AXI_ADRV9001_SSI_CLK_RATE_SET(param->rx_clk_rate) |
    AXI_ADRV9001_SSI_NUM_OF_BITS_SET(param->num_of_bits) |
    AXI_ADRV9001_SSI_LSTRB1_PSTRB0_SET(param->lstrobe1_pstrobe0) |
    AXI_ADRV9001_SSI_MLANE1_SLANE0_SET(param->mlane1_slane0) |
    AXI_ADRV9001_SSI_SDR1_DDR0_SET(param->sdr1_ddr0) |
    AXI_ADRV9001_SSI_LSB1_MSB0_SET(param->lsb1_msb0) |
    AXI_ADRV9001_SSI_Q1_I0_SET(param->q1_i0) |
    AXI_ADRV9001_SSI_FALL1_RISE0_SET(param->fall1_rise0)));

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_IO_CONTROL_ADDR),
    (AXI_ADRV9001_SSI_BUF_ENABLE_SET(param->tx_buf_enable) |
    AXI_ADRV9001_SSI_CLK_SEL_SET(param->tx_clk_sel) |
    AXI_ADRV9001_SSI_CLK_MSB_SET(param->tx_clk_msb)));

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR),
    (AXI_ADRV9001_SSI_RX_DEC_RATIO_SET(param->rx_dec_ratio) |
    AXI_ADRV9001_SSI_ORX_DEC_RATIO_SET(param->orx_dec_ratio)));

  return(0);
}

void axi_adrv9001_ssi_tx_buf_enable_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  uint32_t reg_data;

  reg_data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_IO_CONTROL_ADDR));
  reg_data = reg_data & ~(AXI_ADRV9001_SSI_BUF_ENABLE_SET(0x1));
  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_IO_CONTROL_ADDR),
    (reg_data | AXI_ADRV9001_SSI_BUF_ENABLE_SET(data)));
}

uint32_t axi_adrv9001_ssi_tx_buf_enable_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_IO_CONTROL_ADDR));
  return(AXI_ADRV9001_SSI_BUF_ENABLE_GET(data));
}

uint32_t axi_adrv9001_ssi_rx_dec_ratio_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR));
  return(AXI_ADRV9001_SSI_RX_DEC_RATIO_GET(data));
}

uint32_t axi_adrv9001_ssi_orx_dec_ratio_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR));
  return(AXI_ADRV9001_SSI_ORX_DEC_RATIO_GET(data));
}

void axi_adrv9001_ssi_rx_dec_ratio_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  uint32_t reg_data;

  reg_data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR));
  reg_data = AXI_ADRV9001_SSI_ORX_DEC_RATIO_GET(reg_data);
  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR),
    (AXI_ADRV9001_SSI_RX_DEC_RATIO_SET(data) | AXI_ADRV9001_SSI_ORX_DEC_RATIO_SET(reg_data)));
}

void axi_adrv9001_ssi_orx_dec_ratio_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  uint32_t reg_data;

  reg_data = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR));
  reg_data = AXI_ADRV9001_SSI_RX_DEC_RATIO_GET(reg_data);
  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RX_ORX_DEC_CONTROL_ADDR),
    (AXI_ADRV9001_SSI_RX_DEC_RATIO_SET(reg_data) | AXI_ADRV9001_SSI_ORX_DEC_RATIO_SET(data)));
}

uint32_t axi_adrv9001_ssi_init_get(void *device, uint32_t peripheral_id, uint32_t ssi_id) {

  return(axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_INIT_ADDR)));
}

void axi_adrv9001_ssi_init_set(void *device, uint32_t peripheral_id, uint32_t ssi_id) {

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_INIT_ADDR), 0x1);
}

// **********************************************************************************
// **********************************************************************************
// SSI data functions

void axi_adrv9001_ssi_data_sel_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t sel) {

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_SEL_ADDR), sel);
}

uint32_t axi_adrv9001_ssi_data_sel_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  return(axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_SEL_ADDR)));
}

void axi_adrv9001_ssi_data_pattern_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t *pattern) {

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_PAT_ADDR),
    *(pattern + 0));
  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_PAT1_ADDR),
    *(pattern + 1));
}

void axi_adrv9001_ssi_data_pattern_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t *pattern) {

  *(pattern + 0) = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_PAT_ADDR));
  *(pattern + 1) = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_PAT1_ADDR));
}

void axi_adrv9001_ssi_data_received_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t *data) {

  *(data + 0) = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_RECV_ADDR));
  *(data + 1) = axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DATA_RECV1_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// SSI status functions

uint32_t axi_adrv9001_ssi_status_get(void *device, uint32_t peripheral_id, uint32_t ssi_id) {

  return(axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_STATUS_ADDR)));
}

void axi_adrv9001_ssi_status_clear(void *device, uint32_t peripheral_id, uint32_t ssi_id) {

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_STATUS_ADDR), (uint32_t) -1);
}

// **********************************************************************************
// **********************************************************************************
// SSI delay functions

uint32_t axi_adrv9001_ssi_delay_status(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  return(axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_DELAY_LOCKED_ADDR)));
}

uint32_t axi_adrv9001_ssi_delay_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, (ssi_id + (pin * 4) +
    AXI_ADRV9001_SSI_DELAY_CONTROL_ADDR));
  return(AXI_ADRV9001_SSI_DELAY_GET(data));
}

void axi_adrv9001_ssi_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin, uint32_t data) {

  axi_reg_write(device, peripheral_id, (ssi_id + (pin * 4) +
    AXI_ADRV9001_SSI_DELAY_CONTROL_ADDR), data);
}

void axi_adrv9001_ssi_data_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  axi_adrv9001_ssi_delay_set(device, peripheral_id, ssi_id, 0, data);
  axi_adrv9001_ssi_delay_set(device, peripheral_id, ssi_id, 1, data);
  axi_adrv9001_ssi_delay_set(device, peripheral_id, ssi_id, 2, data);
  axi_adrv9001_ssi_delay_set(device, peripheral_id, ssi_id, 3, data);
  axi_adrv9001_ssi_delay_set(device, peripheral_id, ssi_id, 4, data);
}

void axi_adrv9001_ssi_clk_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  axi_adrv9001_ssi_delay_set(device, peripheral_id, ssi_id, 5, data);
}

uint32_t axi_adrv9001_ssi_in_delay_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, (ssi_id + (pin * 4) +
    AXI_ADRV9001_SSI_IN_DELAY_CONTROL_ADDR));
  return(AXI_ADRV9001_SSI_DELAY_GET(data));
}

void axi_adrv9001_ssi_in_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin, uint32_t data) {

  axi_reg_write(device, peripheral_id, (ssi_id + (pin * 4) +
    AXI_ADRV9001_SSI_IN_DELAY_CONTROL_ADDR), data);
}

void axi_adrv9001_ssi_in_int_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  axi_adrv9001_ssi_in_delay_set(device, peripheral_id, ssi_id, 0, data);
}

void axi_adrv9001_ssi_in_ext_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  axi_adrv9001_ssi_in_delay_set(device, peripheral_id, ssi_id, 1, data);
}

// **********************************************************************************
// **********************************************************************************
// SSI timer, clock monitor functions

void axi_adrv9001_ssi_timer_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data) {

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_TIMER_ADDR), data);
}

uint32_t axi_adrv9001_ssi_timer_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  return(axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_TIMER_ADDR)));
}

void axi_adrv9001_ssi_delay_us(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t delay) {

  axi_adrv9001_ssi_timer_set(device, peripheral_id, ssi_id, (delay * 100));
  while ((axi_adrv9001_ssi_timer_get(device, peripheral_id, ssi_id)) != 0);
}

uint32_t axi_adrv9001_ssi_clk_freq_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id) {

  return(axi_adrv9001_clk_freq_get(device, peripheral_id,
    (ssi_id + AXI_ADRV9001_SSI_CLK_MON_ADDR)));
}

void axi_adrv9001_ssi_reset(void *device, uint32_t peripheral_id, uint32_t ssi_id) {

  axi_reg_write(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RESET_ADDR), 0x1);
  while (axi_reg_read(device, peripheral_id, (ssi_id + AXI_ADRV9001_SSI_RESET_ADDR)) == 0x1);
}

// **********************************************************************************
// **********************************************************************************
// SSI delay calibration

int32_t axi_adrv9001_ssi_delay_calibrate(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t verbose, uint32_t *delay) {

  int32_t d;
  int32_t cur_start_delay;
  int32_t cur_stop_delay;
  int32_t cur_range;
  uint32_t ssi_rx_id;
  uint32_t status;

  *delay = (uint32_t) -1;
  cur_start_delay = -1;
  cur_stop_delay = 0;
  cur_range = -1;

  ssi_rx_id = ssi_id;

  if (ssi_id == AXI_ADRV9001_SSI_TX0_ID) {
    ssi_rx_id = AXI_ADRV9001_SSI_RX0_ID;
  }

  if (ssi_id == AXI_ADRV9001_SSI_TX1_ID) {
    ssi_rx_id = AXI_ADRV9001_SSI_RX1_ID;
  }

  if (axi_adrv9001_ssi_delay_status(device, peripheral_id, ssi_id) == 0) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "SSI-ID[0x%x]: delay controller is NOT locked, can not continue.\n",
      __FILE__, __LINE__, peripheral_id, ssi_id);
    return(-2);
  }

  if (verbose == 1) {
    axi_platform_log(device, AXI_PLATFORM_INFO, "AXI-ADRV9001-ID[0x%x]: "
        "SSI-ID[0x%x]: DELAY-SCAN:\n", peripheral_id, ssi_id);
    axi_platform_log(device, AXI_PLATFORM_NOTE, "      ");
    for (d = 0; d < 0x20; d++) {
      axi_platform_log(device, AXI_PLATFORM_NOTE, "-%02d", d);
    }
    axi_platform_log(device, AXI_PLATFORM_NOTE, "\n");
    axi_platform_log(device, AXI_PLATFORM_NOTE, "      ");
  }

  for (d = 0; d < 0x20; d++) {

    if (ssi_rx_id == ssi_id) {
      axi_adrv9001_ssi_data_delay_set(device, peripheral_id, ssi_id, (uint32_t) d);
    } else {
      axi_adrv9001_ssi_in_ext_delay_set(device, peripheral_id, ssi_id, (uint32_t) d);
    }

    axi_adrv9001_ssi_delay_us(device, peripheral_id, ssi_rx_id, 1000);
    axi_adrv9001_ssi_status_clear(device, peripheral_id, ssi_rx_id);
    axi_adrv9001_ssi_timer_set(device, peripheral_id, ssi_rx_id, 10000);

    while (axi_adrv9001_ssi_timer_get(device, peripheral_id, ssi_rx_id) != 0) {
      status = axi_adrv9001_ssi_status_get(device, peripheral_id, ssi_rx_id);
      if (status != 0) break;
    }

    if (verbose == 1) {
      axi_platform_log(device, AXI_PLATFORM_NOTE, "--%x", status);
    }

    if ((status != 0x0) || (d == 0x1f)) {
      if (cur_start_delay >= 0) {
        cur_stop_delay = d;
        if ((cur_stop_delay - cur_start_delay) > cur_range) {
          *delay = (cur_stop_delay + cur_start_delay)/2;
          cur_range = cur_stop_delay - cur_start_delay;
        }
        cur_start_delay = -1;
      }
    } else if (status == 0x0) {
      if (cur_start_delay < 0) {
        cur_start_delay = d;
        cur_stop_delay = d;
      }
    }
  }

  if (verbose == 1) {
    axi_platform_log(device, AXI_PLATFORM_NOTE, "\n");
  }

  if (cur_range < 0) {
    axi_platform_log(device, AXI_PLATFORM_ERROR, "%s:%d: AXI-ADRV9001-ID[0x%x]: "
      "SSI-ID[0x%x]: calibration failed, no suitable delay found.\n",
      __FILE__, __LINE__, peripheral_id, ssi_id);
    return(-3);
  }

  if (ssi_rx_id == ssi_id) {
    axi_adrv9001_ssi_data_delay_set(device, peripheral_id, ssi_id, *delay);
  } else {
    axi_adrv9001_ssi_in_ext_delay_set(device, peripheral_id, ssi_id, *delay);
  }

  return(0);
}

// **********************************************************************************
// **********************************************************************************
// TDD functions

void axi_adrv9001_tdd_frame_config_set(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_tdd_frame_params *param) {

  uint32_t addr;

  addr = AXI_ADRV9001_TDD_FRAME_PERIOD_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->period);
  addr = AXI_ADRV9001_TDD_NUM_OF_FRAMES_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->num_of_frames);
  addr = AXI_ADRV9001_TDD_FRAME_SWITCH_PERIOD_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->frame_switch_period);
  addr = AXI_ADRV9001_TDD_FRAME_SWITCH_NUMBER_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->frame_switch_num);
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TDD_TRIGGER_MODE_ADDR,
    (AXI_ADRV9001_TDD_TRIGGER_MODE_SET(0, param->trigger_mode[0]) |
    AXI_ADRV9001_TDD_TRIGGER_MODE_SET(1, param->trigger_mode[1]) |
    AXI_ADRV9001_TDD_TRIGGER_MODE_SET(2, param->trigger_mode[2]) |
    AXI_ADRV9001_TDD_TRIGGER_MODE_SET(3, param->trigger_mode[3])));
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR,
    (AXI_ADRV9001_TDD_SEQUENCE_REPEAT_SET(param->continous) |
    AXI_ADRV9001_TDD_FRAME_SWITCH_ENABLE_SET(param->frame_switch_enable)));
}

void axi_adrv9001_tdd_frame_config_get(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_tdd_frame_params *param) {

  uint32_t addr;
  uint32_t data;

  addr = AXI_ADRV9001_TDD_FRAME_PERIOD_ADDR;
  param->period = axi_reg_read(device, peripheral_id, addr);
  addr = AXI_ADRV9001_TDD_NUM_OF_FRAMES_ADDR;
  param->num_of_frames = axi_reg_read(device, peripheral_id, addr);
  addr = AXI_ADRV9001_TDD_FRAME_SWITCH_PERIOD_ADDR;
  param->frame_switch_period = axi_reg_read(device, peripheral_id, addr);
  addr = AXI_ADRV9001_TDD_FRAME_SWITCH_NUMBER_ADDR;
  param->frame_switch_num = axi_reg_read(device, peripheral_id, addr);
  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_TDD_TRIGGER_MODE_ADDR);
  param->trigger_mode[0] = AXI_ADRV9001_TDD_TRIGGER_MODE_GET(0, data);
  param->trigger_mode[1] = AXI_ADRV9001_TDD_TRIGGER_MODE_GET(1, data);
  param->trigger_mode[2] = AXI_ADRV9001_TDD_TRIGGER_MODE_GET(2, data);
  param->trigger_mode[3] = AXI_ADRV9001_TDD_TRIGGER_MODE_GET(3, data);
  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR);
  param->continous = AXI_ADRV9001_TDD_SEQUENCE_REPEAT_GET(data);
  param->frame_switch_enable = AXI_ADRV9001_TDD_FRAME_SWITCH_ENABLE_GET(data);
}

void axi_adrv9001_tdd_start(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR);
  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR,
    (data | AXI_ADRV9001_TDD_START_SET(0x1)));
}

void axi_adrv9001_tdd_stop(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR);
  if (AXI_ADRV9001_TDD_START_GET(data) == 0x1) {
    axi_reg_write(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR,
      (data | AXI_ADRV9001_TDD_STOP_SET(0x1)));
  }
}

void axi_adrv9001_tdd_reset(void *device, uint32_t peripheral_id) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TDD_SWRESET_ADDR, 0x1);
}

uint32_t axi_adrv9001_tdd_status(void *device, uint32_t peripheral_id) {

  uint32_t data;

  data = axi_reg_read(device, peripheral_id, AXI_ADRV9001_TDD_SEQUENCE_CONTROL_ADDR);
  return(AXI_ADRV9001_TDD_START_GET(data));
}

void axi_adrv9001_tdd_timer_set(void *device, uint32_t peripheral_id,
  uint32_t data) {

  axi_reg_write(device, peripheral_id, AXI_ADRV9001_TDD_TIMER_ADDR, data);
}

uint32_t axi_adrv9001_tdd_timer_get(void *device, uint32_t peripheral_id) {

  return(axi_reg_read(device, peripheral_id, AXI_ADRV9001_TDD_TIMER_ADDR));
}

void axi_adrv9001_tdd_delay_us(void *device, uint32_t peripheral_id, uint32_t delay) {

  axi_adrv9001_tdd_timer_set(device, peripheral_id, (delay * 100));
  while (axi_adrv9001_tdd_timer_get(device, peripheral_id) != 0);
}

uint32_t axi_adrv9001_tdd_clk_freq_get(void *device, uint32_t peripheral_id) {

  return(axi_adrv9001_clk_freq_get(device, peripheral_id,
    AXI_ADRV9001_TDD_CLK_MON_ADDR));
}

// **********************************************************************************
// **********************************************************************************
// TDD enable functions

void axi_adrv9001_tdd_enable_config_set(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, struct axi_adrv9001_tdd_enable_params *param) {

  uint32_t addr;

  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_ASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->primary_assert);
  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_DEASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->primary_deassert);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_ASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->secondary_assert);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_DEASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->secondary_deassert);
  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_FRAME_ASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->primary_frame_assert);
  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_FRAME_DEASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->primary_frame_deassert);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_FRAME_ASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->secondary_frame_assert);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_FRAME_DEASSERT_ADDR;
  axi_reg_write(device, peripheral_id, addr, param->secondary_frame_deassert);
}

void axi_adrv9001_tdd_enable_config_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, struct axi_adrv9001_tdd_enable_params *param) {

  uint32_t addr;

  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_ASSERT_ADDR;
  param->primary_assert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_DEASSERT_ADDR;
  param->primary_deassert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_ASSERT_ADDR;
  param->secondary_assert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_DEASSERT_ADDR;
  param->secondary_deassert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_FRAME_ASSERT_ADDR;
  param->primary_frame_assert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_PRIMARY_FRAME_DEASSERT_ADDR;
  param->primary_frame_deassert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_FRAME_ASSERT_ADDR;
  param->secondary_frame_assert = axi_reg_read(device, peripheral_id, addr);
  addr = tdd_id + AXI_ADRV9001_TDD_SECONDARY_FRAME_DEASSERT_ADDR;
  param->secondary_frame_deassert = axi_reg_read(device, peripheral_id, addr);
}

void axi_adrv9001_tdd_enable_mode_set(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, uint32_t mode) {

  axi_reg_write(device, peripheral_id, (tdd_id + AXI_ADRV9001_TDD_MODE_ADDR), mode);
}

uint32_t axi_adrv9001_tdd_enable_mode_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id) {

  return(axi_reg_read(device, peripheral_id, (tdd_id + AXI_ADRV9001_TDD_MODE_ADDR)));
}

void axi_adrv9001_tdd_enable_sync_set(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, uint32_t sync) {

  axi_reg_write(device, peripheral_id, (tdd_id + AXI_ADRV9001_TDD_SYNC_ADDR), sync);
}

uint32_t axi_adrv9001_tdd_enable_sync_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id) {

  return(axi_reg_read(device, peripheral_id, (tdd_id + AXI_ADRV9001_TDD_SYNC_ADDR)));
}

uint32_t axi_adrv9001_tdd_enable_clk_ratio_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id) {

  return(axi_reg_read(device, peripheral_id, (tdd_id + AXI_ADRV9001_TDD_CLK_RATIO_ADDR)));
}

// **********************************************************************************
// **********************************************************************************
