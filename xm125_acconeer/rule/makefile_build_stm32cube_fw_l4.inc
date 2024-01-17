CFLAGS += \
    -I$(STM32CUBE_FW_L4_ROOT)/Drivers/CMSIS/Include \
    -I$(STM32CUBE_FW_L4_ROOT)/Drivers/STM32L4xx_HAL_Driver/Inc \
    -I$(STM32CUBE_FW_L4_ROOT)/Drivers/CMSIS/Device/ST/STM32L4xx/Include

vpath %.c $(STM32CUBE_FW_L4_ROOT)/Drivers/STM32L4xx_HAL_Driver/Src

STM32_CUBE_DRIVER_SOURCES = \
    stm32l4xx_hal.c \
    stm32l4xx_hal_cortex.c \
    stm32l4xx_hal_dma.c \
    stm32l4xx_hal_gpio.c \
    stm32l4xx_hal_i2c.c \
    stm32l4xx_hal_i2c_ex.c \
    stm32l4xx_hal_rcc.c \
    stm32l4xx_hal_rcc_ex.c \
    stm32l4xx_hal_rtc.c \
    stm32l4xx_hal_pwr.c \
    stm32l4xx_hal_pwr_ex.c \
    stm32l4xx_hal_rtc.c \
    stm32l4xx_hal_rtc_ex.c \
    stm32l4xx_hal_spi.c \
    stm32l4xx_hal_uart.c \
    stm32l4xx_hal_uart_ex.c

STM32_CUBE_DRIVER_OBJECTS = $(addprefix $(OUT_OBJ_DIR)/,$(patsubst %.c,%.o,$(STM32_CUBE_DRIVER_SOURCES)))

ifneq (,$(findstring gcc,$(TOOLS_CC)))
$(foreach object, $(STM32_CUBE_DRIVER_OBJECTS), $(eval CFLAGS-$(object) += -Wno-pedantic))
endif
ifneq (,$(findstring clang,$(TOOLS_CC)))
$(foreach object, $(STM32_CUBE_DRIVER_OBJECTS), $(eval CFLAGS-$(object) += -Wno-embedded-directive))
endif

$(OUT_LIB_DIR)/libstm32cube_fw_l4.a : $(STM32_CUBE_DRIVER_OBJECTS)
	@echo "    Creating archive $(notdir $@)"
	$(SUPPRESS)rm -f $@
	$(SUPPRESS)$(TOOLS_AR) $(ARFLAGS) $@ $^

BUILD_LIBS += $(OUT_LIB_DIR)/libstm32cube_fw_l4.a
