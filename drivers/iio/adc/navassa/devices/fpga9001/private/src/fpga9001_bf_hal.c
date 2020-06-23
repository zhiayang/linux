/**
* \file fpga9001_bf_hal.c
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*/

/**
* Disclaimer Legal Disclaimer
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information.
* see the "LICENSE.txt" file in this zip file.
*/
 
#include "adi_common_hal.h"  
#include "adi_fpga9001.h"
#include "adi_fpga9001_hal.h"
#include "adi_common_error.h"
#include "fpga9001_bf_hal.h"



/**
* \brief ensures the cache is empty. if not it writes the cache to the part.
*
* \param device Pointer to the ADRV9001 device data structure.
* 
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfReadCacheInit(adi_fpga9001_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
//    uint16_t numRdBytes = 0;
//    uint64_t rdData = 0;
//    int32_t halError = ADI_COMMON_HAL_OK;

//    ADI_NULL_DEVICE_PTR_RETURN(device);
//
//    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//    if (device->common.cacheInfo.rdCacheState == ADRV9001_RD_BF_CACHE_ON)
//    {
//        recoveryAction = fpga9001_BfReadCacheFlush(device, &rdData, &numRdBytes);
//        ADI_ERROR_RETURN(device->common.error.newAction);
//        /* if Global Caching is being used, no further flushing should be performed
//        *  utill the user calls bf flush*/
//    }
    return recoveryAction;
}

/**
* \brief ensures the cache is empty. if not it writes the cache to the part.
*
* \param device Pointer to the ADRV9001 device data structure.
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfWriteCacheInit(adi_fpga9001_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
//   int32_t halError = ADI_COMMON_HAL_OK;

//    ADI_NULL_DEVICE_PTR_RETURN(device);
//
//    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//    /* skip cacheInit if Global level Cache is on.*/
//    if (device->common.cacheInfo.wrCacheState == ADRV9001_WR_BF_CACHE_ON)
//    {
//        /*Ensures the cache is empty if bitField Level Caching is on*/
//        
//        recoveryAction = fpga9001_BfWriteCacheFlush(device);
//        ADI_ERROR_RETURN(device->common.error.newAction);
//        /* if using bitField level caching reenable caching after each flush*/
//        device->common.cacheInfo.wrFlushEn = true;
//    }
//    else
//    {
//        /* disable flushing within the bitfield function when global 
//           caching is being used*/
//        device->common.cacheInfo.wrFlushEn = false;
//    }

    return recoveryAction;
}



/**
* \brief Used to read a register.
* Merges the write if merging is on
* Caches the write if caching is on, 
* Imediately writes the value if caching is off.
*
* \param device Pointer to the ADRV9001 device data structure
* \param addr The address of the Register to read.
* \param regVal The value to be written to the register.
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfRegisterWrite(adi_fpga9001_Device_t *device,
                             uint32_t addr,
                             uint32_t regVal)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//   //TODO add array size check
//
//    if (device->common.cacheInfo.wrCacheState == ADRV9001_WR_GBL_MERGE_ON)
//    {
//        /* Write merging is on, so the cache is searched to see if address is in the cache*/
//        for (i = 0; i < device->common.cacheInfo.wrCacheIdx; i++)
//        {
//            /* The cache is packed in the form AABBMMDD
//            where AA is MSB Addr, BB is the LSB addr,
//            MM is the mask and DD is the Data */
//            //TODO fix this
//            //if the address part of the cache word = wanted address
//            if ((uint16_t)(device->common.cacheInfo.wrCache[i] >> SPI_ADDR_SIZE) == addr)
//            {
//                //TODO: Warn of overwriteing data
//                //TODO: add warning as error in Error Handler.
//                addrFoundInCache = true;
//                halError = ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//                recoveryAction = ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_ADIHAL, halError, ADI_COMMON_ACT_WARN_RESET_LOG, halError, "Log failure");
//                /* preserve the address but clear the data. */
//
//                //TODO: make a test to check this
//                device->common.cacheInfo.wrCache[i] &= 0xFFFFFF00;
//                device->common.cacheInfo.wrCache[i] |= (((uint32_t)0xFF << SPI_MASK_SIZE) | regVal);
//            }
//        }
//    }
//
//    if ((device->common.cacheInfo.wrCacheState != ADRV9001_WR_CACHE_OFF) && 
//        (addrFoundInCache == false))
//    {
//        /* Write caching is turned on and the bitField has not been merged*/
//        tmp_wrCacheIdx = device->common.cacheInfo.wrCacheIdx;
//        if ((tmp_wrCacheIdx++) >= ADRV9001_BF_WRCACHE_ARRAY_TRIP_SIZE)
//        {
//            /* if cache is full then flush cache and add next instruction.
//            should not be a problem flushing a write cache.
//            */
//            device->common.cacheInfo.wrFlushEn = true;
//            recoveryAction = fpga9001_BfWriteCacheFlush(device);
//            ADI_ERROR_RETURN(device->common.error.newAction);
//            device->common.cacheInfo.wrFlushEn = false;
//        }
//
//        device->common.cacheInfo.wrCache[device->common.cacheInfo.wrCacheIdx++] = ((uint32_t)addr << SPI_ADDR_SIZE) | (uint32_t)0xFF << SPI_MASK_SIZE | regVal;
//    }
//
//    if (device->common.cacheInfo.wrCacheState == ADRV9001_WR_CACHE_OFF)
//    {
//        /* Write caching is not being used, so do an immediate write */
//        if (device->common.devHalInfo != NULL)
//        {
//            recoveryAction = adi_fpga9001_hal_SpiByteWrite(device, addr, regVal);
//            ADI_ERROR_RETURN(device->common.error.newAction);
//        }
//    }
    adi_fpga9001_hal_Register_Write(device, addr, regVal); 
    ADI_ERROR_RETURN(device->common.error.newAction);  
    return recoveryAction;
}


