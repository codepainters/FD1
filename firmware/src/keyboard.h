/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

// Note: keyboard module calls MIDI_QueueNoteOn/Off directly, in the context of
// Keyboard_TimerTick

void Keyboard_Init();

void Keyboard_TimerTick();

#endif // __KEYBOARD_H__
