TOOLS_PREFIX := $(GNU_INSTALL_ROOT)/arm-none-eabi-
TOOLS_AR         := $(TOOLS_PREFIX)ar
TOOLS_AS         := $(TOOLS_PREFIX)as
TOOLS_CC         := $(TOOLS_PREFIX)gcc
TOOLS_OBJDUMP    := $(TOOLS_PREFIX)objdump
TOOLS_OBJCOPY    := $(TOOLS_PREFIX)objcopy
TOOLS_SIZE       := $(TOOLS_PREFIX)size
TOOLS_LD         := $(TOOLS_PREFIX)gcc

TARGET_ARCHITECTURE := cm4

CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DTARGET_ARCH_cm4 -DFLOAT_ABI_HARD -std=c99 -pedantic -Wall -Werror -Wextra -Wdouble-promotion -Wstrict-prototypes -Wcast-qual -Wmissing-prototypes -Winit-self -Wpointer-arith -Wshadow -MMD -MP -O2 -g -fno-math-errno -ffunction-sections -fdata-sections -flto=auto -ffat-lto-objects

# Override optimization level
ifneq ($(ACC_CFG_OPTIM_LEVEL),)
	CFLAGS  += $(ACC_CFG_OPTIM_LEVEL)
endif

ASFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

LDFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -L$(OUT_DIR) -Werror -Wl,--gc-sections

ARFLAGS := cr --plugin `find ${GNU_INSTALL_ROOT}/../ -name liblto_plugin.so`

LDLIBS += -lm
