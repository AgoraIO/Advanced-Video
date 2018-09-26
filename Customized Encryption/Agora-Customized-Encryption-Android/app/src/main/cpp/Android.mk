LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# All of the source files that we will compile.
LOCAL_SRC_FILES := \
	packet_processing_plugin_jni.cpp \

# The JNI headers
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \

LOCAL_LDLIBS := -ldl -llog

LOCAL_MODULE := apm-plugin-packet-processing

include $(BUILD_SHARED_LIBRARY)
