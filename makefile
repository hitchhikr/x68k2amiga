TARGET = x68k2amiga
OBJS = x68k2amiga.o depackers.o
LIB_PATH = 
CFLAGS = -Wall -Os -fomit-frame-pointer -fno-strict-aliasing -m68000 -noixemul
XTRA_INCLUDES = -I./

CC = m68k-amigaos-gcc
CPP = m68k-amigaos-g++
LD = m68k-amigaos-ld
AS = m68k-amigaos-as
RM = rm
AR = m68k-amigaos-ar
RANLIB = m68k-amigaos-ranlib

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -s -noixemul -o $@ $(OBJS)

%.o: %.s
	$(AS) -m68000 -o $@ $(ASFLAGS) $<

%.o: %.c
	$(CC) $(CC_INCLUDE) $(CFLAGS) -c -o $@ $<

.PHONY : clean

clean :
	-$(RM) -f $(OBJS) $(TARGET)
