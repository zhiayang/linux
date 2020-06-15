/**
 * \file
 * \brief Contains top level TDD fpga9001 related functions
 *
 * FPGA9001 API Version: $ADI_FPGA9001_API_VERSION$
 */

/**
 * Copyright 2019 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "fpga9001_utilities.h"
#include "adi_fpga9001_tdd.h"

/* TDD framing registers */

static int32_t fpga9001_Tdd_Frame_Set(adi_fpga9001_Device_t *device,
                                        adi_fpga9001_TddFraming_t *framing,
                                        uint32_t frameSwitchNum)
{
    struct axi_adrv9001_tdd_frame_params tdd_params;

    tdd_params.period = framing->framePeriod;
    tdd_params.num_of_frames = framing->numberFrames;

    /* api-dll is setting this only if numberFrames > 0, this needs to be fixed */
     
    tdd_params.continous = 1;

    /* A TDD frame consists of framePeriod clock cycles. The frames, can be shortened (prematurely terminated) */
    /* using the frame switch registers below. The frame is prematurely terminated, if the frame-switch-enable */
    /* bit is set, frame count matches the frame-switch-num and clock count matches the frame-switch-period. */

    /* for now ignore these, it seems the labview is confused about it */

    tdd_params.frame_switch_period = 0;
    tdd_params.frame_switch_num = 0;
    tdd_params.frame_switch_enable = 0;

    tdd_params.trigger_mode[0] = (framing->enableSyncExtTrig) ? AXI_ADRV9001_TDD_TRIGGER_RISING_EDGE :
      AXI_ADRV9001_TDD_TRIGGER_DISABLED;
    tdd_params.trigger_mode[1] = AXI_ADRV9001_TDD_TRIGGER_DISABLED;
    tdd_params.trigger_mode[2] = AXI_ADRV9001_TDD_TRIGGER_DISABLED;
    tdd_params.trigger_mode[3] = AXI_ADRV9001_TDD_TRIGGER_DISABLED;
    axi_adrv9001_tdd_frame_config_set((void *)device, AXI_ADRV9001_ID, &tdd_params);
	
    ADI_API_RETURN(device);
}

static int32_t fpga9001_Tdd_Frame_Get(adi_fpga9001_Device_t *device,
                                        adi_fpga9001_TddFraming_t *framing,
                                        uint32_t *frameSwitchNum)
{
    struct axi_adrv9001_tdd_frame_params tdd_params;

    axi_adrv9001_tdd_frame_config_get((void *)device, AXI_ADRV9001_ID, &tdd_params);
    framing->framePeriod = tdd_params.period;
    framing->numberFrames = tdd_params.num_of_frames;
    framing->repeatFrameSequence = tdd_params.continous;
    framing->frameSwitchTime = tdd_params.frame_switch_period;
    *frameSwitchNum = tdd_params.frame_switch_num;

    framing->enableSyncExtTrig = (tdd_params.trigger_mode[0] == AXI_ADRV9001_TDD_TRIGGER_RISING_EDGE) ? true : false;
    ADI_API_RETURN(device);
}

/* TDD Enable registers */

static int32_t fpga9001_Tdd_Enable_Set(adi_fpga9001_Device_t *device,
                                       uint32_t fpga9001_Tdd_Id,
                                       adi_fpga9001_TddTiming_t *timing,
                                       adi_fpga9001_TddSequence_t *sequence,
									   bool enable)
{

    struct axi_adrv9001_tdd_enable_params tdd_params;

    tdd_params.primary_assert = timing->primaryAssert;
    tdd_params.primary_deassert = timing->primaryDeassert;
    tdd_params.secondary_assert = timing->secondaryAssert;
    tdd_params.secondary_deassert = timing->secondaryDeassert;
    tdd_params.primary_frame_assert = sequence->primaryFrameAssert;
    tdd_params.primary_frame_deassert = sequence->primaryFrameDeassert;
    tdd_params.secondary_frame_assert = sequence->secondaryFrameAssert;
    tdd_params.secondary_frame_deassert = sequence->secondaryFrameDeassert;
    axi_adrv9001_tdd_enable_config_set((void *)device, AXI_ADRV9001_ID, fpga9001_Tdd_Id, &tdd_params);
	if (enable == true)
	{
		axi_adrv9001_tdd_enable_mode_set((void *)device, AXI_ADRV9001_ID, fpga9001_Tdd_Id, AXI_ADRV9001_TDD_ENABLE_AUTO);	
	}
	else
	{
		axi_adrv9001_tdd_enable_mode_set((void *)device, AXI_ADRV9001_ID, fpga9001_Tdd_Id, AXI_ADRV9001_TDD_ENABLE_LOW);
	}
    
    ADI_API_RETURN(device);
}

