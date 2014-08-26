

# ------------------------------------------------------------------------
# Project files
# ------------------------------------------------------------------------

INCLUDE_PATHS = -I. 
OBJS = main.o display.o buttons.o gpio_pin.o settings.o \
       panel.o keyboard.o midi.o

# ------------------------------------------------------------------------
# Files from LPC1343CodeBase library
# ------------------------------------------------------------------------

CODEBASE=LPC1343CodeBase
CB_CORE=$(CODEBASE)/core/

INCLUDE_PATHS += -I$(CODEBASE) -I$(CODEBASE)/core
OBJS += $(CB_CORE)/cpu/cpu.o \
		$(CB_CORE)/gpio/gpio.o \
		$(CB_CORE)/uart/uart.o \
		$(CB_CORE)/uart/uart_buf.o \
		$(CB_CORE)/ssp/ssp.o \
		$(CB_CORE)/timer16/timer16.o

LPC1XXX_PATH = $(CODEBASE)/lpc1xxx
OBJS += $(LPC1XXX_PATH)/LPC13xx_handlers.o $(LPC1XXX_PATH)/LPC1xxx_startup.o

# ------------------------------------------------------------------------
# GNU GCC toolkit settings
# ------------------------------------------------------------------------

CROSS_COMPILE = arm-none-eabi-
AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
OUTFILE = firmware
LPCRC = python ./tools/lpcrc.py

# ------------------------------------------------------------------------
# Compiler settings, parameters and flags
# ------------------------------------------------------------------------

CPU_TYPE = cortex-m3
LD_SCRIPT = LPC1343_flash.ld

COMMON_FLAGS = -c -g -O0 $(INCLUDE_PATHS) -std=c99 -Wall -mthumb \
	-ffunction-sections -fdata-sections -fmessage-length=0 \
	-mcpu=$(CPU_TYPE) 

CFLAGS  = $(COMMON_FLAGS) -DTARGET=$(TARGET) -fno-builtin
ASFLAGS = $(COMMON_FLAGS) -D__ASSEMBLY__ -x assembler-with-cpp
LDFLAGS = -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections -Wl,-Map=$(OUTFILE).map -g
LDLIBS  = -lm
OCFLAGS = --strip-unneeded

all: firmware

%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $<

firmware: $(OBJS) $(SYS_OBJS)
	$(LD) $(LDFLAGS) -T $(LD_SCRIPT) -o $(OUTFILE).elf $(OBJS) $(LDLIBS)
	-@echo ""
	$(SIZE) $(OUTFILE).elf
	-@echo ""
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUTFILE).elf $(OUTFILE).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUTFILE).elf $(OUTFILE).hex
	-@echo ""
	$(LPCRC) firmware.bin

clean:
	rm -f $(OBJS) $(LD_TEMP) $(OUTFILE).elf $(OUTFILE).bin $(OUTFILE).hex
