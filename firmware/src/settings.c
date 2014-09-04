/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "settings.h"

// this variable contains current settings
Settings_t settings = {
    .midiChannel = MIDI_CHANNEL_DEFAULT,
    .octave = OCTAVE_SHIFT_DEFAULT,
    .velocity = MIDI_VELOCITY_DEFAULT
};
