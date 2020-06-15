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

#ifndef AXI_ADRV9001_H
#define AXI_ADRV9001_H

#include "axi_common.h"
#include "axi_adrv9001_regs.h"

// axi_adrv9001 parameters

struct axi_adrv9001_mcs_params {
  uint32_t period;
  uint32_t width;
  uint32_t count;
  uint32_t fall1_rise0;
};

struct axi_adrv9001_clk_params {
  uint32_t ref_clk_freq_hz;
  uint32_t dev_clk_freq_hz;
  uint32_t ssi_rx0_clk_freq_hz;
  uint32_t ssi_rx1_clk_freq_hz;
  uint32_t ssi_tx0_clk_freq_hz;
  uint32_t ssi_tx1_clk_freq_hz;
  uint32_t gen_tdd_clk_freq_hz;
  uint32_t gen_gpio_clk_freq_hz;
};

struct axi_adrv9001_ssi_params {
  uint32_t cmos1_lvds0;
  uint32_t gpio1_ssi0;
  uint32_t rx_clk_rate;
  uint32_t tx_clk_sel;
  uint32_t tx_clk_msb;
  uint32_t tx_buf_enable;
  uint32_t num_of_bits;
  uint32_t lstrobe1_pstrobe0;
  uint32_t mlane1_slane0;
  uint32_t sdr1_ddr0;
  uint32_t lsb1_msb0;
  uint32_t q1_i0;
  uint32_t fall1_rise0;
  uint32_t rx_dec_ratio;
  uint32_t orx_dec_ratio;
};

struct axi_adrv9001_tdd_frame_params {
  uint32_t period;
  uint32_t num_of_frames;
  uint32_t frame_switch_period;
  uint32_t frame_switch_num;
  uint32_t frame_switch_enable;
  uint32_t continous;
  uint32_t trigger_mode[4];
};

struct axi_adrv9001_tdd_enable_params {
  uint32_t primary_assert;
  uint32_t primary_deassert;
  uint32_t secondary_assert;
  uint32_t secondary_deassert;
  uint32_t primary_frame_assert;
  uint32_t primary_frame_deassert;
  uint32_t secondary_frame_assert;
  uint32_t secondary_frame_deassert;
};

// **********************************************************************************
// **********************************************************************************

uint32_t axi_adrv9001_instance_id_get(void *device, uint32_t peripheral_id);
void axi_adrv9001_timer_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_timer_get(void *device, uint32_t peripheral_id);
void axi_adrv9001_delay_us(void *device, uint32_t peripheral_id, uint32_t delay);
uint32_t axi_adrv9001_clk_freq_get(void *device, uint32_t peripheral_id, uint32_t addr);

/* reset, mode and interrupts */

void axi_adrv9001_reset_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_reset_get(void *device, uint32_t peripheral_id);
void axi_adrv9001_mode_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_mode_get(void *device, uint32_t peripheral_id);

uint32_t axi_adrv9001_gpint_status(void *device, uint32_t peripheral_id);
void axi_adrv9001_gpint_status_clear(void *device, uint32_t peripheral_id);
void axi_adrv9001_gpint_mode_set(void *device, uint32_t peripheral_id, uint32_t data);

/* GPIO functions */

void axi_adrv9001_usr_gpio_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_usr_gpio_get(void *device, uint32_t peripheral_id);

/* MCS functions */

void axi_adrv9001_mcs_config(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_mcs_params *param);
void axi_adrv9001_mcs_start(void *device, uint32_t peripheral_id);
uint32_t axi_adrv9001_mcs_status(void *device, uint32_t peripheral_id);
void axi_adrv9001_mcs_select_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_mcs_select_get(void *device, uint32_t peripheral_id);
uint32_t axi_adrv9001_mcs_clk_freq_get(void *device, uint32_t peripheral_id);

/* MMCM functions */

uint32_t axi_adrv9001_mmcm_busy(void *device, uint32_t peripheral_id);
void axi_adrv9001_mmcm_write(void *device, uint32_t peripheral_id,
  uint32_t addr, uint32_t data);
