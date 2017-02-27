# Copyright (c) 2012, Google Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# ndk-build module definition for the Google Breakpad client library
#
# To use this file, do the following:
#
#   1/ Include this file from your own Android.mk, either directly
#      or with through the NDK's import-module function.
#
#   2/ Use the client static library in your project with:
#
#      LOCAL_STATIC_LIBRARIES += breakpad_client
#
#   3/ In your source code, include "src/client/linux/exception_handler.h"
#      and use the Linux instructions to use it.
#
# This module works with either the STLport or GNU libstdc++, but you need
# to select one in your Application.mk
#

# The top Google Breakpad directory.
# We assume this Android.mk to be under 'android/google_breakpad'

MY_LOCAL_PATH := $(call my-dir)

# Defube the client library module, as a simple static library that
# exports the right include path / linker flags to its users.

include $(CLEAR_VARS)

LOCAL_MODULE := breakpad_client

LOCAL_CPP_EXTENSION := .cc

# Breakpad uses inline ARM assembly that requires the library
# to be built in ARM mode. Otherwise, the build will fail with
# cryptic assembler messages like:
#   Compile++ thumb  : google_breakpad_client <= crash_generation_client.cc
#   /tmp/cc8aMSoD.s: Assembler messages:
#   /tmp/cc8aMSoD.s:132: Error: invalid immediate: 288 is out of range
#   /tmp/cc8aMSoD.s:244: Error: invalid immediate: 296 is out of range
LOCAL_ARM_MODE := arm

# List of client source files, directly taken from Makefile.am
LOCAL_SRC_FILES := \
    googlebreakpad/src/client/linux/crash_generation/crash_generation_client.cc \
    googlebreakpad/src/client/linux/handler/exception_handler.cc \
    googlebreakpad/src/client/linux/handler/minidump_descriptor.cc \
    googlebreakpad/src/client/linux/log/log.cc \
    googlebreakpad/src/client/linux/minidump_writer/linux_dumper.cc \
    googlebreakpad/src/client/linux/minidump_writer/linux_ptrace_dumper.cc \
    googlebreakpad/src/client/linux/minidump_writer/minidump_writer.cc \
	googlebreakpad/src/client/linux/microdump_writer/microdump_writer.cc \
	googlebreakpad/src/client/linux/dump_writer_common/ucontext_reader.cc \
	googlebreakpad/src/client/linux/dump_writer_common/seccomp_unwinder.cc \
	googlebreakpad/src/client/linux/dump_writer_common/thread_info.cc \
    googlebreakpad/src/client/minidump_file_writer.cc \
    googlebreakpad/src/common/android/breakpad_getcontext.S \
    googlebreakpad/src/common/convert_UTF.c \
    googlebreakpad/src/common/md5.cc \
    googlebreakpad/src/common/string_conversion.cc \
    googlebreakpad/src/common/linux/elfutils.cc \
    googlebreakpad/src/common/linux/file_id.cc \
    googlebreakpad/src/common/linux/guid_creator.cc \
    googlebreakpad/src/common/linux/linux_libc_support.cc \
    googlebreakpad/src/common/linux/memory_mapped_file.cc \
    googlebreakpad/src/common/linux/safe_readlink.cc

LOCAL_C_INCLUDES        := $(MY_LOCAL_PATH)/src/common/android/include \
                           $(MY_LOCAL_PATH)/src

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_LDLIBS     := -llog

include $(BUILD_STATIC_LIBRARY)

# Done.