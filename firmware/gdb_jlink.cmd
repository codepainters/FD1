target remote localhost:2331
monitor flash device = LPC1343
monitor flash download = 1
monitor reset
load firmware.elf
