# mini-coreutils - A minimal set of core utilities for Unix-like systems
# Copyright (C) 2025 Qiu Yixiang
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
#

# Coreutils Makefile

# Directories definitions
BUILD_DIR			:=	build
SRC_DIR				:=	src
ROOT_DIR			:=	..
ROOT_INC_DIR		:=	$(ROOT_DIR)/include
EXTERNAL_DIR		:=	$(ROOT_DIR)/external
EXTERNAL_SRC_DIR	:=	$(EXTERNAL_DIR)/src
EXTERNAL_INC_DIR	:=	$(EXTERNAL_DIR)/include
OBJ_DIR				:=	$(BUILD_DIR)/obj
DEP_DIR				:=	$(BUILD_DIR)/dep
LIB_DIR				:=	$(ROOT_DIR)/build/lib
BIN_DIR				:=	$(ROOT_DIR)/build/bin

# Module directories and names
MODULE_DIRS			:=	$(wildcard $(SRC_DIR)/*/)
MODULE_NAMES		:=	$(patsubst $(SRC_DIR)/%/,%,$(MODULE_DIRS))

# source files and object files
COREUTILS_SRC_FILES	:=	$(foreach dir,$(MODULE_DIRS),$(wildcard $(dir)*.c))
COREUTILS_OBJ_FILES	:=	$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(COREUTILS_SRC_FILES:.c=.o))
# external source files and object files
EXTERNAL_SRC_FILES	:=	$(wildcard $(EXTERNAL_SRC_DIR)/*.c)
EXTERNAL_OBJ_FILES	:=	$(patsubst $(EXTERNAL_SRC_DIR)/%,$(OBJ_DIR)/external/%,$(EXTERNAL_SRC_FILES:.c=.o))
# main object files and executable files
MODULE_MAIN_OBJS	:=	$(foreach mod,$(MODULE_NAMES),$(OBJ_DIR)/$(mod)/main.o)
COREUTILS_BIN_FILES	:=	$(foreach mod,$(MODULE_NAMES),$(BIN_DIR)/$(mod))
# Object files excluding main.o files
COREUTILS_OBJ_NO_MAIN	:=	$(filter-out $(MODULE_MAIN_OBJS),$(COREUTILS_OBJ_FILES))

# CC flags
CC_WARN_FLAGS		:=	-Wall -Wextra -Werror -Wshadow
CC_DEBUG_FLAGS		:=
CC_STD_FLAGS		:=	-std=c11
CC_DEP_FLAGS		:=	-MMD -MP -MF
CC_OPTIMIZE_FLAGS	:=	
CC_INCLUDE_FLAGS	:=	-I$(ROOT_INC_DIR) -I$(EXTERNAL_INC_DIR)

ifeq ($(DEBUG_MODE), 1)
CC_DEBUG_FLAGS		+=	-g
endif

ifeq ($(DEBUG_MODE), 1)
CC_OPTIMIZE_FLAGS	+=	-O0
else
CC_OPTIMIZE_FLAGS	+=	-O3
endif

CC_FLAGS			:=	$(CC_WARN_FLAGS) $(CC_DEBUG_FLAGS) $(CC_STD_FLAGS)
CC_FLAGS			+=	$(CC_INCLUDE_FLAGS) $(CC_DEP_FLAGS) $(CC_OPTIMIZE_FLAGS) -fPIC

# AR tool
AR				:=	ar
ifdef TOOL_CHAIN_PREFIX
AR				:=	$(TOOL_CHAIN_PREFIX)ar
endif

# Shared library postfix
HOST_OS			:=	$(shell uname -s)
ifeq ($(HOST_OS), Linux)
SHARED_LIB_POSTFIX	:=	.so
else
SHARED_LIB_POSTFIX	:=	.dylib
endif

# Object file rule: preserve directory structure
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo " + CC\t$@"
	@mkdir -p $(dir $@)
	@mkdir -p $(patsubst $(OBJ_DIR)/%,$(DEP_DIR)/%,$(dir $@))
	@$(CC) $(CC_FLAGS) $(CC_DEP_FLAGS) $(patsubst $(OBJ_DIR)/%,$(DEP_DIR)/%,$(@:.o=.d)) -c $< -o $@

# External object file rule
$(OBJ_DIR)/external/%.o: $(EXTERNAL_SRC_DIR)/%.c
	@echo " + CC\t$@"
	@mkdir -p $(dir $@)
	@mkdir -p $(patsubst $(OBJ_DIR)/%,$(DEP_DIR)/%,$(dir $@))
	@$(CC) $(CC_FLAGS) $(CC_DEP_FLAGS) $(patsubst $(OBJ_DIR)/%,$(DEP_DIR)/%,$(@:.o=.d)) -c $< -o $@

# Executable rule: link module's main.o with other object files and external libraries
$(BIN_DIR)/%: $(OBJ_DIR)/%/main.o $(COREUTILS_OBJ_NO_MAIN) $(EXTERNAL_OBJ_FILES)
	@echo " + LD\t$@"
	@mkdir -p $(dir $@)
	@$(CC) -o $@ $< $(COREUTILS_OBJ_NO_MAIN) $(EXTERNAL_OBJ_FILES)

# Library rule: create static library
$(LIB_DIR)/libcoreutils.a: $(COREUTILS_OBJ_FILES) $(EXTERNAL_OBJ_FILES)
	@echo " + AR\t$@"
	@$(AR) rcs $@ $^

# Shared library rule: create shared library
$(LIB_DIR)/libcoreutils$(SHARED_LIB_POSTFIX): $(COREUTILS_OBJ_FILES) $(EXTERNAL_OBJ_FILES)
	@echo " + LD\t$@"
	@$(CC) -shared -o $@ $^

# Targets
LIB_TARGETS		:=	$(LIB_DIR)/libcoreutils.a $(LIB_DIR)/libcoreutils$(SHARED_LIB_POSTFIX)
BIN_TARGETS		:=	$(COREUTILS_BIN_FILES)
TARGETS			:=	$(LIB_TARGETS) $(BIN_TARGETS)

.DEFAULT_GOAL		:=	all
.PHONY:			all test clean _check_dir

-include $(shell find $(DEP_DIR) -name "*.d" 2>/dev/null)

all: _check_dir $(TARGETS)

# Integrated test target
test:
	@:

# Clean build directory
clean:
	@rm -rf $(BUILD_DIR)

# Check and create build directories
_check_dir:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR) $(DEP_DIR) $(LIB_DIR) $(BIN_DIR)
