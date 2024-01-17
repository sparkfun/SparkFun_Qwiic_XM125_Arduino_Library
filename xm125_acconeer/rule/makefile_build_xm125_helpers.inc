# Generic helper functions for creating targets, etc

LDLIBS := -Wl,--start-group $(LDLIBS) -Wl,--end-group

# Create a target
# (1) target name
define define_target
$(eval OBJECTS := $(addprefix $(OUT_OBJ_DIR)/, $(notdir $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(SOURCES)))))) \
$(eval .PHONY: $(1)) \
$(eval $(1): $(OUT_DIR)/$(1).elf $(OUT_DIR)/$(1).hex) \
$(eval $(OUT_DIR)/$(1).elf: $(OBJECTS) $(EXTRA_OBJECTS) $(BUILD_LIBS) | $$(OUT_DIR)
	@echo "Linking $$@"
	$$(SUPPRESS)$$(TOOLS_LD) -Wl,-Map=$$(basename $$@).map $$(LDFLAGS) $$^ $$(LDLIBS) -o $$@
	$$(SUPPRESS)$$(TOOLS_OBJDUMP) -h -S $$@ > $$(basename $$@).list
	$$(SUPPRESS)$$(TOOLS_OBJCOPY) -O binary -S $$@ $$(basename $$@).bin
	$$(SUPPRESS)$$(TOOLS_SIZE) -t $$@ > $$(basename $$@)_size.txt) \
$(eval $(OUT_DIR)/$(1).hex: $(OUT_DIR)/$(1).elf
	$$(SUPPRESS)$$(TOOLS_OBJCOPY) -O ihex -S $$^ $$@)
endef

# Create a jlink flash target based on the target name
# (1) target name
define define_jlink_flash_target
.PHONY: flash_jlink_$(strip $(1))
flash_jlink_$(strip $(1)): $(OUT_DIR)/$(strip $(1)).hex
	@echo Flashing: $$^
	$(SUPPRESS)rm -f $(OUT_DIR)/$(strip $(1)).jlink
	$(SUPPRESS)for ARG in "device STM32L431CB" "si swd" "speed 4000" "erase" "loadfile $$^" r go exit ; do \
		echo $$$$ARG >> $(OUT_DIR)/$(strip $(1)).jlink ; \
	done ; \
	JLinkExe -CommandFile $(OUT_DIR)/$(strip $(1)).jlink
	$(SUPPRESS)rm -f $(OUT_DIR)/$(strip $(1)).jlink
endef