static int32_t fpga9001_Tdd_Enable_Get(adi_fpga9001_Device_t *device,
                                       uint32_t fpga9001_Tdd_Id,
                                       adi_fpga9001_TddTiming_t *timing,
                                       adi_fpga9001_TddSequence_t *sequence,
									   bool *enable)
{

    struct axi_adrv9001_tdd_enable_params tdd_params;

    axi_adrv9001_tdd_enable_config_get((void *)device, AXI_ADRV9001_ID, fpga9001_Tdd_Id, &tdd_params);
    timing->primaryAssert = tdd_params.primary_assert;
    timing->primaryDeassert = tdd_params.primary_deassert;
    timing->secondaryAssert = tdd_params.secondary_assert;
    timing->secondaryDeassert = tdd_params.secondary_deassert;
    sequence->primaryFrameAssert = tdd_params.primary_frame_assert;
    sequence->primaryFrameDeassert = tdd_params.primary_frame_deassert;
    sequence->secondaryFrameAssert = tdd_params.secondary_frame_assert;
    sequence->secondaryFrameDeassert = tdd_params.secondary_frame_deassert;
	uint32_t enableRead = 0;
	enableRead = axi_adrv9001_tdd_enable_mode_get((void *)device, AXI_ADRV9001_ID, fpga9001_Tdd_Id);
	if (enableRead == AXI_ADRV9001_TDD_ENABLE_AUTO)
	{
		*enable = true;
	}
	else
	{
		*enable = false;
	}
    ADI_API_RETURN(device);
}

static int32_t fpga9001_Tdd_Enable_Validate(adi_fpga9001_Device_t *device,
                                            adi_fpga9001_TddTiming_t *timing,
                                            adi_fpga9001_TddSequence_t *sequence,
                                            uint32_t framePeriod,
                                            uint32_t numberFrames)
{
    ADI_RANGE_CHECK(device, timing->primaryAssert, 1, (framePeriod - 1));
    ADI_RANGE_CHECK(device, timing->primaryDeassert, 1, (framePeriod - 1));

    /* This is not an error, this means signal is disabled */

    if (timing->primaryAssert == timing->primaryDeassert)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, timing->primaryDeassert,
            "Invalid parameter value, timing->primaryDeassert must not equal timing->primaryAssert");
    }
        
    /* Secondary event is optional */

    if (timing->secondaryAssert > 0)
    {
        /* Primary event cannot cross frame boundary if there is a secondary event */
        ADI_RANGE_CHECK(device, timing->primaryDeassert, (timing->primaryAssert + 1), (framePeriod - 1));
        
        /* Secondary event cannot overlap primary event */
        ADI_RANGE_CHECK(device, timing->secondaryAssert, (timing->primaryDeassert + 1), (framePeriod - 1));
        ADI_RANGE_CHECK(device, timing->secondaryDeassert, 1, (framePeriod - 1));
        
        if (timing->secondaryAssert == timing->secondaryDeassert)
        {
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM, timing->secondaryDeassert,
                "Invalid parameter value. timing->secondaryDeassert must not equal timing->secondaryAssert");
        }
        
        /* If secondary event crosses frame boundary */
        if (timing->secondaryDeassert < timing->secondaryAssert)
        {
            /* Secondary event cannot overlap primary event */
            if (timing->secondaryDeassert >= timing->primaryAssert)
            {
                ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM, timing->secondaryDeassert,
                    "Invalid parameter value. Secondary event cannot overlap primary event, "
                    "timing->secondaryDeassert must be less than timing->primaryAssert");
            }
        }
    }
    
    /* Only applies for a non-zero numberFrames */
    if (numberFrames > 0)
    {
        /* All assert/deasserts must be less than or equal to numberFrames */
        /* At least 1 frame must be active */
        ADI_RANGE_CHECK(device, sequence->primaryFrameAssert, 1, numberFrames);
        
        /* Deassert must not come before assert */
        ADI_RANGE_CHECK(device, sequence->primaryFrameDeassert, sequence->primaryFrameAssert, numberFrames);
        
        /* Secondary is not required */
        if (sequence->secondaryFrameAssert > 0)
        {
            /* Secondary assert must come after primary deassert */
            ADI_RANGE_CHECK(device, sequence->secondaryFrameAssert, sequence->primaryFrameDeassert + 1, numberFrames);
            
            /* Deassert must not come before assert */
            ADI_RANGE_CHECK(device, sequence->secondaryFrameDeassert, sequence->secondaryFrameAssert, numberFrames);
        }
    }
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_Tdd_ManualEnable_Validate(adi_fpga9001_Device_t *device,
                                                          adi_common_Port_e port,
                                                          adi_common_ChannelNumber_e channel)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(device);
}

