#ifndef __MIDI_H__
#define __MIDI_H__

#include <stdbool.h>

void MIDI_Init();

bool MIDI_QueueNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);

void MIDI_QueueNoteOff(unsigned int channel, unsigned int note);

void MIDI_SendQueued();

#endif // __MIDI_H__