uint32_t axi_adrv9001_mmcm_read(void *device, uint32_t peripheral_id,
  uint32_t addr);
void axi_adrv9001_mmcm_read_modify_write(void *device, uint32_t peripheral_id,
  uint32_t addr, uint32_t mask, uint32_t data);
void axi_adrv9001_mmcm_set_div(void *device, uint32_t peripheral_id,
  uint32_t addr, uint32_t div);
void axi_adrv9001_mmcm_set_lock(void *device, uint32_t peripheral_id, uint32_t div);
void axi_adrv9001_mmcm_set_filter(void *device, uint32_t peripheral_id, uint32_t div);
void axi_adrv9001_clk_reset_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_clk_status(void *device, uint32_t peripheral_id);
int32_t axi_adrv9001_clk_config(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_clk_params *param, uint32_t *locked);
uint32_t axi_adrv9001_mmcm_ref_clk_freq_get(void *device, uint32_t peripheral_id);

/* External trigger functions */

void axi_adrv9001_trig_out_enable_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_trig_out_enable_get(void *device, uint32_t peripheral_id);

/* GPIO functions */

void axi_adrv9001_gpio_timer_set(void *device, uint32_t peripheral_id, uint32_t data);
uint32_t axi_adrv9001_gpio_timer_get(void *device, uint32_t peripheral_id);
void axi_adrv9001_gpio_delay_us(void *device, uint32_t peripheral_id, uint32_t delay);
uint32_t axi_adrv9001_gpio_group_base_get(void *device, uint32_t peripheral_id, uint32_t group);
void axi_adrv9001_gpio_pin_set(void *device, uint32_t peripheral_id, uint32_t offset,
  uint32_t group, uint32_t pin, uint32_t data);
uint32_t axi_adrv9001_gpio_pin_get(void *device, uint32_t peripheral_id, uint32_t offset,
  uint32_t group, uint32_t pin);
void axi_adrv9001_gpio_pin_mode_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t mode);
uint32_t axi_adrv9001_gpio_pin_mode_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin);
void axi_adrv9001_gpio_pin_direction_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t direction);
uint32_t axi_adrv9001_gpio_pin_direction_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin);
void axi_adrv9001_gpio_group_data_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t data);
uint32_t axi_adrv9001_gpio_group_data_get(void *device, uint32_t peripheral_id, uint32_t group);
void axi_adrv9001_gpio_pin_data_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t data);
uint32_t axi_adrv9001_gpio_pin_data_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin);
void axi_adrv9001_gpio_pin_source_set(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin, uint32_t source);
uint32_t axi_adrv9001_gpio_pin_source_get(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin);
void axi_adrv9001_gpio_trig_sel(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t pin);
void axi_adrv9001_gpio_gainindex_sel(void *device, uint32_t peripheral_id, uint32_t group,
  uint32_t* pins);
uint32_t axi_adrv9001_gpio_clk_freq_get(void *device, uint32_t peripheral_id);

/* SSI mode functions */

uint32_t axi_adrv9001_ssi_type_get(void *device, uint32_t peripheral_id, uint32_t ssi_id);
int32_t axi_adrv9001_ssi_type_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t ssi_type);
int32_t axi_adrv9001_ssi_init_param(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_ssi_params *param, uint32_t ssi_mode);
int32_t axi_adrv9001_ssi_config(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, struct axi_adrv9001_ssi_params *param);
void axi_adrv9001_ssi_tx_buf_enable_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
uint32_t axi_adrv9001_ssi_tx_buf_enable_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
uint32_t axi_adrv9001_ssi_rx_dec_ratio_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
uint32_t axi_adrv9001_ssi_orx_dec_ratio_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
void axi_adrv9001_ssi_rx_dec_ratio_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
void axi_adrv9001_ssi_orx_dec_ratio_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
uint32_t axi_adrv9001_ssi_init_get(void *device, uint32_t peripheral_id, uint32_t ssi_id);
void axi_adrv9001_ssi_init_set(void *device, uint32_t peripheral_id, uint32_t ssi_id);

