.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is not set)
endif

ifeq ($(strip $(DEVKITPPC)),)
$(error DEVKITPPC is not set)
endif

TARGET := ransom-wii

CC := $(DEVKITPPC)/bin/powerpc-eabi-gcc
ELF2DOL := $(DEVKITPPC)/bin/elf2dol

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
LIBS += -lm

OBJECTS := main.o game.o

.PHONY: all
all: $(TARGET).dol

main.o: main.c game.h
	@echo "Compiling main.c"
	$(CC) $(CFLAGS) -c main.c -o main.o

game.o: game.c game.h
	@echo "Compiling game.c"
	$(CC) $(CFLAGS) -c game.c -o game.o

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
	rm -f $(TARGET).elf
	rm -f $(TARGET).dol
