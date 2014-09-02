/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __FLASH_IAP_H__
#define __FLASH_IAP_H__

#include "sysdefs.h"

typedef struct {
    uint8_t* address;
    uint8_t number;
} Sector_t;

typedef enum {
    IAP_SUCCESS = 0
} IapResult_t;

IapResult_t FlashIap_EraseSector(const Sector_t* sector);

IapResult_t FlashIap_PrepareSector(const Sector_t* sector);

IapResult_t FlashIap_WriteSector(const Sector_t* sector, uint8_t* buffer, size_t bufferSize);

IapResult_t FlashIap_VerifySector(const Sector_t* sector, uint8_t* buffer, size_t bufferSize);

#endif // __FLASH_IAP_H__
