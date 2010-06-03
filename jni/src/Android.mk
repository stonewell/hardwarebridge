LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

NDK_APP_DST_DIR := $(NDK_PROJECT_PATH)/bin/$(TARGET_ARCH_ABI)
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog

LOCAL_MODULE    := hardware_bridge
LOCAL_SRC_FILES := \
	hardware_bridge.h \
	hardware_bridge.cpp \
	wifi.h \
	wifi.cpp \
	vendor.h \
	vendor.cpp \
	vibrator.h \
	vibrator.cpp \
	IHardwareBridge.h \
	IHardwareBridge.cpp \
	flashlight.h \
	flashlight.cpp


include $(BUILD_EXECUTABLE)
