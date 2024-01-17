# Copyright (c) Acconeer AB, 2022-2023
# All rights reserved

.DEFAULT_GOAL := all

TARGETS := \
    example_bring_up \
    example_control_helper \
    example_diagnostic_test \
    example_detector_distance \
    example_detector_distance_low_power_hibernate \
    example_detector_distance_low_power_off \
    example_detector_distance_with_iq_data_print \
    example_detector_presence \
    example_detector_presence_low_power_hibernate \
    example_detector_presence_low_power_off \
    example_detector_presence_multiple_configurations \
    example_processing_amplitude \
    example_processing_coherent_mean \
    example_processing_noncoherent_mean \
    example_processing_peak_interpolation \
    example_processing_static_presence \
    example_processing_subtract_adaptive_bg \
    example_service \
    example_service_low_power_sensor_disable \
    example_service_low_power_sensor_hibernate \
    example_service_multiple_configurations \
    example_service_sensor_disable \
    example_service_sensor_hibernate \
    example_service_sensor_off \
    example_service_subsweeps \
    example_surface_velocity \
    i2c_distance_detector \
    i2c_presence_detector \
    ref_app_breathing \
    ref_app_smart_presence \
    ref_app_tank_level

# Variable to modify output messages
ifneq ($(V),)
SUPPRESS :=
else
SUPPRESS := @
endif

OUT_DIR     := out
OUT_OBJ_DIR := $(OUT_DIR)/obj
OUT_LIB_DIR := $(OUT_DIR)/lib

INC_FOLDERS := -I. -IInc

vpath %.c Src
vpath %.c Src/algorithms
vpath %.c Src/applications
vpath %.c Src/applications/i2c
vpath %.c Src/integration
vpath %.c Src/examples/getting_started
vpath %.c Src/examples/advanced_control
vpath %.c Src/examples/helper
vpath %.c Src/examples/processing
vpath %.c Src/examples/troubleshooting
vpath %.c Src/use_cases/example_apps
vpath %.c Src/use_cases/reference_apps
vpath %.s Src

LDLIBS := \
    -lacconeer_a121 \
    -lacc_detector_distance_a121 \
    -lacc_detector_presence_a121

# Important to have startup_stm32l431xx.s first when using -flto
# see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83967
STM32_CUBE_GENERATED_FILES := \
    startup_stm32l431xx.s \
    main.c \
    stm32l4xx_hal_msp.c \
    stm32l4xx_it.c \
    system_stm32l4xx.c \
    sysmem.c

STM32_CUBE_INTEGRATION_FILES := \
    acc_hal_integration_stm32cube_xm.c \
    syscalls.c

CONTROL_HELPER_FILES := \
    acc_control_helper.c \
    acc_processing_helpers.c

RSS_INTEGRATION_FILES := \
    acc_integration_log.c \
    acc_integration_stm32.c

SOURCES_EXAMPLE_BRING_UP := \
    example_bring_up.c

SOURCES_EXAMPLE_CONTROL_HELPER := \
    example_control_helper.c

SOURCES_EXAMPLE_DIAGNOSTIC_TEST := \
    example_diagnostic_test.c

SOURCES_EXAMPLE_DETECTOR_DISTANCE := \
    example_detector_distance.c

SOURCES_EXAMPLE_DETECTOR_DISTANCE_LOW_POWER_HIBERNATE := \
    example_detector_distance_low_power_hibernate.c

SOURCES_EXAMPLE_DETECTOR_DISTANCE_LOW_POWER_OFF := \
    example_detector_distance_low_power_off.c

SOURCES_EXAMPLE_DETECTOR_DISTANCE_WITH_IQ_DATA_PRINT := \
    example_detector_distance_with_iq_data_print.c

SOURCES_EXAMPLE_DETECTOR_PRESENCE := \
    example_detector_presence.c

SOURCES_EXAMPLE_DETECTOR_PRESENCE_LOW_POWER_HIBERNATE := \
    example_detector_presence_low_power_hibernate.c

SOURCES_EXAMPLE_DETECTOR_PRESENCE_LOW_POWER_OFF := \
    example_detector_presence_low_power_off.c

SOURCES_EXAMPLE_DETECTOR_PRESENCE_MULTIPLE_CONFIGURATIONS := \
    example_detector_presence_multiple_configurations.c

SOURCES_EXAMPLE_SERVICE_LOW_POWER_SENSOR_DISABLE := \
    example_service_low_power_sensor_disable.c

SOURCES_EXAMPLE_SERVICE_LOW_POWER_SENSOR_HIBERNATE := \
    example_service_low_power_sensor_hibernate.c

SOURCES_EXAMPLE_PROCESSING_AMPLITUDE := \
    example_processing_amplitude.c

SOURCES_EXAMPLE_PROCESSING_COHERENT_MEAN := \
    example_processing_coherent_mean.c

SOURCES_EXAMPLE_PROCESSING_NONCOHERENT_MEAN := \
    example_processing_noncoherent_mean.c

SOURCES_EXAMPLE_PROCESSING_PEAK_INTERPOLATION := \
    example_processing_peak_interpolation.c

SOURCES_EXAMPLE_PROCESSING_STATIC_PRESENCE := \
    example_processing_static_presence.c

SOURCES_EXAMPLE_PROCESSING_SUBTRACT_ADAPTIVE_BG := \
    example_processing_subtract_adaptive_bg.c

SOURCES_EXAMPLE_SERVICE := \
    example_service.c

SOURCES_EXAMPLE_SERVICE_MULTIPLE_CONFIGURATIONS := \
    example_service_multiple_configurations.c