/* External user functions */

/* If enable == true; pin is asserted high, else asserted low. */
/* This function also disables hardware control of TDD state machine. */

int32_t adi_fpga9001_Tdd_ManualEnable_Set(adi_fpga9001_Device_t *device,
                                          adi_common_Port_e port,
                                          adi_common_ChannelNumber_e channel,
                                          bool enable)
{
    uint32_t tdd_mode;
    uint32_t tdd_id;
    
    ADI_PERFORM_VALIDATION(adi_fpga9001_Tdd_ManualEnable_Validate, device, port, channel);

    tdd_id = fpga9001_TddIdGet(device, port, channel);
    if (tdd_id == (uint32_t) -1) return(-1);

    tdd_mode = (enable) ? AXI_ADRV9001_TDD_ENABLE_HIGH : AXI_ADRV9001_TDD_ENABLE_LOW;
    axi_adrv9001_tdd_enable_mode_set((void *)device, AXI_ADRV9001_ID, tdd_id, tdd_mode);
    
    tdd_id = fpga9001_TddDmaIdGet(device, port, channel);
    if (tdd_id == (uint32_t) - 1) return (-1);
    axi_adrv9001_tdd_enable_mode_set((void *)device, AXI_ADRV9001_ID, tdd_id, tdd_mode);
    
    ADI_API_RETURN(device);
}

static int32_t adi_fpga9001_Tdd_ManualEnable_Get_Validate(adi_fpga9001_Device_t *device,
                                                          adi_common_Port_e port,
                                                          adi_common_ChannelNumber_e channel,
                                                          bool *enable)
{
    ADI_EXPECT(adi_fpga9001_Tdd_ManualEnable_Validate, device, port, channel);
    ADI_NULL_PTR_RETURN(&device->common, enable);
    ADI_API_RETURN(device);
}

/* This function is probably useless, what is it supposed to return if enable is in auto mode? */
int32_t adi_fpga9001_Tdd_ManualEnable_Get(adi_fpga9001_Device_t *device,
                                          adi_common_Port_e port,
                                          adi_common_ChannelNumber_e channel,
                                          bool *enable)
{
    uint32_t tdd_mode;
    uint32_t tdd_id;

    ADI_PERFORM_VALIDATION(adi_fpga9001_Tdd_ManualEnable_Get_Validate, device, port, channel, enable);

    tdd_id = fpga9001_TddIdGet(device, port, channel);
    if (tdd_id == (uint32_t) -1) return(-1);

    tdd_mode = axi_adrv9001_tdd_enable_mode_get((void *)device, AXI_ADRV9001_ID, tdd_id);
    if (tdd_mode == AXI_ADRV9001_TDD_ENABLE_AUTO)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_OK, ADI_COMMON_ACT_NO_ACTION, 0,
            "Incapacitated function, TDD Enable is in HW Mode!");
        return(-1);
    }

    *enable = (tdd_mode == AXI_ADRV9001_TDD_ENABLE_HIGH) ? true : false;
    ADI_API_RETURN(device);
}

/* Why this is validating just the framing but not the enables? */
/* Also not necessarily required, and even if so, can be done by the calling party. */