/**
* \brief Used to read a register.
* Caches the read if caching is on, imediately reads the value with caching off.
*
* \param device Pointer to the ADRV9001 device data structure
* \param addr The address of the Register to read.
* \param regVal The value of the register just read.
* \param endOfBf when using read caching the upper nibble of this field indicates the end of bitfield with the Value F.
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfRegisterRead(adi_fpga9001_Device_t *device,
                            uint32_t addr,
                            uint32_t *regVal,
                            uint8_t endOfBf)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);

    ADI_NULL_PTR_RETURN(&device->common, regVal);
//
//    if (device->common.cacheInfo.rdCacheState > ADRV9001_RD_CACHE_OFF)
//    {
//        /* Read Caching is turned on */
//        int tmp_rdCacheIdx = device->common.cacheInfo.rdCacheIdx;
//        if ((tmp_rdCacheIdx++) >= ADRV9001_BF_RDCACHE_ARRAY_TRIP_SIZE)
//        {
//            /* if the cache is full then raise an error*/
//            ADI_ERROR_REPORT(&device->common.error, ADI_COMMON_ERRSRC_DEVICEBF, 
//                         ADRV9001_BF_CACHE_OVERFLOW, 
//                         ADI_COMMON_ACT_ERR_CHECK_PARAM, 
//                         NULL, 
//                         "Rx Cache size exceeded error");
//            ADI_ERROR_RETURN(device->common.error.newAction);
//        }
//        /* set bits [31:16] of the cache to the 16 bit address 
//           set bits [15:8] of cache to the 8 bit mask address
//           set bits [7:0] of cache to the 8 bit endOfBf.
//           endOfBf contains two 4 bit fields.
//           [7:4] is 0xF or 0x0 and masks a end of bitField data value.
//           [3:0] is the register startBit ie where the value resides in the register.
//           */
//        device->common.cacheInfo.rdCache[device->common.cacheInfo.rdCacheIdx++] = ((uint32_t)addr << SPI_ADDR_SIZE) | 
//                                                                    ((uint32_t)0xFF << SPI_DATA_SIZE) | 
//                                                                    endOfBf;
//    }
//    else
//    {
//       // recoveryAction = adi_fpga9001_hal_SpiByteRead(device, addr, regVal);

    //int32_t adi_fpga9001_hal_Register_Read(adi_fpga9001_Device_t *device, uint32_t offset, uint32_t *data)
    //ToDo replace theis with fieldRead.

    adi_fpga9001_hal_Register_Read(device, addr, regVal);   
    ADI_ERROR_RETURN(device->common.error.newAction);