SOURCES_EXAMPLE_SERVICE_SENSOR_DISABLE := \
    example_service_sensor_disable.c

SOURCES_EXAMPLE_SERVICE_SENSOR_HIBERNATE := \
    example_service_sensor_hibernate.c

SOURCES_EXAMPLE_SERVICE_SENSOR_OFF := \
    example_service_sensor_off.c

SOURCES_EXAMPLE_SERVICE_SUBSWEEPS := \
    example_service_subsweeps.c

SOURCES_EXAMPLE_SURFACE_VELOCITY := \
    acc_algorithm.c \
    acc_processing_helpers.c \
    example_surface_velocity.c

SOURCES_I2C_DISTANCE_DETECTOR := \
    acc_integration_cortex.c \
    acc_reg_protocol.c \
    distance_reg_protocol.c \
    distance_reg_protocol_access.c \
    i2c_application_system_stm32.c \
    i2c_distance_detector.c

SOURCES_I2C_PRESENCE_DETECTOR := \
    acc_integration_cortex.c \
    acc_reg_protocol.c \
    presence_reg_protocol.c \
    presence_reg_protocol_access.c \
    i2c_application_system_stm32.c \
    i2c_presence_detector.c

SOURCES_REF_APP_BREATHING := \
    acc_algorithm.c \
    ref_app_breathing.c

SOURCES_REF_APP_SMART_PRESENCE := \
    ref_app_smart_presence.c

SOURCES_REF_APP_TANK_LEVEL := \
    ref_app_tank_level.c

_SOURCES := $(STM32_CUBE_INTEGRATION_FILES) $(STM32_CUBE_GENERATED_FILES) $(RSS_INTEGRATION_FILES) $(CONTROL_HELPER_FILES)

include $(sort $(wildcard rule/makefile_target_*.inc))
include $(sort $(wildcard rule/makefile_define_*.inc))
include $(sort $(wildcard rule/makefile_build_*.inc))

# Use DMA for SPI transfers
CFLAGS += -DA121_USE_SPI_DMA

CFLAGS-$(OUT_OBJ_DIR)/syscalls.o += -Wno-missing-prototypes -fno-lto
CFLAGS-$(OUT_OBJ_DIR)/sysmem.o = -std=gnu11 -Wno-strict-prototypes -Wno-pedantic -Wno-missing-prototypes -fno-lto
CFLAGS-$(OUT_OBJ_DIR)/printf.o = -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL -fno-lto

.PHONY: all
all: $(TARGETS) $(OUT_DIR)/acc_exploration_server_a121.elf

$(BUILD_LIBS) : | $(OUT_LIB_DIR)

$(OUT_OBJ_DIR)/%.o: %.c | $(OUT_OBJ_DIR)
	@echo "Compiling $(notdir $<)"
	$(SUPPRESS)$(TOOLS_CC) $^ -c $(CFLAGS) $(CFLAGS-$@) $(INC_FOLDERS) -o $@

$(OUT_OBJ_DIR)/%.o: %.s | $(OUT_OBJ_DIR)
	@echo "Assembling $(notdir $<)"
	$(SUPPRESS)$(TOOLS_AS) $^ $(ASFLAGS) -o $@

$(OUT_LIB_DIR):
	$(SUPPRESS)mkdir -p $@

$(OUT_OBJ_DIR):
	$(SUPPRESS)mkdir -p $@

$(OUT_DIR):
	$(SUPPRESS)mkdir -p $@

# Function to convert string to uppercase
# Parameter 1: string to convert to uppercase
uc = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst \
	h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst \
	p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst \
	x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))

# Create all target recipes
$(foreach target, $(TARGETS), $(eval SOURCES := $(_SOURCES) $($(addsuffix $(call uc,$(target)),SOURCES_))) $(call define_target,$(target)))

# Create jlink flash targets
$(foreach target, $(TARGETS), $(eval $(call define_jlink_flash_target, $(target))))

.PHONY : clean
clean:
	$(SUPPRESS)rm -rf out

###
### XM125 EXPLORATION SERVER
###

EXPLORATION_SERVER_LDLIBS := \
			-lacconeer_exploration_server_a121 \
			-lacconeer_a121

EXPLORATION_SERVER_SOURCES := \
	${STM32_CUBE_GENERATED_FILES} \
	${STM32_CUBE_INTEGRATION_FILES} \
	acc_hal_integration_stm32cube_xm.c \
	acc_exploration_server_stm32.c \
	acc_integration_stm32.c

EXPLORATION_SERVER_OBJECTS := $(addprefix $(OUT_OBJ_DIR)/, $(notdir $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(EXPLORATION_SERVER_SOURCES)))))

EXPLORATION_SERVER_LDFLAGS := -u _printf_float

$(OUT_DIR)/acc_exploration_server_a121.elf: $(EXPLORATION_SERVER_OBJECTS) $(BUILD_LIBS) | $(OUT_DIR)
	@echo "Linking $@"
	$(SUPPRESS)$(TOOLS_LD) $(LDFLAGS) ${EXPLORATION_SERVER_LDFLAGS} $^ ${EXPLORATION_SERVER_LDLIBS} -o $@
	$(SUPPRESS)$(TOOLS_OBJDUMP) -h -S $@ > $(basename $@).lss
	$(SUPPRESS)$(TOOLS_SIZE) -t $@ > $(basename $@)_size.txt
	$(SUPPRESS)$(TOOLS_OBJCOPY) -O ihex $@ $(basename $@).hex
	$(SUPPRESS)$(TOOLS_OBJCOPY) -O binary $@ $(basename $@).bin

# Create jlink flash targets
$(eval $(call define_jlink_flash_target, acc_exploration_server_a121))
