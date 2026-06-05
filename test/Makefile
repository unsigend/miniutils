# miniutils - A minimal GNU coreutils implementation
# Copyright (C) 2025 - 2026 Qiu Yixiang
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CUR_DIR       := .
ROOT_DIR      := $(abspath $(CUR_DIR)/..)
CONFIG_PATH   := $(ROOT_DIR)/config
BUILD_PATH    := $(ROOT_DIR)/build
BIN_PATH      := $(BUILD_PATH)/bin
LIB_PATH      := $(BUILD_PATH)/lib
INCLUDE_PATH      := $(ROOT_DIR)/include
EXTERNAL_INC_PATH := $(CUR_DIR)/external/includes
EXTERNAL_SRC_PATH := $(CUR_DIR)/external/src
UNIT_PATH         := $(CUR_DIR)/unit
UNIT_BUILD    := $(CUR_DIR)/build
UNIT_OBJ      := $(UNIT_BUILD)/obj
UNIT_DEP      := $(UNIT_BUILD)/dep
CMD_TESTS     := $(CUR_DIR)/cmd

include $(CONFIG_PATH)/config.mk

ifeq ($(filter $(LIB_BUILD),static shared),)
$(error LIB_BUILD must be static or shared (got $(LIB_BUILD)))
endif

BATS ?= bats

HOST_OS := $(shell uname -s)

ifeq ($(LIB_BUILD),shared)
ifeq ($(HOST_OS),Darwin)
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).dylib
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).so
endif
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).a
endif

UNIT_CC_FLAGS := -std=c11 -Wall -Wextra -Werror -Wshadow
UNIT_CC_FLAGS += -I$(INCLUDE_PATH) -I$(EXTERNAL_INC_PATH)
UNIT_CC_FLAGS += $(addprefix -I,$(shell find $(UNIT_PATH) -type d 2>/dev/null))
ifeq ($(HOST_OS),Linux)
UNIT_CC_FLAGS += -D_XOPEN_SOURCE=700
endif

UNIT_LDFLAGS :=
ifeq ($(DEBUG),1)
UNIT_CC_FLAGS += -g -O0
UNIT_CC_FLAGS += -fsanitize=address,undefined
UNIT_LDFLAGS += -fsanitize=address,undefined
else
UNIT_CC_FLAGS += -O2
endif

ifeq ($(LIB_BUILD),shared)
UNIT_LD_FLAGS := -L$(LIB_PATH) -l$(LIB_NAME) -lpthread
UNIT_LD_FLAGS += -Wl,-rpath,$(abspath $(LIB_PATH))
else
UNIT_LD_FLAGS := $(LIB_ARTIFACT) -lpthread
endif
UNIT_LD_FLAGS += $(UNIT_LDFLAGS)

UNIT_CC_DEP := -MMD -MP -MF

HDRS_UNIT := $(shell find $(UNIT_PATH) -name '*.h' 2>/dev/null | sort)

SRCS_UNIT := $(shell find $(UNIT_PATH) -name '*.c' 2>/dev/null | sort)
SRCS_EXT  := $(shell find $(EXTERNAL_SRC_PATH) -name '*.c' 2>/dev/null | sort)
OBJS_UNIT := $(patsubst $(UNIT_PATH)/%.c,$(UNIT_OBJ)/%.o,$(SRCS_UNIT))
OBJS_EXT  := $(patsubst $(EXTERNAL_SRC_PATH)/%.c,$(UNIT_OBJ)/external/%.o,$(SRCS_EXT))
DEPS_UNIT := $(patsubst $(UNIT_OBJ)/%.o,$(UNIT_DEP)/%.d,$(OBJS_UNIT))
DEPS_EXT  := $(patsubst $(UNIT_OBJ)/external/%.o,$(UNIT_DEP)/external/%.d,$(OBJS_EXT))
OBJS_ALL  := $(OBJS_UNIT) $(OBJS_EXT)
DEPS_ALL  := $(DEPS_UNIT) $(DEPS_EXT)

export BIN_PATH

.DEFAULT_GOAL := test

.PHONY: test test-% unit unit-% clean unit_create_build_dir

unit_create_build_dir:
	@mkdir -p $(UNIT_OBJ)/external $(UNIT_DEP)/external
	@mkdir -p $(UNIT_OBJ) $(UNIT_DEP)

$(LIB_ARTIFACT):
	@$(MAKE) -C $(ROOT_DIR) lib

$(UNIT_OBJ)/external/%.o: $(EXTERNAL_SRC_PATH)/%.c | unit_create_build_dir
	@mkdir -p $(dir $@) $(dir $(UNIT_DEP)/external/$*.d)
	@$(CC) $(UNIT_CC_FLAGS) $(UNIT_CC_DEP) $(UNIT_DEP)/external/$*.d -MT $@ -c $< -o $@
	@echo "  + CC	$<"

$(UNIT_OBJ)/%.o: $(UNIT_PATH)/%.c $(HDRS_UNIT) | unit_create_build_dir
	@mkdir -p $(dir $@) $(dir $(UNIT_DEP)/$*.d)
	@$(CC) $(UNIT_CC_FLAGS) $(UNIT_CC_DEP) $(UNIT_DEP)/$*.d -MT $@ -c $< -o $@
	@echo "  + CC	$<"

-include $(DEPS_ALL)

.SECONDARY: $(DEPS_ALL)

ifeq ($(strip $(OBJS_ALL)),)
$(UNIT_BUILD)/test: unit_create_build_dir $(LIB_ARTIFACT)
	@echo "Error: no unit test sources under $(UNIT_PATH) or $(EXTERNAL_SRC_PATH)"
	@exit 1
else
$(UNIT_BUILD)/test: unit_create_build_dir $(OBJS_ALL) $(LIB_ARTIFACT)
	@mkdir -p $(dir $@)
	@$(CC) $(UNIT_CC_FLAGS) -o $@ $(OBJS_ALL) $(UNIT_LD_FLAGS)
	@echo "  + LD	$@"
endif

test:
	@$(MAKE) -C $(ROOT_DIR) bins
	@$(BATS) $(CMD_TESTS)

test-%:
	@$(MAKE) -C $(ROOT_DIR) bins
	@$(BATS) $(CMD_TESTS)/$*.bats

unit: $(UNIT_BUILD)/test
	@$(UNIT_BUILD)/test

unit-%: $(UNIT_BUILD)/test
	@$(UNIT_BUILD)/test $*

clean:
	@rm -rf $(UNIT_BUILD)
