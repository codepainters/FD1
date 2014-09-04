/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __SETTINGS_STORE_H__
#define __SETTINGS_STORE_H__

/**
 * Initializes settings Flash store
 *
 * During initialization Flash sectors are formatted if necessary,
 * settings are copied from Flash to global settings variable.
 */
void SettingsStore_Init();

/**
 * Save current settings to Flash
 *
 * Note: this is a synchronous operation with interrupts disabled.
 */
void SettingsStore_Save();

#endif // __SETTINGS_STORE_H__
