#include "midi.h"
#include "uart/uart.h"

#define BUFFER_SIZE     (32)

// FIFO buffer
static uint8_t fifo[BUFFER_SIZE];

static unsigned int fifoWr = 0;
static unsigned int fifoRd = 0;
static unsigned int fifoCount = 0;

#define NOTE_ON     (0x90)
#define NOTE_OFF    (0x80)

static bool MIDI_QueueNoteMsg(unsigned int msg, unsigned int note, unsigned int velocity);

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
    // TODO: take care of concurrency
    while (fifoCount > 0) {
        // Note: this part is the only code being executed outside of the interrupt context,
        // so we can do it in a blocking manner
        uartSendByte(fifo[fifoRd++]);
        fifoRd %= BUFFER_SIZE;
        fifoCount--;
    }
}

static bool MIDI_QueueNoteMsg(unsigned int msg, unsigned int note, unsigned int velocity)
{
    if (fifoCount + 3 > BUFFER_SIZE)
        return false;

    fifo[fifoWr++] = msg;
    fifoWr %= BUFFER_SIZE;

    fifo[fifoWr++] = note;
    fifoWr %= BUFFER_SIZE;

    fifo[fifoWr++] = velocity;
    fifoWr %= BUFFER_SIZE;

    fifoCount += 3;

    return true;
}
