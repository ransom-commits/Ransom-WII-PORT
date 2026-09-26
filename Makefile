.SUFFIXES:

ifeq ($(strip $(DEVKITPPC)),)
$(error "DEVKITPPC is not set")
endif

include $(DEVKITPPC)/wii_rules

TARGET := ransom-wii

CFILES := main.c game.c
OFILES := main.o game.o

CFLAGS := -O2 -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += $(MACHDEP)

# libogc headers
INCLUDE := -I$(LIBOGC_INC)
INCLUDE += -I.

# libogc Wii libraries
LIBPATHS := -L$(LIBOGC_LIB)

LIBS := -lwiiuse
LIBS += -lbte
LIBS += -lfat
LIBS += -logc
LIBS += -lm

LDFLAGS := $(MACHDEP)
LDFLAGS += -Wl,--gc-sections

.PHONY: all clean

all: $(TARGET).dol

main.o: main.c game.h
	@echo "Compiling main.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c -o main.o

game.o: game.c game.h
	@echo "Compiling game.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c game.c -o game.o

$(TARGET).elf: $(OFILES)
	@echo "Linking $(TARGET).elf"
	$(CC) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $(TARGET).elf

$(TARGET).dol: $(TARGET).elf
	@echo "Creating $(TARGET).dol"
	$(ELF2DOL) $(TARGET).elf $(TARGET).dol

clean:
	rm -f main.o
	rm -f game.o
	rm -f $(TARGET).elf
	rm -f $(TARGET).dol
