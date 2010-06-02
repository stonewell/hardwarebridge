LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

NDK_APP_DST_DIR := $(NDK_PROJECT_PATH)/bin/$(TARGET_ARCH_ABI)

LOCAL_MODULE    := hardware_bridge
LOCAL_SRC_FILES := hardware_bridge.c \
	cmd_dispatch.c \
	wifi.c \
	vendor.c \
	vibrator.c \
	vendor.h \
	wifi.h \
	hardware_bridge.h \
	cmd_dispatch.h 
LOCAL_CFLAGS    := -Werror -Wall -Werror=declaration-after-statement
LOCAL_LDLIBS    := -llog


include $(BUILD_EXECUTABLE)
