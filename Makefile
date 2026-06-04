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

CUR_DIR              := .
SRC_PATH             := $(CUR_DIR)/src
LIBCORE_SRC_PATH     := $(SRC_PATH)/libcore
UTIL_SRC_PATH        := $(SRC_PATH)/utils
EXTERNAL_SRC_PATH    := $(CUR_DIR)/external/src
EXTERNAL_INC_PATH 	 := $(CUR_DIR)/external/include
INCLUDE_PATH         := $(CUR_DIR)/include
CONFIG_PATH          := $(CUR_DIR)/config
BUILD_PATH           := $(CUR_DIR)/build
OBJ_PATH             := $(BUILD_PATH)/obj
DEP_PATH             := $(BUILD_PATH)/dep
BIN_PATH             := $(BUILD_PATH)/bin
LIB_PATH             := $(BUILD_PATH)/lib
TESTS_PATH           := $(CUR_DIR)/test

include $(CONFIG_PATH)/config.mk

ifeq ($(filter $(LIB_BUILD),static shared),)
$(error LIB_BUILD must be static or shared (got $(LIB_BUILD)))
endif

HOST_OS := $(shell uname -s)

LIBCORE_SRCS := $(shell find $(LIBCORE_SRC_PATH) -name '*.c' 2>/dev/null | sort)
UTIL_SRCS := $(wildcard $(UTIL_SRC_PATH)/*.c)
EXTERNAL_SRCS := $(shell find $(EXTERNAL_SRC_PATH) -name '*.c' 2>/dev/null | sort)

LIBCORE_OBJS := $(patsubst $(LIBCORE_SRC_PATH)/%.c,$(OBJ_PATH)/libcore/%.o,$(LIBCORE_SRCS))
UTIL_OBJS := $(patsubst $(UTIL_SRC_PATH)/%.c,$(OBJ_PATH)/util/%.o,$(UTIL_SRCS))
EXTERNAL_OBJS := $(patsubst $(EXTERNAL_SRC_PATH)/%.c,$(OBJ_PATH)/external/%.o,$(EXTERNAL_SRCS))
UTIL_BINS := $(patsubst $(UTIL_SRC_PATH)/%.c,$(BIN_PATH)/%,$(UTIL_SRCS))

LIBCORE_DEPS := $(patsubst $(LIBCORE_SRC_PATH)/%.c,$(DEP_PATH)/libcore/%.d,$(LIBCORE_SRCS))
UTIL_DEPS := $(patsubst $(UTIL_SRC_PATH)/%.c,$(DEP_PATH)/util/%.d,$(UTIL_SRCS))
EXTERNAL_DEPS := $(patsubst $(EXTERNAL_SRC_PATH)/%.c,$(DEP_PATH)/external/%.d,$(EXTERNAL_SRCS))

ifeq ($(LIB_BUILD),shared)
ifeq ($(HOST_OS),Darwin)
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).dylib
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).so
endif
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).a
endif

CC_FLAGS := -std=c11 -Wall -Wextra -Werror -Wshadow
CC_FLAGS += -I$(INCLUDE_PATH) -I$(EXTERNAL_INC_PATH)
ifeq ($(HOST_OS),Linux)
CC_FLAGS += -D_XOPEN_SOURCE=700
endif
ifeq ($(LIB_BUILD),shared)
CC_FLAGS += -fPIC
endif
ifeq ($(DEBUG),1)
CC_FLAGS += -g -O0
CC_FLAGS += -fsanitize=address,undefined
else
CC_FLAGS += -O2
endif

LDFLAGS :=
LDLIBS :=

ifeq ($(LIB_BUILD),shared)
UTIL_LINK_FLAGS := -L$(LIB_PATH) -l$(LIB_NAME)
UTIL_LINK_FLAGS += -Wl,-rpath,$(abspath $(LIB_PATH))
else
UTIL_LINK_FLAGS := $(LIB_ARTIFACT)
endif

AR_FLAGS := -rcs
CC_DEP := -MMD -MP -MF

$(OBJ_PATH)/libcore/%.o: $(LIBCORE_SRC_PATH)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_PATH)/libcore/$*.d)
	@$(CC) $(CC_FLAGS) $(CC_DEP) $(DEP_PATH)/libcore/$*.d -MT $@ -c $< -o $@
	@echo "  + CC	$<"

$(OBJ_PATH)/util/%.o: $(UTIL_SRC_PATH)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_PATH)/util/$*.d)
	@$(CC) $(CC_FLAGS) $(CC_DEP) $(DEP_PATH)/util/$*.d -MT $@ -c $< -o $@
	@echo "  + CC	$<"

$(OBJ_PATH)/external/%.o: $(EXTERNAL_SRC_PATH)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_PATH)/external/$*.d)
	@$(CC) $(CC_FLAGS) $(CC_DEP) $(DEP_PATH)/external/$*.d -MT $@ -c $< -o $@
	@echo "  + CC	$<"

-include $(LIBCORE_DEPS) $(UTIL_DEPS) $(EXTERNAL_DEPS)

.DEFAULT_GOAL := help
.PHONY: all lib bins clean help create_build_dir list info flags format docker \
	qsh test unit test-% unit-% clang

create_build_dir:
	@mkdir -p $(OBJ_PATH)/libcore $(OBJ_PATH)/util $(OBJ_PATH)/external
	@mkdir -p $(DEP_PATH)/libcore $(DEP_PATH)/util $(DEP_PATH)/external
	@mkdir -p $(BIN_PATH) $(LIB_PATH)

ifeq ($(strip $(LIBCORE_OBJS)),)
$(LIB_ARTIFACT): create_build_dir
	@echo "Error: no .c files under $(LIBCORE_SRC_PATH)"
	@exit 1
else
$(LIB_ARTIFACT): create_build_dir $(LIBCORE_OBJS)
ifeq ($(LIB_BUILD),static)
	@$(AR) $(AR_FLAGS) $@ $(LIBCORE_OBJS)
	@echo "  + AR	$@"
else
	@$(LD) -shared $(LDFLAGS) -o $@ $(LIBCORE_OBJS) $(LDLIBS)
	@echo "  + LD	$@"
endif
endif

ifeq ($(strip $(UTIL_BINS)),)
bins: create_build_dir lib
	@echo "Note: no utility sources in $(UTIL_SRC_PATH)/*.c"
else
bins: create_build_dir lib $(UTIL_BINS)

$(UTIL_BINS): $(BIN_PATH)/%: $(OBJ_PATH)/util/%.o $(LIB_ARTIFACT) $(EXTERNAL_OBJS)
	@$(CC) $(CC_FLAGS) $(LDFLAGS) -o $@ $(OBJ_PATH)/util/$*.o $(UTIL_LINK_FLAGS) $(EXTERNAL_OBJS) $(LDLIBS)
	@echo "  + LD	$@"
endif

lib: $(LIB_ARTIFACT)

all: lib bins

qsh:
	@echo "qsh: not implemented yet"

clean:
	@rm -rf $(BUILD_PATH)
	@$(MAKE) -C $(TESTS_PATH) clean

list:
	@echo "Libcore sources:"
	@echo $(LIBCORE_SRCS) | tr ' ' '\n' | sed 's/^/  /'
	@echo "Utility sources:"
	@echo $(UTIL_SRCS) | tr ' ' '\n' | sed 's/^/  /'
	@echo "Utilities:"
	@echo $(UTIL_BINS) | tr ' ' '\n' | sed 's/^/  /'
	@echo "External sources:"
	@echo $(EXTERNAL_SRCS) | tr ' ' '\n' | sed 's/^/  /'

info:
	@echo "Build configuration"
	@echo "  LIB_NAME     : $(LIB_NAME)"
	@echo "  LIB_BUILD    : $(LIB_BUILD)"
	@echo "  LIB_ARTIFACT : $(LIB_ARTIFACT)"
	@echo "  DEBUG        : $(DEBUG)"
	@echo "  HOST_OS      : $(HOST_OS)"
	@echo ""

help:
	@echo "USAGE:"
	@echo "  make all         - build library and utilities"
	@echo "  make lib         - build miniutils library"
	@echo "  make bins        - build standalone executables"
	@echo "  make qsh         - build qsh shell executable"
	@echo "  make clean       - remove build directory"
	@echo "  make list        - list sources and targets"
	@echo "  make info        - show build configuration"
	@echo "  make flags       - show compiler flags"
	@echo "  make format      - format .c and .h"
	@echo "  make docker      - build and run development container"
	@echo "  make test        - run command tests"
	@echo "  make test-NAME   - run one command test"
	@echo "  make unit        - run unit tests"
	@echo "  make unit-NAME   - run one unit test"
	@echo "  make clang       - run clang with bear"
	@echo "  make help        - this message"
	@echo ""

flags:
	@echo "Compiler flags:"
	@echo $(CC_FLAGS)
	@echo "Linker flags:"
	@echo $(LDFLAGS)
	@echo "Link libraries:"
	@echo $(LDLIBS)

clang:
	@$(MAKE) clean
	@bear -- $(MAKE) all

format:
	@for d in $(INCLUDE_PATH) $(EXTERNAL_INC_PATH) $(SRC_PATH) $(EXTERNAL_SRC_PATH); do \
		[ -d "$$d" ] || continue; \
		find "$$d" \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} +; \
	done
	@echo "Format done."

test:
	@$(MAKE) -C $(TESTS_PATH) test

test-%:
	@$(MAKE) -C $(TESTS_PATH) test-$*

unit:
	@$(MAKE) -C $(TESTS_PATH) unit

unit-%:
	@$(MAKE) -C $(TESTS_PATH) unit-$*

export CC

DOCKER_IMAGE := miniutils
docker:
	@if [ -z "$$(docker images -q $(DOCKER_IMAGE) 2>/dev/null)" ]; then \
		echo "Building Docker image $(DOCKER_IMAGE)..."; \
		docker build -t $(DOCKER_IMAGE) -f Dockerfile .; \
	fi
	@if [ -n "$$(docker ps -aq -f name=$(DOCKER_IMAGE)-container 2>/dev/null)" ]; then \
		docker rm -f $(DOCKER_IMAGE)-container 2>/dev/null || true; \
	fi
	@docker run -it --name $(DOCKER_IMAGE)-container -v $(CUR_DIR):/workspace $(DOCKER_IMAGE) /bin/bash
