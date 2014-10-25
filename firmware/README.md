FD-1 Firmware
============

This directory contains the controller's firmware source code, written in plain C. A few files in `lpc1xxx` directory (namely: linker script, header file with register definitions, startup routine) were "borrowed" from [LPC1343CodeBase](http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/LPC1343CodeBase.aspx) project.

Toolkit
---------

To build the firmware you need a GCC cross-toolchain for ARM. The toolkit available [here](https://launchpad.net/gcc-arm-embedded/+download) was used for development, but any recent GCC toolkit will do. Note: builidng is possible on Mac and Linux, **no Windows support**. Sorry!

The project's makefile expects the `PATH` environment variable to include toolkit's `bin` directory, so `arm-none-eabi-gcc` can be executed without giving the full path.

Other dependencies
----------------------------

You also need:

 * `GNU make` - v3.81 used for development
 * `python` - for the script calculating the firmware checksum

Building
-----------

It's enough to execute `make` in this directory. You should get the following files:

 * `firmware.elf` - to be used e.g. for debugging or flashing with GDB
 * `firmware.bin` - binary firmware (with correct checksum, as needed by the LPC1343 bootloader)

Flashing
-----------

The easiest way to flash the firmware is to use LPC1343's built-in bootloader's feature - programming via USB mass storage (disk) device. Follow these steps:

 * short the `J6` pins (next to `JTAG` socket) with a jumper
 * connect the USB cable - the device should enumerate as a mass-storage device
 * copy the `firmware.bin` file onto this virtual drive
 * unmount the device
 * disconnect USB cable, remove the jumper

That's it!
