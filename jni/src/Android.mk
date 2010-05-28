LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

NDK_APP_DST_DIR := $(NDK_PROJECT_PATH)/bin/$(TARGET_ARCH_ABI)

LOCAL_MODULE    := hardware_bridge
LOCAL_SRC_FILES := hardware_bridge.c \
	cmd_dispatch.c \
	wifi.c
LOCAL_CFLAGS    := -Werror -Wall -Werror=declaration-after-statement
LOCAL_LDLIBS    := -llog


include $(BUILD_EXECUTABLE)
