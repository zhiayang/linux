/**
 * \file
 * \brief Contains private fpga9001 utility related functions
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "fpga9001_utilities.h"
#include "adi_fpga9001_datachain.h"

int32_t fpga9001_Utilities_CalculateLcm(adi_fpga9001_Device_t *fpga9001,
                                        uint32_t number1,
                                        uint32_t number2,
                                        uint32_t *lcm)
{
    uint32_t num1 = 0;
    uint32_t num2 = 0;
    uint8_t i = 0;
    
    *lcm = 0;
    
    if (number1 == 0)
    {
        ADI_ERROR_REPORT(&fpga9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         number1,
                         "Cannot take LCM with first number equal to 0");
    }
    
    if (number2 == 0)
    {
        ADI_ERROR_REPORT(&fpga9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         number2,
                         "Cannot take LCM with second number equal to 0");
    }

    if (number1 > number2)
    {
        num1 = number1;
        num2 = number2;
    }
    else
    {
        num1 = number2;
        num2 = number1;
    }

    for (i = 1; i < num2; i++)
    {
        if (((num1 * i) % num2) == 0)
        {
            *lcm = num1 * i;
            ADI_API_RETURN(fpga9001);

        }
    }

    *lcm = num1 * num2;
    ADI_API_RETURN(fpga9001);
}

uint32_t fpga9001_SsiIdGet(adi_fpga9001_Device_t *device,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return(AXI_ADRV9001_SSI_RX0_ID);
    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return(AXI_ADRV9001_SSI_RX1_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_1)) return(AXI_ADRV9001_SSI_TX0_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_2)) return(AXI_ADRV9001_SSI_TX1_ID);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return((uint32_t) -1);
}

uint32_t fpga9001_TddIdGet(adi_fpga9001_Device_t *device,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return(AXI_ADRV9001_TDD_RX0_DEVICE_ID);
    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return(AXI_ADRV9001_TDD_RX1_DEVICE_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_1)) return(AXI_ADRV9001_TDD_TX0_DEVICE_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_2)) return(AXI_ADRV9001_TDD_TX1_DEVICE_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return(AXI_ADRV9001_TDD_ORX0_DEVICE_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return(AXI_ADRV9001_TDD_ORX1_DEVICE_ID);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return((uint32_t) -1);
}

uint32_t fpga9001_TddDmaIdGet(adi_fpga9001_Device_t *device,
                              adi_common_Port_e port,
                              adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9001_TDD_RX0_DMA_ID);
    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9001_TDD_RX1_DMA_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9001_TDD_TX0_DMA_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9001_TDD_TX1_DMA_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9001_TDD_ORX0_DMA_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9001_TDD_ORX1_DMA_ID);
    

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return ((uint32_t) - 1);
}

uint32_t fpga9001_DMAIdGet(adi_fpga9001_Device_t *device,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return(AXI_DMA_RX0_ID);
    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return(AXI_DMA_RX1_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_1)) return(AXI_DMA_TX0_ID);
    if ((port == ADI_TX)  && (channel == ADI_CHANNEL_2)) return(AXI_DMA_TX1_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return(AXI_DMA_ORX0_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return(AXI_DMA_ORX1_ID);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return((uint32_t) -1);
}

uint32_t fpga9001_BufBaseAddrGet(adi_common_Port_e port,
                                 adi_common_ChannelNumber_e channel)
{
    if ((port == ADI_RX) && (channel == ADI_CHANNEL_1)) return(ADI_FPGA9001_RX1_ADDR_OFFSET);
    if ((port == ADI_RX) && (channel == ADI_CHANNEL_2)) return(ADI_FPGA9001_RX2_ADDR_OFFSET);
    if ((port == ADI_TX) && (channel == ADI_CHANNEL_1)) return(ADI_FPGA9001_TX1_ADDR_OFFSET);
    if ((port == ADI_TX) && (channel == ADI_CHANNEL_2)) return(ADI_FPGA9001_TX2_ADDR_OFFSET);

    if (channel == ADI_CHANNEL_1) return(ADI_FPGA9001_ORX1_ADDR_OFFSET);
    if (channel == ADI_CHANNEL_2) return(ADI_FPGA9001_ORX2_ADDR_OFFSET);

    return((uint32_t) -1);
}


