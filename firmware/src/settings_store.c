/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "settings_store.h"
#include "settings.h"
#include "flash_iap.h"

/*
 * Types
 */

// Each sector starts with a header like this, used to store the status.
// A 16-bytes flash chunk can be written only once, hence the structure.
typedef struct {
  uint8_t flag1;
  uint8_t _padding1[15];
  uint8_t flag2;
  uint8_t _padding2[15];
} __attribute__ ((__packed__)) SectorHeader_t;

// Settings are stored in records described by this structure
typedef struct {
    // marks record as used/unused
    uint8_t flags;
    // xor checksum of the entire structure
    uint8_t checksum;

    // actual settings go below

    uint8_t midiChannel;
    uint8_t octave;
    uint8_t velocity;
} __attribute__ ((__packed__)) SettingsRecord_t;

/*
 * Macros, constants, variables
 */

#define RECORD_EMPTY    (0xFF)
#define RECORD_USED     (0xAA)

// number of settings sectors in use
#define NUM_SECTORS     (2)

// returns index of the "opposite" settings sector
#define OTHER_SECTOR(i) (1 - (i))

// rounds given value up to multiple of 16
#define ROUND_UP_16(n)  ((((n) + 15) >> 4) << 4)

// sector is just formatted
#define FLAGS_EMPTY     (0xFFFF)

// sector is partialy used, i.e. it contains at least one valid record
#define FLAGS_USED      (0xAAFF)

// sector is full - this is temporary status during sector swapping
#define FLAGS_FULL      (0xAAAA)

// returns status flags for a given Sector_t*
#define SECTOR_FLAGS(sector)  ( \
    (((SectorHeader_t*) (sector)->address)->flag1 << 8) | \
    (((SectorHeader_t*) (sector)->address)->flag2) )

// returns status flags for a secotor with a given index
#define SECTOR_FLAGS_N(sector_num)  SECTOR_FLAGS(&sectors[(sector_num)])

// size of a single record rounded up to paragraph multiple
#define RECORD_SIZE         (ROUND_UP_16(sizeof(SettingsRecord_t)))

// number of records that can be stored per sector
#define RECORDS_PER_SECTOR  ((SECTOR_SIZE - sizeof(SectorHeader_t)) / RECORD_SIZE)

// Flash sectors used for settings storage, actual space is reserved by the linker script
static const Sector_t sectors[NUM_SECTORS] = {
    { (uint8_t*) 0x6000, 6 },
    { (uint8_t*) 0x7000, 7 }
};

// current valid sector
static unsigned int currentSector;

// offset of the first empty slot in the current sector
static unsigned int firstEmptySlot;

/*
 * Forward declarations
 */

static uint8_t SettingsStore_CalcChecksum(const SettingsRecord_t* record);

static bool SettingsStore_IsRecordValid(const SettingsRecord_t* record);

static unsigned int SettingsStore_InitSettingsFromSector(const Sector_t* sector);

static bool SettingsStore_SetSectorFlags(const Sector_t* sector, const uint32_t flags);

static bool SettingsStore_WriteSettingsAtSlot(const Sector_t* sector, const unsigned int slot);

static bool SettingsStore_SwapSectors(const Sector_t* from, const Sector_t* to);

/*
 * Public functions
 */

