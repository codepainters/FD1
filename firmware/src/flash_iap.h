/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

/* This file defines interface to the Flash In-Application Programming
 * routines stored in MCU's ROM. For details see applicatio note AN11008.
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

// size fo single Flash sector in bytes
#define SECTOR_SIZE     (4096)

// size of a Flash page size (smallest size of a single write operation)
#define PAGE_SIZE       (256)

/**
 * Erase a single Flash sector
 *
 * This call issues both prepare (0x50) and erase (0x52) commands.
 */
IapResult_t FlashIap_EraseSector(const Sector_t* sector);

/**
 * Writes a given buffer into the Flash (includes preparing)
 *
 * @param sector        sector to write to
 * @param offset        offset from the start of the sector, must be a 256 multiply
 * @param buffer        source buffer
 * @param bufferSize    buffer size, must be 256, 512, 1024 or 4096
 * @return status code
 */
IapResult_t FlashIap_WriteSector(const Sector_t* sector, const size_t offset,
                                 const uint8_t* buffer, const size_t bufferSize);

/**
 * Verifies written data
 *
 * @param sector        sector to check
 * @param offset        offset in the sector (word boundry)
 * @param buffer        buffer to verify against (word boundry)
 * @param bufferSize    size of the chunk to verify (multiple of 4)
 * @return
 */
IapResult_t FlashIap_Verify(const Sector_t* sector, const size_t offset,
                            const uint8_t* buffer, const size_t bufferSize);

#endif // __FLASH_IAP_H__
