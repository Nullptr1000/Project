LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := unityesp

LOCAL_CFLAGS := -std=c++20 -O2 -fvisibility=hidden -Wall

LOCAL_LDLIBS := -llog -lEGL -lGLESv2 -ldl -landroid

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := \
    main.cpp \
    draw.cpp \
    esp.cpp \
    aimbot.cpp \
    hooks.cpp

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
