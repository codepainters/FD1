/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "midi.h"
#include "display.h"

#include "lpc134x.h"

#define BUFFER_SIZE     (32)

// FIFO buffer
static volatile uint8_t fifo[BUFFER_SIZE];

static volatile unsigned int fifoWr = 0;
static volatile unsigned int fifoRd = 0;

#define NOTE_ON     (0x90)
#define NOTE_OFF    (0x80)

#define UART_CLK_DIV    SCB_UARTCLKDIV_DIV1
#define BAUDRATE        (31250)

static bool MIDI_QueueNoteMsg(unsigned int msg, unsigned int note, unsigned int velocity);
static inline void MIDI_QueueByte(uint8_t b);

void MIDI_Init()
{
    // setup PIO1.7 as TXD
    IOCON_PIO1_7 &= ~IOCON_PIO1_7_FUNC_MASK;
    IOCON_PIO1_7 |= IOCON_PIO1_7_FUNC_UART_TXD;

    // enable UART clock (div by 1)
    SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_UART;
    SCB_UARTCLKDIV = UART_CLK_DIV;

    // configure standard 8N1 mode, enable baud rate updating
    UART_U0LCR = UART_U0LCR_Word_Length_Select_8Chars |
                 UART_U0LCR_Stop_Bit_Select_1Bits |
                 UART_U0LCR_Parity_Disabled |
                 UART_U0LCR_Break_Control_Disabled |
                 UART_U0LCR_Divisor_Latch_Access_Enabled;

    // setup baudrate
    uint32_t fDiv = (((CFG_CPU_CCLK * SCB_SYSAHBCLKDIV) / UART_CLK_DIV) / 16) / BAUDRATE;
    UART_U0DLM = fDiv / 256;
    UART_U0DLL = fDiv % 256;

    // disable baudrate updates again
    UART_U0LCR = UART_U0LCR_Word_Length_Select_8Chars |
                 UART_U0LCR_Stop_Bit_Select_1Bits |
                 UART_U0LCR_Parity_Disabled |
                 UART_U0LCR_Break_Control_Disabled |
                 UART_U0LCR_Divisor_Latch_Access_Disabled;

    // enable and reset the FIFOs
    UART_U0FCR = UART_U0FCR_FIFO_Enabled | UART_U0FCR_Rx_FIFO_Reset | UART_U0FCR_Tx_FIFO_Reset;
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

        while (!(UART_U0LSR & UART_U0LSR_THRE));
        UART_U0THR = fifo[fifoRd];

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