static int32_t adi_fpga9001_Tdd_Configure_Validate(adi_fpga9001_Device_t *device,
                                                   adi_fpga9001_TddConfig_t *tddConfig)
{
    ADI_NULL_PTR_RETURN(&device->common, tddConfig);
    
    if (tddConfig->framing.framePeriod == 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, 
            ADI_COMMON_ACT_ERR_CHECK_PARAM, tddConfig->framing.framePeriod,
            "Invalid parameter value, tddConfig->framing.framePeriod must be greater than 0.");
    }
    
    ADI_RANGE_CHECK(device, tddConfig->framing.frameSwitchTime, 0, tddConfig->framing.framePeriod);
    
    if ((tddConfig->framing.numberFrames == 0) && (tddConfig->framing.repeatFrameSequence))
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, tddConfig->framing.repeatFrameSequence,
            "Invalid parameter value, tddConfig->framing.numberFrames must be greater than 0, if repeat sequence is set.");
    }

    /* Also validate individual enables. */
    
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->rxControls[0].pinTiming,
        &tddConfig->rxControls[0].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->txControls[0].pinTiming,
        &tddConfig->txControls[0].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->rxControls[1].pinTiming,
        &tddConfig->rxControls[1].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->txControls[1].pinTiming,
        &tddConfig->txControls[1].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->orxControls[0].pinTiming,
        &tddConfig->orxControls[0].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->orxControls[1].pinTiming,
        &tddConfig->orxControls[1].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->txGpioControls[0].pinTiming,
        &tddConfig->txGpioControls[0].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->txGpioControls[1].pinTiming,
        &tddConfig->txGpioControls[1].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->interFrameControlSignal1[0].pinTiming,
        &tddConfig->interFrameControlSignal1[0].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->interFrameControlSignal1[1].pinTiming,
        &tddConfig->interFrameControlSignal1[1].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->interFrameControlSignal2[0].pinTiming,
        &tddConfig->interFrameControlSignal2[0].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->interFrameControlSignal2[1].pinTiming,
        &tddConfig->interFrameControlSignal2[1].pinSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);

    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->rxControls[0].tollgateTiming,
        &tddConfig->rxControls[0].tollgateSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->txControls[0].tollgateTiming,
        &tddConfig->txControls[0].tollgateSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->rxControls[1].tollgateTiming,
        &tddConfig->rxControls[1].tollgateSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->txControls[1].tollgateTiming,
        &tddConfig->txControls[1].tollgateSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->orxControls[0].tollgateTiming,
        &tddConfig->orxControls[0].tollgateSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);
    fpga9001_Tdd_Enable_Validate(device,
        &tddConfig->orxControls[1].tollgateTiming,
        &tddConfig->orxControls[1].tollgateSequence,
        tddConfig->framing.framePeriod,
        tddConfig->framing.numberFrames);

    ADI_API_RETURN(device);
}

/* Set all TDD hardware mode registers. */

