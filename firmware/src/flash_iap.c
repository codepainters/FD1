/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "flash_iap.h"
#include "lpc134x.h"

// entry point to the IAP code in ROM, see AN11008
#define IAP_LOCATION    (0x1FFF1FF1)

typedef void (*IAP)(uint32_t[], uint32_t[]);
static IAP IAPEntry = (IAP)IAP_LOCATION;

// CPU clock in kHz
#define CPU_CLK (72000)

IapResult_t FlashIap_EraseSector(const Sector_t*  sector)
{
    uint32_t result[5];

    // prepare sector
    uint32_t prepareCommand[5] = { 50, sector->number, sector->number };
    __disable_irq();
    IAPEntry(prepareCommand, result);
    __enable_irq();
    if (result[0] != IAP_SUCCESS) {
        return result[0];
    }

    // erase sector
    uint32_t eraseCommand[5] = { 52, sector->number, sector->number, CPU_CLK };
    __disable_irq();
    IAPEntry(eraseCommand, result);
    __enable_irq();

    return result[0];
}

IapResult_t FlashIap_WriteSector(const Sector_t* sector, const size_t offset,
                                 const uint8_t* buffer, const size_t bufferSize)
{
    uint32_t result[5];

    // prepare sector
    uint32_t prepareCommand[5] = { 50, sector->number, sector->number };
    __disable_irq();
    IAPEntry(prepareCommand, result);
    __enable_irq();
    if (result[0] != IAP_SUCCESS) {
        return result[0];
    }

    // write to sector
    uint32_t writeCommand[5] = { 51, (uint32_t) sector->address + offset, (uint32_t) buffer, bufferSize, CPU_CLK };
    __disable_irq();
    IAPEntry(writeCommand, result);
    __enable_irq();
    return result[0];
}

IapResult_t FlashIap_Verify(const Sector_t* sector, const size_t offset,
                            const uint8_t* buffer, const size_t bufferSize)
{
    uint32_t result[5];

    uint32_t command[5] = { 56, (uint32_t) sector->address + offset, (uint32_t) buffer, bufferSize };
    __disable_irq();
    IAPEntry(command, result);
    __enable_irq();

    return result[0];
}
