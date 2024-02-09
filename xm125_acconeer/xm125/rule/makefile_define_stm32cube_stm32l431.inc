OPENOCD_CONFIG += -f interface/stlink.cfg -f target/stm32l4x.cfg

LDFLAGS += --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections
LDFLAGS += -TSTM32L431CBYx_FLASH.ld
LDFLAGS += -Llib -L"$(OUT_LIB_DIR)"

CFLAGS += -DSTM32L431xx -DUSE_HAL_DRIVER
CFLAGS-$(OUT_OBJ_DIR)/printf.o = -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL

STM32CUBE_FW_ROOT := $(STM32CUBE_FW_L4_ROOT)

vpath %.s $(STM32CUBE_FW_L4_ROOT)/Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/gcc