//    }
    return recoveryAction;
}

/**
* \brief Used to write to a sub part of a register.
* Caches the write if caching is on, imediately writes the value with caching off.
* Uses the HW_RMW function if the HW_RMW is enabled otherwise used a spi transaction to read the register
* and a spi write transaction to write the modified byte back to the register.
*
* \param device Pointer to the ADRV9001 device data structure
* \param addr The Register address that contains the field.
* \param fieldVal The value of the field to be written.
* \param mask The mask to use when inserting the data into the register.
* \param startBit The location of the first bit of the field within the register.
* when using read caching the upper nibble of this field indicates the end of bitfield with the Value F.
* This value is used to shift the value so that it aligns with the location of the field within the register.
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfFieldWrite(adi_fpga9001_Device_t *device,
                              uint32_t addr,
                              uint32_t fieldVal,
                              uint32_t mask,
                              uint8_t startBit)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    uint32_t regVal = ((fieldVal << startBit) & mask);

    uint32_t registerVal = 0;
    uint32_t newRegVal = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);

//TODO: remove this if not used
//    if (device->common.cacheInfo.wrCacheState > ADRV9001_WR_HW_RMW_ON)
//    {
//        /* if caching is turned on*/
//        if (device->common.cacheInfo.wrCacheState == ADRV9001_WR_GBL_MERGE_ON)
//        {
//            /* if merging of multiple register writes with the same address is turned on*/
//            for (i = 0; i < device->common.cacheInfo.wrCacheIdx; i++)
//            {
//                /* search the cache for addr in the cache 
//                *  if found merge field val and the mask into the original values*/
//                if ((uint16_t)(device->common.cacheInfo.wrCache[i] >> SPI_ADDR_SIZE) == addr)
//                {
//                    addrFoundInCache = 1;
//                    cachemask = (uint8_t)(device->common.cacheInfo.wrCache[i] | 0xFF00);
//                    newMask = cachemask | mask;
//                    cacheVal = (uint8_t)(device->common.cacheInfo.wrCache[i]);
//                    newVal = cacheVal | regVal;
//                    /* clear org mask and data. */
//
//                    device->common.cacheInfo.wrCache[i] &= 0xFFFF0000;
//                    device->common.cacheInfo.wrCache[i] |= ((uint32_t)newMask << 8) | newVal;
//                }
//            }
//        }
//        if ((device->common.cacheInfo.wrCacheState > ADRV9001_WR_CACHE_OFF) & !addrFoundInCache)
//        {
//            /* if caching is on or
//            * merging is on but addr was not found in the cache 
//            * add the wr instruction to the cache. */
//            tmp_wrCacheIdx = device->common.cacheInfo.wrCacheIdx;
//            if ((tmp_wrCacheIdx++) >= ADRV9001_BF_WRCACHE_ARRAY_TRIP_SIZE)
//            {
//                recoveryAction = fpga9001_BfWriteCacheFlush(device);
//                ADI_ERROR_RETURN(device->common.error.newAction);
//            }
//            device->common.cacheInfo.wrCache[device->common.cacheInfo.wrCacheIdx++] = ((uint32_t)addr << SPI_ADDR_SIZE) | 
//                                                                        ((uint32_t)mask << 8) |
//                                                                        regVal;
//        }
//    }
//    else
//    {
//        /* Immediate Read modify write using HW_RMW function */
//        if (device->common.cacheInfo.wrCacheState == ADRV9001_WR_HW_RMW_ON && 
//            (device->common.cacheInfo.HW_RMW_Enabled > 0))
//        {
//            if (device->common.cacheInfo.wrCacheIdx >= ADRV9001_BF_WRCACHE_ARRAY_TRIP_SIZE)
//            {
//                //it is possible the cache may cause the spi buffer to exceed its size
//                //given this is a write operation it should be ok to flush and empty the buffrer
//                recoveryAction = fpga9001_BfWriteCacheFlush(device);
//                ADI_ERROR_RETURN(device->common.error.newAction);
//            }
//            recoveryAction = adi_fpga9001_hal_SpiByteWrite(device, addr, newRegVal);
//            ADI_ERROR_RETURN(device->common.error.newAction);
//        }
//        else
//        {
            /* Immediate Standard read Modify Write using seperate read 
               and writespi transactions. */
            recoveryAction = adi_fpga9001_hal_Register_Read(device, addr, &registerVal);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* modify the register value*/
            newRegVal = (registerVal & ~mask) | regVal;

            /* write to the register with modified value*/
            recoveryAction = adi_fpga9001_hal_Register_Write(device, addr, newRegVal);
            ADI_ERROR_RETURN(device->common.error.newAction);