int32_t adi_fpga9001_Tdd_Configure(adi_fpga9001_Device_t *device, adi_fpga9001_TddConfig_t *tddConfig)
{
    uint32_t frameSwitchNum;

    ADI_PERFORM_VALIDATION(adi_fpga9001_Tdd_Configure_Validate, device, tddConfig);

    frameSwitchNum = 0;
    
    fpga9001_Tdd_Frame_Set(device, &tddConfig->framing, frameSwitchNum);

    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_RX0_DEVICE_ID,
                            &tddConfig->rxControls[0].pinTiming,
                            &tddConfig->rxControls[0].pinSequence, 
							tddConfig->rxControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_TX0_DEVICE_ID,
                            &tddConfig->txControls[0].pinTiming,
                            &tddConfig->txControls[0].pinSequence,
							tddConfig->txControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_RX1_DEVICE_ID,
                            &tddConfig->rxControls[1].pinTiming,
                            &tddConfig->rxControls[1].pinSequence,
							tddConfig->rxControls[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_TX1_DEVICE_ID,
                            &tddConfig->txControls[1].pinTiming,
                            &tddConfig->txControls[1].pinSequence,
							tddConfig->txControls[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_ORX0_DEVICE_ID,
                            &tddConfig->orxControls[0].pinTiming,
                            &tddConfig->orxControls[0].pinSequence,
							tddConfig->orxControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_ORX1_DEVICE_ID,
                            &tddConfig->orxControls[1].pinTiming,
                            &tddConfig->orxControls[1].pinSequence,
							tddConfig->orxControls[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_AUX0_DEVICE_ID,
                            &tddConfig->txGpioControls[0].pinTiming,
                            &tddConfig->txGpioControls[0].pinSequence,
							tddConfig->txGpioControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_AUX1_DEVICE_ID,
                            &tddConfig->txGpioControls[1].pinTiming,
                            &tddConfig->txGpioControls[1].pinSequence,
							tddConfig->txGpioControls[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_CTL0_DEVICE_ID,
                            &tddConfig->interFrameControlSignal1[0].pinTiming,
                            &tddConfig->interFrameControlSignal1[0].pinSequence,
							tddConfig->interFrameControlSignal1[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_CTL1_DEVICE_ID,
                            &tddConfig->interFrameControlSignal1[1].pinTiming,
                            &tddConfig->interFrameControlSignal1[1].pinSequence,
							tddConfig->interFrameControlSignal1[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_CTL2_DEVICE_ID,
                            &tddConfig->interFrameControlSignal2[0].pinTiming,
                            &tddConfig->interFrameControlSignal2[0].pinSequence,
							tddConfig->interFrameControlSignal2[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_CTL3_DEVICE_ID,
                            &tddConfig->interFrameControlSignal2[1].pinTiming,
                            &tddConfig->interFrameControlSignal2[1].pinSequence,
							tddConfig->interFrameControlSignal2[1].enable);
    
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_TRIG1_DEVICE_ID,
                            &tddConfig->smaOutput1.pinTiming,
                            &tddConfig->smaOutput1.pinSequence,
							tddConfig->smaOutput1.enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_TRIG2_DEVICE_ID,
                            &tddConfig->smaOutput2.pinTiming,
                            &tddConfig->smaOutput2.pinSequence,
							tddConfig->smaOutput2.enable);

    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_RX0_DMA_ID,
                            &tddConfig->rxControls[0].tollgateTiming,
                            &tddConfig->rxControls[0].tollgateSequence,
							tddConfig->rxControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_TX0_DMA_ID,
                            &tddConfig->txControls[0].tollgateTiming,
                            &tddConfig->txControls[0].tollgateSequence,
							tddConfig->txControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_RX1_DMA_ID,
                            &tddConfig->rxControls[1].tollgateTiming,
                            &tddConfig->rxControls[1].tollgateSequence,
							tddConfig->rxControls[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_TX1_DMA_ID,
                            &tddConfig->txControls[1].tollgateTiming,
                            &tddConfig->txControls[1].tollgateSequence,
							tddConfig->txControls[1].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_ORX0_DMA_ID,
                            &tddConfig->orxControls[0].tollgateTiming,
                            &tddConfig->orxControls[0].tollgateSequence,
							tddConfig->orxControls[0].enable);
    fpga9001_Tdd_Enable_Set(device,
                            AXI_ADRV9001_TDD_ORX1_DMA_ID,
                            &tddConfig->orxControls[1].tollgateTiming,
                            &tddConfig->orxControls[1].tollgateSequence,
							tddConfig->orxControls[1].enable);
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Tdd_Inspect(adi_fpga9001_Device_t *device, adi_fpga9001_TddConfig_t *tddConfig)
{
    uint32_t frameSwitchNum;

    ADI_NULL_PTR_RETURN(&device->common, tddConfig);
    
    fpga9001_Tdd_Frame_Get(device, &tddConfig->framing, &frameSwitchNum);
    tddConfig->framing.frameSwitchTimeChannel = 0;

    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_RX0_DEVICE_ID,
                            &tddConfig->rxControls[0].pinTiming,
                            &tddConfig->rxControls[0].pinSequence,
							&tddConfig->rxControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_TX0_DEVICE_ID,
                            &tddConfig->txControls[0].pinTiming,
                            &tddConfig->txControls[0].pinSequence,
							&tddConfig->txControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_RX1_DEVICE_ID,
                            &tddConfig->rxControls[1].pinTiming,
                            &tddConfig->rxControls[1].pinSequence,
							&tddConfig->rxControls[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_TX1_DEVICE_ID,
                            &tddConfig->txControls[1].pinTiming,
                            &tddConfig->txControls[1].pinSequence,
							&tddConfig->txControls[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_ORX0_DEVICE_ID,
                            &tddConfig->orxControls[0].pinTiming,
                            &tddConfig->orxControls[0].pinSequence,
							&tddConfig->orxControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_ORX1_DEVICE_ID,
                            &tddConfig->orxControls[1].pinTiming,
                            &tddConfig->orxControls[1].pinSequence,
							&tddConfig->orxControls[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_AUX0_DEVICE_ID,
                            &tddConfig->txGpioControls[0].pinTiming,
                            &tddConfig->txGpioControls[0].pinSequence,
							&tddConfig->txGpioControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_AUX1_DEVICE_ID,
                            &tddConfig->txGpioControls[1].pinTiming,
                            &tddConfig->txGpioControls[1].pinSequence,
							&tddConfig->txGpioControls[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_CTL0_DEVICE_ID,
                            &tddConfig->interFrameControlSignal1[0].pinTiming,
                            &tddConfig->interFrameControlSignal1[0].pinSequence,
							&tddConfig->interFrameControlSignal1[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_CTL1_DEVICE_ID,
                            &tddConfig->interFrameControlSignal1[1].pinTiming,
                            &tddConfig->interFrameControlSignal1[1].pinSequence,
							&tddConfig->interFrameControlSignal1[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_CTL2_DEVICE_ID,
                            &tddConfig->interFrameControlSignal2[0].pinTiming,
                            &tddConfig->interFrameControlSignal2[0].pinSequence,
							&tddConfig->interFrameControlSignal2[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_CTL3_DEVICE_ID,
                            &tddConfig->interFrameControlSignal2[1].pinTiming,
                            &tddConfig->interFrameControlSignal2[1].pinSequence,
							&tddConfig->interFrameControlSignal2[1].enable);
    
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_TRIG1_DEVICE_ID,
                            &tddConfig->smaOutput1.pinTiming,
                            &tddConfig->smaOutput1.pinSequence,
							&tddConfig->smaOutput1.enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_TRIG2_DEVICE_ID,
                            &tddConfig->smaOutput2.pinTiming,
                            &tddConfig->smaOutput2.pinSequence,
							&tddConfig->smaOutput2.enable);

    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_RX0_DMA_ID,
                            &tddConfig->rxControls[0].tollgateTiming,
                            &tddConfig->rxControls[0].tollgateSequence,
							&tddConfig->rxControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_TX0_DMA_ID,
                            &tddConfig->txControls[0].tollgateTiming,
                            &tddConfig->txControls[0].tollgateSequence,
							&tddConfig->txControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_RX1_DMA_ID,
                            &tddConfig->rxControls[1].tollgateTiming,
                            &tddConfig->rxControls[1].tollgateSequence,
							&tddConfig->rxControls[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_TX1_DMA_ID,
                            &tddConfig->txControls[1].tollgateTiming,
                            &tddConfig->txControls[1].tollgateSequence,
							&tddConfig->txControls[1].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_ORX0_DMA_ID,
                            &tddConfig->orxControls[0].tollgateTiming,
                            &tddConfig->orxControls[0].tollgateSequence,
							&tddConfig->orxControls[0].enable);
    fpga9001_Tdd_Enable_Get(device,
                            AXI_ADRV9001_TDD_ORX1_DMA_ID,
                            &tddConfig->orxControls[1].tollgateTiming,
                            &tddConfig->orxControls[1].tollgateSequence,
							&tddConfig->orxControls[1].enable);
    
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Tdd_ProgrammedEnable_Set(adi_fpga9001_Device_t *device, bool enable)
{
    if (enable)
    {
        axi_adrv9001_tdd_start((void *)device, AXI_ADRV9001_ID);
    }
    else
    {
        axi_adrv9001_tdd_stop((void *)device, AXI_ADRV9001_ID);
    }
    ADI_API_RETURN(device);
}

int32_t adi_fpga9001_Tdd_ProgrammedEnable_Get(adi_fpga9001_Device_t *device, bool *enable)
{
    *enable = (axi_adrv9001_tdd_status((void *)device, AXI_ADRV9001_ID) == 0x1) ? true : false;
    ADI_API_RETURN(device);
}

