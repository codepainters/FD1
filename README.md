FD-1 MIDI Foot Keyboard Controller
==================================

FD-1 is a simple MIDI Foot (organ) Keyboard Controller project, 
based on LPC1343 Cortex-M3 microcontroller. 

The design features:

 * separate keyboard and controller modules, connected using an HDMI cable 
   (the cheapest multi-wire cable available)

 * a simple control panel with a 7-segment display and a rotary encoder, 
   allowing for changing the MIDI channel, velocity and octave shift on the fly 
   (note: the keyboard used for the project has only a single NO contact per key,
   thus the keyboard can't be dynamic)

 * persistent storage (in Flash) of the settings

 * USB port, at the moment used for firmware flashing only, there's a plan to 
   include MIDI-over-USB implementation later

 * switching-mode voltage regulator - controller can be powered from 6-12V DC 
   (usual 9V DC power supply recommended, as used for guitar stomp-boxes)

The project repository contains:

 * `hardware` - schematics and PCBs
 * `firmware` - all the C code
 * `datasheets` - datasheets and manuals used during the development

Dedicated project page (with pictures) to follow soon.

All the project files are licensed under Modified BSD License (see LICENSE.txt).