//        }
//    }
    return recoveryAction;
}

/**
* \brief Used to read a sub part of a register
* caches the read if caching is on, imediately read the value with caching off
*
* \param device Pointer to the ADRV9001 device data structure
* \param addr The Register address that contains the field.
* \param fieldVal The value of the field to be written.
* \param mask The mask to use when inserting the data into the register.
* \param startBit The location of the first bit of the field within the register.
*
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_FPGA9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfFieldRead(adi_fpga9001_Device_t *device,
                             uint32_t addr,
                             uint32_t *fieldVal,
                             uint32_t mask,
                             uint8_t startBit)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
    
    ADI_NULL_PTR_RETURN(&device->common, fieldVal);

    //TODO: remove this if not used
//
//    if (device->common.cacheInfo.rdCacheState > ADRV9001_RD_CACHE_OFF)
//    {
//        //adds spi call to cache 
//        int rdCacheIdx = device->common.cacheInfo.rdCacheIdx;
//        if ((rdCacheIdx++) >= ADRV9001_BF_RDCACHE_ARRAY_TRIP_SIZE)
//        {
//            ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_DEVICEBF, 
//                         ADI_COMMON_ERR_INV_PARAM, 
//                         ADI_COMMON_ACT_ERR_CHECK_PARAM, 
//                         NULL, 
//                         "Rx Cache size exceeded error");
//
//            ADI_ERROR_RETURN(device->common.error.newAction);
//        }
//        device->common.cacheInfo.rdCache[device->common.cacheInfo.rdCacheIdx++] = ((uint32_t)addr << SPI_ADDR_SIZE) |
//                                                                    ((uint32_t)mask << SPI_MASK_SIZE) |
//                                                                    startBit;
//    }
//    else
//    {
//        /* Implements immediate spi transaction call only used when caching is turned off. */
        //recoveryAction = adi_fpga9001_hal_SpiFieldRead(device, addr, fieldVal, mask, startBit);

        startBit = startBit & 0x3F;  /* remove the End Of BitField Marker */

        recoveryAction = adi_fpga9001_hal_Field_Read(device, addr, fieldVal, mask, startBit);

//        ADI_ERROR_RETURN(device->common.error.newAction);
//    }

    return recoveryAction;
}

/**
* \brief Used by the bitField API to ensure the cache is flushed when using BitField Caching
* and to assemble the register bytes when caching is not used.
*
* \param device Pointer to the ADRV9001 device data structure
* \param rxBfData The raw data read from the registers prior to concatenation.
* \param numRxBfData The number of elements used in the rxBfData.
* \param rxData The assembled bitfield value.
*
* \retval ADI_FPGA90019001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_FPGA90019001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_FPGA90019001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_FPGA90019001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfReadAssembleData(adi_fpga9001_Device_t *device,
                                    uint32_t *rxBfData,
                                    uint8_t numRxBfData,
                                    uint64_t *rxData)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);

    ADI_NULL_PTR_RETURN(&device->common, rxBfData);
    ADI_NULL_PTR_RETURN(&device->common, rxData);
//TODO: remove this if not used
//
//    if (device->common.cacheInfo.rdCacheState == ADRV9001_RD_BF_CACHE_ON)
//    {
//        recoveryAction = fpga9001_BfReadCacheFlush(device, rxData, &numRxBitFields);
//        ADI_ERROR_RETURN(device->common.error.newAction);
//    }
//    else if (device->common.cacheInfo.rdCacheState == ADRV9001_RD_CACHE_OFF)
//    {
//        /* Caching is not used so then assembly the bitFiled register values 
//           into a bitField Value. The max bitField Value is 64bits long.
//           so rdData could be shifted 2 times to concatenate a 64 bit value.*/
        for (i = 0; i < numRxBfData; i++)
        {
            /* shift rxData to the left to make room for the next lower byte 
               in the bitField*/
            *rxData = (*rxData << 32) | (uint64_t)rxBfData[i];
        }
