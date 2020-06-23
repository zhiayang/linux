/**
 * \file
 * Analog Devices ZC706 Platform + microzed hardware abstraction layer
 *
 * Copyright 2018 Analog Devices Inc.
 *
 * Support for linux layer timer functions
 */

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information.
* see the "LICENSE.txt" file in this zip file.
*/

#include <errno.h>
#include <time.h>
#include "zc706sd20_timer.h"

/**
* \brief Function to open/allocate any necessary resources for the timer wait
*        functions below.
*
* \param devHalCfg Pointer to device instance specific platform settings
*
* \retval ADI_HAL_OK Function completed successfully
*/
int32_t zc706sd20_TimerOpen(void *devHalCfg)
{
    /* ADI ZC706 platform does not require any timer open /close */
    return (int32_t)ADI_HAL_OK;
}

/**
* \brief Function to close any necessary resources for the timer wait
*        functions below.
*
* \param devHalCfg Pointer to device instance specific platform settings
*
* \retval ADI_HAL_OK Function completed successfully
*/
int32_t zc706sd20_TimerClose(void *devHalCfg)
{
    /* ADI ZC706 platform does not require any timer open /close */
    return (int32_t)ADI_HAL_OK;
}

/**
* \brief Function to initialize any necessary resources for the timer wait
*        functions below.
*
* \param devHalCfg Pointer to device instance specific platform settings
*
* \retval ADI_HAL_OK Function completed successfully
*/
int32_t zc706sd20_TimerInit(void *devHalCfg)
{
    /* ADI ZC706 platform does not require any timer init */
    return (int32_t)ADI_HAL_OK;
}

/**
* \brief Provides a blocking delay of the current thread
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param time_us the time to delay in mico seconds
*
* \retval ADI_HAL_OK Function completed successfully
* \retval ADI_HAL_NULL_PTR the function has been called with a null pointer
*/
int32_t zc706sd20_TimerWait_us(void *devHalCfg, uint32_t time_us)
{
    int32_t halError = (int32_t)ADI_HAL_OK;
    
#ifdef __GNUC__
    struct timespec t0;
    struct timespec t1;
    struct timespec *temp;
    struct timespec *waitTime = &t0;
    struct timespec *remaining = &t1;
    
    waitTime->tv_sec = time_us / 1000000;
    waitTime->tv_nsec = (time_us % 1000000) * (1000);

    do {
        halError = nanosleep(waitTime, remaining);
        temp = waitTime;
        waitTime = remaining;
        remaining = temp;
    } while ((halError == -1) && (errno == EINTR));
    
#else
    if (devHalCfg == NULL)
    {
        halError = (int32_t)ADI_HAL_NULL_PTR;
        return halError;
    }
    
#endif /* __GNUC__ */
    return halError;
}

/**
* \brief Provides a blocking delay of the current thread
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param time_ms the Time to delay in milli seconds
*
* \retval ADI_HAL_OK Function completed successfully
* \retval ADI_HAL_NULL_PTR the function has been called with a null pointer
*/
int32_t zc706sd20_TimerWait_ms(void *devHalCfg, uint32_t time_ms)
{
    int32_t halError = (int32_t)ADI_HAL_OK;

#ifdef __GNUC__
    struct timespec t0;
    struct timespec t1;
    struct timespec *temp;
    struct timespec *waitTime = &t0;
    struct timespec *remaining = &t1;

    waitTime->tv_sec = time_ms / 1000;
    waitTime->tv_nsec = (time_ms % 1000) * (1000000);

    do {
        halError = nanosleep(waitTime, remaining);
        temp = waitTime;
        waitTime = remaining;
        remaining = temp;
    } while ((halError == -1) && (errno == EINTR));

#else
    if (devHalCfg == NULL)
    {
        halError = (int32_t)ADI_HAL_NULL_PTR;
        return halError;
    }
#endif /* __GNUC__ */
    return halError;
}

