/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __MIDI_H__
#define __MIDI_H__

#include "sysdefs.h"

void MIDI_Init();

/**
 * Queue NOTE ON message
 *
 * Can be called from within interrupt context.
 *
 * @return  returns true, if message was succesfully queued, false on FIFO overrun
 */
bool MIDI_QueueNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);

/**
 * Queue NOTE OFF message
 *
 * Can be called from within interrupt context.
 */
void MIDI_QueueNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);

/**
 * Send any pending MIDI bytes
 */
void MIDI_SendQueued();

#endif // __MIDI_H__