void SettingsStore_Init()
{
    // First check if the sectors needs formatting
    for (unsigned int i = 0; i < NUM_SECTORS; i++) {
        uint32_t flags = SECTOR_FLAGS_N(i);

        // invalid value means something is badly corrupted and all we can do
        // is to format the sector
        if (flags != FLAGS_EMPTY && flags != FLAGS_USED && flags != FLAGS_FULL) {
            FlashIap_EraseSector(&sectors[i]);
        }
    }

    // if any sector has FULL status, it means that sector swapping was interrupted
    // let it be finished then
    if (SECTOR_FLAGS_N(0) == FLAGS_FULL) {
        SettingsStore_SwapSectors(&sectors[0], &sectors[1]);
    }
    else if (SECTOR_FLAGS_N(1) == FLAGS_FULL) {
        SettingsStore_SwapSectors(&sectors[1], &sectors[0]);
    }

    // at this point we should have at most one sector marked as USED, and no FULL sectors
    if (SECTOR_FLAGS_N(0) == FLAGS_USED && SECTOR_FLAGS_N(1) == FLAGS_USED) {
        // if both sectors are USED, something is wrong, so just go end erase the second
        FlashIap_EraseSector(&sectors[1]);
        currentSector = 0;
    }
    else if (SECTOR_FLAGS_N(1) == FLAGS_USED) {
        currentSector = 1;
    }
    else if (SECTOR_FLAGS_N(0) == FLAGS_USED) {
        currentSector = 0;
    }
    else {
        // both empty, just use the first one
        currentSector = 0;
        SettingsStore_SetSectorFlags(&sectors[currentSector], FLAGS_USED);
    }

    // sectors prepared, now we can fetch latest record from the "active" sector
    firstEmptySlot = SettingsStore_InitSettingsFromSector(&sectors[currentSector]);
}

void SettingsStore_Save()
{
    if (firstEmptySlot >= RECORDS_PER_SECTOR) {
        // No space anymore in the current sector, do the swap
        unsigned int otherSector = OTHER_SECTOR(currentSector);

        // Note: if it fails, we don't switch the sectors - not much more we can do
        if (SettingsStore_SwapSectors(&sectors[currentSector], &sectors[otherSector])) {
            currentSector = otherSector;
        }
    }
    else {
        if(SettingsStore_WriteSettingsAtSlot(&sectors[currentSector], firstEmptySlot)) {
            firstEmptySlot++;
        }
    }
}

/*
 * Internal functions
 */

/**
 * Calculates XOR checksum over a given settings record
 */
static uint8_t SettingsStore_CalcChecksum(const SettingsRecord_t* record)
{
    uint8_t checksum = 0;
    for (unsigned int i = 0; i < sizeof(SettingsRecord_t); i++) {
        checksum ^= ((uint8_t*)record) [i];
    }

    return checksum;
}

/**
 * Checks validity of settings record (checksum, ranges)
 *
 * @param record    record to check
 * @return true if record is valid
 */
static bool SettingsStore_IsRecordValid(const SettingsRecord_t* record)
{
    if (record->flags != RECORD_USED) {
        return false;
    }

    // all bytes XOR'ed should give 0
    if (SettingsStore_CalcChecksum(record) != 0) {
        return false;
    }

    // check if actual settings are valid
    if ((record->midiChannel < MIDI_CHANNEL_MIN) || (record->midiChannel > MIDI_CHANNEL_MAX) ||
        (record->octave < OCTAVE_SHIFT_MIN) || (record->octave > OCTAVE_SHIFT_MAX) ||
        (record->velocity < MIDI_VELOCITY_MIN) || (record->velocity > MIDI_VELOCITY_MAX)) {
        return false;
    }

    return true;
}

/**
 * Initialize global settings from a given sector
 *
 * @return index of the first empty slot in a given sector
 */
static unsigned int SettingsStore_InitSettingsFromSector(const Sector_t* sector)
{
    const SettingsRecord_t* records = (SettingsRecord_t*)(sector->address + sizeof(SectorHeader_t));

    unsigned int firstEmptySlot = RECORDS_PER_SECTOR;

    // iterate from the end, looking for first valid sector
    for (int i = RECORDS_PER_SECTOR - 1; i >= 0; i--) {

        // at the same time we look for the first empty record
        if(records[i].flags == RECORD_EMPTY) {
            firstEmptySlot = i;
        }
        else if (SettingsStore_IsRecordValid(&records[i])) {
            settings.midiChannel = records[i].midiChannel;
            settings.octave = records[i].octave;
            settings.velocity = records[i].velocity;

            break;
        }
    }

    return firstEmptySlot;
}

/**
 * Writes sector header flags
 *
 * Note: only FLAGS_EMPTY -> FLAGS_USED -> FLAGS_FULL transitions are valid.
 *
 * @param sector    sector to write the flags to
 * @param flags     new flags value
 * @return true, if completed succesfully
 */
