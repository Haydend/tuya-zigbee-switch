# =========================================================================
#   Unity - A Test Framework for C
#   ThrowTheSwitch.org
#   Copyright (c) 2007-24 Mike Karlesky, Mark VanderVoord, & Greg Williams
#   SPDX-License-Identifier: MIT
# =========================================================================

#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

PROJECT_ROOT=..
UNITY_ROOT=$(PROJECT_ROOT)/test_tools/unity
CMOCK_ROOT=$(PROJECT_ROOT)/test_tools/cmock

CFLAGS := -Wall -Wno-unused-parameter -Wno-unused-variable -g -O0 \
    -DHAL_STUB -DSTACK_BUILD=1001 -D_DEFAULT_SOURCE -DVERSION_STR="0.0.0" \
	-DNVM_MIGRATIONS_VERSION=1 -DUNITY_USE_COMMAND_LINE_ARGS \
	-std=c99

# Source files for stub build
SRC_DIR            := ../src
SOURCES := \
	$(CMOCK_ROOT)/src/cmock.c \
	./mocks/Mockgpio.c \
	./mocks/gpio_callback_helper.c \
	./mocks/Mocktimer.c \
	./mocks/Mockzigbee.c \
	./mocks/Mocktasks.c \
	$(SRC_DIR)/base_components/encoder.c \

COMMON_SRC = $(UNITY_ROOT)/src/unity.c $(SOURCES)

# Auto-discover all test files
TEST_SRCS := $(wildcard test/test_*.c)
TEST_NAMES := $(patsubst test/%.c,%,$(TEST_SRCS))
TEST_TARGETS := $(patsubst %,%$(TARGET_EXTENSION),$(TEST_NAMES))

# Per-test extra sources (e.g. module under test, additional mocks).
# See test_extra_sources.mk — only needed for tests with non-standard deps.
-include test_extra_sources.mk

INC_DIRS=-I$(UNITY_ROOT)/src -I$(SRC_DIR) -I$(SRC_DIR)/hal -I$(SRC_DIR)/zigbee -I$(CMOCK_ROOT)/src -I./mocks
SYMBOLS=

.PHONY: all clean ci test/all
.PRECIOUS: test/test_runners/%_runner.c

unity/%:
	$(MAKE) -f unity.mk $*

cmock/%:
	$(MAKE) -f cmock.mk $*

$(UNITY_ROOT)/src:
	$(MAKE) -f unity.mk setup

$(CMOCK_ROOT)/src:
	$(MAKE) -f cmock.mk setup

all: clean test/all

# Generate test runner from test source
test/test_runners/%_runner.c: test/%.c | $(UNITY_ROOT)/src
	$(MKDIR) test/test_runners
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb unity_runner_config.yaml $< $@

# Compile a test binary: make build/test_toggle_switch
build/%: test/%.c test/test_runners/%_runner.c | $(UNITY_ROOT)/src $(CMOCK_ROOT)/src
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(COMMON_SRC) $(EXTRA_SRC_$*) $< test/test_runners/$*_runner.c -o $*$(TARGET_EXTENSION)

# Compile all test binaries: make build/all
build/all: $(addprefix build/,$(TEST_NAMES))

# Compile and run a single test: make test/test_toggle_switch
test/%: build/%
	- ./$*$(TARGET_EXTENSION) $(if $(RUN_TEST),-f $(RUN_TEST))

# Run all tests: make test/all
test/all: $(addprefix test/,$(TEST_NAMES))

clean:
	$(CLEANUP) $(TEST_TARGETS)

ci: CFLAGS += -Werror
ci: test/all