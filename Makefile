.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is not set)
endif

ifeq ($(strip $(DEVKITPPC)),)
$(error DEVKITPPC is not set)
endif

TARGET := ransom-wii

CC := $(DEVKITPPC)/bin/powerpc-eabi-gcc
LD := $(DEVKITPPC)/bin/powerpc-eabi-ld
ELF2DOL := $(DEVKITPRO)/tools/bin/elf2dol

LIBOGC_INC := $(DEVKITPRO)/libogc/include
LIBOGC_LIB := $(DEVKITPRO)/libogc/lib/wii

CFLAGS := -O2
CFLAGS += -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -DGEKKO
CFLAGS += -mrvl
CFLAGS += -mcpu=750
CFLAGS += -meabi
CFLAGS += -mhard-float
CFLAGS += -I$(LIBOGC_INC)
CFLAGS += -I.

LDFLAGS := -DGEKKO
LDFLAGS += -mrvl
LDFLAGS += -mcpu=750
LDFLAGS += -meabi
LDFLAGS += -mhard-float
LDFLAGS += -Wl,--gc-sections

LIBS := -L$(LIBOGC_LIB)
LIBS += -lwiiuse
LIBS += -lbte
LIBS += -lfat
LIBS += -logc
LIBS += -lasnd
LIBS += -lm

SOUND_RAW := $(wildcard assets/sounds/*.raw)
SOUND_OBJS := $(SOUND_RAW:.raw=.o)
OBJECTS := main.o game.o sound.o $(SOUND_OBJS)

.PHONY: all
all: $(TARGET).dol

main.o: main.c game.h sound.h
	@echo "Compiling main.c"
	$(CC) $(CFLAGS) -c main.c -o main.o

game.o: game.c game.h sound.h
	@echo "Compiling game.c"
	$(CC) $(CFLAGS) -c game.c -o game.o

sound.o: sound.c sound.h
	@echo "Compiling sound.c"
	$(CC) $(CFLAGS) -c sound.c -o sound.o

# Convert each raw PCM file into a relocatable object. This keeps the
# original samples inside the DOL without requiring a filesystem at runtime.
assets/sounds/%.o: assets/sounds/%.raw
	@echo "Embedding sound $<"
	$(LD) -r -b binary -o $@ $<

$(TARGET).elf: $(OBJECTS)
	@echo "Linking $(TARGET).elf"
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(TARGET).elf

$(TARGET).dol: $(TARGET).elf
	@echo "Creating $(TARGET).dol"
	test -x "$(ELF2DOL)"
	"$(ELF2DOL)" "$(TARGET).elf" "$(TARGET).dol"

.PHONY: clean
clean:
	rm -f main.o
	rm -f game.o
	rm -f sound.o
	rm -f assets/sounds/*.o
	rm -f $(TARGET).elf
	rm -f $(TARGET).dol
