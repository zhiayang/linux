/**
 * \file:
 * \brief Functions to configure and control the FPGA9001 GPIO pins
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
 * Copyright 2018-2019 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_fpga9001_user.h"
#include "adi_fpga9001_gpio.h"
#include "adi_fpga9001.h"
#include "fpga9001_utilities.h"

static int32_t adi_fpga9001_gpio_Mode_SetValidate(adi_fpga9001_Device_t *device, adi_fpga9001_GpioModes_e gpioMode)
{
    ADI_RANGE_CHECK(device, gpioMode, ADI_FPGA9001_GPIO_MODE_NORMAL, ADI_FPGA9001_GPIO_MODE_JTAG);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_Mode_Set(adi_fpga9001_Device_t *device, adi_fpga9001_GpioModes_e gpioMode)
{
    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_Mode_SetValidate, device, gpioMode);
    axi_adrv9001_mode_set((void *)device, AXI_ADRV9001_ID, (uint32_t) gpioMode);
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_gpio_Mode_GetValidate(adi_fpga9001_Device_t *device, adi_fpga9001_GpioModes_e *gpioMode)
{
    ADI_NULL_PTR_RETURN(&device->common, gpioMode);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_Mode_Get(adi_fpga9001_Device_t *device, adi_fpga9001_GpioModes_e *gpioMode)
{
    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_Mode_GetValidate, device, gpioMode);
    *gpioMode = (axi_adrv9001_mode_get((void *)device, AXI_ADRV9001_ID) == 0) ?
        ADI_FPGA9001_GPIO_MODE_NORMAL : ADI_FPGA9001_GPIO_MODE_JTAG;
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_gpio_ReadValidate(adi_fpga9001_Device_t *device, uint16_t *rdData)
{
    ADI_NULL_PTR_RETURN(&device->common, rdData);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_Read(adi_fpga9001_Device_t *device, uint16_t *rdData)
{
    uint32_t data;

    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_ReadValidate, device, rdData);
    data = axi_adrv9001_gpio_group_data_get((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID);
    *rdData = (uint16_t) data;
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_gpio_pin_Configure_Validate(adi_fpga9001_Device_t *device,
                                                        adi_fpga9001_GpioPin_e pin,
                                                        adi_fpga9001_GpioSourceModes_e sourceMode)
{
    ADI_RANGE_CHECK(device, pin, ADI_FPGA9001_GPIO_00, ADI_FPGA9001_GPIO_11);
    ADI_RANGE_CHECK(device, 
                    sourceMode,
                    ADI_FPGA9001_GPIO_SOURCE_MODE_INPUT,
                    (ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL1_CONTROL2_ENABLE | ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_CONSTANT_ZERO));
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_RESERVED)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         sourceMode,
                         "Selected GPIO source mode is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_pin_Configure(adi_fpga9001_Device_t *device,
                                        adi_fpga9001_GpioPin_e pin,
                                        adi_fpga9001_GpioSourceModes_e sourceMode)
{
    uint32_t gpio_mode;
    uint32_t gpio_pin;

    if (pin == ADI_FPGA9001_GPIO_00) gpio_pin = 0;
    if (pin == ADI_FPGA9001_GPIO_01) gpio_pin = 1;
    if (pin == ADI_FPGA9001_GPIO_02) gpio_pin = 2;
    if (pin == ADI_FPGA9001_GPIO_03) gpio_pin = 3;
    if (pin == ADI_FPGA9001_GPIO_04) gpio_pin = 4;
    if (pin == ADI_FPGA9001_GPIO_05) gpio_pin = 5;
    if (pin == ADI_FPGA9001_GPIO_06) gpio_pin = 6;
    if (pin == ADI_FPGA9001_GPIO_07) gpio_pin = 7;
    if (pin == ADI_FPGA9001_GPIO_08) gpio_pin = 8;
    if (pin == ADI_FPGA9001_GPIO_09) gpio_pin = 9;
    if (pin == ADI_FPGA9001_GPIO_10) gpio_pin = 10;
    if (pin == ADI_FPGA9001_GPIO_11) gpio_pin = 11;

    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_pin_Configure_Validate, device, pin, sourceMode);

    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_INPUT) {
        axi_adrv9001_gpio_pin_mode_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_SW);
        axi_adrv9001_gpio_pin_direction_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_DISABLE);
        ADI_API_RETURN(device);
    }

    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_CONSTANT_ZERO) {
        axi_adrv9001_gpio_pin_mode_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_SW);
        axi_adrv9001_gpio_pin_direction_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_ENABLE);
        axi_adrv9001_gpio_pin_data_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, 0x0);
        ADI_API_RETURN(device);
    }

    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_CONSTANT_ONE) {
        axi_adrv9001_gpio_pin_mode_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_SW);
        axi_adrv9001_gpio_pin_direction_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_ENABLE);
        axi_adrv9001_gpio_pin_data_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
            gpio_pin, 0x1);
        ADI_API_RETURN(device);
    }

    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_INPUT_GAIN_CHANGE_INDICATOR) {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, sourceMode, "gain index change indicator NOT supported.");
        ADI_API_RETURN(device);
    }

    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_INPUT_GAIN_INDEX_DATA) {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, sourceMode, "gain index data NOT supported.");
        ADI_API_RETURN(device);
    }
    
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_TX1_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_TX0_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_RX1_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_RX0_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_ORX1_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_ORX0_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_AUX1_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_AUX0_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_TX2_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_TX1_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_RX2_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_RX1_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_ORX2_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_ORX1_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_AUX2_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_AUX1_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL0_CONTROL1_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL0_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL0_CONTROL2_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL1_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL1_CONTROL1_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL2_ENB;
    if (sourceMode == ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL1_CONTROL2_ENABLE) gpio_mode = AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL3_ENB;

    axi_adrv9001_gpio_pin_mode_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
        gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_HW);
    axi_adrv9001_gpio_pin_direction_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
        gpio_pin, AXI_ADRV9001_GPIO_PIO_DATA_OUT_ENABLE);
    axi_adrv9001_gpio_pin_source_set((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID,
        gpio_pin, gpio_mode);
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_gpio_pin_Inspect_Validate(adi_fpga9001_Device_t *device, 
                                                      adi_fpga9001_GpioPin_e pin,
                                                      adi_fpga9001_GpioSourceModes_e *sourceMode)
{
    ADI_RANGE_CHECK(device, pin, ADI_FPGA9001_GPIO_00, ADI_FPGA9001_GPIO_11);
    ADI_NULL_PTR_RETURN(&device->common, sourceMode);
    
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_pin_Inspect(adi_fpga9001_Device_t *device, 
                                      adi_fpga9001_GpioPin_e pin,
                                      adi_fpga9001_GpioSourceModes_e *sourceMode)
{
    uint32_t gpio_pin;
    uint32_t gpio_mode;
    uint32_t gpio_direction;
    uint32_t gpio_data;
    uint32_t gpio_source;

    if (pin == ADI_FPGA9001_GPIO_00) gpio_pin = 0;
    if (pin == ADI_FPGA9001_GPIO_01) gpio_pin = 1;
    if (pin == ADI_FPGA9001_GPIO_02) gpio_pin = 2;
    if (pin == ADI_FPGA9001_GPIO_03) gpio_pin = 3;
    if (pin == ADI_FPGA9001_GPIO_04) gpio_pin = 4;
    if (pin == ADI_FPGA9001_GPIO_05) gpio_pin = 5;
    if (pin == ADI_FPGA9001_GPIO_06) gpio_pin = 6;
    if (pin == ADI_FPGA9001_GPIO_07) gpio_pin = 7;
    if (pin == ADI_FPGA9001_GPIO_08) gpio_pin = 8;
    if (pin == ADI_FPGA9001_GPIO_09) gpio_pin = 9;
    if (pin == ADI_FPGA9001_GPIO_10) gpio_pin = 10;
    if (pin == ADI_FPGA9001_GPIO_11) gpio_pin = 11;

    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_pin_Inspect_Validate, device, pin, sourceMode);
    *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_RESERVED;

    gpio_mode = axi_adrv9001_gpio_pin_mode_get((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID, gpio_pin);
    gpio_direction = axi_adrv9001_gpio_pin_direction_get((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID, gpio_pin);
    gpio_data = axi_adrv9001_gpio_pin_data_get((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID, gpio_pin);
    gpio_source = axi_adrv9001_gpio_pin_source_get((void *)device, AXI_ADRV9001_ID, AXI_ADRV9001_GPIO_PIO_ID, gpio_pin);

    if ((gpio_mode == AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_HW) && (gpio_direction == AXI_ADRV9001_GPIO_PIO_DATA_OUT_ENABLE)) {
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_TX0_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_TX1_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_RX0_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_RX1_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_ORX0_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_ORX1_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_AUX0_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_AUX1_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_TX1_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_TX2_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_RX1_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_RX2_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_ORX1_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_ORX2_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_AUX1_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_AUX2_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL0_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL0_CONTROL1_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL1_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL0_CONTROL2_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL2_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL1_CONTROL1_ENABLE;
        if (gpio_source == AXI_ADRV9001_GPIO_PIO_DATA_OUT_TDD_CTL3_ENB) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_TDD_CTL1_CONTROL2_ENABLE;
        ADI_API_RETURN(device);
    }

    if ((gpio_mode == AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_SW) && (gpio_direction == AXI_ADRV9001_GPIO_PIO_DATA_OUT_DISABLE)) {
        *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_INPUT;
        ADI_API_RETURN(device);
    }

    if ((gpio_mode == AXI_ADRV9001_GPIO_PIO_DATA_OUT_MODE_SW) && (gpio_direction == AXI_ADRV9001_GPIO_PIO_DATA_OUT_ENABLE)) {
        if (gpio_data == 0) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_CONSTANT_ZERO;
        if (gpio_data == 1) *sourceMode = ADI_FPGA9001_GPIO_SOURCE_MODE_OUTPUT_CONSTANT_ONE;
        ADI_API_RETURN(device);
    }

    
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
        ADI_COMMON_ACT_ERR_CHECK_PARAM, 0, "hardware error, read invalid gpio settings.");
    ADI_API_RETURN(device);
}

static uint8_t adi_fpga9001_gpio_pin_SourceMode_Configure_Validate(adi_fpga9001_Device_t *device,
                                                                   adi_fpga9001_Gpio_PinCfg_t *gpioPinConfig,
                                                                   uint8_t length)
{
    ADI_NULL_PTR_RETURN(&device->common, gpioPinConfig);

    ADI_RANGE_CHECK(device, length, 1, ADI_FPGA9001_NUM_GPIO_PINS);

    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_pin_SourceMode_Configure(adi_fpga9001_Device_t *device, adi_fpga9001_Gpio_PinCfg_t *gpioPinConfig, uint8_t length)
{
    uint8_t i = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_pin_SourceMode_Configure_Validate, device, gpioPinConfig, length);

    for (i = 0; i < length; i++)
    {
        ADI_EXPECT(adi_fpga9001_gpio_pin_Configure, device, gpioPinConfig->gpioPins[i], gpioPinConfig->gpioSourceMode[i]);
    }

    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_gpio_pin_SourceMode_Inspect_Validate(adi_fpga9001_Device_t *device,
                                                                 adi_fpga9001_Gpio_PinCfg_t *gpioPinConfig)
{
    ADI_NULL_PTR_RETURN(&device->common, gpioPinConfig);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_gpio_pin_SourceMode_Inspect(adi_fpga9001_Device_t *device,
                                                 adi_fpga9001_Gpio_PinCfg_t *gpioPinConfig)
{
    uint8_t i = 0;

    ADI_PERFORM_VALIDATION(adi_fpga9001_gpio_pin_SourceMode_Inspect_Validate, device, gpioPinConfig);

    for (i = 0; i < ADI_FPGA9001_NUM_GPIO_PINS; i++)
    {
        ADI_EXPECT(adi_fpga9001_gpio_pin_Inspect, device, gpioPinConfig->gpioPins[i], &gpioPinConfig->gpioSourceMode[i]);
    }

    ADI_API_RETURN(device);
}