static bool SettingsStore_SetSectorFlags(const Sector_t* sector, const uint32_t flags)
{
    uint8_t buffer[PAGE_SIZE];
    for (unsigned int i = 0; i < sizeof(buffer); buffer[i] = 0) {
        buffer[i] = 0xFF;
    }

    SectorHeader_t* header = (SectorHeader_t*) buffer;
    header->flag1 = (flags >> 8) & 0xFF;
    header->flag2 = flags & 0xFF;

    if (FlashIap_WriteSector(sector, 0, buffer, sizeof(buffer)) != IAP_SUCCESS) {
        return false;
    }
    if(FlashIap_Verify(sector, 0, buffer, sizeof(SectorHeader_t) != IAP_SUCCESS)) {
        return false;
    }

    return true;
}

/**
 * Writes a record containing current set of settings to a given slot
 *
 * @param sector    sector to write to
 * @param slot      index of the slot to write the settings to
 * @return true if completed succesfully
 */
static bool SettingsStore_WriteSettingsAtSlot(const Sector_t* sector, const unsigned int slot)
{
    // note: PAGE_SIZE bytes is a minimum write request size
    uint8_t pageBuffer[PAGE_SIZE];
    for (unsigned int i = 0; i < sizeof(pageBuffer); pageBuffer[i] = 0) {
        pageBuffer[i] = 0xFF;
    }

    // offset in the sector where this record shall be written
    const unsigned int sectorOffset = sizeof(SectorHeader_t) + (RECORD_SIZE * slot);
    const unsigned int offsetInPage = sectorOffset % PAGE_SIZE;
    const unsigned int pageStart = sectorOffset - offsetInPage;

    // record pointer in the buffer - this assumes record never crosses 256-bytes alignment boundry
    SettingsRecord_t* record = (SettingsRecord_t*)(pageBuffer + offsetInPage);

    // prepare record data
    record->flags = RECORD_USED;
    record->midiChannel = settings.midiChannel;
    record->octave = settings.octave;
    record->velocity = settings.velocity;
    record->checksum = 0;

    // update checksum
    uint8_t checksum = 0;
    for (unsigned int i = 0; i < sizeof(SettingsRecord_t); i++) {
        checksum ^= ((uint8_t*)record) [i];
    }
    record->checksum = checksum;

    // now we can write the buffer. Note, that we have to write whole page, but writing 0xFF
    // doesn't do anything to current cell value
    if (FlashIap_WriteSector(sector, pageStart, pageBuffer, sizeof(pageBuffer)) != IAP_SUCCESS) {
        return false;
    }

    // we verify only the chunk written
    if (FlashIap_Verify(sector, sectorOffset, (uint8_t*) record, sizeof(SettingsRecord_t)) != IAP_SUCCESS) {
        return false;
    }

    return true;
}

/**
 * Performs a sector swap
 *
 * This involves erasing target sector, writing current set of settings
 * as a first record there and marking the record as valid. The the source
 * sector is erased.
 *
 * @param from  sector to swap from
 * @param to    sector to swap to
 * @return true if completed succesfully
 */
static bool SettingsStore_SwapSectors(const Sector_t* from, const Sector_t* to)
{
    // make sure destiation sector is erased
    if (SECTOR_FLAGS(to) != FLAGS_EMPTY) {
        if (FlashIap_EraseSector(to) != IAP_SUCCESS) {
            return false;
        }
    }

    // mark source sector as full
    if (!SettingsStore_SetSectorFlags(from, FLAGS_FULL)) {
        return false;
    }

    // write current settings to the new sector, at the very beginning
    if (!SettingsStore_WriteSettingsAtSlot(to, 0)) {
        return false;
    }

    // mark target sector as used
    if (!SettingsStore_SetSectorFlags(to, FLAGS_USED)) {
        return false;
    }

    // now it is safe to erase the source sector, and we're done
    if(FlashIap_EraseSector(from) != IAP_SUCCESS) {
        return false;
    }

    return true;
}