//    }
    return recoveryAction;
}


/**
* \brief Used to send multiple spi register reads in one spi transaction.
*  multiple register data and sub register data is concatenated and shifted appropriately.
*
* \param device Pointer to the ADRV9001 device data structure
* \param rdData The bytefield data being returned in the order it was read.
* \param numRdBitFields The number of bitfields being returned
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfReadCacheFlush(adi_fpga9001_Device_t *device,
                                  uint64_t *rdData,
                                  uint16_t *numRdBitFields)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
//TODO: remove this if not used
//    uint32_t bfValue = 0;
//    uint32_t i = 0;
//    uint16_t addr = 0;
//    uint16_t rdAddr[ADRV9001_BF_RDCACHE_ARRAY_SIZE] = { 0 };
//
//    uint8_t regVal = 0;
//    uint8_t mask = 0;
//    uint8_t startBit = 0;
//    uint8_t endOfBitfield = 0;
//
//    uint8_t regData[ADRV9001_BF_RDCACHE_ARRAY_SIZE] = { 0 };
//    int32_t halError = ADI_COMMON_HAL_OK;

//    ADI_NULL_DEVICE_PTR_RETURN(device);
//
//    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//    ADI_NULL_PTR_RETURN(&device->common,(&device->common.error, rdData);
//    ADI_NULL_PTR_RETURN(&device->common,(&device->common.error, numRdBitFields);
//
//    *numRdBitFields = 0;
//
//    if (device->common.cacheInfo.rdCacheIdx > ADRV9001_BF_RDCACHE_ARRAY_SIZE)
//    {
//        ADI_ERROR_REPORT(&device->common,  2, ADI_COMMON_ERRSRC_DEVICEBF, 
//                     ADI_ADRV9001_ACT_ERR_FLUSH_CACHE,
//                     NULL, 
//                     "Rd Cache index exceeded the spi cache Size ");
//        ADI_ERROR_RETURN(device->common.error.newAction);
//    }
//    /* the rdData supplied by the calling function is smaller then the rdCacheIdx*/
//    if (device->common.cacheInfo.rdCacheIdx > sizeof(rdData))
//    {
//        ADI_ERROR_REPORT(&device->common,  2, ADI_COMMON_ERRSRC_DEVICEBF,
//                     ADI_ADRV9001_ACT_ERR_FLUSH_CACHE,
//                     NULL,
//                     "Rd Cache index exceeded the user supplied rdData ");
//        ADI_ERROR_RETURN(device->common.error.newAction);
//    }
//
//    /* pack all the read data into one array*/
//    for (i = 0; i < device->common.cacheInfo.rdCacheIdx; i++)
//    {
//        /* extract the address (cache[31-16]) from each of the 32 bit words in the cache 
//           into a address array suitable for ADRV9001HAL_spiReadBytes*/
//        rdAddr[i] = device->common.cacheInfo.rdCache[i] >> SPI_ADDR_SIZE;
//    }
//
//    /* send the array and recieve an array of register values */
//    adi_fpga9001_hal_SpiBytesRead(device, &rdAddr[0], &regData[0], device->common.cacheInfo.rdCacheIdx);
//    ADI_ERROR_RETURN(device->common.error.newAction);
//
//    /* depack the array of register values into an array of bitField values*/
//    for (i = 0; i < device->common.cacheInfo.rdCacheIdx; i++)
//    {
//        /* for each of the register values in the cache (if any)
//           combine the data into a bitfield value*/
//
//        mask = (uint8_t)(device->common.cacheInfo.rdCache[i] >> 8);
//        startBit = (uint8_t)(device->common.cacheInfo.rdCache[i]);
//
//        /*The last byte in the bit field is marked by setting the upper
//        nible of the startBit to F, this allows the lower nible to
//        handle the byte with values from 0 to 7.
//        This marker is added within the get bitfield function.
//        */
//        endOfBitfield = startBit > 0xF;
//
//        if (endOfBitfield)
//        {
//            startBit = startBit & 0xF;
//        }
//        regVal = regData[i];
//
//        bfValue = (bfValue << 8) | ((regVal & mask) >> startBit);
//
//        device->common.cacheInfo.rdCache[i] = '\0';
//
//        if (endOfBitfield)
//        {
//            rdData[(*numRdBitFields)++] = bfValue;
//            bfValue = 0;
//        }
//    }
//    device->common.cacheInfo.rdCacheIdx = 0;
//    device->common.cacheInfo.rdCache[device->common.cacheInfo.rdCacheIdx] = '\0';
    return recoveryAction;
}

