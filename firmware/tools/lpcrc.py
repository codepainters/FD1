#!/usr/bin/python
#
# This tool updates the 2's complement checksum stored in the LPC13xx firmware binary file.
# It is necessary for binaries flashed ofer USB, as NXP's bootloader checks it.
#
# It's a Python rewrite of lpcrc tool from LPC143CodeBase project.
#
# Author: Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
#

from sys import argv, exit
from struct import Struct, pack

HEADER = Struct('<IIIIIII')


if __name__ == '__main__':
    if len(argv) < 2:
        print 'Usage: %s <firmware.bin>' % argv[0]
        exit(1)

    try:
        with open(argv[1], 'rb+') as f:
            data = f.read(HEADER.size)
            if len(data) < HEADER.size:
                raise Exception('could not read required amount of bytes, file too short?')

            # Checksum is calculated over first 7 wrods...
            checksum = ((sum(HEADER.unpack(data)) ^ 0xFFFFFFFF) + 1) & 0xFFFFFFFF

            # ...and stored as the 8th word
            f.write(pack('<I', checksum))
            print 'checksum updated to: 0x%08x' % checksum

    except Exception, e:
        print 'error: ' + str(e)
        exit(1)
