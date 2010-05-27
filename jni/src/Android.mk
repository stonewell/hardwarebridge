LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

NDK_APP_DST_DIR := $(NDK_PROJECT_PATH)/bin/$(TARGET_ARCH_ABI)

LOCAL_MODULE    := hardware_bridge
LOCAL_SRC_FILES := hardware_bridge.c

include $(BUILD_EXECUTABLE)
