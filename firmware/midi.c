#include "midi.h"
#include "uart/uart.h"
#include "display.h"

#define BUFFER_SIZE     (32)

// FIFO buffer
static volatile uint8_t fifo[BUFFER_SIZE];

static volatile unsigned int fifoWr = 0;
static volatile unsigned int fifoRd = 0;

#define NOTE_ON     (0x90)
#define NOTE_OFF    (0x80)

static bool MIDI_QueueNoteMsg(unsigned int msg, unsigned int note, unsigned int velocity);
static inline void MIDI_QueueByte(uint8_t b);

void MIDI_Init()
{
    uartInit(31250);
}

bool MIDI_QueueNoteOn(unsigned int channel, unsigned int note, unsigned int velocity)
{
    return MIDI_QueueNoteMsg(NOTE_ON | channel, note, velocity);
}

void MIDI_QueueNoteOff(unsigned int channel, unsigned int note, unsigned int velocity)
{
    MIDI_QueueNoteMsg(NOTE_OFF | channel, note, velocity);
}

void MIDI_SendQueued()
{
    // Note: we use the "one slot unused" FIFO variant, and thus don't need to disable
    // interrupts here
    while (fifoRd != fifoWr) {
        Display_BlinkDP();
        uartSendByte(fifo[fifoRd]);
        fifoRd = (fifoRd + 1) % BUFFER_SIZE;
    }
}

static bool MIDI_QueueNoteMsg(unsigned int msg, unsigned int note, unsigned int velocity)
{
    // Avoid puting partial message in the queue - check if there are 3 bytes left
    int count = (fifoWr + BUFFER_SIZE - fifoRd) % BUFFER_SIZE;
    if (count + 3 > BUFFER_SIZE)
        return false;

    MIDI_QueueByte(msg);
    MIDI_QueueByte(note);
    MIDI_QueueByte(velocity);

    return true;
}

static inline void MIDI_QueueByte(uint8_t b)
{
    fifo[fifoWr] = b;
    fifoWr = (fifoWr + 1) % BUFFER_SIZE;
}

