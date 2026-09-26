TARGET := ransom-wii
BUILD := build
SOURCES := source
INCLUDES := include

CFILES := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
OFILES := $(CFILES:%.c=$(BUILD)/%.o)

CFLAGS := -O2 -Wall -mcpu=750 -meabi -mhard-float -ffunction-sections -fdata-sections $(foreach dir,$(INCLUDES),-I$(dir))
LDFLAGS := -Wl,--gc-sections
LIBS := -logc -lwiiuse -lbte -lfat

.PHONY: all clean

all: $(TARGET).dol

$(BUILD):
	mkdir -p $(BUILD)/source

$(BUILD)/%.o: %.c | $(BUILD)
	mkdir -p $(dir $@)
	powerpc-eabi-gcc $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OFILES)
	powerpc-eabi-gcc $(LDFLAGS) $^ $(LIBS) -o $@

$(TARGET).dol: $(TARGET).elf
	elf2dol $< $@

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).dol
