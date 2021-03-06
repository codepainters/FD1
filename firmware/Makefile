

# ------------------------------------------------------------------------
# Project files
# ------------------------------------------------------------------------

INCLUDE_PATHS = -Isrc 
OBJS = $(addprefix objs/,main.o display.o buttons.o gpio_pin.o \
	   settings.o settings_store.o panel.o keyboard.o midi.o flash_iap.o)

vpath %.c	src
vpath %.h	src

# ------------------------------------------------------------------------
# Files from LPC1343CodeBase library
# ------------------------------------------------------------------------

LPC1XXX_PATH = lpc1xxx

INCLUDE_PATHS += -I$(LPC1XXX_PATH)
OBJS += $(addprefix objs/, $(LPC1XXX_PATH)/LPC13xx_handlers.o \
						   $(LPC1XXX_PATH)/LPC1xxx_startup.o)

vpath %.c	$(LPC1XXX_PATH)
vpath %.h	$(LPC1XXX_PATH)

# ------------------------------------------------------------------------
# GNU GCC toolkit settings
# ------------------------------------------------------------------------

ARM_TOOLKIT_PATH ?=
CROSS_COMPILE = $(TOOLKIT_PATH)arm-none-eabi-
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

$(OBJS): | objs

objs:
	mkdir -p objs
	mkdir -p objs/$(LPC1XXX_PATH)

objs/%.o : %.c 
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
	rm -f $(OBJS) $(LD_TEMP) $(OUTFILE).elf $(OUTFILE).bin $(OUTFILE).hex $(OUTFILE).map