/* SSI data functions */

void axi_adrv9001_ssi_data_sel_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t sel);
uint32_t axi_adrv9001_ssi_data_sel_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
void axi_adrv9001_ssi_data_pattern_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t *pattern);
void axi_adrv9001_ssi_data_pattern_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t *pattern);
void axi_adrv9001_ssi_data_received_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t *data);

/* SSI status functions */

uint32_t axi_adrv9001_ssi_status_get(void *device, uint32_t peripheral_id, uint32_t ssi_id);
void axi_adrv9001_ssi_status_clear(void *device, uint32_t peripheral_id, uint32_t ssi_id);

/* SSI delay functions */

uint32_t axi_adrv9001_ssi_delay_status(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
uint32_t axi_adrv9001_ssi_delay_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin);
void axi_adrv9001_ssi_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin, uint32_t data);
void axi_adrv9001_ssi_data_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
void axi_adrv9001_ssi_clk_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
uint32_t axi_adrv9001_ssi_in_delay_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin);
void axi_adrv9001_ssi_in_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t pin, uint32_t data);
void axi_adrv9001_ssi_in_int_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
void axi_adrv9001_ssi_in_ext_delay_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);

/* SSI timer, clock monitor functions */

void axi_adrv9001_ssi_timer_set(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t data);
uint32_t axi_adrv9001_ssi_timer_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
void axi_adrv9001_ssi_delay_us(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t delay);
uint32_t axi_adrv9001_ssi_clk_freq_get(void *device, uint32_t peripheral_id,
  uint32_t ssi_id);
void axi_adrv9001_ssi_reset(void *device, uint32_t peripheral_id, uint32_t ssi_id);

/* SSI delay calibration */

int32_t axi_adrv9001_ssi_delay_calibrate(void *device, uint32_t peripheral_id,
  uint32_t ssi_id, uint32_t verbose, uint32_t *delay);

/* TDD functions */

void axi_adrv9001_tdd_frame_config_set(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_tdd_frame_params *param);
void axi_adrv9001_tdd_frame_config_get(void *device, uint32_t peripheral_id,
  struct axi_adrv9001_tdd_frame_params *param);
void axi_adrv9001_tdd_start(void *device, uint32_t peripheral_id);
void axi_adrv9001_tdd_stop(void *device, uint32_t peripheral_id);
void axi_adrv9001_tdd_reset(void *device, uint32_t peripheral_id);
uint32_t axi_adrv9001_tdd_status(void *device, uint32_t peripheral_id);
void axi_adrv9001_tdd_timer_set(void *device, uint32_t peripheral_id,
  uint32_t data);
uint32_t axi_adrv9001_tdd_timer_get(void *device, uint32_t peripheral_id);
void axi_adrv9001_tdd_delay_us(void *device, uint32_t peripheral_id, uint32_t delay);
uint32_t axi_adrv9001_tdd_clk_freq_get(void *device, uint32_t peripheral_id);

/* TDD enable functions */

void axi_adrv9001_tdd_enable_config_set(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, struct axi_adrv9001_tdd_enable_params *param);
void axi_adrv9001_tdd_enable_config_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, struct axi_adrv9001_tdd_enable_params *param);
void axi_adrv9001_tdd_enable_mode_set(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, uint32_t mode);
uint32_t axi_adrv9001_tdd_enable_mode_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id);
void axi_adrv9001_tdd_enable_sync_set(void *device, uint32_t peripheral_id,
  uint32_t tdd_id, uint32_t sync);
uint32_t axi_adrv9001_tdd_enable_sync_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id);
uint32_t axi_adrv9001_tdd_enable_clk_ratio_get(void *device, uint32_t peripheral_id,
  uint32_t tdd_id);

#endif

// **********************************************************************************
// **********************************************************************************