/**
* \brief Used to send multiple spi register writes in one spi transaction.
*  if the spi buffer size is exceeded the data will be sent in buffer size blocks.
*  Only full bytes are written ie there are no RMW operations
*
* \param device Pointer to the ADRV9001 device data structure
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t fpga9001_BfWriteCacheFlush(adi_fpga9001_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
//TODO: remove this if not used
//    uint32_t i = 0;
//    uint16_t wrAddr[ADRV9001_BF_WRCACHE_ARRAY_SIZE] = { 0 };
//    uint8_t wrData[ADRV9001_BF_WRCACHE_ARRAY_SIZE] = { 0 }; 
//    int32_t halError = ADI_COMMON_HAL_OK;

//    ADI_NULL_DEVICE_PTR_RETURN(device);
//
//    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//    if (device->common.cacheInfo.wrCacheIdx > ADRV9001_BF_WRCACHE_ARRAY_SIZE)
//    {
//        ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_DEVICEBF,
//                     ADRV9001_BF_CACHE_OVERFLOW,
//                     ADI_ADRV9001_ACT_ERR_FLUSH_CACHE,
//                     NULL,
//                     "Wr Cache index exceeded the spi cache Size ");
//        ADI_ERROR_RETURN(device->common.error.newAction);
//    }
//
//    if (device->common.cacheInfo.wrFlushEn == true)
//    {
//        /* convert the cache into two arrays spiWriteBytes can use*/
//        for (i = 0; i < device->common.cacheInfo.wrCacheIdx; i++)
//        {
//            wrAddr[i] = (uint16_t)((device->common.cacheInfo.wrCache[i] >> SPI_ADDR_SIZE) & 0xFFFF);
//            wrData[i] = (uint8_t)(device->common.cacheInfo.wrCache[i] & 0xFF);
//            device->common.cacheInfo.wrCache[i] = '\0';
//        }
//        /* write the cache to the part */
//        recoveryAction = adi_fpga9001_hal_SpiBytesWrite(device, &wrAddr[0], 
//                                                   &wrData[0], 
//                                                   device->common.cacheInfo.wrCacheIdx);
//        ADI_ERROR_RETURN(device->common.error.newAction);
//
//        device->common.cacheInfo.wrCacheIdx = 0;
//        device->common.cacheInfo.wrCache[device->common.cacheInfo.wrCacheIdx] = '\0';
//    }
//    device->common.cacheInfo.wrFlushEn = true;

    return recoveryAction;
}


/**
* \brief allows the user to set the Write cache state.
*
* \param device Pointer to the ADRV9001 device data structure.
* \param wrCacheState the cache state to be set.
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
//int32_t fpga9001_BfWriteCacheEn(adi_fpga9001_Device_t *device,
//                                fpga9001_Bf_WrCache_State_e wrCacheState)
//{
//    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
//    int32_t halError = ADI_COMMON_HAL_OK;

//    ADI_NULL_DEVICE_PTR_RETURN(device);
//
//    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//    device->common.cacheInfo.wrCacheState = wrCacheState;
//    return recoveryAction;
//}

/**
* \brief allows the user to set the read cache state.
*
* \param device Pointer to the ADRV9001 device data structure.
* \param rdCacheState the cache state to be set.
*
* \retval ADRV9001_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9001_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9001_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9001_ACT_NO_ACTION Function completed successfully, no action required
*
*/
//int32_t fpga9001_BfReadCacheEn(adi_fpga9001_Device_t *device,
//                               fpga9001_Bf_RdCache_State_e rdCacheState)
//{
//    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
//    int32_t halError = ADI_COMMON_HAL_OK;

//    ADI_NULL_DEVICE_PTR_RETURN(device);
//
//    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_BF);
//
//    device->common.cacheInfo.rdCacheState = rdCacheState;
//    return recoveryAction;
//}