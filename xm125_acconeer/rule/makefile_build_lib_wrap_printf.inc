CFLAGS-$(OUT_OBJ_DIR)/acc_wrap_printf.o += -Wno-missing-prototypes -fno-lto
CFLAGS-$(OUT_OBJ_DIR)/printf.o += -fno-lto

LDFLAGS += -Wl,--wrap=puts -Wl,--wrap=fputs -Wl,--wrap=printf -Wl,--wrap=snprintf -Wl,--wrap=vsnprintf

$(OUT_LIB_DIR)/libwrapprintf.a :  $(OUT_OBJ_DIR)/acc_wrap_printf.o $(OUT_OBJ_DIR)/printf.o
	@echo "    Creating archive $(notdir $@)"
	$(SUPPRESS)rm -f $@
	$(SUPPRESS)$(TOOLS_AR) $(ARFLAGS) $@ $^

BUILD_LIBS += $(OUT_LIB_DIR)/libwrapprintf.a

LDLIBS += -lwrapprintf
