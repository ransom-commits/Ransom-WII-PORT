.SUFFIXES:

ifeq ($(strip $(DEVKITPPC)),)
$(error "DEVKITPPC is not set")
endif

include $(DEVKITPPC)/wii_rules

TARGET := ransom-wii

CFILES := main.c game.c
OFILES := $(CFILES:.c=.o)

CFLAGS := -O2 -Wall -ffunction-sections -fdata-sections
CFLAGS += $(MACHDEP)

INCLUDE := -I.

LIBS := -lwiiuse -lbte -lfat -logc -lm

LIBPATHS := -L$(LIBOGC_LIB)

LDFLAGS := $(MACHDEP)
LDFLAGS += -Wl,--gc-sections

.PHONY: all clean

all: $(TARGET).dol

%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET).elf: $(OFILES)
	@echo "Linking $@"
	$(CC) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

$(TARGET).dol: $(TARGET).elf
	@echo "Creating DOL"
	$(ELF2DOL) $< $@

clean:
	rm -f $(OFILES)
	rm -f $(TARGET).elf
	rm -f $(TARGET).dol
