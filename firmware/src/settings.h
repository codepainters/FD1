#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "lpc134x.h"

#define MIDI_CHANNEL_MIN    (0)
#define MIDI_CHANNEL_MAX    (15)

#define OCTAVE_SHIFT_MIN    (0)
#define OCTAVE_SHIFT_MAX    (5)

#define MIDI_VELOCITY_MIN   (0)
#define MIDI_VELOCITY_MAX   (127)

/**
 * This structure contains all the user-settable parameters.
 */
typedef struct Settings_tag {

    // MIDI channel to send the messages to [0..15]
    unsigned int midiChannel;

    // octave shift of the keyboard notes
    unsigned int octave;

    // MIDI notes velocity [0..127]
    unsigned int velocity;

} Settings_t;

extern Settings_t settings;

#endif // __SETTINGS_H__
