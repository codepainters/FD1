/**
 * Definitions for LPC1343 - 32kB Flash, 8kB RAM
 *
 * At the beginning of RAM there are 384 bytes reserved 
 * for USB driver. 
 */

MEMORY
{
  flash(rx): ORIGIN = 0x00000000,       LENGTH = 32K
  sram(rwx): ORIGIN = 0x10000000 + 384, LENGTH = 8K - 384
}

INCLUDE LPC1343CodeBase/lpc1xxx/linkscript.ld
